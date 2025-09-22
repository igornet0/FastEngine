# 🖥️ Project Simulator CLI - Руководство по использованию

## Обзор

**Project Simulator CLI** - это мощный инструмент командной строки для тестирования и симуляции проектов FastEngine. Он позволяет автоматизировать процессы тестирования, интеграции в CI/CD и пакетной обработки проектов.

## 🚀 Быстрый старт

### 1. Сборка CLI

```bash
cd simulator
make build
```

### 2. Базовое использование

```bash
# Показать справку
./scripts/simulator-cli --help

# Список проектов
./scripts/simulator-cli --list-projects

# Тестирование проекта
./scripts/simulator-cli test -p ./projects/basic_game

# Симуляция проекта
./scripts/simulator-cli simulate -p ./projects/basic_game
```

## 📋 Команды

### Основные команды

| Команда | Описание | Пример |
|---------|----------|--------|
| `test` | Запуск тестов проекта | `simulator-cli test -p ./projects/basic_game` |
| `simulate` | Запуск симуляции проекта | `simulator-cli simulate -p ./projects/basic_game` |
| `validate` | Валидация проекта | `simulator-cli validate -p ./projects/basic_game` |
| `create` | Создание нового проекта | `simulator-cli create -p ./projects/my_game` |
| `batch-test` | Тестирование всех проектов | `simulator-cli batch-test` |

### Опции

| Опция | Короткая | Описание | Пример |
|-------|----------|----------|--------|
| `--help` | `-h` | Показать справку | `simulator-cli --help` |
| `--project` | `-p` | Путь к проекту | `-p ./projects/basic_game` |
| `--output` | `-o` | Директория для результатов | `-o ./test_results` |
| `--test-types` | `-t` | Типы тестов | `-t unit,integration` |
| `--platforms` | | Целевые платформы | `--platforms desktop,ios` |
| `--iterations` | `-i` | Количество итераций | `-i 5` |
| `--timeout` | | Таймаут в секундах | `--timeout 60` |
| `--verbose` | `-v` | Подробный вывод | `--verbose` |
| `--no-report` | | Не генерировать HTML отчет | `--no-report` |
| `--config` | `-c` | Файл конфигурации | `-c unit_tests.conf` |

## 🧪 Типы тестов

### Unit Tests
Тестирование отдельных компонентов:
```bash
simulator-cli test -p ./projects/basic_game -t unit
```

### Integration Tests
Тестирование взаимодействия систем:
```bash
simulator-cli test -p ./projects/basic_game -t integration
```

### Performance Tests
Тестирование производительности:
```bash
simulator-cli test -p ./projects/basic_game -t performance -i 3
```

### Compatibility Tests
Тестирование совместимости платформ:
```bash
simulator-cli test -p ./projects/basic_game -t compatibility --platforms desktop,ios,android
```

### Stress Tests
Стресс-тестирование:
```bash
simulator-cli test -p ./projects/basic_game -t stress -i 10 --timeout 120
```

## 📱 Платформы

### Desktop
```bash
simulator-cli test -p ./projects/basic_game --platforms desktop
```

### iOS
```bash
simulator-cli test -p ./projects/basic_game --platforms ios
```

### Android
```bash
simulator-cli test -p ./projects/basic_game --platforms android
```

### Все платформы
```bash
simulator-cli test -p ./projects/basic_game --platforms desktop,ios,android
```

## 🔧 Конфигурационные файлы

### Создание конфигурации

```bash
# Создать конфигурацию для unit тестов
cat > unit_tests.conf << EOF
project_path=./projects/basic_game
output_path=./test_results/unit
test_types=unit
platforms=desktop
iterations=1
timeout=10
verbose=true
generate_report=true
EOF
```

### Использование конфигурации

```bash
simulator-cli test -c unit_tests.conf
```

### Готовые конфигурации

В папке `configs/` доступны готовые конфигурации:

- `unit_tests.conf` - Unit тесты
- `integration_tests.conf` - Интеграционные тесты
- `performance_tests.conf` - Тесты производительности
- `cross_platform_tests.conf` - Кроссплатформенные тесты
- `stress_tests.conf` - Стресс-тесты

## 📊 Примеры использования

### 1. Тестирование одного проекта

```bash
# Базовое тестирование
simulator-cli test -p ./projects/basic_game

# Подробное тестирование с отчетом
simulator-cli test -p ./projects/basic_game -v -t unit,integration,performance

# Тестирование на всех платформах
simulator-cli test -p ./projects/basic_game --platforms desktop,ios,android
```

### 2. Пакетное тестирование

```bash
# Тестирование всех проектов
simulator-cli batch-test

# Тестирование с настройками
simulator-cli batch-test -t unit,integration --platforms desktop -v
```

### 3. Симуляция проектов

```bash
# Запуск симуляции
simulator-cli simulate -p ./projects/basic_game

# Симуляция с подробным выводом
simulator-cli simulate -p ./projects/basic_game -v
```

### 4. Валидация проектов

```bash
# Проверка корректности проекта
simulator-cli validate -p ./projects/basic_game

# Валидация с подробным выводом
simulator-cli validate -p ./projects/basic_game -v
```

### 5. Создание проектов

```bash
# Создание нового проекта
simulator-cli create -p ./projects/my_new_game

# Создание с шаблоном
simulator-cli create -p ./projects/my_new_game --template basic_game
```

