#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#define MAX_ATTEMPTS 100 // Limit the number of attempts

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N должно быть положительным числом.\n");
        exit(EXIT_FAILURE);
    }
    setlocale(LC_ALL, "ru_RU");

    for (int game = 1; game <= 2; ++game) {
        int secret_number;
        int attempts = 0;
        int guess;
        int pipefd[2];
        bool used_numbers[N + 1];

        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process (Guesser)
            close(pipefd[1]);
            srand(time(NULL) + getpid());
            for (int i = 0; i <= N; ++i) used_numbers[i] = false;

            while (attempts < MAX_ATTEMPTS) {
                read(pipefd[0], &secret_number, sizeof(int));
                do {
                    guess = rand() % N + 1;
                } while (used_numbers[guess]);
                used_numbers[guess] = true;

                printf("Игрок %d предполагает: %d\n", (game == 1) ? 2 : 1, guess);
                if (guess == secret_number) {
                    printf("Игрок %d: Я угадал число за %d попыток!\n", (game == 1) ? 2 : 1, ++attempts);
                    break;
                }
                attempts++;
            }
            if (attempts == MAX_ATTEMPTS) {
                printf("Игрок %d: Не смог угадать число за %d попыток.\n", (game == 1) ? 2 : 1, attempts);
            }
            close(pipefd[0]);
            exit(attempts); // Return the number of attempts
        } else { // Parent process (Number Chooser)
            close(pipefd[0]);
            srand(time(NULL) + getpid());
            secret_number = rand() % N + 1;
            printf("Игрок %d загадал число от 1 до %d: %d\n", game, N, secret_number);
            write(pipefd[1], &secret_number, sizeof(int));
            close(pipefd[1]);
            int status;
            wait(&status); // Wait for child process and get exit status
            int attempts_made = WEXITSTATUS(status); // Extract the number of attempts
            printf("Игрок %d: Игрок %d угадал (или не угадал) число за %d попыток.\n", game, (game == 1) ? 2 : 1, attempts_made);
        }
    }
    return 0;
}
