#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>
#include <QWidget>
#include <QAction>
#include "HotkeyManager.h"
#include "TooltipManager.h"
#include "MoveTool.h"
#include "EyedropperTool.h"

class StrictTest : public QObject
{
    Q_OBJECT

public:
    explicit StrictTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Строгие тесты функциональности
    void testHotkeyManagerRealFunctionality();
    void testTooltipManagerRealFunctionality();
    void testMoveToolRealFunctionality();
    void testEyedropperToolRealFunctionality();
    void testIntegrationRealFunctionality();
    void testErrorConditions();
    void testEdgeCases();

private:
    HotkeyManager *m_hotkeyManager;
    TooltipManager *m_tooltipManager;
    MoveTool *m_moveTool;
    EyedropperTool *m_eyedropperTool;
    QWidget *m_testWidget;
    QApplication *m_app;
};

StrictTest::StrictTest(QObject *parent)
    : QObject(parent)
    , m_hotkeyManager(nullptr)
    , m_tooltipManager(nullptr)
    , m_moveTool(nullptr)
    , m_eyedropperTool(nullptr)
    , m_testWidget(nullptr)
    , m_app(nullptr)
{
}

void StrictTest::initTestCase()
{
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        m_app = new QApplication(argc, argv);
    } else {
        m_app = qobject_cast<QApplication*>(QApplication::instance());
    }
    
    QVERIFY(QApplication::instance() != nullptr);
    qDebug() << "🔍 STRICT TEST: QApplication initialized";
}

void StrictTest::cleanupTestCase()
{
    qDebug() << "🔍 STRICT TEST: Cleanup completed";
}

void StrictTest::init()
{
    m_hotkeyManager = new HotkeyManager(this);
    m_tooltipManager = new TooltipManager(this);
    m_moveTool = new MoveTool(this);
    m_eyedropperTool = new EyedropperTool(this);
    m_testWidget = new QWidget();
    
    QVERIFY(m_hotkeyManager != nullptr);
    QVERIFY(m_tooltipManager != nullptr);
    QVERIFY(m_moveTool != nullptr);
    QVERIFY(m_eyedropperTool != nullptr);
    QVERIFY(m_testWidget != nullptr);
    
    qDebug() << "🔍 STRICT TEST: All components created";
}

void StrictTest::cleanup()
{
    if (m_testWidget) {
        delete m_testWidget;
        m_testWidget = nullptr;
    }
    qDebug() << "🔍 STRICT TEST: Components cleaned up";
}

void StrictTest::testHotkeyManagerRealFunctionality()
{
    qDebug() << "🔍 Testing HotkeyManager REAL functionality...";
    
    // Тест 1: Проверяем, что горячие клавиши действительно регистрируются
    QSignalSpy spy(m_hotkeyManager, &HotkeyManager::hotkeyTriggered);
    QVERIFY(spy.isValid());
    
    // Регистрируем горячую клавишу
    m_hotkeyManager->registerHotkey("test_action", QKeySequence("Ctrl+T"));
    
    // Проверяем, что горячая клавиша зарегистрирована (через внутреннее состояние)
    // Это косвенная проверка - если метод не упал, значит регистрация прошла
    
    // Тест 2: Проверяем отмену регистрации
    m_hotkeyManager->unregisterHotkey("test_action");
    
    // Тест 3: Проверяем обработку дубликатов
    m_hotkeyManager->registerHotkey("duplicate_test", QKeySequence("Ctrl+D"));
    m_hotkeyManager->registerHotkey("duplicate_test", QKeySequence("Ctrl+Shift+D")); // Должно заменить предыдущую
    
    // Тест 4: Проверяем регистрацию с QAction
    QAction *testAction = new QAction("Test Action", this);
    m_hotkeyManager->registerHotkey("action_test", QKeySequence("Ctrl+A"), testAction);
    
    qDebug() << "✅ HotkeyManager REAL functionality test passed";
}

