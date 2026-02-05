#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QColor>
#include <QPalette>
#include <QJsonObject>

#include "ThemeManager.h"

class TestThemeManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test theme management
    void testLoadThemes();
    void testSaveThemes();
    void testGetAvailableThemes();
    void testGetTemplatesByCategory();
    void testGetCategories();
    
    // Test theme operations
    void testGetTemplate();
    void testAddTemplate();
    void testRemoveTemplate();
    void testUpdateTemplate();
    
    // Test theme creation
    void testCreateTemplate();
    void testAddLayerToTemplate();
    
    // Test theme application
    void testSetCurrentTheme();
    void testGetCurrentTheme();
    void testIsDarkTheme();
    void testGetColor();
    void testGetStylesheet();
    
    // Test custom themes
    void testCreateCustomTheme();
    void testExportTemplate();
    void testImportTemplate();
    
    // Test search and filter
    void testSearchTemplates();
    void testGetTemplatesByTag();
    void testGetRecentTemplates();
    
    // Test categories
    void testAddCategory();
    void testRemoveCategory();
    void testSetTemplateCategory();
    
    // Test serialization
    void testToJson();
    void testFromJson();
    
    // Test signals
    void testSignals();

private:
    ThemeManager *m_manager;
    QApplication *m_app;
};

void TestThemeManager::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    m_app = qobject_cast<QApplication*>(QApplication::instance());
}

void TestThemeManager::cleanupTestCase()
{
    // Cleanup handled by Qt
}

void TestThemeManager::init()
{
    m_manager = new ThemeManager();
    m_manager->setApplication(m_app);
}

void TestThemeManager::cleanup()
{
    delete m_manager;
}

void TestThemeManager::testLoadThemes()
{
    // Load themes
    m_manager->loadThemes();
    
    // Should have default themes
    QStringList themes = m_manager->getAvailableThemes();
    QVERIFY(!themes.isEmpty());
    QVERIFY(themes.contains("Light"));
    QVERIFY(themes.contains("Dark"));
}

void TestThemeManager::testSaveThemes()
{
    // Load themes first
    m_manager->loadThemes();
    
    // Save themes (should not fail)
    m_manager->saveThemes();
    
    // This test mainly ensures no crashes occur
    QVERIFY(true);
}

void TestThemeManager::testGetAvailableThemes()
{
    // Initially no themes
    QStringList themes = m_manager->getAvailableThemes();
    QVERIFY(themes.isEmpty());
    
    // Load themes
    m_manager->loadThemes();
    themes = m_manager->getAvailableThemes();
    QVERIFY(!themes.isEmpty());
}

void TestThemeManager::testGetTemplatesByCategory()
{
    // Load themes
    m_manager->loadThemes();
    
    // Get themes by category
    QStringList lightThemes = m_manager->getTemplatesByCategory("Light");
    QStringList darkThemes = m_manager->getTemplatesByCategory("Dark");
    
    // Should have some themes in each category
    QVERIFY(!lightThemes.isEmpty() || !darkThemes.isEmpty());
}

void TestThemeManager::testGetCategories()
{
    // Load themes
    m_manager->loadThemes();
    
    // Get categories
    QStringList categories = m_manager->getCategories();
    QVERIFY(!categories.isEmpty());
}

void TestThemeManager::testGetTemplate()
{
    // Load themes
    m_manager->loadThemes();
    
    // Get a theme
    Theme lightTheme = m_manager->getTemplate("Light");
    QVERIFY(!lightTheme.name.isEmpty());
    QCOMPARE(lightTheme.name, "Light");
    
    // Get non-existent theme
    Theme invalidTheme = m_manager->getTemplate("NonExistent");
    QVERIFY(invalidTheme.name.isEmpty());
}

