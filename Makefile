# Определение компилятора и флагов
CC = gcc
CFLAGS = -Wall -Wextra -pedantic

# Имя исполняемого файла
TARGET = guess_number

# Исходные файлы
SRCS = guess_number.c

# Правило по умолчанию
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Правило для очистки скомпилированных файлов
clean:
	rm -f $(TARGET)

# Правило для запуска программы с аргументом N
run: $(TARGET)
	@echo "Запуск программы с аргументом N (например, 100):"
	@./$(TARGET) 100

.PHONY: all clean run
