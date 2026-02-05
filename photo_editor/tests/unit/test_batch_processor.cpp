#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QColor>

#include "BatchProcessor.h"
#include "FilterManager.h"

class TestBatchProcessor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test job management
    void testAddJob();
    void testClearJobs();
    void testGetJobCount();
    
    // Test processing control
    void testStartProcessing();
    void testStopProcessing();
    void testIsProcessing();
    
    // Test progress
    void testGetCurrentJob();
    void testGetTotalJobs();
    void testGetProgress();
    
    // Test job processing
    void testProcessJob();
    void testLoadImage();
    void testSaveImage();
    void testApplyOperations();
    
    // Test signals
    void testSignals();

private:
    BatchProcessor *m_processor;
    FilterManager *m_filterManager;
    QString m_testDirectory;
    QImage m_testImage;
};

void TestBatchProcessor::initTestCase()
{
    // Initialize QApplication if not already done
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
    
    // Create test directory
    m_testDirectory = QDir::tempPath() + "/PhotoEditorBatchTest";
    QDir().mkpath(m_testDirectory);
    
    // Create test image
    m_testImage = QImage(100, 100, QImage::Format_RGB32);
    m_testImage.fill(Qt::red);
}

void TestBatchProcessor::cleanupTestCase()
{
    // Clean up test directory
    QDir(m_testDirectory).removeRecursively();
}

void TestBatchProcessor::init()
{
    m_processor = new BatchProcessor();
    m_filterManager = new FilterManager();
    m_processor->m_filterManager = m_filterManager;
}

void TestBatchProcessor::cleanup()
{
    delete m_processor;
    delete m_filterManager;
}

void TestBatchProcessor::testAddJob()
{
    QCOMPARE(m_processor->getJobCount(), 0);
    
    BatchJob job;
    job.inputFile = "test_input.png";
    job.outputFile = "test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    job.success = false;
    
    m_processor->addJob(job);
    QCOMPARE(m_processor->getJobCount(), 1);
    
    // Add another job
    m_processor->addJob(job);
    QCOMPARE(m_processor->getJobCount(), 2);
}

void TestBatchProcessor::testClearJobs()
{
    // Add some jobs
    BatchJob job;
    job.inputFile = "test_input.png";
    job.outputFile = "test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    m_processor->addJob(job);
    m_processor->addJob(job);
    QCOMPARE(m_processor->getJobCount(), 2);
    
    // Clear jobs
    m_processor->clearJobs();
    QCOMPARE(m_processor->getJobCount(), 0);
}

void TestBatchProcessor::testGetJobCount()
{
    QCOMPARE(m_processor->getJobCount(), 0);
    
    BatchJob job;
    job.inputFile = "test_input.png";
    job.outputFile = "test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    for (int i = 0; i < 5; ++i) {
        m_processor->addJob(job);
        QCOMPARE(m_processor->getJobCount(), i + 1);
    }
}

void TestBatchProcessor::testStartProcessing()
{
    // Add a job
    BatchJob job;
    job.inputFile = m_testDirectory + "/test_input.png";
    job.outputFile = m_testDirectory + "/test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    // Save test image
    m_testImage.save(job.inputFile);
    
    m_processor->addJob(job);
    
    // Start processing
    m_processor->startProcessing();
    
    // Wait for processing to complete
    QThread::msleep(100);
    
    QVERIFY(!m_processor->isProcessing());
}

void TestBatchProcessor::testStopProcessing()
{
    // Add a job
    BatchJob job;
    job.inputFile = m_testDirectory + "/test_input.png";
    job.outputFile = m_testDirectory + "/test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    m_processor->addJob(job);
    
    // Start processing
    m_processor->startProcessing();
    
    // Stop processing
    m_processor->stopProcessing();
    
    QVERIFY(!m_processor->isProcessing());
}

void TestBatchProcessor::testIsProcessing()
{
    QVERIFY(!m_processor->isProcessing());
    
    // Add a job and start processing
    BatchJob job;
    job.inputFile = m_testDirectory + "/test_input.png";
    job.outputFile = m_testDirectory + "/test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    m_processor->addJob(job);
    m_processor->startProcessing();
    
    // Processing should be running (briefly)
    QVERIFY(m_processor->isProcessing());
    
    // Wait for completion
    QThread::msleep(100);
    QVERIFY(!m_processor->isProcessing());
}

void TestBatchProcessor::testGetCurrentJob()
{
    QCOMPARE(m_processor->getCurrentJob(), -1);
    
    // Add jobs
    BatchJob job1, job2, job3;
    job1.inputFile = m_testDirectory + "/test_input1.png";
    job1.outputFile = m_testDirectory + "/test_output1.png";
    job1.operations = QStringList() << "Brightness/Contrast";
    
    job2.inputFile = m_testDirectory + "/test_input2.png";
    job2.outputFile = m_testDirectory + "/test_output2.png";
    job2.operations = QStringList() << "Brightness/Contrast";
    
    job3.inputFile = m_testDirectory + "/test_input3.png";
    job3.outputFile = m_testDirectory + "/test_output3.png";
    job3.operations = QStringList() << "Brightness/Contrast";
    
    m_processor->addJob(job1);
    m_processor->addJob(job2);
    m_processor->addJob(job3);
    
    QCOMPARE(m_processor->getTotalJobs(), 3);
}

