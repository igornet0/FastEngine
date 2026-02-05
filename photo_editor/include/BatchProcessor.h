#ifndef BATCHPROCESSOR_H
#define BATCHPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QString>
#include <QDir>
#include <QImage>
#include <QVariantMap>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QJsonObject>

class FilterManager;

struct BatchOperation {
    QString operation;
    QVariantMap parameters;
    bool enabled;
    
    BatchOperation() : enabled(true) {}
    BatchOperation(const QString &op, const QVariantMap &params = QVariantMap(), bool en = true)
        : operation(op), parameters(params), enabled(en) {}
};

struct BatchJob {
    QString inputFile;
    QString outputFile;
    QStringList operations;
    QVariantMap globalParameters;
    bool success;
    QString errorMessage;
    
    BatchJob() : success(false) {}
};

class BatchProcessor : public QThread
{
    Q_OBJECT

public:
    explicit BatchProcessor(QObject *parent = nullptr);
    ~BatchProcessor();

    // Job management
    void addJob(const BatchJob &job);
    void clearJobs();
    int getJobCount() const { return m_jobs.size(); }
    
    // Processing control
    void startProcessing();
    void stopProcessing();
    bool isProcessing() const { return m_processing; }
    
    // Progress
    int getCurrentJob() const { return m_currentJob; }
    int getTotalJobs() const { return m_jobs.size(); }
    float getProgress() const;

signals:
    void jobStarted(int jobIndex, const QString &fileName);
    void jobCompleted(int jobIndex, bool success, const QString &message);
    void jobProgress(int jobIndex, float progress);
    void processingFinished();
    void processingStopped();

protected:
    void run() override;

public:
    // Public methods for testing
    bool processJob(const BatchJob &job);
    QImage loadImage(const QString &fileName);
    bool saveImage(const QImage &image, const QString &fileName);
    QImage applyOperations(const QImage &image, const QStringList &operations, const QVariantMap &globalParams);

public: // Public for testing

    QList<BatchJob> m_jobs;
    bool m_processing;
    bool m_stopRequested;
    int m_currentJob;
    FilterManager *m_filterManager;
};

class BatchProcessorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchProcessorDialog(QWidget *parent = nullptr);
    ~BatchProcessorDialog();

    void setFilterManager(FilterManager *manager);

signals:
    void processingRequested(const QList<BatchJob> &jobs);

private slots:
    void onAddFiles();
    void onAddFolder();
    void onRemoveFiles();
    void onClearFiles();
    void onAddOperation();
    void onRemoveOperation();
    void onMoveOperationUp();
    void onMoveOperationDown();
    void onStartProcessing();
    void onStopProcessing();
    void onJobStarted(int jobIndex, const QString &fileName);
    void onJobCompleted(int jobIndex, bool success, const QString &message);
    void onJobProgress(int jobIndex, float progress);
    void onProcessingFinished();
    void onProcessingStopped();

private:
    void setupUI();
    void setupConnections();
    void updateFileList();
    void updateOperationList();
    void updateButtons();
    QList<BatchJob> createJobs() const;
    QString generateOutputFileName(const QString &inputFile) const;

    FilterManager *m_filterManager;
    BatchProcessor *m_processor;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    
    // File selection
    QGroupBox *m_filesGroup;
    QVBoxLayout *m_filesLayout;
    QHBoxLayout *m_filesButtonsLayout;
    QPushButton *m_addFilesButton;
    QPushButton *m_addFolderButton;
    QPushButton *m_removeFilesButton;
    QPushButton *m_clearFilesButton;
    QListWidget *m_filesList;
    
    // Output settings
    QGroupBox *m_outputGroup;
    QFormLayout *m_outputLayout;
    QLineEdit *m_outputDirectoryEdit;
    QPushButton *m_outputDirectoryButton;
    QComboBox *m_outputFormatCombo;
    QCheckBox *m_keepStructureCheck;
    QCheckBox *m_overwriteCheck;
    
    // Operations
    QGroupBox *m_operationsGroup;
    QVBoxLayout *m_operationsLayout;
    QHBoxLayout *m_operationsButtonsLayout;
    QPushButton *m_addOperationButton;
    QPushButton *m_removeOperationButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    QListWidget *m_operationsList;
    
    // Progress
    QGroupBox *m_progressGroup;
    QVBoxLayout *m_progressLayout;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QLabel *m_currentFileLabel;
    
    // Buttons
    QHBoxLayout *m_buttonsLayout;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_closeButton;
    
    // Data
    QStringList m_inputFiles;
    QList<BatchOperation> m_operations;
    QString m_outputDirectory;
    QString m_outputFormat;
    bool m_keepStructure;
    bool m_overwrite;
};

#endif // BATCHPROCESSOR_H