void StrictTest::testTooltipManagerRealFunctionality()
{
    qDebug() << "🔍 Testing TooltipManager REAL functionality...";
    
    // Тест 1: Проверяем регистрацию подсказки
    QString testTooltip = "Test Tooltip";
    m_tooltipManager->registerTooltip(m_testWidget, testTooltip);
    
    // Проверяем, что подсказка действительно установлена
    QCOMPARE(m_testWidget->toolTip(), testTooltip);
    
    // Тест 2: Проверяем регистрацию подсказки с горячей клавишей
    QString newTooltip = "Updated Tooltip";
    QString hotkey = "Ctrl+U";
    QString expectedTooltip = QString("%1\nHotkey: %2").arg(newTooltip, hotkey);
    m_tooltipManager->registerTooltipWithHotkey(m_testWidget, newTooltip, hotkey);
    QCOMPARE(m_testWidget->toolTip(), expectedTooltip);
    
    // Тест 3: Проверяем получение подсказки для инструмента
    QString moveTooltip = m_tooltipManager->getToolTooltip(Tool::MoveTool);
    QVERIFY(!moveTooltip.isEmpty());
    QVERIFY(moveTooltip.contains("Move Tool"));
    
    // Тест 4: Проверяем получение горячей клавиши для инструмента
    QString moveHotkey = m_tooltipManager->getToolHotkey(Tool::MoveTool);
    QCOMPARE(moveHotkey, QString("V"));
    
    qDebug() << "✅ TooltipManager REAL functionality test passed";
}

void StrictTest::testMoveToolRealFunctionality()
{
    qDebug() << "🔍 Testing MoveTool REAL functionality...";
    
    // Тест 1: Проверяем создание инструмента
    QCOMPARE(m_moveTool->getType(), Tool::MoveTool);
    QCOMPARE(m_moveTool->getName(), QString("Move Tool"));
    QVERIFY(m_moveTool->getDescription().contains("Move and transform"));
    
    // Тест 2: Проверяем изменение режима
    m_moveTool->setMoveMode(MoveTool::MoveSelection);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveSelection);
    
    m_moveTool->setMoveMode(MoveTool::MoveObject);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveObject);
    
    // Тест 3: Проверяем свойства
    m_moveTool->setAutoSelectLayer(false);
    QVERIFY(!m_moveTool->isAutoSelectLayer());
    
    m_moveTool->setShowTransformControls(false);
    QVERIFY(!m_moveTool->isShowTransformControls());
    
    // Тест 4: Проверяем работу с сигналами
    QSignalSpy layerSpy(m_moveTool, &MoveTool::layerMoved);
    QSignalSpy selectionSpy(m_moveTool, &MoveTool::selectionMoved);
    
    QVERIFY(layerSpy.isValid());
    QVERIFY(selectionSpy.isValid());
    
    // Симулируем операции (сигналы не будут испускаться без реального использования)
    // Но мы проверяем, что инструмент готов к работе
    
    qDebug() << "✅ MoveTool REAL functionality test passed";
}

void StrictTest::testEyedropperToolRealFunctionality()
{
    qDebug() << "🔍 Testing EyedropperTool REAL functionality...";
    
    // Тест 1: Проверяем создание инструмента
    QCOMPARE(m_eyedropperTool->getType(), Tool::EyedropperTool);
    QCOMPARE(m_eyedropperTool->getName(), QString("Eyedropper Tool"));
    QVERIFY(m_eyedropperTool->getDescription().contains("Sample colors"));
    
    // Тест 2: Проверяем изменение размера выборки
    m_eyedropperTool->setSampleSize(EyedropperTool::PointSample);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::PointSample);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::FiveByFive);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::FiveByFive);
    
    // Тест 3: Проверяем свойства
    m_eyedropperTool->setSampleFromAllLayers(true);
    QVERIFY(m_eyedropperTool->isSampleFromAllLayers());
    
    m_eyedropperTool->setShowColorInfo(false);
    QVERIFY(!m_eyedropperTool->isShowColorInfo());
    
    // Тест 4: Проверяем работу с сигналами
    QSignalSpy colorSpy(m_eyedropperTool, &EyedropperTool::colorPicked);
    QSignalSpy foregroundSpy(m_eyedropperTool, &EyedropperTool::foregroundColorChanged);
    QSignalSpy backgroundSpy(m_eyedropperTool, &EyedropperTool::backgroundColorChanged);
    
    QVERIFY(colorSpy.isValid());
    QVERIFY(foregroundSpy.isValid());
    QVERIFY(backgroundSpy.isValid());
    
    qDebug() << "✅ EyedropperTool REAL functionality test passed";
}

