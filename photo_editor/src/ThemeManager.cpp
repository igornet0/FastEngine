#include "ThemeManager.h"
#include <QApplication>
#include <QPalette>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDebug>
#include <QStyleFactory>

ThemeColors::ThemeColors()
    : window(Qt::white)
    , windowText(Qt::black)
    , base(Qt::white)
    , alternateBase(QColor(240, 240, 240))
    , text(Qt::black)
    , button(QColor(240, 240, 240))
    , buttonText(Qt::black)
    , highlight(QColor(0, 120, 215))
    , highlightedText(Qt::white)
    , toolTipBase(Qt::white)
    , toolTipText(Qt::black)
    , link(QColor(0, 102, 204))
    , linkVisited(QColor(82, 24, 139))
    , dark(QColor(160, 160, 160))
    , mid(QColor(192, 192, 192))
    , light(QColor(255, 255, 255))
    , shadow(QColor(105, 105, 105))
{
}

ThemeColors::ThemeColors(const QPalette &palette)
    : window(palette.color(QPalette::Window))
    , windowText(palette.color(QPalette::WindowText))
    , base(palette.color(QPalette::Base))
    , alternateBase(palette.color(QPalette::AlternateBase))
    , text(palette.color(QPalette::Text))
    , button(palette.color(QPalette::Button))
    , buttonText(palette.color(QPalette::ButtonText))
    , highlight(palette.color(QPalette::Highlight))
    , highlightedText(palette.color(QPalette::HighlightedText))
    , toolTipBase(palette.color(QPalette::ToolTipBase))
    , toolTipText(palette.color(QPalette::ToolTipText))
    , link(palette.color(QPalette::Link))
    , linkVisited(palette.color(QPalette::LinkVisited))
    , dark(palette.color(QPalette::Dark))
    , mid(palette.color(QPalette::Mid))
    , light(palette.color(QPalette::Light))
    , shadow(palette.color(QPalette::Shadow))
{
}

QPalette ThemeColors::toPalette() const
{
    QPalette palette;
    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::WindowText, windowText);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, alternateBase);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::ButtonText, buttonText);
    palette.setColor(QPalette::Highlight, highlight);
    palette.setColor(QPalette::HighlightedText, highlightedText);
    palette.setColor(QPalette::ToolTipBase, toolTipBase);
    palette.setColor(QPalette::ToolTipText, toolTipText);
    palette.setColor(QPalette::Link, link);
    palette.setColor(QPalette::LinkVisited, linkVisited);
    palette.setColor(QPalette::Dark, dark);
    palette.setColor(QPalette::Mid, mid);
    palette.setColor(QPalette::Light, light);
    palette.setColor(QPalette::Shadow, shadow);
    return palette;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_app(nullptr)
    , m_currentTheme("Light")
    , m_themesDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/themes")
{
    setupDefaultThemes();
}

ThemeManager::~ThemeManager()
{
    saveThemes();
}

void ThemeManager::setApplication(QApplication *app)
{
    m_app = app;
}

void ThemeManager::loadThemes()
{
    // Load themes from directory
    QDir themesDir(m_themesDirectory);
    if (themesDir.exists()) {
        QStringList themeFiles = themesDir.entryList(QStringList() << "*.json", QDir::Files);
        for (const QString &file : themeFiles) {
            QString filePath = themesDir.absoluteFilePath(file);
            loadThemeFromFile(filePath);
        }
    }
    
    // Load default themes if none exist
    if (m_themes.isEmpty()) {
        setupDefaultThemes();
    }
}

void ThemeManager::saveThemes()
{
    // Create themes directory if it doesn't exist
    QDir themesDir(m_themesDirectory);
    if (!themesDir.exists()) {
        themesDir.mkpath(".");
    }
    
    // Save all themes
    for (auto it = m_themes.begin(); it != m_themes.end(); ++it) {
        if (it.value().isCustom) {
            QString filePath = themesDir.absoluteFilePath(it.key() + ".json");
            saveThemeToFile(it.value(), filePath);
        }
    }
}