void TestBatchProcessor::testGetTotalJobs()
{
    QCOMPARE(m_processor->getTotalJobs(), 0);
    
    BatchJob job;
    job.inputFile = "test_input.png";
    job.outputFile = "test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    for (int i = 0; i < 3; ++i) {
        m_processor->addJob(job);
        QCOMPARE(m_processor->getTotalJobs(), i + 1);
    }
}

void TestBatchProcessor::testGetProgress()
{
    QCOMPARE(m_processor->getProgress(), 0.0f);
    
    // Add jobs
    BatchJob job1, job2, job3;
    job1.inputFile = m_testDirectory + "/test_input1.png";
    job1.outputFile = m_testDirectory + "/test_output1.png";
    job1.operations = QStringList() << "Brightness/Contrast";
    
    job2.inputFile = m_testDirectory + "/test_input2.png";
    job2.outputFile = m_testDirectory + "/test_output2.png";
    job2.operations = QStringList() << "Brightness/Contrast";
    
    job3.inputFile = m_testDirectory + "/test_input3.png";
    job3.outputFile = m_testDirectory + "/test_output3.png";
    job3.operations = QStringList() << "Brightness/Contrast";
    
    m_processor->addJob(job1);
    m_processor->addJob(job2);
    m_processor->addJob(job3);
    
    QCOMPARE(m_processor->getTotalJobs(), 3);
    
    // Progress should be 0 initially
    QCOMPARE(m_processor->getProgress(), 0.0f);
}

void TestBatchProcessor::testProcessJob()
{
    // Create a test job
    BatchJob job;
    job.inputFile = m_testDirectory + "/test_input.png";
    job.outputFile = m_testDirectory + "/test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    job.globalParameters = QVariantMap{{"brightness", 10}, {"contrast", 5}};
    
    // Save test image
    m_testImage.save(job.inputFile);
    
    // Process job
    bool success = m_processor->processJob(job);
    QVERIFY(success);
    
    // Verify output file was created
    QVERIFY(QFile::exists(job.outputFile));
}

void TestBatchProcessor::testLoadImage()
{
    // Save test image
    QString fileName = m_testDirectory + "/test_load.png";
    m_testImage.save(fileName);
    
    // Load image
    QImage loadedImage = m_processor->loadImage(fileName);
    QVERIFY(!loadedImage.isNull());
    QCOMPARE(loadedImage.size(), m_testImage.size());
    
    // Test loading non-existent file
    QImage nullImage = m_processor->loadImage("non_existent.png");
    QVERIFY(nullImage.isNull());
}

void TestBatchProcessor::testSaveImage()
{
    // Test saving image
    QString fileName = m_testDirectory + "/test_save.png";
    bool success = m_processor->saveImage(m_testImage, fileName);
    QVERIFY(success);
    QVERIFY(QFile::exists(fileName));
    
    // Test saving to non-existent directory
    QString invalidFileName = "/invalid/path/test.png";
    bool invalidSuccess = m_processor->saveImage(m_testImage, invalidFileName);
    QVERIFY(!invalidSuccess);
}

void TestBatchProcessor::testApplyOperations()
{
    // Test applying operations
    QImage result = m_processor->applyOperations(m_testImage, QStringList() << "Brightness/Contrast", QVariantMap{{"brightness", 10}});
    QVERIFY(!result.isNull());
    QCOMPARE(result.size(), m_testImage.size());
    
    // Test with empty operations list
    QImage emptyResult = m_processor->applyOperations(m_testImage, QStringList(), QVariantMap());
    QVERIFY(!emptyResult.isNull());
    QCOMPARE(emptyResult, m_testImage);
    
    // Test with invalid operation
    QImage invalidResult = m_processor->applyOperations(m_testImage, QStringList() << "InvalidOperation", QVariantMap());
    QVERIFY(!invalidResult.isNull());
}

void TestBatchProcessor::testSignals()
{
    // Test signals
    QSignalSpy startedSpy(m_processor, &BatchProcessor::jobStarted);
    QSignalSpy completedSpy(m_processor, &BatchProcessor::jobCompleted);
    QSignalSpy progressSpy(m_processor, &BatchProcessor::jobProgress);
    QSignalSpy finishedSpy(m_processor, &BatchProcessor::processingFinished);
    QSignalSpy stoppedSpy(m_processor, &BatchProcessor::processingStopped);
    
    // Add a job
    BatchJob job;
    job.inputFile = m_testDirectory + "/test_input.png";
    job.outputFile = m_testDirectory + "/test_output.png";
    job.operations = QStringList() << "Brightness/Contrast";
    
    // Save test image
    m_testImage.save(job.inputFile);
    
    m_processor->addJob(job);
    
    // Start processing
    m_processor->startProcessing();
    
    // Wait for processing to complete
    QThread::msleep(100);
    
    // Check signals were emitted
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
    
    // Verify signal parameters
    QList<QVariant> startedArgs = startedSpy.takeFirst();
    QCOMPARE(startedArgs.at(0).toInt(), 0);
    QCOMPARE(startedArgs.at(1).toString(), job.inputFile);
    
    QList<QVariant> completedArgs = completedSpy.takeFirst();
    QCOMPARE(completedArgs.at(0).toInt(), 0);
    QVERIFY(completedArgs.at(1).toBool()); // Should be successful
}

QTEST_MAIN(TestBatchProcessor)
