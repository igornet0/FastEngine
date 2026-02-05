#include "BatchProcessor.h"
#include "FilterManager.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QLineEdit>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>

BatchProcessor::BatchProcessor(QObject *parent)
    : QThread(parent)
    , m_processing(false)
    , m_stopRequested(false)
    , m_currentJob(-1)
    , m_filterManager(nullptr)
{
}

BatchProcessor::~BatchProcessor()
{
    if (isRunning()) {
        stopProcessing();
        wait();
    }
}

void BatchProcessor::addJob(const BatchJob &job)
{
    m_jobs.append(job);
}

void BatchProcessor::clearJobs()
{
    m_jobs.clear();
}

float BatchProcessor::getProgress() const
{
    if (m_jobs.isEmpty()) {
        return 0.0f;
    }
    return static_cast<float>(m_currentJob + 1) / m_jobs.size();
}

void BatchProcessor::startProcessing()
{
    if (!isRunning() && !m_jobs.isEmpty()) {
        m_processing = true;
        m_stopRequested = false;
        m_currentJob = -1;
        start();
    }
}

void BatchProcessor::stopProcessing()
{
    m_stopRequested = true;
    m_processing = false;
}

void BatchProcessor::run()
{
    m_currentJob = 0;
    
    for (int i = 0; i < m_jobs.size() && !m_stopRequested; ++i) {
        m_currentJob = i;
        const BatchJob &job = m_jobs[i];
        
        emit jobStarted(i, job.inputFile);
        
        bool success = processJob(job);
        
        emit jobCompleted(i, success, success ? "Success" : job.errorMessage);
        emit jobProgress(i, getProgress());
    }
    
    m_processing = false;
    
    if (m_stopRequested) {
        emit processingStopped();
    } else {
        emit processingFinished();
    }
}

bool BatchProcessor::processJob(const BatchJob &job)
{
    // Load input image
    QImage image = loadImage(job.inputFile);
    if (image.isNull()) {
        return false;
    }
    
    // Apply operations
    QImage processedImage = applyOperations(image, job.operations, job.globalParameters);
    if (processedImage.isNull()) {
        return false;
    }
    
    // Save output image
    return saveImage(processedImage, job.outputFile);
}

QImage BatchProcessor::loadImage(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull()) {
        qDebug() << "Failed to load image:" << fileName;
    }
    return image;
}

bool BatchProcessor::saveImage(const QImage &image, const QString &fileName)
{
    // Ensure output directory exists
    QFileInfo fileInfo(fileName);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create output directory:" << dir.absolutePath();
            return false;
        }
    }
    
    bool success = image.save(fileName);
    if (!success) {
        qDebug() << "Failed to save image:" << fileName;
    }
    return success;
}

QImage BatchProcessor::applyOperations(const QImage &image, const QStringList &operations, const QVariantMap &globalParams)
{
    QImage result = image;
    
    if (!m_filterManager) {
        return result;
    }
    
    for (const QString &operation : operations) {
        if (m_stopRequested) {
            break;
        }
        
        QVariantMap params = globalParams;
        result = m_filterManager->applyFilter(result, operation, params);
        
        if (result.isNull()) {
            qDebug() << "Failed to apply operation:" << operation;
            break;
        }
    }
    
    return result;
}

// BatchProcessorDialog implementation

BatchProcessorDialog::BatchProcessorDialog(QWidget *parent)
    : QDialog(parent)
    , m_filterManager(nullptr)
    , m_processor(new BatchProcessor(this))
    , m_outputDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
    , m_outputFormat("PNG")
    , m_keepStructure(true)
    , m_overwrite(false)
{
    setupUI();
    setupConnections();
    updateButtons();
}

BatchProcessorDialog::~BatchProcessorDialog()
{
    if (m_processor->isProcessing()) {
        m_processor->stopProcessing();
        m_processor->wait();
    }
}

void BatchProcessorDialog::setFilterManager(FilterManager *manager)
{
    m_filterManager = manager;
    m_processor->m_filterManager = manager;
}