QStringList ThemeManager::getAvailableThemes() const
{
    return m_themes.keys();
}

void ThemeManager::setCurrentTheme(const QString &themeName)
{
    if (m_themes.contains(themeName) && m_currentTheme != themeName) {
        m_currentTheme = themeName;
        applyTheme(m_themes[themeName]);
        emit themeChanged(themeName);
    }
}

Theme ThemeManager::getTheme(const QString &themeName) const
{
    if (m_themes.contains(themeName)) {
        return m_themes[themeName];
    }
    return Theme();
}

void ThemeManager::addTheme(const Theme &theme)
{
    m_themes[theme.name] = theme;
    emit themeAdded(theme.name);
}

void ThemeManager::removeTheme(const QString &themeName)
{
    if (m_themes.contains(themeName) && m_themes[themeName].isCustom) {
        m_themes.remove(themeName);
        emit themeRemoved(themeName);
        
        // If this was the current theme, switch to default
        if (m_currentTheme == themeName) {
            setCurrentTheme("Light");
        }
    }
}

void ThemeManager::updateTheme(const Theme &theme)
{
    if (m_themes.contains(theme.name)) {
        m_themes[theme.name] = theme;
        emit themeUpdated(theme.name);
        
        // If this is the current theme, reapply it
        if (m_currentTheme == theme.name) {
            applyTheme(theme);
        }
    }
}

void ThemeManager::createCustomTheme(const QString &name, const ThemeColors &colors, const QString &stylesheet)
{
    Theme theme;
    theme.name = name;
    theme.displayName = name;
    theme.description = "Custom theme";
    theme.colors = colors;
    theme.stylesheet = stylesheet;
    theme.isDark = colors.window.lightness() < 128;
    theme.isCustom = true;
    
    addTheme(theme);
}

void ThemeManager::exportTheme(const QString &themeName, const QString &filePath)
{
    if (m_themes.contains(themeName)) {
        saveThemeToFile(m_themes[themeName], filePath);
    }
}

void ThemeManager::importTheme(const QString &filePath)
{
    loadThemeFromFile(filePath);
}

bool ThemeManager::isDarkTheme() const
{
    if (m_themes.contains(m_currentTheme)) {
        return m_themes[m_currentTheme].isDark;
    }
    return false;
}

QColor ThemeManager::getColor(const QString &colorName) const
{
    if (m_themes.contains(m_currentTheme)) {
        const ThemeColors &colors = m_themes[m_currentTheme].colors;
        
        if (colorName == "window") return colors.window;
        if (colorName == "windowText") return colors.windowText;
        if (colorName == "base") return colors.base;
        if (colorName == "alternateBase") return colors.alternateBase;
        if (colorName == "text") return colors.text;
        if (colorName == "button") return colors.button;
        if (colorName == "buttonText") return colors.buttonText;
        if (colorName == "highlight") return colors.highlight;
        if (colorName == "highlightedText") return colors.highlightedText;
        if (colorName == "toolTipBase") return colors.toolTipBase;
        if (colorName == "toolTipText") return colors.toolTipText;
        if (colorName == "link") return colors.link;
        if (colorName == "linkVisited") return colors.linkVisited;
        if (colorName == "dark") return colors.dark;
        if (colorName == "mid") return colors.mid;
        if (colorName == "light") return colors.light;
        if (colorName == "shadow") return colors.shadow;
    }
    
    return QColor();
}

QString ThemeManager::getStylesheet() const
{
    if (m_themes.contains(m_currentTheme)) {
        return m_themes[m_currentTheme].stylesheet;
    }
    return QString();
}

void ThemeManager::applySystemTheme()
{
    // Detect system theme and apply appropriate theme
    detectSystemTheme();
}

void ThemeManager::detectSystemTheme()
{
    // Simple detection based on system palette
    QPalette systemPalette = QApplication::palette();
    bool isDark = systemPalette.color(QPalette::Window).lightness() < 128;
    
    QString themeName = isDark ? "Dark" : "Light";
    if (m_themes.contains(themeName)) {
        setCurrentTheme(themeName);
    }
}