## 🛠️ Использование Makefile

### Основные команды

```bash
# Сборка
make build

# Тестирование всех проектов
make test-all

# Unit тесты
make test-unit

# Интеграционные тесты
make test-integration

# Тесты производительности
make test-performance

# Кроссплатформенные тесты
make test-cross-platform

# Стресс-тесты
make test-stress
```

### Тестирование конкретного проекта

```bash
# Тестирование проекта
make test PROJECT=./projects/basic_game

# Тестирование с дополнительными параметрами
make test PROJECT=./projects/basic_game ARGS="--verbose --test-types unit,performance"
```

### Симуляция и валидация

```bash
# Симуляция проекта
make simulate PROJECT=./projects/basic_game

# Валидация проекта
make validate PROJECT=./projects/basic_game

# Создание проекта
make create PROJECT=./projects/my_game
```

### Использование конфигураций

```bash
# Тестирование с конфигурацией
make test-config CONFIG=unit_tests

# Тестирование с дополнительными параметрами
make test-config CONFIG=performance_tests ARGS="--verbose"
```

## 🔄 CI/CD интеграция

### GitHub Actions

```yaml
name: Project Simulator Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup Project Simulator
        run: |
          cd simulator
          make build
      - name: Run Tests
        run: |
          cd simulator
          make ci-full
      - name: Upload Results
        uses: actions/upload-artifact@v2
        with:
          name: test-results
          path: simulator/test_results/
```

### Jenkins Pipeline

```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'cd simulator && make build'
            }
        }
        stage('Test') {
            steps {
                sh 'cd simulator && make ci-full'
            }
        }
        stage('Report') {
            steps {
                publishHTML([
                    allowMissing: false,
                    alwaysLinkToLastBuild: true,
                    keepAll: true,
                    reportDir: 'simulator/test_results',
                    reportFiles: '*.html',
                    reportName: 'Test Report'
                ])
            }
        }
    }
}
```

### GitLab CI

```yaml
stages:
  - build
  - test
  - report

build:
  stage: build
  script:
    - cd simulator
    - make build

test:
  stage: test
  script:
    - cd simulator
    - make ci-full
  artifacts:
    reports:
      junit: simulator/test_results/*.xml
    paths:
      - simulator/test_results/
    expire_in: 1 week

report:
  stage: report
  script:
    - echo "Test results available in artifacts"
  dependencies:
    - test
```

## 📈 Мониторинг и отчеты

### HTML отчеты

```bash
# Генерация HTML отчета
simulator-cli test -p ./projects/basic_game -o ./reports

# Отчет будет создан в ./reports/test_report_<timestamp>.html
```

### JSON отчеты для CI

```bash
# CI тесты с JSON отчетом
./scripts/ci_test.sh --test-types unit,integration --output ./ci_results
```

### Логирование

```bash
# Подробные логи
simulator-cli test -p ./projects/basic_game -v

# Логи сохраняются в output директории
```

## 🚨 Обработка ошибок

### Коды возврата

- `0` - Успешное выполнение
- `1` - Ошибка выполнения
- `2` - Ошибка аргументов
- `3` - Ошибка конфигурации

### Типичные ошибки

1. **"Project not found"**
   ```bash
   # Проверьте путь к проекту
   ls -la ./projects/basic_game/project.conf
   ```

2. **"CLI not built"**
   ```bash
   # Соберите CLI
   make build
   ```

3. **"Permission denied"**
   ```bash
   # Установите права выполнения
   chmod +x scripts/simulator-cli
   ```

## 🔧 Расширенное использование

### Скрипты-обертки

```bash
#!/bin/bash
# my_test_script.sh

# Настройки
PROJECT="./projects/my_game"
TEST_TYPES="unit,integration,performance"
PLATFORMS="desktop,ios"

# Запуск тестов
./scripts/simulator-cli test \
    -p "$PROJECT" \
    -t "$TEST_TYPES" \
    --platforms "$PLATFORMS" \
    -v \
    -o "./test_results/$(date +%Y%m%d_%H%M%S)"
```

### Автоматизация

```bash
#!/bin/bash
# auto_test.sh - Автоматическое тестирование при изменениях

# Мониторинг изменений
while inotifywait -e modify,create,delete -r ./projects/; do
    echo "Changes detected, running tests..."
    make test-all
done
```

### Интеграция с IDE

```json
// VS Code tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Test Project",
            "type": "shell",
            "command": "make",
            "args": ["test", "PROJECT=${workspaceFolder}/projects/${input:projectName}"],
            "group": "test"
        }
    ],
    "inputs": [
        {
            "id": "projectName",
            "description": "Project name",
            "type": "pickString",
            "options": ["basic_game", "sprite_demo", "input_test"]
        }
    ]
}
```

## 📚 Дополнительные ресурсы

- [Полная документация](README.md)
- [Примеры проектов](projects/sample_projects/)
- [Конфигурации](configs/)
- [Скрипты](scripts/)

## 🤝 Поддержка

Если у вас есть вопросы или проблемы:

1. Проверьте [справку](CLI_USAGE.md)
2. Посмотрите [примеры](#примеры-использования)
3. Изучите [логи](#логирование)
4. Создайте [Issue](https://github.com/your-repo/issues)

---

**Project Simulator CLI** - мощный инструмент для автоматизации тестирования! 🚀