void BatchProcessorDialog::setupUI()
{
    setWindowTitle("Batch Processor");
    setModal(true);
    resize(600, 500);
    
    m_mainLayout = new QVBoxLayout(this);
    
    // File selection group
    m_filesGroup = new QGroupBox("Input Files");
    m_filesLayout = new QVBoxLayout(m_filesGroup);
    
    m_filesButtonsLayout = new QHBoxLayout();
    m_addFilesButton = new QPushButton("Add Files...");
    m_addFolderButton = new QPushButton("Add Folder...");
    m_removeFilesButton = new QPushButton("Remove");
    m_clearFilesButton = new QPushButton("Clear All");
    
    m_filesButtonsLayout->addWidget(m_addFilesButton);
    m_filesButtonsLayout->addWidget(m_addFolderButton);
    m_filesButtonsLayout->addWidget(m_removeFilesButton);
    m_filesButtonsLayout->addWidget(m_clearFilesButton);
    m_filesButtonsLayout->addStretch();
    
    m_filesList = new QListWidget();
    m_filesList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    m_filesLayout->addLayout(m_filesButtonsLayout);
    m_filesLayout->addWidget(m_filesList);
    
    // Output settings group
    m_outputGroup = new QGroupBox("Output Settings");
    m_outputLayout = new QFormLayout(m_outputGroup);
    
    m_outputDirectoryEdit = new QLineEdit(m_outputDirectory);
    m_outputDirectoryButton = new QPushButton("Browse...");
    QHBoxLayout *outputDirLayout = new QHBoxLayout();
    outputDirLayout->addWidget(m_outputDirectoryEdit);
    outputDirLayout->addWidget(m_outputDirectoryButton);
    
    m_outputFormatCombo = new QComboBox();
    m_outputFormatCombo->addItems({"PNG", "JPEG", "BMP", "TIFF"});
    m_outputFormatCombo->setCurrentText(m_outputFormat);
    
    m_keepStructureCheck = new QCheckBox("Keep folder structure");
    m_keepStructureCheck->setChecked(m_keepStructure);
    
    m_overwriteCheck = new QCheckBox("Overwrite existing files");
    m_overwriteCheck->setChecked(m_overwrite);
    
    m_outputLayout->addRow("Output Directory:", outputDirLayout);
    m_outputLayout->addRow("Format:", m_outputFormatCombo);
    m_outputLayout->addRow(m_keepStructureCheck);
    m_outputLayout->addRow(m_overwriteCheck);
    
    // Operations group
    m_operationsGroup = new QGroupBox("Operations");
    m_operationsLayout = new QVBoxLayout(m_operationsGroup);
    
    m_operationsButtonsLayout = new QHBoxLayout();
    m_addOperationButton = new QPushButton("Add Operation...");
    m_removeOperationButton = new QPushButton("Remove");
    m_moveUpButton = new QPushButton("Move Up");
    m_moveDownButton = new QPushButton("Move Down");
    
    m_operationsButtonsLayout->addWidget(m_addOperationButton);
    m_operationsButtonsLayout->addWidget(m_removeOperationButton);
    m_operationsButtonsLayout->addWidget(m_moveUpButton);
    m_operationsButtonsLayout->addWidget(m_moveDownButton);
    m_operationsButtonsLayout->addStretch();
    
    m_operationsList = new QListWidget();
    
    m_operationsLayout->addLayout(m_operationsButtonsLayout);
    m_operationsLayout->addWidget(m_operationsList);
    
    // Progress group
    m_progressGroup = new QGroupBox("Progress");
    m_progressLayout = new QVBoxLayout(m_progressGroup);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    
    m_statusLabel = new QLabel("Ready");
    m_currentFileLabel = new QLabel();
    m_currentFileLabel->setVisible(false);
    
    m_progressLayout->addWidget(m_progressBar);
    m_progressLayout->addWidget(m_statusLabel);
    m_progressLayout->addWidget(m_currentFileLabel);
    
    // Buttons
    m_buttonsLayout = new QHBoxLayout();
    m_startButton = new QPushButton("Start Processing");
    m_stopButton = new QPushButton("Stop");
    m_closeButton = new QPushButton("Close");
    
    m_stopButton->setEnabled(false);
    
    m_buttonsLayout->addStretch();
    m_buttonsLayout->addWidget(m_startButton);
    m_buttonsLayout->addWidget(m_stopButton);
    m_buttonsLayout->addWidget(m_closeButton);
    
    // Add all groups to main layout
    m_mainLayout->addWidget(m_filesGroup);
    m_mainLayout->addWidget(m_outputGroup);
    m_mainLayout->addWidget(m_operationsGroup);
    m_mainLayout->addWidget(m_progressGroup);
    m_mainLayout->addLayout(m_buttonsLayout);
}