QJsonObject ThemeManager::toJson() const
{
    QJsonObject json;
    json["currentTheme"] = m_currentTheme;
    
    QJsonArray themes;
    for (auto it = m_themes.begin(); it != m_themes.end(); ++it) {
        QJsonObject themeJson;
        const Theme &theme = it.value();
        
        themeJson["name"] = theme.name;
        themeJson["displayName"] = theme.displayName;
        themeJson["description"] = theme.description;
        themeJson["isDark"] = theme.isDark;
        themeJson["isCustom"] = theme.isCustom;
        themeJson["stylesheet"] = theme.stylesheet;
        
        // Colors
        QJsonObject colorsJson;
        colorsJson["window"] = theme.colors.window.name();
        colorsJson["windowText"] = theme.colors.windowText.name();
        colorsJson["base"] = theme.colors.base.name();
        colorsJson["alternateBase"] = theme.colors.alternateBase.name();
        colorsJson["text"] = theme.colors.text.name();
        colorsJson["button"] = theme.colors.button.name();
        colorsJson["buttonText"] = theme.colors.buttonText.name();
        colorsJson["highlight"] = theme.colors.highlight.name();
        colorsJson["highlightedText"] = theme.colors.highlightedText.name();
        colorsJson["toolTipBase"] = theme.colors.toolTipBase.name();
        colorsJson["toolTipText"] = theme.colors.toolTipText.name();
        colorsJson["link"] = theme.colors.link.name();
        colorsJson["linkVisited"] = theme.colors.linkVisited.name();
        colorsJson["dark"] = theme.colors.dark.name();
        colorsJson["mid"] = theme.colors.mid.name();
        colorsJson["light"] = theme.colors.light.name();
        colorsJson["shadow"] = theme.colors.shadow.name();
        
        themeJson["colors"] = colorsJson;
        themes.append(themeJson);
    }
    
    json["themes"] = themes;
    return json;
}

void ThemeManager::fromJson(const QJsonObject &json)
{
    m_currentTheme = json["currentTheme"].toString();
    
    QJsonArray themes = json["themes"].toArray();
    for (const QJsonValue &value : themes) {
        QJsonObject themeJson = value.toObject();
        
        Theme theme;
        theme.name = themeJson["name"].toString();
        theme.displayName = themeJson["displayName"].toString();
        theme.description = themeJson["description"].toString();
        theme.isDark = themeJson["isDark"].toBool();
        theme.isCustom = themeJson["isCustom"].toBool();
        theme.stylesheet = themeJson["stylesheet"].toString();
        
        // Colors
        QJsonObject colorsJson = themeJson["colors"].toObject();
        theme.colors.window = QColor(colorsJson["window"].toString());
        theme.colors.windowText = QColor(colorsJson["windowText"].toString());
        theme.colors.base = QColor(colorsJson["base"].toString());
        theme.colors.alternateBase = QColor(colorsJson["alternateBase"].toString());
        theme.colors.text = QColor(colorsJson["text"].toString());
        theme.colors.button = QColor(colorsJson["button"].toString());
        theme.colors.buttonText = QColor(colorsJson["buttonText"].toString());
        theme.colors.highlight = QColor(colorsJson["highlight"].toString());
        theme.colors.highlightedText = QColor(colorsJson["highlightedText"].toString());
        theme.colors.toolTipBase = QColor(colorsJson["toolTipBase"].toString());
        theme.colors.toolTipText = QColor(colorsJson["toolTipText"].toString());
        theme.colors.link = QColor(colorsJson["link"].toString());
        theme.colors.linkVisited = QColor(colorsJson["linkVisited"].toString());
        theme.colors.dark = QColor(colorsJson["dark"].toString());
        theme.colors.mid = QColor(colorsJson["mid"].toString());
        theme.colors.light = QColor(colorsJson["light"].toString());
        theme.colors.shadow = QColor(colorsJson["shadow"].toString());
        
        m_themes[theme.name] = theme;
    }
}