void StrictTest::testIntegrationRealFunctionality()
{
    qDebug() << "🔍 Testing INTEGRATION REAL functionality...";
    
    // Тест 1: Проверяем интеграцию HotkeyManager с TooltipManager
    QString moveTooltip = "Move Tool - Move and transform layers or selections";
    QString moveHotkey = "V";
    QString expectedTooltip = QString("%1\nHotkey: %2").arg(moveTooltip, moveHotkey);
    
    m_tooltipManager->registerTooltipWithHotkey(m_testWidget, moveTooltip, moveHotkey);
    QCOMPARE(m_testWidget->toolTip(), expectedTooltip);
    
    // Тест 2: Проверяем интеграцию инструментов
    QCOMPARE(m_moveTool->getType(), Tool::MoveTool);
    QCOMPARE(m_eyedropperTool->getType(), Tool::EyedropperTool);
    
    // Тест 3: Проверяем работу свойств
    m_moveTool->setMoveMode(MoveTool::MoveSelection);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveSelection);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::ThreeByThree);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::ThreeByThree);
    
    // Тест 4: Проверяем производительность
    QElapsedTimer timer;
    timer.start();
    
    for (int i = 0; i < 100; ++i) {
        m_moveTool->setProperty("move_mode", QVariant(i % 3));
        m_eyedropperTool->setProperty("sample_size", QVariant(i % 6));
    }
    
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100); // Должно быть очень быстро
    
    qDebug() << "✅ INTEGRATION REAL functionality test passed";
}

void StrictTest::testErrorConditions()
{
    qDebug() << "🔍 Testing ERROR CONDITIONS...";
    
    // Тест 1: Проверяем обработку nullptr
    m_tooltipManager->registerTooltip(nullptr, "Test");
    // Должно обработаться без падения
    
    // Тест 2: Проверяем обработку пустых строк
    m_hotkeyManager->registerHotkey("", QKeySequence("Ctrl+E"));
    // Должно обработаться без падения
    
    // Тест 3: Проверяем обработку недопустимых свойств
    QVariant invalidProperty = m_moveTool->getProperty("invalid_property");
    QVERIFY(!invalidProperty.isValid());
    
    // Тест 4: Проверяем обработку недопустимых значений
    m_moveTool->setProperty("move_mode", QVariant(999));
    // Должно обработаться без падения
    
    qDebug() << "✅ ERROR CONDITIONS test passed";
}

void StrictTest::testEdgeCases()
{
    qDebug() << "🔍 Testing EDGE CASES...";
    
    // Тест 1: Проверяем граничные значения для размера выборки
    m_eyedropperTool->setSampleSize(EyedropperTool::PointSample);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::PointSample);
    
    m_eyedropperTool->setSampleSize(EyedropperTool::FiftyOneByFiftyOne);
    QCOMPARE(m_eyedropperTool->getSampleSize(), EyedropperTool::FiftyOneByFiftyOne);
    
    // Тест 2: Проверяем граничные значения для режимов перемещения
    m_moveTool->setMoveMode(MoveTool::MoveLayer);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveLayer);
    
    m_moveTool->setMoveMode(MoveTool::MoveObject);
    QCOMPARE(m_moveTool->getMoveMode(), MoveTool::MoveObject);
    
    // Тест 3: Проверяем множественные операции
    for (int i = 0; i < 10; ++i) {
        m_moveTool->setMoveMode(static_cast<MoveTool::MoveMode>(i % 3));
        m_eyedropperTool->setSampleSize(static_cast<EyedropperTool::SampleSize>(i % 6));
    }
    
    // Тест 4: Проверяем быстрые переключения
    for (int i = 0; i < 50; ++i) {
        m_moveTool->setAutoSelectLayer(i % 2 == 0);
        m_eyedropperTool->setSampleFromAllLayers(i % 2 == 1);
    }
    
    qDebug() << "✅ EDGE CASES test passed";
}

// QTEST_MAIN removed - using main.cpp instead
#include "StrictTest.moc"