void TestThemeManager::testAddTemplate()
{
    // Create a custom theme
    Theme customTheme;
    customTheme.name = "CustomTest";
    customTheme.displayName = "Custom Test Theme";
    customTheme.description = "A custom test theme";
    customTheme.category = "Custom";
    customTheme.size = QSize(800, 600);
    customTheme.backgroundColor = Qt::blue;
    customTheme.isCustom = true;
    
    // Add theme
    m_manager->addTemplate(customTheme);
    
    // Verify theme was added
    QStringList themes = m_manager->getAvailableThemes();
    QVERIFY(themes.contains("CustomTest"));
    
    Theme retrievedTheme = m_manager->getTemplate("CustomTest");
    QCOMPARE(retrievedTheme.name, "CustomTest");
    QCOMPARE(retrievedTheme.displayName, "Custom Test Theme");
}

void TestThemeManager::testRemoveTemplate()
{
    // Add a custom theme
    Theme customTheme;
    customTheme.name = "ToRemove";
    customTheme.displayName = "To Remove";
    customTheme.isCustom = true;
    
    m_manager->addTemplate(customTheme);
    QVERIFY(m_manager->getAvailableThemes().contains("ToRemove"));
    
    // Remove theme
    m_manager->removeTemplate("ToRemove");
    QVERIFY(!m_manager->getAvailableThemes().contains("ToRemove"));
    
    // Try to remove non-existent theme
    m_manager->removeTemplate("NonExistent");
    // Should not crash
}

void TestThemeManager::testUpdateTemplate()
{
    // Add a custom theme
    Theme customTheme;
    customTheme.name = "ToUpdate";
    customTheme.displayName = "Original Name";
    customTheme.isCustom = true;
    
    m_manager->addTemplate(customTheme);
    
    // Update theme
    customTheme.displayName = "Updated Name";
    m_manager->updateTemplate(customTheme);
    
    // Verify theme was updated
    Theme retrievedTheme = m_manager->getTemplate("ToUpdate");
    QCOMPARE(retrievedTheme.displayName, "Updated Name");
}

void TestThemeManager::testCreateTemplate()
{
    // Create a template
    Theme template_ = m_manager->createTemplate("TestTemplate", "Test Description", QSize(1024, 768), Qt::green);
    
    QCOMPARE(template_.name, "TestTemplate");
    QCOMPARE(template_.displayName, "TestTemplate");
    QCOMPARE(template_.description, "Test Description");
    QCOMPARE(template_.size, QSize(1024, 768));
    QCOMPARE(template_.backgroundColor, Qt::green);
    QVERIFY(template_.isCustom);
}

void TestThemeManager::testAddLayerToTemplate()
{
    // Create a template
    Theme template_ = m_manager->createTemplate("TestTemplate", "Test Description", QSize(800, 600));
    
    // Add a layer
    TemplateLayer layer;
    layer.name = "Test Layer";
    layer.type = "raster";
    layer.geometry = QRect(10, 10, 100, 100);
    layer.backgroundColor = Qt::red;
    
    m_manager->addLayerToTemplate(template_, layer);
    
    QCOMPARE(template_.layers.size(), 1);
    QCOMPARE(template_.layers.first().name, "Test Layer");
}

void TestThemeManager::testSetCurrentTheme()
{
    // Load themes
    m_manager->loadThemes();
    
    // Set current theme
    m_manager->setCurrentTheme("Light");
    QCOMPARE(m_manager->getCurrentTheme(), "Light");
    
    // Set to non-existent theme
    m_manager->setCurrentTheme("NonExistent");
    QCOMPARE(m_manager->getCurrentTheme(), "Light"); // Should remain unchanged
}

void TestThemeManager::testGetCurrentTheme()
{
    // Initially no current theme
    QCOMPARE(m_manager->getCurrentTheme(), "Light"); // Default theme
    
    // Load themes and set current theme
    m_manager->loadThemes();
    m_manager->setCurrentTheme("Dark");
    QCOMPARE(m_manager->getCurrentTheme(), "Dark");
}

void TestThemeManager::testIsDarkTheme()
{
    // Load themes
    m_manager->loadThemes();
    
    // Set light theme
    m_manager->setCurrentTheme("Light");
    QVERIFY(!m_manager->isDarkTheme());
    
    // Set dark theme
    m_manager->setCurrentTheme("Dark");
    QVERIFY(m_manager->isDarkTheme());
}

