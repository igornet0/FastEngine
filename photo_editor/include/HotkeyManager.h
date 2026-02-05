#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <QObject>
#include <QShortcut>
#include <QKeySequence>
#include <QMap>
#include <QAction>

class HotkeyManager : public QObject
{
    Q_OBJECT

public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager();

    // Register hotkeys
    void registerHotkey(const QString &action, const QKeySequence &keySequence, QAction *actionObject = nullptr);
    void unregisterHotkey(const QString &action);
    
    // Tool hotkeys
    void registerToolHotkeys();
    
    // File operation hotkeys
    void registerFileHotkeys();
    
    // View operation hotkeys
    void registerViewHotkeys();
    
    // Edit operation hotkeys
    void registerEditHotkeys();

signals:
    void hotkeyTriggered(const QString &action);
    void toolHotkeyTriggered(int toolType);

private slots:
    void onShortcutActivated();

private:
    QMap<QString, QShortcut*> m_shortcuts;
    QMap<QString, QAction*> m_actions;
    
    // Tool hotkey mapping
    QMap<QKeySequence, int> m_toolHotkeys;
};

#endif // HOTKEYMANAGER_H
