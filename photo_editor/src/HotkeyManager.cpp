#include "HotkeyManager.h"
#include <QApplication>
#include <QDebug>

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject(parent)
{
    registerToolHotkeys();
    registerFileHotkeys();
    registerViewHotkeys();
    registerEditHotkeys();
}

HotkeyManager::~HotkeyManager()
{
    qDeleteAll(m_shortcuts);
}

void HotkeyManager::registerHotkey(const QString &action, const QKeySequence &keySequence, QAction *actionObject)
{
    if (m_shortcuts.contains(action)) {
        delete m_shortcuts[action];
    }
    
    QShortcut *shortcut = new QShortcut(keySequence, this);
    connect(shortcut, &QShortcut::activated, this, &HotkeyManager::onShortcutActivated);
    
    m_shortcuts[action] = shortcut;
    
    if (actionObject) {
        m_actions[action] = actionObject;
    }
}

void HotkeyManager::unregisterHotkey(const QString &action)
{
    if (m_shortcuts.contains(action)) {
        delete m_shortcuts[action];
        m_shortcuts.remove(action);
    }
    
    if (m_actions.contains(action)) {
        m_actions.remove(action);
    }
}

void HotkeyManager::registerToolHotkeys()
{
    // Tool hotkeys matching Photoshop
    registerHotkey("move_tool", QKeySequence("V"));
    registerHotkey("marquee_tool", QKeySequence("M"));
    registerHotkey("lasso_tool", QKeySequence("L"));
    registerHotkey("magic_wand_tool", QKeySequence("W"));
    registerHotkey("brush_tool", QKeySequence("B"));
    registerHotkey("pencil_tool", QKeySequence("B")); // Same as brush, will cycle
    registerHotkey("clone_stamp_tool", QKeySequence("S"));
    registerHotkey("healing_brush_tool", QKeySequence("J"));
    registerHotkey("eraser_tool", QKeySequence("E"));
    registerHotkey("gradient_tool", QKeySequence("G"));
    registerHotkey("paint_bucket_tool", QKeySequence("G")); // Same as gradient, will cycle
    registerHotkey("crop_tool", QKeySequence("C"));
    registerHotkey("eyedropper_tool", QKeySequence("I"));
    registerHotkey("hand_tool", QKeySequence("H"));
    registerHotkey("zoom_tool", QKeySequence("Z"));
    registerHotkey("text_tool", QKeySequence("T"));
    registerHotkey("pen_tool", QKeySequence("P"));
    registerHotkey("shape_tool", QKeySequence("U"));
    
    // Tool hotkey mapping for direct tool switching
    m_toolHotkeys[QKeySequence("V")] = 0;  // Move Tool
    m_toolHotkeys[QKeySequence("M")] = 1;  // Marquee Tool
    m_toolHotkeys[QKeySequence("L")] = 2;  // Lasso Tool
    m_toolHotkeys[QKeySequence("W")] = 3;  // Magic Wand Tool
    m_toolHotkeys[QKeySequence("B")] = 4;  // Brush Tool
    m_toolHotkeys[QKeySequence("S")] = 5;  // Clone Stamp Tool
    m_toolHotkeys[QKeySequence("J")] = 6;  // Healing Brush Tool
    m_toolHotkeys[QKeySequence("E")] = 7;  // Eraser Tool
    m_toolHotkeys[QKeySequence("G")] = 8;  // Gradient Tool
    m_toolHotkeys[QKeySequence("C")] = 9;  // Crop Tool
    m_toolHotkeys[QKeySequence("I")] = 10; // Eyedropper Tool
    m_toolHotkeys[QKeySequence("H")] = 11; // Hand Tool
    m_toolHotkeys[QKeySequence("Z")] = 12; // Zoom Tool
    m_toolHotkeys[QKeySequence("T")] = 13; // Text Tool
    m_toolHotkeys[QKeySequence("P")] = 14; // Pen Tool
    m_toolHotkeys[QKeySequence("U")] = 15; // Shape Tool
}

void HotkeyManager::registerFileHotkeys()
{
    // File operations
    registerHotkey("new_file", QKeySequence("Ctrl+N"));
    registerHotkey("open_file", QKeySequence("Ctrl+O"));
    registerHotkey("save_file", QKeySequence("Ctrl+S"));
    registerHotkey("save_as_file", QKeySequence("Ctrl+Shift+S"));
    registerHotkey("export_file", QKeySequence("Ctrl+E"));
    registerHotkey("close_file", QKeySequence("Ctrl+W"));
    registerHotkey("quit_app", QKeySequence("Ctrl+Q"));
}

void HotkeyManager::registerViewHotkeys()
{
    // View operations
    registerHotkey("zoom_in", QKeySequence("Ctrl++"));
    registerHotkey("zoom_out", QKeySequence("Ctrl+-"));
    registerHotkey("zoom_fit", QKeySequence("Ctrl+0"));
    registerHotkey("zoom_actual", QKeySequence("Ctrl+1"));
    registerHotkey("toggle_grid", QKeySequence("Ctrl+'"));
    registerHotkey("toggle_rulers", QKeySequence("Ctrl+R"));
}

void HotkeyManager::registerEditHotkeys()
{
    // Edit operations
    registerHotkey("undo", QKeySequence("Ctrl+Z"));
    registerHotkey("redo", QKeySequence("Ctrl+Y"));
    registerHotkey("cut", QKeySequence("Ctrl+X"));
    registerHotkey("copy", QKeySequence("Ctrl+C"));
    registerHotkey("paste", QKeySequence("Ctrl+V"));
    registerHotkey("select_all", QKeySequence("Ctrl+A"));
    registerHotkey("deselect_all", QKeySequence("Ctrl+D"));
    registerHotkey("invert_selection", QKeySequence("Ctrl+Shift+I"));
}

void HotkeyManager::onShortcutActivated()
{
    QShortcut *shortcut = qobject_cast<QShortcut*>(sender());
    if (!shortcut) return;
    
    QString action = m_shortcuts.key(shortcut);
    
    // Check if it's a tool hotkey
    if (m_toolHotkeys.contains(shortcut->key())) {
        int toolType = m_toolHotkeys[shortcut->key()];
        emit toolHotkeyTriggered(toolType);
        return;
    }
    
    // Trigger action if available
    if (m_actions.contains(action)) {
        m_actions[action]->trigger();
    }
    
    emit hotkeyTriggered(action);
}