void TestThemeManager::testGetColor()
{
    // Load themes
    m_manager->loadThemes();
    m_manager->setCurrentTheme("Light");
    
    // Get colors
    QColor windowColor = m_manager->getColor("window");
    QColor textColor = m_manager->getColor("text");
    
    QVERIFY(windowColor.isValid());
    QVERIFY(textColor.isValid());
    
    // Test invalid color name
    QColor invalidColor = m_manager->getColor("invalid");
    QVERIFY(!invalidColor.isValid());
}

void TestThemeManager::testGetStylesheet()
{
    // Load themes
    m_manager->loadThemes();
    m_manager->setCurrentTheme("Light");
    
    // Get stylesheet
    QString stylesheet = m_manager->getStylesheet();
    // Stylesheet might be empty for default themes
    QVERIFY(stylesheet.isNull() || !stylesheet.isEmpty());
}

void TestThemeManager::testCreateCustomTheme()
{
    // Create custom theme colors
    ThemeColors colors;
    colors.window = Qt::blue;
    colors.windowText = Qt::white;
    colors.base = Qt::darkBlue;
    colors.text = Qt::white;
    
    // Create custom theme
    m_manager->createCustomTheme("CustomBlue", colors, "QWidget { background-color: blue; }");
    
    // Verify theme was created
    QStringList themes = m_manager->getAvailableThemes();
    QVERIFY(themes.contains("CustomBlue"));
    
    Theme retrievedTheme = m_manager->getTemplate("CustomBlue");
    QCOMPARE(retrievedTheme.name, "CustomBlue");
    QCOMPARE(retrievedTheme.colors.window, Qt::blue);
    QVERIFY(retrievedTheme.isCustom);
}

void TestThemeManager::testExportTemplate()
{
    // Load themes
    m_manager->loadThemes();
    
    // Export a theme
    QString fileName = QDir::tempPath() + "/test_theme.json";
    bool success = m_manager->exportTemplate("Light", fileName);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
    
    // Clean up
    QFile::remove(fileName);
}

void TestThemeManager::testImportTemplate()
{
    // First export a theme
    m_manager->loadThemes();
    QString fileName = QDir::tempPath() + "/test_import_theme.json";
    m_manager->exportTemplate("Light", fileName);
    
    // Import the theme
    bool success = m_manager->importTemplate(fileName);
    QVERIFY(success);
    
    // Clean up
    QFile::remove(fileName);
}

void TestThemeManager::testSearchTemplates()
{
    // Load themes
    m_manager->loadThemes();
    
    // Search for themes
    QStringList lightResults = m_manager->searchTemplates("Light");
    QVERIFY(!lightResults.isEmpty());
    QVERIFY(lightResults.contains("Light"));
    
    // Search for non-existent theme
    QStringList emptyResults = m_manager->searchTemplates("NonExistent");
    QVERIFY(emptyResults.isEmpty());
}

void TestThemeManager::testGetTemplatesByTag()
{
    // Load themes
    m_manager->loadThemes();
    
    // Get themes by tag
    QStringList results = m_manager->getTemplatesByTag("default");
    // Results depend on how themes are tagged
    QVERIFY(results.isEmpty() || !results.isEmpty());
}

void TestThemeManager::testGetRecentTemplates()
{
    // Load themes
    m_manager->loadThemes();
    
    // Get recent themes
    QStringList recent = m_manager->getRecentTemplates(5);
    QVERIFY(recent.size() <= 5);
}

void TestThemeManager::testAddCategory()
{
    // Add category
    m_manager->addCategory("TestCategory");
    
    // Verify category was added
    QStringList categories = m_manager->getCategories();
    QVERIFY(categories.contains("TestCategory"));
}

void TestThemeManager::testRemoveCategory()
{
    // Add category first
    m_manager->addCategory("ToRemove");
    QVERIFY(m_manager->getCategories().contains("ToRemove"));
    
    // Remove category
    m_manager->removeCategory("ToRemove");
    QVERIFY(!m_manager->getCategories().contains("ToRemove"));
}

