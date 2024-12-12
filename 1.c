#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t guessed = 0; // Флаг для отслеживания угаданного числа
volatile sig_atomic_t guess = 0;    // Угаданное число

// Обработчик сигнала для успешного угадывания
void handle_correct_guess(int sig) {
    guessed = 1;
}

// Обработчик сигнала для неверного угадывания
void handle_incorrect_guess(int sig) {
    guessed = 0;
}

// Функция для первого игрока (загадать число)
void player_one(int n) {
    srand(time(NULL));
    int secret_number = rand() % n + 1; // Загадать число от 1 до N
    printf("Игрок 1 загадал число: %d\n", secret_number);
    
    // Отправляем загаданное число второму игроку через сигнал
    kill(getppid(), SIGUSR1); // Уведомление о начале игры

    while (1) {
        pause(); // Ожидание сигнала от второго игрока
        
        if (guessed) {
            printf("Игрок 1: Игрок 2 угадал число %d!\n", secret_number);
            break; // Выход из цикла, если угадано
        } else {
            printf("Игрок 1: Игрок 2 не угадал. Загаданное число было %d.\n", secret_number);
        }
        
        // Меняем местами игроков и начинаем новую игру
        sleep(1); // Небольшая пауза перед новой игрой
        secret_number = rand() % n + 1; // Новое загаданное число
        printf("Игрок 1 загадал новое число: %d\n", secret_number);
    }
}

// Функция для второго игрока (угадать число)
void player_two(int n) {
    int attempts = 0;
    srand(time(NULL) ^ getpid()); // Инициализация генератора случайных чисел

    while (1) {
        attempts++;
        guess = rand() % n + 1; // Генерация предположения
        
        printf("Игрок 2: Я думаю, что это %d\n", guess);
        
        // Отправляем сигнал в зависимости от того, угадали ли мы или нет
        if (guess == guess) {
            kill(getppid(), SIGUSR1); // Угадали
            break;
        } else {
            kill(getppid(), SIGUSR2); // Не угадали
        }
        
        pause(); // Ожидание ответа от первого игрока
    }
    
    printf("Игрок 2: Я угадал число %d за %d попыток!\n", guess, attempts);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    
    if (n <= 0) {
        fprintf(stderr, "N должно быть положительным числом.\n");
        return EXIT_FAILURE;
    }

    signal(SIGUSR1, handle_correct_guess);
    signal(SIGUSR2, handle_incorrect_guess);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при создании процесса");
        return EXIT_FAILURE;
    }

    if (pid == 0) { // Второй игрок
        player_two(n);
    } else { // Первый игрок
        player_one(n);
        wait(NULL); // Ожидание завершения второго процесса
    }

    return EXIT_SUCCESS;
}
