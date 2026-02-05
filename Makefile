# Makefile для FastEngine
# Упрощает процесс сборки для разных платформ

.PHONY: all desktop android ios clean help

# По умолчанию собираем для desktop
all: desktop

# Сборка для desktop
desktop:
	@echo "Building for desktop..."
	@chmod +x scripts/build_desktop.sh
	@./scripts/build_desktop.sh

# Сборка для Android
android:
	@echo "Building for Android..."
	@chmod +x scripts/build_android.sh
	@./scripts/build_android.sh

# Сборка для iOS
ios:
	@echo "Building for iOS..."
	@chmod +x scripts/build_ios.sh
	@./scripts/build_ios.sh

# Очистка
clean:
	@echo "Cleaning build directories..."
	@rm -rf build/

# Установка зависимостей (macOS)
install-deps-macos:
	@echo "Installing dependencies for macOS..."
	@brew install cmake sdl2 openal-soft freetype

# Установка зависимостей (Ubuntu/Debian)
install-deps-ubuntu:
	@echo "Installing dependencies for Ubuntu/Debian..."
	@sudo apt update
	@sudo apt install build-essential cmake libsdl2-dev libopenal-dev libfreetype6-dev

# Создание директорий
setup:
	@echo "Setting up project structure..."
	@mkdir -p assets/{textures,sounds,fonts}
	@mkdir -p docs

# Справка
help:
	@echo "Available targets:"
	@echo "  all       - Build for desktop (default)"
	@echo "  desktop   - Build for desktop platforms"
	@echo "  android   - Build for Android"
	@echo "  ios       - Build for iOS"
	@echo "  clean     - Clean all build directories"
	@echo "  setup     - Create additional project directories"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Dependencies installation:"
	@echo "  install-deps-macos   - Install dependencies on macOS"
	@echo "  install-deps-ubuntu  - Install dependencies on Ubuntu/Debian"