void ThemeManager::setupDefaultThemes()
{
    // Light theme
    addTheme(DefaultThemes::createLightTheme());
    
    // Dark theme
    addTheme(DefaultThemes::createDarkTheme());
    
    // High contrast theme
    addTheme(DefaultThemes::createHighContrastTheme());
    
    // Color themes
    addTheme(DefaultThemes::createBlueTheme());
    addTheme(DefaultThemes::createGreenTheme());
    addTheme(DefaultThemes::createPurpleTheme());
    addTheme(DefaultThemes::createOrangeTheme());
    addTheme(DefaultThemes::createRedTheme());
    addTheme(DefaultThemes::createGrayTheme());
    addTheme(DefaultThemes::createMonochromeTheme());
}

void ThemeManager::applyTheme(const Theme &theme)
{
    if (!m_app) {
        return;
    }
    
    // Apply color palette
    m_app->setPalette(theme.colors.toPalette());
    
    // Apply stylesheet
    if (!theme.stylesheet.isEmpty()) {
        m_app->setStyleSheet(theme.stylesheet);
    } else {
        m_app->setStyleSheet(QString());
    }
}

QString ThemeManager::getThemeFilePath() const
{
    return m_themesDirectory + "/themes.json";
}

void ThemeManager::loadThemeFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject themeJson = doc.object();
        
        Theme theme;
        theme.name = themeJson["name"].toString();
        theme.displayName = themeJson["displayName"].toString();
        theme.description = themeJson["description"].toString();
        theme.isDark = themeJson["isDark"].toBool();
        theme.isCustom = themeJson["isCustom"].toBool();
        theme.stylesheet = themeJson["stylesheet"].toString();
        
        // Colors
        QJsonObject colorsJson = themeJson["colors"].toObject();
        theme.colors.window = QColor(colorsJson["window"].toString());
        theme.colors.windowText = QColor(colorsJson["windowText"].toString());
        theme.colors.base = QColor(colorsJson["base"].toString());
        theme.colors.alternateBase = QColor(colorsJson["alternateBase"].toString());
        theme.colors.text = QColor(colorsJson["text"].toString());
        theme.colors.button = QColor(colorsJson["button"].toString());
        theme.colors.buttonText = QColor(colorsJson["buttonText"].toString());
        theme.colors.highlight = QColor(colorsJson["highlight"].toString());
        theme.colors.highlightedText = QColor(colorsJson["highlightedText"].toString());
        theme.colors.toolTipBase = QColor(colorsJson["toolTipBase"].toString());
        theme.colors.toolTipText = QColor(colorsJson["toolTipText"].toString());
        theme.colors.link = QColor(colorsJson["link"].toString());
        theme.colors.linkVisited = QColor(colorsJson["linkVisited"].toString());
        theme.colors.dark = QColor(colorsJson["dark"].toString());
        theme.colors.mid = QColor(colorsJson["mid"].toString());
        theme.colors.light = QColor(colorsJson["light"].toString());
        theme.colors.shadow = QColor(colorsJson["shadow"].toString());
        
        m_themes[theme.name] = theme;
    }
}

