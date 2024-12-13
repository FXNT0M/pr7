#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>

// Флаг для обозначения, что число угадано
volatile sig_atomic_t guessed = 0;
// Указатель на загаданное число
int *secret_number;
// Количество попыток
int *attempts;

void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        guessed = 1;
    }
}

void player_one(int N) {
    // Cоздает уникальное значение для инициализации генератора случайных чисел.
    srand(time(NULL) + getpid()); 

    while (1) {
        // Загадываем число
        *secret_number = rand() % N + 1; 
        printf("Игрок 1 загадал число от 1 до %d: %d\n", N, *secret_number);

        guessed = 0;
        *attempts = 0;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Дочерний процесс: игрок 2
            int guess;
            while (1) {
                (*attempts)++;
                // Угадываем случайное число
                guess = rand() % N + 1; 
                printf("Игрок 2 предполагает: %d\n", guess);

                if (guess == *secret_number) {
                    printf("Игрок 2: Я угадал число за %d попыток!\n", *attempts);
                    kill(getppid(), SIGUSR1);
                    exit(EXIT_SUCCESS);
                }
            }
        } else {
            // Родительский процесс: игрок 1
            while (!guessed) {
                // Ждём завершения процесса игрока 2
                wait(NULL); 
            }
            printf("Игрок 1: Игрок 2 угадал число %d за %d попыток.\n", *secret_number, *attempts);
            break;
        }
    }
}

void player_two(int N) {
    srand(time(NULL) + getpid());

    while (1) {
        // Загадываем число
        *secret_number = rand() % N + 1; 
        printf("Игрок 2 загадал число от 1 до %d: %d\n", N, *secret_number);

        guessed = 0;
        *attempts = 0;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Дочерний процесс: игрок 1
            int guess;
            while (1) {
                (*attempts)++;
                // Угадываем случайное число
                guess = rand() % N + 1; 
                printf("Игрок 1 предполагает: %d\n", guess);

                if (guess == *secret_number) {
                    printf("Игрок 1: Я угадал число за %d попыток!\n", *attempts);
                    kill(getppid(), SIGUSR1);
                    exit(EXIT_SUCCESS);
                }
            }
        } else {
            // Родительский процесс: игрок 2
            while (!guessed) {
                // Ждём завершения процесса игрока 1
                wait(NULL); 
            }
            printf("Игрок 2: Игрок 1 угадал число %d за %d попыток.\n", *secret_number, *attempts);
            break;
        }
    }
}

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

    // Создаем общую память для хранения загаданного числа и количества попыток
    secret_number = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    attempts = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (secret_number == MAP_FAILED || attempts == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, signal_handler);

    while (1) {
        printf("\n--- Игра между игроками 1 и 2 ---\n");
        player_one(N);

        printf("\n--- Игра между игроками 2 и 1 ---\n");
        player_two(N);
    }

    munmap(secret_number, sizeof(int));
    munmap(attempts, sizeof(int));
    return 0;
}
