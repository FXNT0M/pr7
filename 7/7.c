#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

int secret_number;
pid_t child_pid;
int attempts; // Переменная для отслеживания количества попыток

void handler_result(int sig) {
    if (sig == SIGUSR1) {
        printf("Игрок 2 угадал число %d!\n", secret_number);
        printf("Количество попыток: %d\n", attempts);
        exit(0); // Завершить процесс после успешного угадывания
    } else if (sig == SIGUSR2) {
        printf("Игрок 2 не угадал. Попробуйте снова.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int player_turn = 2;
    
    srand(time(NULL)); // Инициализация генератора случайных чисел

    for (int game = 0; game < 10; game++) { // 10 игр
        secret_number = rand() % N + 1; // Генерация случайного числа от 1 до N
        printf("Игрок 1 загадывает число: %d\n", secret_number);

        child_pid = fork();
        
        if (child_pid < 0) {
            perror("Ошибка fork");
            exit(1);
        } else if (child_pid == 0) { // Процесс игрока 2
            signal(SIGUSR1, handler_result);
            signal(SIGUSR2, handler_result);

            attempts = 0; // Сброс количества попыток
            int guess;
            while (1) {
                guess = rand() % N + 1; // Генерация случайного предположения от 1 до N
                attempts++; // Увеличиваем счетчик попыток
                printf("Игрок 2 пытается угадать: %d\n", guess);

                // Отправляем сигнал о том, что игрок 2 сделал предположение
                if (guess == secret_number) {
                    kill(getppid(), SIGUSR1); // Угадал
                } else {
                    kill(getppid(), SIGUSR2); // Не угадал
                }

                pause(); // Ожидание сигнала
            }
            exit(0);
        } 
        else { // Процесс игрока 1
            signal(SIGUSR1, handler_result);
            signal(SIGUSR2, handler_result);
            while(1) {
                pause(); // Ожидание сигнала от игрока 2
                if (waitpid(child_pid, NULL, WNOHANG) > 0) {
                    break; // Если процесс игрока 2 завершился, выходим из цикла
                }
            } 
            printf("Игра %d завершена.\n", game + 1);
        }

        // Меняем местами игроков
        player_turn = (player_turn == 1) ? 2 : 1; // Меняем местами
 
        }
    return 0;
}