void BatchProcessorDialog::setupConnections()
{
    // File operations
    connect(m_addFilesButton, &QPushButton::clicked, this, &BatchProcessorDialog::onAddFiles);
    connect(m_addFolderButton, &QPushButton::clicked, this, &BatchProcessorDialog::onAddFolder);
    connect(m_removeFilesButton, &QPushButton::clicked, this, &BatchProcessorDialog::onRemoveFiles);
    connect(m_clearFilesButton, &QPushButton::clicked, this, &BatchProcessorDialog::onClearFiles);
    
    // Output settings
    connect(m_outputDirectoryButton, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory", m_outputDirectory);
        if (!dir.isEmpty()) {
            m_outputDirectory = dir;
            m_outputDirectoryEdit->setText(dir);
        }
    });
    
    connect(m_outputFormatCombo, &QComboBox::currentTextChanged, [this](const QString &format) {
        m_outputFormat = format;
    });
    
    connect(m_keepStructureCheck, &QCheckBox::toggled, [this](bool checked) {
        m_keepStructure = checked;
    });
    
    connect(m_overwriteCheck, &QCheckBox::toggled, [this](bool checked) {
        m_overwrite = checked;
    });
    
    // Operations
    connect(m_addOperationButton, &QPushButton::clicked, this, &BatchProcessorDialog::onAddOperation);
    connect(m_removeOperationButton, &QPushButton::clicked, this, &BatchProcessorDialog::onRemoveOperation);
    connect(m_moveUpButton, &QPushButton::clicked, this, &BatchProcessorDialog::onMoveOperationUp);
    connect(m_moveDownButton, &QPushButton::clicked, this, &BatchProcessorDialog::onMoveOperationDown);
    
    // Processing
    connect(m_startButton, &QPushButton::clicked, this, &BatchProcessorDialog::onStartProcessing);
    connect(m_stopButton, &QPushButton::clicked, this, &BatchProcessorDialog::onStopProcessing);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    // Processor signals
    connect(m_processor, &BatchProcessor::jobStarted, this, &BatchProcessorDialog::onJobStarted);
    connect(m_processor, &BatchProcessor::jobCompleted, this, &BatchProcessorDialog::onJobCompleted);
    connect(m_processor, &BatchProcessor::jobProgress, this, &BatchProcessorDialog::onJobProgress);
    connect(m_processor, &BatchProcessor::processingFinished, this, &BatchProcessorDialog::onProcessingFinished);
    connect(m_processor, &BatchProcessor::processingStopped, this, &BatchProcessorDialog::onProcessingStopped);
}

void BatchProcessorDialog::onAddFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "Select Images",
        QString(),
        "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff);;All Files (*)"
    );
    
    for (const QString &file : files) {
        if (!m_inputFiles.contains(file)) {
            m_inputFiles.append(file);
        }
    }
    
    updateFileList();
    updateButtons();
}

void BatchProcessorDialog::onAddFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!folder.isEmpty()) {
        QDir dir(folder);
        QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff"};
        QStringList files = dir.entryList(filters, QDir::Files);
        
        for (const QString &file : files) {
            QString fullPath = dir.absoluteFilePath(file);
            if (!m_inputFiles.contains(fullPath)) {
                m_inputFiles.append(fullPath);
            }
        }
        
        updateFileList();
        updateButtons();
    }
}

void BatchProcessorDialog::onRemoveFiles()
{
    QList<QListWidgetItem*> selectedItems = m_filesList->selectedItems();
    for (QListWidgetItem *item : selectedItems) {
        int row = m_filesList->row(item);
        m_inputFiles.removeAt(row);
        delete item;
    }
    
    updateButtons();
}

void BatchProcessorDialog::onClearFiles()
{
    m_inputFiles.clear();
    m_filesList->clear();
    updateButtons();
}

void BatchProcessorDialog::onAddOperation()
{
    // This would open a dialog to select and configure operations
    // For now, add a placeholder operation
    BatchOperation operation("Brightness/Contrast", QVariantMap{{"brightness", 10}, {"contrast", 5}});
    m_operations.append(operation);
    updateOperationList();
    updateButtons();
}

void BatchProcessorDialog::onRemoveOperation()
{
    QList<QListWidgetItem*> selectedItems = m_operationsList->selectedItems();
    for (QListWidgetItem *item : selectedItems) {
        int row = m_operationsList->row(item);
        m_operations.removeAt(row);
        delete item;
    }
    
    updateButtons();
}

void BatchProcessorDialog::onMoveOperationUp()
{
    int currentRow = m_operationsList->currentRow();
    if (currentRow > 0) {
        m_operations.move(currentRow, currentRow - 1);
        updateOperationList();
        m_operationsList->setCurrentRow(currentRow - 1);
    }
}

