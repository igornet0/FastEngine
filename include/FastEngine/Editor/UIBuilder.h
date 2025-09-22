#pragma once

#include "UIElement.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace FastEngine {

/**
 * Режимы UI Builder
 */
enum class UIBuilderMode {
    Design,
    Preview,
    Code
};

/**
 * Редактор UI
 * Управляет созданием и редактированием пользовательских интерфейсов
 */
class UIBuilder {
public:
    UIBuilder();
    ~UIBuilder();

    /**
     * Инициализация редактора UI
     * @return true если инициализация успешна
     */
    bool Initialize();

    /**
     * Обновление редактора UI
     * @param deltaTime время между кадрами
     */
    void Update(float deltaTime);

    /**
     * Рендеринг редактора UI
     * @param renderer рендерер
     */
    void Render(void* renderer);

    /**
     * Обработка ввода
     * @param mouseX позиция мыши X
     * @param mouseY позиция мыши Y
     * @param mouseButton кнопка мыши
     * @param key клавиша
     * @return true если ввод обработан
     */
    bool HandleInput(float mouseX, float mouseY, int mouseButton, int key);

    /**
     * Создание UI элемента
     * @param type тип элемента
     * @param position позиция
     * @return созданный элемент
     */
    std::shared_ptr<UIElement> CreateElement(UIElementType type, const glm::vec2& position = glm::vec2(0.0f));

    /**
     * Удаление UI элемента
     * @param elementId ID элемента
     * @return true если элемент удален
     */
    bool RemoveElement(const std::string& elementId);

    /**
     * Получение UI элемента по ID
     * @param elementId ID элемента
     * @return элемент или nullptr
     */
    std::shared_ptr<UIElement> GetElement(const std::string& elementId);

    /**
     * Получение всех UI элементов
     */
    const std::vector<std::shared_ptr<UIElement>>& GetElements() const { return m_elements; }

    /**
     * Выбор UI элемента
     * @param elementId ID элемента
     */
    void SelectElement(const std::string& elementId);

    /**
     * Отмена выбора
     */
    void DeselectAll();

    /**
     * Получение выбранного элемента
     */
    std::shared_ptr<UIElement> GetSelectedElement() const;

    /**
     * Перемещение элемента
     * @param elementId ID элемента
     * @param newPosition новая позиция
     */
    void MoveElement(const std::string& elementId, const glm::vec2& newPosition);

    /**
     * Изменение размера элемента
     * @param elementId ID элемента
     * @param newSize новый размер
     */
    void ResizeElement(const std::string& elementId, const glm::vec2& newSize);

    /**
     * Установка режима редактора
     * @param mode режим
     */
    void SetMode(UIBuilderMode mode) { m_mode = mode; }

    /**
     * Получение режима редактора
     */
    UIBuilderMode GetMode() const { return m_mode; }

    /**
     * Переключение режима
     */
    void ToggleMode();

    /**
     * Очистка редактора
     */
    void Clear();

    /**
     * Сохранение UI
     * @param filename имя файла
     * @return true если сохранение успешно
     */
    bool Save(const std::string& filename);

    /**
     * Загрузка UI
     * @param filename имя файла
     * @return true если загрузка успешна
     */
    bool Load(const std::string& filename);

    /**
     * Генерация кода UI
     * @return код UI
     */
    std::string GenerateCode() const;

    /**
     * Валидация UI
     * @return true если UI валиден
     */
    bool Validate();

    /**
     * Получение ошибок валидации
     */
    const std::vector<std::string>& GetValidationErrors() const { return m_validationErrors; }

    /**
     * Установка callback для обновления UI
     * @param callback функция обратного вызова
     */
    void SetUpdateCallback(std::function<void()> callback) { m_updateCallback = callback; }

    /**
     * Получение состояния редактора
     */
    bool IsInitialized() const { return m_initialized; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    std::vector<std::shared_ptr<UIElement>> m_elements;
    std::map<std::string, std::shared_ptr<UIElement>> m_elementMap;
    
    std::string m_selectedElementId;
    UIBuilderMode m_mode;
    bool m_initialized;
    bool m_visible;
    
    std::vector<std::string> m_validationErrors;
    
    std::function<void()> m_updateCallback;
    
    // Вспомогательные методы
    void RenderElements(void* renderer);
    void RenderGrid(void* renderer);
    void RenderUI(void* renderer);
    void RenderToolbox(void* renderer);
    void RenderProperties(void* renderer);
    
    bool HandleElementInput(std::shared_ptr<UIElement> element, float mouseX, float mouseY, int mouseButton);
    void HandleDragAndDrop(float mouseX, float mouseY);
    
    void UpdateLayout();
    void ValidateElements();
    
    std::string GenerateElementId() const;
    void AddValidationError(const std::string& error);
    void ClearValidationErrors();
    
    // Генерация кода
    std::string GenerateElementCode(std::shared_ptr<UIElement> element, int indent = 0) const;
    std::string GenerateStyleCode(const UIStyle& style) const;
    std::string GenerateLayoutCode(UIAlignment alignment) const;
};

} // namespace FastEngine

