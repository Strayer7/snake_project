# Snake & Tetris Game Project

Проект содержит две классические игры: Snake и Tetris, с поддержкой как консольного, так и графического интерфейса.

## 🎮 Игры

- **Snake** - классическая змейка
- **Tetris** - тетрис с падающими фигурами

## 🖥️ Интерфейсы

- **Консольный** - использует ncurses для текстового интерфейса
- **Графический** - использует Qt5 для GUI

## 📋 Требования

### Зависимости
- **Qt5** - для графического интерфейса
- **ncurses** - для консольного интерфейса
- **pkg-config** - для настройки компиляции
- **gcc/g++** - компиляторы C/C++

### Установка зависимостей (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install qtbase5-dev libncurses-dev pkg-config
```

## 🔨 Сборка

### Автоматическая сборка
```bash
make
```

### Установка зависимостей через Makefile
```bash
make install-deps
```

### Очистка
```bash
make clean
```

## 🚀 Запуск

```bash
./game
```

После запуска выберите режим:
1. Snake (Console) - змейка в консоли
2. Snake (Desktop) - змейка с GUI
3. Tetris (Console) - тетрис в консоли  
4. Tetris (Desktop) - тетрис с GUI

## 🎯 Управление

### Snake
- **WASD** или **стрелки** - движение
- **P** - пауза
- **Enter** - начать игру
- **Esc** - выход
- **Space** - ускорение

### Tetris
- **Стрелки** - движение и ускорение падения
- **R** - поворот фигуры
- **P** - пауза
- **Q** - выход

## 🏗️ Архитектура

```
snake_project/
├── game.cpp                 # Главный файл с меню
├── common.h                 # Общие определения
├── Makefile                 # Система сборки
├── gui/
│   ├── cli/                 # Консольные интерфейсы
│   │   ├── snake_console.cpp
│   │   ├── snake_console.h
│   │   ├── tetris_cli_frontend.c
│   │   └── tetris_cli_frontend.h
│   └── desktop/             # Qt интерфейсы
│       ├── snake_frontend.cpp
│       ├── snake_frontend.h
│       ├── tetris_frontend.cpp
│       └── tetris_frontend.h
└── brick_game/
    ├── snake/               # Логика Snake (C++)
    │   ├── model.cpp
    │   ├── model.h
    │   ├── controller.cpp
    │   └── controller.h
    └── tetris/              # Логика Tetris (C)
        ├── tetris_backend.c
        ├── tetris_backend.h
        ├── figures.c
        └── figures.h
```

## 🐛 Исправленные проблемы

1. ✅ **Отсутствующие зависимости** - установлены Qt5 и ncurses
2. ✅ **Отсутствующие определения** - добавлен enum Game_state
3. ✅ **Дублирование определений** - убраны дубли WIDTH/HEIGHT
4. ✅ **Проблемы с включением заголовков** - исправлены пути и extern "C"
5. ✅ **Отсутствие системы сборки** - создан Makefile с MOC поддержкой
6. ✅ **Проблемы с линковкой** - исправлены extern "C" блоки
7. ✅ **Проблемы с Qt MOC** - добавлена генерация MOC файлов

## 🧪 Тестирование

Запустите тестовый скрипт:
```bash
./test_game.sh
```

## 📝 Примечания

- Проект использует смешанную архитектуру C/C++
- Qt классы требуют MOC (Meta-Object Compiler)
- Консольные версии работают в терминале
- Графические версии требуют X11/Wayland

## 🎉 Статус

Проект полностью функционален и готов к использованию!