void BatchProcessorDialog::onMoveOperationDown()
{
    int currentRow = m_operationsList->currentRow();
    if (currentRow >= 0 && currentRow < m_operations.size() - 1) {
        m_operations.move(currentRow, currentRow + 1);
        updateOperationList();
        m_operationsList->setCurrentRow(currentRow + 1);
    }
}

void BatchProcessorDialog::onStartProcessing()
{
    if (m_inputFiles.isEmpty() || m_operations.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please add input files and operations.");
        return;
    }
    
    m_processor->clearJobs();
    QList<BatchJob> jobs = createJobs();
    
    for (const BatchJob &job : jobs) {
        m_processor->addJob(job);
    }
    
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, m_processor->getTotalJobs());
    m_progressBar->setValue(0);
    m_statusLabel->setText("Processing...");
    
    m_processor->startProcessing();
}

void BatchProcessorDialog::onStopProcessing()
{
    m_processor->stopProcessing();
}

void BatchProcessorDialog::onJobStarted(int jobIndex, const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    m_currentFileLabel->setText(QString("Processing: %1").arg(fileInfo.fileName()));
    m_currentFileLabel->setVisible(true);
}

void BatchProcessorDialog::onJobCompleted(int jobIndex, bool success, const QString &message)
{
    m_progressBar->setValue(jobIndex + 1);
    
    if (!success) {
        m_statusLabel->setText(QString("Error: %1").arg(message));
    }
}

void BatchProcessorDialog::onJobProgress(int jobIndex, float progress)
{
    // Update progress if needed
}

void BatchProcessorDialog::onProcessingFinished()
{
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setVisible(false);
    m_currentFileLabel->setVisible(false);
    m_statusLabel->setText("Processing completed!");
    
    QMessageBox::information(this, "Complete", "Batch processing completed successfully!");
}

void BatchProcessorDialog::onProcessingStopped()
{
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setVisible(false);
    m_currentFileLabel->setVisible(false);
    m_statusLabel->setText("Processing stopped.");
}

void BatchProcessorDialog::updateFileList()
{
    m_filesList->clear();
    for (const QString &file : m_inputFiles) {
        QFileInfo fileInfo(file);
        m_filesList->addItem(fileInfo.fileName());
    }
}

void BatchProcessorDialog::updateOperationList()
{
    m_operationsList->clear();
    for (const BatchOperation &operation : m_operations) {
        QString text = operation.operation;
        if (!operation.parameters.isEmpty()) {
            text += " (configured)";
        }
        m_operationsList->addItem(text);
    }
}

void BatchProcessorDialog::updateButtons()
{
    bool hasFiles = !m_inputFiles.isEmpty();
    bool hasOperations = !m_operations.isEmpty();
    bool isProcessing = m_processor->isProcessing();
    
    m_removeFilesButton->setEnabled(hasFiles && m_filesList->currentItem());
    m_clearFilesButton->setEnabled(hasFiles);
    m_removeOperationButton->setEnabled(hasOperations && m_operationsList->currentItem());
    m_moveUpButton->setEnabled(hasOperations && m_operationsList->currentRow() > 0);
    m_moveDownButton->setEnabled(hasOperations && m_operationsList->currentRow() >= 0 && m_operationsList->currentRow() < m_operations.size() - 1);
    
    m_startButton->setEnabled(hasFiles && hasOperations && !isProcessing);
    m_stopButton->setEnabled(isProcessing);
}

QList<BatchJob> BatchProcessorDialog::createJobs() const
{
    QList<BatchJob> jobs;
    
    for (const QString &inputFile : m_inputFiles) {
        BatchJob job;
        job.inputFile = inputFile;
        job.outputFile = generateOutputFileName(inputFile);
        
        for (const BatchOperation &operation : m_operations) {
            if (operation.enabled) {
                job.operations.append(operation.operation);
            }
        }
        
        jobs.append(job);
    }
    
    return jobs;
}

QString BatchProcessorDialog::generateOutputFileName(const QString &inputFile) const
{
    QFileInfo inputInfo(inputFile);
    QString baseName = inputInfo.baseName();
    QString suffix = inputInfo.suffix();
    
    if (m_keepStructure) {
        // Keep relative path structure
        QString relativePath = inputInfo.absolutePath();
        QString outputPath = m_outputDirectory + "/" + relativePath;
        return outputPath + "/" + baseName + "_processed." + m_outputFormat.toLower();
    } else {
        // Flatten to output directory
        return m_outputDirectory + "/" + baseName + "_processed." + m_outputFormat.toLower();
    }
}
