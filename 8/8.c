#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N должно быть положительным числом.\n");
        return 1;
    }

    int pipe_fd[2];
    pipe(pipe_fd);

    for (int i = 0; i < 10; i++) { // 10 циклов
        pid_t pid = fork();
        if (pid < 0) {
            perror("Ошибка fork");
            exit(1);
        }

        if (pid == 0) { // Второй игрок
            close(pipe_fd[1]); // Закрываем запись в канале

            int guess;
            while (read(pipe_fd[0], &guess, sizeof(guess)) > 0) {
                printf("Введите ваше предположение (от 1 до %d): ", N);
                scanf("%d", &guess);

                write(pipe_fd[1], &guess, sizeof(guess)); // Отправляем предположение
                if (guess == N) { // Угадал
                    printf("Второй игрок угадал число %d!\n", N);
                    break;
                } else {
                    printf("Попытка угадать число %d не удалась.\n", guess);
                }
            }
            close(pipe_fd[0]);
            exit(0);
        } else { // Первый игрок
            close(pipe_fd[0]); // Закрываем чтение из канала

            int secret_number = rand() % N + 1;
            printf("Первый игрок загадал число: %d\n", secret_number);

            write(pipe_fd[1], &secret_number, sizeof(secret_number)); // Отправляем загаданное число

            wait(NULL); // Ожидание завершения второго процесса
            close(pipe_fd[1]);
        }
    }

    return 0;
}
