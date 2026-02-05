#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QPalette>
#include <QJsonObject>
#include <QHash>
#include <QList>

class QApplication;
class QWidget;

struct ThemeColors {
    QColor window;
    QColor windowText;
    QColor base;
    QColor alternateBase;
    QColor text;
    QColor button;
    QColor buttonText;
    QColor highlight;
    QColor highlightedText;
    QColor toolTipBase;
    QColor toolTipText;
    QColor link;
    QColor linkVisited;
    QColor dark;
    QColor mid;
    QColor light;
    QColor shadow;
    
    ThemeColors();
    ThemeColors(const QPalette &palette);
    QPalette toPalette() const;
};

struct Theme {
    QString name;
    QString displayName;
    QString description;
    ThemeColors colors;
    QString stylesheet;
    bool isDark;
    bool isCustom;
    
    Theme() : isDark(false), isCustom(false) {}
};

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    // Theme management
    void setApplication(QApplication *app);
    void loadThemes();
    void saveThemes();
    
    QStringList getAvailableThemes() const;
    QString getCurrentTheme() const { return m_currentTheme; }
    void setCurrentTheme(const QString &themeName);
    
    Theme getTheme(const QString &themeName) const;
    void addTheme(const Theme &theme);
    void removeTheme(const QString &themeName);
    void updateTheme(const Theme &theme);
    
    // Custom themes
    void createCustomTheme(const QString &name, const ThemeColors &colors, const QString &stylesheet = QString());
    void exportTheme(const QString &themeName, const QString &filePath);
    void importTheme(const QString &filePath);
    
    // Theme properties
    bool isDarkTheme() const;
    QColor getColor(const QString &colorName) const;
    QString getStylesheet() const;
    
    // System integration
    void applySystemTheme();
    void detectSystemTheme();
    
    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void themeChanged(const QString &themeName);
    void themeAdded(const QString &themeName);
    void themeRemoved(const QString &themeName);
    void themeUpdated(const QString &themeName);

private:
    void setupDefaultThemes();
    void applyTheme(const Theme &theme);
    QString getThemeFilePath() const;
    void loadThemeFromFile(const QString &filePath);
    void saveThemeToFile(const Theme &theme, const QString &filePath);
    
    QApplication *m_app;
    QHash<QString, Theme> m_themes;
    QString m_currentTheme;
    QString m_themesDirectory;
};

// Predefined themes
class DefaultThemes
{
public:
    static Theme createLightTheme();
    static Theme createDarkTheme();
    static Theme createHighContrastTheme();
    static Theme createBlueTheme();
    static Theme createGreenTheme();
    static Theme createPurpleTheme();
    static Theme createOrangeTheme();
    static Theme createRedTheme();
    static Theme createGrayTheme();
    static Theme createMonochromeTheme();
};

#endif // THEMEMANAGER_H