void ThemeManager::saveThemeToFile(const Theme &theme, const QString &filePath)
{
    QJsonObject themeJson;
    themeJson["name"] = theme.name;
    themeJson["displayName"] = theme.displayName;
    themeJson["description"] = theme.description;
    themeJson["isDark"] = theme.isDark;
    themeJson["isCustom"] = theme.isCustom;
    themeJson["stylesheet"] = theme.stylesheet;
    
    // Colors
    QJsonObject colorsJson;
    colorsJson["window"] = theme.colors.window.name();
    colorsJson["windowText"] = theme.colors.windowText.name();
    colorsJson["base"] = theme.colors.base.name();
    colorsJson["alternateBase"] = theme.colors.alternateBase.name();
    colorsJson["text"] = theme.colors.text.name();
    colorsJson["button"] = theme.colors.button.name();
    colorsJson["buttonText"] = theme.colors.buttonText.name();
    colorsJson["highlight"] = theme.colors.highlight.name();
    colorsJson["highlightedText"] = theme.colors.highlightedText.name();
    colorsJson["toolTipBase"] = theme.colors.toolTipBase.name();
    colorsJson["toolTipText"] = theme.colors.toolTipText.name();
    colorsJson["link"] = theme.colors.link.name();
    colorsJson["linkVisited"] = theme.colors.linkVisited.name();
    colorsJson["dark"] = theme.colors.dark.name();
    colorsJson["mid"] = theme.colors.mid.name();
    colorsJson["light"] = theme.colors.light.name();
    colorsJson["shadow"] = theme.colors.shadow.name();
    
    themeJson["colors"] = colorsJson;
    
    QJsonDocument doc(themeJson);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

// DefaultThemes implementation

Theme DefaultThemes::createLightTheme()
{
    Theme theme;
    theme.name = "Light";
    theme.displayName = "Light Theme";
    theme.description = "Default light theme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Light theme colors
    theme.colors.window = QColor(240, 240, 240);
    theme.colors.windowText = QColor(0, 0, 0);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(248, 248, 248);
    theme.colors.text = QColor(0, 0, 0);
    theme.colors.button = QColor(240, 240, 240);
    theme.colors.buttonText = QColor(0, 0, 0);
    theme.colors.highlight = QColor(0, 120, 215);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(255, 255, 225);
    theme.colors.toolTipText = QColor(0, 0, 0);
    theme.colors.link = QColor(0, 102, 204);
    theme.colors.linkVisited = QColor(82, 24, 139);
    theme.colors.dark = QColor(160, 160, 160);
    theme.colors.mid = QColor(192, 192, 192);
    theme.colors.light = QColor(255, 255, 255);
    theme.colors.shadow = QColor(105, 105, 105);
    
    return theme;
}

Theme DefaultThemes::createDarkTheme()
{
    Theme theme;
    theme.name = "Dark";
    theme.displayName = "Dark Theme";
    theme.description = "Dark theme for low-light environments";
    theme.isDark = true;
    theme.isCustom = false;
    
    // Dark theme colors
    theme.colors.window = QColor(53, 53, 53);
    theme.colors.windowText = QColor(255, 255, 255);
    theme.colors.base = QColor(42, 42, 42);
    theme.colors.alternateBase = QColor(66, 66, 66);
    theme.colors.text = QColor(255, 255, 255);
    theme.colors.button = QColor(53, 53, 53);
    theme.colors.buttonText = QColor(255, 255, 255);
    theme.colors.highlight = QColor(42, 130, 218);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(70, 70, 70);
    theme.colors.toolTipText = QColor(255, 255, 255);
    theme.colors.link = QColor(100, 149, 237);
    theme.colors.linkVisited = QColor(186, 85, 211);
    theme.colors.dark = QColor(35, 35, 35);
    theme.colors.mid = QColor(53, 53, 53);
    theme.colors.light = QColor(70, 70, 70);
    theme.colors.shadow = QColor(20, 20, 20);
    
    return theme;
}

Theme DefaultThemes::createHighContrastTheme()
{
    Theme theme;
    theme.name = "HighContrast";
    theme.displayName = "High Contrast";
    theme.description = "High contrast theme for accessibility";
    theme.isDark = true;
    theme.isCustom = false;
    
    // High contrast colors
    theme.colors.window = QColor(0, 0, 0);
    theme.colors.windowText = QColor(255, 255, 255);
    theme.colors.base = QColor(0, 0, 0);
    theme.colors.alternateBase = QColor(0, 0, 0);
    theme.colors.text = QColor(255, 255, 255);
    theme.colors.button = QColor(0, 0, 0);
    theme.colors.buttonText = QColor(255, 255, 255);
    theme.colors.highlight = QColor(255, 255, 0);
    theme.colors.highlightedText = QColor(0, 0, 0);
    theme.colors.toolTipBase = QColor(0, 0, 0);
    theme.colors.toolTipText = QColor(255, 255, 255);
    theme.colors.link = QColor(0, 255, 255);
    theme.colors.linkVisited = QColor(255, 0, 255);
    theme.colors.dark = QColor(0, 0, 0);
    theme.colors.mid = QColor(128, 128, 128);
    theme.colors.light = QColor(255, 255, 255);
    theme.colors.shadow = QColor(0, 0, 0);
    
    return theme;
}

Theme DefaultThemes::createBlueTheme()
{
    Theme theme;
    theme.name = "Blue";
    theme.displayName = "Blue Theme";
    theme.description = "Blue color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Blue theme colors
    theme.colors.window = QColor(240, 248, 255);
    theme.colors.windowText = QColor(0, 0, 139);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(248, 250, 255);
    theme.colors.text = QColor(0, 0, 139);
    theme.colors.button = QColor(173, 216, 230);
    theme.colors.buttonText = QColor(0, 0, 139);
    theme.colors.highlight = QColor(0, 100, 200);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(240, 248, 255);
    theme.colors.toolTipText = QColor(0, 0, 139);
    theme.colors.link = QColor(0, 100, 200);
    theme.colors.linkVisited = QColor(72, 61, 139);
    theme.colors.dark = QColor(100, 149, 237);
    theme.colors.mid = QColor(173, 216, 230);
    theme.colors.light = QColor(240, 248, 255);
    theme.colors.shadow = QColor(70, 130, 180);
    
    return theme;
}

Theme DefaultThemes::createGreenTheme()
{
    Theme theme;
    theme.name = "Green";
    theme.displayName = "Green Theme";
    theme.description = "Green color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Green theme colors
    theme.colors.window = QColor(240, 255, 240);
    theme.colors.windowText = QColor(0, 100, 0);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(248, 255, 248);
    theme.colors.text = QColor(0, 100, 0);
    theme.colors.button = QColor(144, 238, 144);
    theme.colors.buttonText = QColor(0, 100, 0);
    theme.colors.highlight = QColor(0, 150, 0);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(240, 255, 240);
    theme.colors.toolTipText = QColor(0, 100, 0);
    theme.colors.link = QColor(0, 150, 0);
    theme.colors.linkVisited = QColor(34, 139, 34);
    theme.colors.dark = QColor(107, 142, 35);
    theme.colors.mid = QColor(144, 238, 144);
    theme.colors.light = QColor(240, 255, 240);
    theme.colors.shadow = QColor(85, 107, 47);
    
    return theme;
}

Theme DefaultThemes::createPurpleTheme()
{
    Theme theme;
    theme.name = "Purple";
    theme.displayName = "Purple Theme";
    theme.description = "Purple color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Purple theme colors
    theme.colors.window = QColor(248, 240, 255);
    theme.colors.windowText = QColor(75, 0, 130);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(252, 248, 255);
    theme.colors.text = QColor(75, 0, 130);
    theme.colors.button = QColor(221, 160, 221);
    theme.colors.buttonText = QColor(75, 0, 130);
    theme.colors.highlight = QColor(138, 43, 226);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(248, 240, 255);
    theme.colors.toolTipText = QColor(75, 0, 130);
    theme.colors.link = QColor(138, 43, 226);
    theme.colors.linkVisited = QColor(72, 61, 139);
    theme.colors.dark = QColor(128, 0, 128);
    theme.colors.mid = QColor(221, 160, 221);
    theme.colors.light = QColor(248, 240, 255);
    theme.colors.shadow = QColor(75, 0, 130);
    
    return theme;
}

Theme DefaultThemes::createOrangeTheme()
{
    Theme theme;
    theme.name = "Orange";
    theme.displayName = "Orange Theme";
    theme.description = "Orange color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Orange theme colors
    theme.colors.window = QColor(255, 248, 240);
    theme.colors.windowText = QColor(139, 69, 19);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(255, 252, 248);
    theme.colors.text = QColor(139, 69, 19);
    theme.colors.button = QColor(255, 218, 185);
    theme.colors.buttonText = QColor(139, 69, 19);
    theme.colors.highlight = QColor(255, 140, 0);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(255, 248, 240);
    theme.colors.toolTipText = QColor(139, 69, 19);
    theme.colors.link = QColor(255, 140, 0);
    theme.colors.linkVisited = QColor(160, 82, 45);
    theme.colors.dark = QColor(205, 133, 63);
    theme.colors.mid = QColor(255, 218, 185);
    theme.colors.light = QColor(255, 248, 240);
    theme.colors.shadow = QColor(139, 69, 19);
    
    return theme;
}

Theme DefaultThemes::createRedTheme()
{
    Theme theme;
    theme.name = "Red";
    theme.displayName = "Red Theme";
    theme.description = "Red color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Red theme colors
    theme.colors.window = QColor(255, 240, 240);
    theme.colors.windowText = QColor(139, 0, 0);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(255, 248, 248);
    theme.colors.text = QColor(139, 0, 0);
    theme.colors.button = QColor(255, 182, 193);
    theme.colors.buttonText = QColor(139, 0, 0);
    theme.colors.highlight = QColor(220, 20, 60);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(255, 240, 240);
    theme.colors.toolTipText = QColor(139, 0, 0);
    theme.colors.link = QColor(220, 20, 60);
    theme.colors.linkVisited = QColor(128, 0, 0);
    theme.colors.dark = QColor(178, 34, 34);
    theme.colors.mid = QColor(255, 182, 193);
    theme.colors.light = QColor(255, 240, 240);
    theme.colors.shadow = QColor(139, 0, 0);
    
    return theme;
}

Theme DefaultThemes::createGrayTheme()
{
    Theme theme;
    theme.name = "Gray";
    theme.displayName = "Gray Theme";
    theme.description = "Gray color scheme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Gray theme colors
    theme.colors.window = QColor(240, 240, 240);
    theme.colors.windowText = QColor(64, 64, 64);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(248, 248, 248);
    theme.colors.text = QColor(64, 64, 64);
    theme.colors.button = QColor(200, 200, 200);
    theme.colors.buttonText = QColor(64, 64, 64);
    theme.colors.highlight = QColor(100, 100, 100);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(240, 240, 240);
    theme.colors.toolTipText = QColor(64, 64, 64);
    theme.colors.link = QColor(100, 100, 100);
    theme.colors.linkVisited = QColor(80, 80, 80);
    theme.colors.dark = QColor(128, 128, 128);
    theme.colors.mid = QColor(200, 200, 200);
    theme.colors.light = QColor(240, 240, 240);
    theme.colors.shadow = QColor(64, 64, 64);
    
    return theme;
}

Theme DefaultThemes::createMonochromeTheme()
{
    Theme theme;
    theme.name = "Monochrome";
    theme.displayName = "Monochrome Theme";
    theme.description = "Black and white theme";
    theme.isDark = false;
    theme.isCustom = false;
    
    // Monochrome theme colors
    theme.colors.window = QColor(255, 255, 255);
    theme.colors.windowText = QColor(0, 0, 0);
    theme.colors.base = QColor(255, 255, 255);
    theme.colors.alternateBase = QColor(248, 248, 248);
    theme.colors.text = QColor(0, 0, 0);
    theme.colors.button = QColor(240, 240, 240);
    theme.colors.buttonText = QColor(0, 0, 0);
    theme.colors.highlight = QColor(0, 0, 0);
    theme.colors.highlightedText = QColor(255, 255, 255);
    theme.colors.toolTipBase = QColor(255, 255, 255);
    theme.colors.toolTipText = QColor(0, 0, 0);
    theme.colors.link = QColor(0, 0, 0);
    theme.colors.linkVisited = QColor(128, 128, 128);
    theme.colors.dark = QColor(128, 128, 128);
    theme.colors.mid = QColor(192, 192, 192);
    theme.colors.light = QColor(255, 255, 255);
    theme.colors.shadow = QColor(0, 0, 0);
    
    return theme;
}







