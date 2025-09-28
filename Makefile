# Makefile для проекта Snake/Tetris

# Компиляторы
CXX = g++
CC = gcc

# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I.
CFLAGS = -Wall -Wextra -fPIC -I.

# Qt флаги
QT_FLAGS = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)

# ncurses флаги
NCURSES_FLAGS = $(shell pkg-config --cflags ncurses)
NCURSES_LIBS = $(shell pkg-config --libs ncurses)

# Исходные файлы
SOURCES_CXX = game.cpp \
              gui/cli/snake_console.cpp \
              gui/desktop/snake_frontend.cpp \
              gui/desktop/tetris_frontend.cpp \
              brick_game/snake/model.cpp \
              brick_game/snake/controller.cpp

SOURCES_C = brick_game/tetris/tetris_backend.c \
            brick_game/tetris/figures.c \
            gui/cli/tetris_cli_frontend.c

# MOC файлы для Qt
MOC_SOURCES = gui/desktop/moc_snake_frontend.cpp \
              gui/desktop/moc_tetris_frontend.cpp

# Объектные файлы
OBJECTS_CXX = $(SOURCES_CXX:.cpp=.o) $(MOC_SOURCES:.cpp=.o)
OBJECTS_C = $(SOURCES_C:.c=.o)
OBJECTS = $(OBJECTS_CXX) $(OBJECTS_C)

# Исполняемый файл
TARGET = game

# Основная цель
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(QT_LIBS) $(NCURSES_LIBS)

# Компиляция C++ файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(QT_FLAGS) $(NCURSES_FLAGS) -c $< -o $@

# Компиляция C файлов
%.o: %.c
	$(CC) $(CFLAGS) $(NCURSES_FLAGS) -c $< -o $@

# Генерация MOC файлов
gui/desktop/moc_snake_frontend.cpp: gui/desktop/snake_frontend.h
	moc $< -o $@

gui/desktop/moc_tetris_frontend.cpp: gui/desktop/tetris_frontend.h
	moc $< -o $@

# Очистка
clean:
	rm -f $(OBJECTS) $(TARGET) $(MOC_SOURCES)

# Установка зависимостей (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install qtbase5-dev libncurses-dev

# Запуск игры
run: $(TARGET)
	./$(TARGET)

# Помощь
help:
	@echo "Доступные команды:"
	@echo "  make          - Собрать проект"
	@echo "  make clean    - Очистить сборочные файлы"
	@echo "  make run      - Собрать и запустить игру"
	@echo "  make install-deps - Установить зависимости"
	@echo "  make help     - Показать эту справку"

.PHONY: all clean install-deps run help
