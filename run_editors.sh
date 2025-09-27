#!/bin/bash

echo "=== FastEngine Game Editors ==="
echo ""
echo "Выберите редактор для запуска:"
echo "1) Simple Game Editor (Простой редактор)"
echo "2) Advanced Game Editor (Расширенный редактор)"
echo "3) Оба редактора"
echo "4) Выход"
echo ""

read -p "Введите номер (1-4): " choice

case $choice in
    1)
        echo "Запуск Simple Game Editor..."
        cd sdl_editor/build
        ./SimpleGameEditorSDL
        ;;
    2)
        echo "Запуск Advanced Game Editor..."
        cd advanced_editor/build
        ./AdvancedGameEditor
        ;;
    3)
        echo "Запуск обоих редакторов..."
        echo "Simple Game Editor будет запущен в фоновом режиме"
        cd sdl_editor/build
        ./SimpleGameEditorSDL &
        echo "Advanced Game Editor запускается..."
        cd ../../advanced_editor/build
        ./AdvancedGameEditor
        ;;
    4)
        echo "Выход..."
        exit 0
        ;;
    *)
        echo "Неверный выбор. Попробуйте снова."
        exit 1
        ;;
esac