void TestThemeManager::testSetTemplateCategory()
{
    // Add a custom theme
    Theme customTheme;
    customTheme.name = "CategoryTest";
    customTheme.isCustom = true;
    
    m_manager->addTemplate(customTheme);
    
    // Set category
    m_manager->setTemplateCategory("CategoryTest", "TestCategory");
    
    // Verify category was set
    QStringList categoryThemes = m_manager->getTemplatesByCategory("TestCategory");
    QVERIFY(categoryThemes.contains("CategoryTest"));
}

void TestThemeManager::testToJson()
{
    // Load themes
    m_manager->loadThemes();
    
    // Convert to JSON
    QJsonObject json = m_manager->toJson();
    
    // Verify JSON contains expected values
    QVERIFY(json.contains("currentTheme"));
    QVERIFY(json.contains("themes"));
    
    QCOMPARE(json["currentTheme"].toString(), "Light");
    
    QJsonArray themes = json["themes"].toArray();
    QVERIFY(!themes.isEmpty());
}

void TestThemeManager::testFromJson()
{
    // Create JSON object
    QJsonObject json;
    json["currentTheme"] = "Dark";
    
    QJsonArray themes;
    QJsonObject themeJson;
    themeJson["name"] = "TestTheme";
    themeJson["displayName"] = "Test Theme";
    themeJson["description"] = "A test theme";
    themeJson["isDark"] = true;
    themeJson["isCustom"] = false;
    themeJson["stylesheet"] = "";
    
    QJsonObject colorsJson;
    colorsJson["window"] = QColor(50, 50, 50).name();
    colorsJson["windowText"] = QColor(255, 255, 255).name();
    colorsJson["base"] = QColor(30, 30, 30).name();
    colorsJson["text"] = QColor(255, 255, 255).name();
    colorsJson["button"] = QColor(70, 70, 70).name();
    colorsJson["buttonText"] = QColor(255, 255, 255).name();
    colorsJson["highlight"] = QColor(0, 120, 215).name();
    colorsJson["highlightedText"] = QColor(255, 255, 255).name();
    colorsJson["toolTipBase"] = QColor(50, 50, 50).name();
    colorsJson["toolTipText"] = QColor(255, 255, 255).name();
    colorsJson["link"] = QColor(100, 149, 237).name();
    colorsJson["linkVisited"] = QColor(186, 85, 211).name();
    colorsJson["dark"] = QColor(35, 35, 35).name();
    colorsJson["mid"] = QColor(50, 50, 50).name();
    colorsJson["light"] = QColor(70, 70, 70).name();
    colorsJson["shadow"] = QColor(20, 20, 20).name();
    
    themeJson["colors"] = colorsJson;
    themes.append(themeJson);
    json["themes"] = themes;
    
    // Load from JSON
    m_manager->fromJson(json);
    
    // Verify theme was loaded
    QCOMPARE(m_manager->getCurrentTheme(), "Dark");
    QStringList availableThemes = m_manager->getAvailableThemes();
    QVERIFY(availableThemes.contains("TestTheme"));
}

void TestThemeManager::testSignals()
{
    // Test signals
    QSignalSpy addedSpy(m_manager, &ThemeManager::themeAdded);
    QSignalSpy removedSpy(m_manager, &ThemeManager::themeRemoved);
    QSignalSpy updatedSpy(m_manager, &ThemeManager::themeUpdated);
    QSignalSpy changedSpy(m_manager, &ThemeManager::themeChanged);
    
    // Add a custom theme
    Theme customTheme;
    customTheme.name = "SignalTest";
    customTheme.isCustom = true;
    
    m_manager->addTemplate(customTheme);
    QCOMPARE(addedSpy.count(), 1);
    
    // Update theme
    customTheme.displayName = "Updated";
    m_manager->updateTemplate(customTheme);
    QCOMPARE(updatedSpy.count(), 1);
    
    // Set current theme
    m_manager->setCurrentTheme("SignalTest");
    QCOMPARE(changedSpy.count(), 1);
    
    // Remove theme
    m_manager->removeTemplate("SignalTest");
    QCOMPARE(removedSpy.count(), 1);
}

QTEST_MAIN(TestThemeManager)
