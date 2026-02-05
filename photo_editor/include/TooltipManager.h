#ifndef TOOLTIPMANAGER_H
#define TOOLTIPMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QWidget>
#include <QToolTip>

class TooltipManager : public QObject
{
    Q_OBJECT

public:
    explicit TooltipManager(QObject *parent = nullptr);
    ~TooltipManager();

    // Register tooltips for widgets
    void registerTooltip(QWidget *widget, const QString &tooltip);
    void registerTooltipWithHotkey(QWidget *widget, const QString &tooltip, const QString &hotkey);
    
    // Tool-specific tooltips
    void registerToolTooltips();
    void registerMenuTooltips();
    void registerButtonTooltips();
    
    // Show tooltip with hotkey
    void showTooltip(QWidget *widget, const QString &text, const QString &hotkey = "");
    
    // Get tooltip text for tool
    QString getToolTooltip(int toolType) const;
    QString getToolHotkey(int toolType) const;

private:
    QMap<QWidget*, QString> m_tooltips;
    QMap<QWidget*, QString> m_hotkeys;
    
    // Tool tooltip mapping
    QMap<int, QString> m_toolTooltips;
    QMap<int, QString> m_toolHotkeys;
    
    void setupToolMappings();
};

#endif // TOOLTIPMANAGER_H
