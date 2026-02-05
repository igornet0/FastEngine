#include "MetadataViewer.h"
#include <QFileInfo>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHeaderView>
#include <QColorSpace>
#include <QDialog>
#include <QDebug>

MetadataViewer::MetadataViewer(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
}

MetadataViewer::~MetadataViewer()
{
}

void MetadataViewer::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    m_tabWidget = new QTabWidget();
    
    // Basic information tab
    m_basicTab = new QWidget();
    m_basicLayout = new QVBoxLayout(m_basicTab);
    m_basicTree = new QTreeWidget();
    m_basicTree->setHeaderLabels({"Property", "Value", "Description"});
    m_basicTree->setRootIsDecorated(false);
    m_basicTree->setAlternatingRowColors(true);
    m_basicTree->header()->setStretchLastSection(false);
    m_basicTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_basicTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_basicTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_basicLayout->addWidget(m_basicTree);
    m_tabWidget->addTab(m_basicTab, "Basic Info");
    
    // File information tab
    m_fileTab = new QWidget();
    m_fileLayout = new QVBoxLayout(m_fileTab);
    m_fileTree = new QTreeWidget();
    m_fileTree->setHeaderLabels({"Property", "Value", "Description"});
    m_fileTree->setRootIsDecorated(false);
    m_fileTree->setAlternatingRowColors(true);
    m_fileTree->header()->setStretchLastSection(false);
    m_fileTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_fileTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_fileTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_fileLayout->addWidget(m_fileTree);
    m_tabWidget->addTab(m_fileTab, "File Info");
    
    // EXIF data tab
    m_exifTab = new QWidget();
    m_exifLayout = new QVBoxLayout(m_exifTab);
    m_exifTree = new QTreeWidget();
    m_exifTree->setHeaderLabels({"Property", "Value", "Description"});
    m_exifTree->setRootIsDecorated(false);
    m_exifTree->setAlternatingRowColors(true);
    m_exifTree->header()->setStretchLastSection(false);
    m_exifTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_exifTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_exifTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_exifLayout->addWidget(m_exifTree);
    m_tabWidget->addTab(m_exifTab, "EXIF Data");
    
    // Technical information tab
    m_technicalTab = new QWidget();
    m_technicalLayout = new QVBoxLayout(m_technicalTab);
    m_technicalTree = new QTreeWidget();
    m_technicalTree->setHeaderLabels({"Property", "Value", "Description"});
    m_technicalTree->setRootIsDecorated(false);
    m_technicalTree->setAlternatingRowColors(true);
    m_technicalTree->header()->setStretchLastSection(false);
    m_technicalTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_technicalTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_technicalTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_technicalLayout->addWidget(m_technicalTree);
    m_tabWidget->addTab(m_technicalTab, "Technical");
    
    // Color information tab
    m_colorTab = new QWidget();
    m_colorLayout = new QVBoxLayout(m_colorTab);
    m_colorTree = new QTreeWidget();
    m_colorTree->setHeaderLabels({"Property", "Value", "Description"});
    m_colorTree->setRootIsDecorated(false);
    m_colorTree->setAlternatingRowColors(true);
    m_colorTree->header()->setStretchLastSection(false);
    m_colorTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_colorTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_colorTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_colorLayout->addWidget(m_colorTree);
    m_tabWidget->addTab(m_colorTab, "Colors");
    
    // GPS information tab
    m_gpsTab = new QWidget();
    m_gpsLayout = new QVBoxLayout(m_gpsTab);
    m_gpsTree = new QTreeWidget();
    m_gpsTree->setHeaderLabels({"Property", "Value", "Description"});
    m_gpsTree->setRootIsDecorated(false);
    m_gpsTree->setAlternatingRowColors(true);
    m_gpsTree->header()->setStretchLastSection(false);
    m_gpsTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_gpsTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_gpsTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_gpsLayout->addWidget(m_gpsTree);
    m_tabWidget->addTab(m_gpsTab, "GPS");
    
    // Camera information tab
    m_cameraTab = new QWidget();
    m_cameraLayout = new QVBoxLayout(m_cameraTab);
    m_cameraTree = new QTreeWidget();
    m_cameraTree->setHeaderLabels({"Property", "Value", "Description"});
    m_cameraTree->setRootIsDecorated(false);
    m_cameraTree->setAlternatingRowColors(true);
    m_cameraTree->header()->setStretchLastSection(false);
    m_cameraTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_cameraTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_cameraTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_cameraLayout->addWidget(m_cameraTree);
    m_tabWidget->addTab(m_cameraTab, "Camera");
    
    // Software information tab
    m_softwareTab = new QWidget();
    m_softwareLayout = new QVBoxLayout(m_softwareTab);
    m_softwareTree = new QTreeWidget();
    m_softwareTree->setHeaderLabels({"Property", "Value", "Description"});
    m_softwareTree->setRootIsDecorated(false);
    m_softwareTree->setAlternatingRowColors(true);
    m_softwareTree->header()->setStretchLastSection(false);
    m_softwareTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_softwareTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_softwareTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_softwareLayout->addWidget(m_softwareTree);
    m_tabWidget->addTab(m_softwareTab, "Software");
    
    // Actions
    m_actionsLayout = new QHBoxLayout();
    m_exportButton = new QPushButton("Export...");
    m_copyButton = new QPushButton("Copy All");
    m_refreshButton = new QPushButton("Refresh");
    
    m_actionsLayout->addStretch();
    m_actionsLayout->addWidget(m_exportButton);
    m_actionsLayout->addWidget(m_copyButton);
    m_actionsLayout->addWidget(m_refreshButton);
    
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addLayout(m_actionsLayout);
}

void MetadataViewer::setupConnections()
{
    connect(m_exportButton, &QPushButton::clicked, this, &MetadataViewer::onExportMetadata);
    connect(m_copyButton, &QPushButton::clicked, this, &MetadataViewer::onCopyMetadata);
    connect(m_refreshButton, &QPushButton::clicked, this, &MetadataViewer::onRefreshMetadata);
}

void MetadataViewer::setImage(const QImage &image, const QString &filePath)
{
    loadMetadata(image, filePath);
    updateDisplay();
}

void MetadataViewer::clearMetadata()
{
    m_metadata = ImageMetadata();
    updateDisplay();
}

void MetadataViewer::loadMetadata(const QImage &image, const QString &filePath)
{
    m_metadata = ImageMetadata();
    
    if (image.isNull()) {
        return;
    }
    
    // Basic information
    m_metadata.imageSize = image.size();
    m_metadata.bitDepth = image.depth();
    m_metadata.hasAlpha = image.hasAlphaChannel();
    m_metadata.colorSpace = image.colorSpace().isValid() ? "sRGB" : "Unknown";
    
    // File information
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        m_metadata.fileName = fileInfo.fileName();
        m_metadata.filePath = fileInfo.absoluteFilePath();
        m_metadata.fileSize = fileInfo.size();
        m_metadata.created = fileInfo.birthTime();
        m_metadata.modified = fileInfo.lastModified();
        m_metadata.accessed = fileInfo.lastRead();
        m_metadata.format = fileInfo.suffix().toUpper();
    }
    
    // Technical information
    m_metadata.dpiX = image.dotsPerMeterX() * 0.0254; // Convert to DPI
    m_metadata.dpiY = image.dotsPerMeterY() * 0.0254;
    m_metadata.compression = "Unknown";
    
    // Color information
    m_metadata.colorCount = image.colorCount();
    
    // Load EXIF data (simplified - in real implementation would use Exiv2 or similar)
    loadExifData(image, filePath);
    
    emit metadataChanged(m_metadata);
}

void MetadataViewer::loadExifData(const QImage &image, const QString &filePath)
{
    // This is a simplified implementation
    // In a real application, you would use Exiv2 or similar library to read EXIF data
    
    Q_UNUSED(image)
    Q_UNUSED(filePath)
    
    // Add some sample EXIF data for demonstration
    m_metadata.exifData["ImageWidth"] = QString::number(m_metadata.imageSize.width());
    m_metadata.exifData["ImageHeight"] = QString::number(m_metadata.imageSize.height());
    m_metadata.exifData["BitsPerSample"] = QString::number(m_metadata.bitDepth);
    m_metadata.exifData["Compression"] = "JPEG";
    m_metadata.exifData["PhotometricInterpretation"] = "RGB";
    m_metadata.exifData["Orientation"] = "1";
    m_metadata.exifData["SamplesPerPixel"] = m_metadata.hasAlpha ? "4" : "3";
    m_metadata.exifData["PlanarConfiguration"] = "1";
    m_metadata.exifData["YCbCrSubSampling"] = "2 2";
    m_metadata.exifData["YCbCrPositioning"] = "1";
    m_metadata.exifData["XResolution"] = QString::number(m_metadata.dpiX);
    m_metadata.exifData["YResolution"] = QString::number(m_metadata.dpiY);
    m_metadata.exifData["ResolutionUnit"] = "2";
    
    // Camera information (sample data)
    m_metadata.cameraMake = "Canon";
    m_metadata.cameraModel = "EOS 5D Mark IV";
    m_metadata.lensModel = "EF 24-70mm f/2.8L II USM";
    m_metadata.focalLength = "50mm";
    m_metadata.aperture = "f/2.8";
    m_metadata.shutterSpeed = "1/125";
    m_metadata.iso = "100";
    m_metadata.exposureMode = "Manual";
    m_metadata.whiteBalance = "Auto";
    m_metadata.flash = "No Flash";
    
    // Software information
    m_metadata.software = "Photo Editor";
    m_metadata.version = "1.0.0";
    m_metadata.copyright = "© 2024";
    m_metadata.artist = "User";
}

void MetadataViewer::updateDisplay()
{
    populateBasicInfo();
    populateFileInfo();
    populateExifData();
    populateTechnicalInfo();
    populateColorInfo();
    populateGpsInfo();
    populateCameraInfo();
    populateSoftwareInfo();
}

void MetadataViewer::populateBasicInfo()
{
    m_basicTree->clear();
    
    addTreeItem(m_basicTree, "File Name", m_metadata.fileName, "Name of the image file");
    addTreeItem(m_basicTree, "Dimensions", QString("%1 × %2").arg(m_metadata.imageSize.width()).arg(m_metadata.imageSize.height()), "Image width and height in pixels");
    addTreeItem(m_basicTree, "Bit Depth", QString::number(m_metadata.bitDepth), "Number of bits per pixel");
    addTreeItem(m_basicTree, "Color Space", m_metadata.colorSpace, "Color space of the image");
    addTreeItem(m_basicTree, "Has Alpha", m_metadata.hasAlpha ? "Yes" : "No", "Whether the image has an alpha channel");
    addTreeItem(m_basicTree, "Format", m_metadata.format, "Image file format");
}

void MetadataViewer::populateFileInfo()
{
    m_fileTree->clear();
    
    addTreeItem(m_fileTree, "File Path", m_metadata.filePath, "Full path to the image file");
    addTreeItem(m_fileTree, "File Size", formatFileSize(m_metadata.fileSize), "Size of the image file");
    addTreeItem(m_fileTree, "Created", formatDateTime(m_metadata.created), "File creation date and time");
    addTreeItem(m_fileTree, "Modified", formatDateTime(m_metadata.modified), "File modification date and time");
    addTreeItem(m_fileTree, "Accessed", formatDateTime(m_metadata.accessed), "File last access date and time");
}

void MetadataViewer::populateExifData()
{
    m_exifTree->clear();
    
    for (auto it = m_metadata.exifData.begin(); it != m_metadata.exifData.end(); ++it) {
        addTreeItem(m_exifTree, it.key(), it.value(), formatExifValue(it.key(), it.value()));
    }
}

void MetadataViewer::populateTechnicalInfo()
{
    m_technicalTree->clear();
    
    addTreeItem(m_technicalTree, "DPI X", QString::number(m_metadata.dpiX, 'f', 1), "Horizontal resolution in dots per inch");
    addTreeItem(m_technicalTree, "DPI Y", QString::number(m_metadata.dpiY, 'f', 1), "Vertical resolution in dots per inch");
    addTreeItem(m_technicalTree, "Compression", m_metadata.compression, "Image compression method");
    addTreeItem(m_technicalTree, "Color Count", QString::number(m_metadata.colorCount), "Number of unique colors in the image");
}

void MetadataViewer::populateColorInfo()
{
    m_colorTree->clear();
    
    addTreeItem(m_colorTree, "Color Count", QString::number(m_metadata.colorCount), "Number of unique colors");
    addTreeItem(m_colorTree, "Dominant Colors", m_metadata.dominantColors, "Most prominent colors in the image");
    addTreeItem(m_colorTree, "Bit Depth", QString::number(m_metadata.bitDepth), "Bits per color channel");
    addTreeItem(m_colorTree, "Color Space", m_metadata.colorSpace, "Color space used");
}

void MetadataViewer::populateGpsInfo()
{
    m_gpsTree->clear();
    
    if (m_metadata.latitude != 0.0 || m_metadata.longitude != 0.0) {
        addTreeItem(m_gpsTree, "Latitude", formatGpsCoordinate(m_metadata.latitude, true), "GPS latitude coordinate");
        addTreeItem(m_gpsTree, "Longitude", formatGpsCoordinate(m_metadata.longitude, false), "GPS longitude coordinate");
        addTreeItem(m_gpsTree, "Altitude", QString::number(m_metadata.altitude, 'f', 2) + " m", "GPS altitude in meters");
        addTreeItem(m_gpsTree, "Location", m_metadata.gpsLocation, "GPS location description");
    } else {
        addTreeItem(m_gpsTree, "GPS Data", "Not available", "No GPS information found in the image");
    }
}

void MetadataViewer::populateCameraInfo()
{
    m_cameraTree->clear();
    
    addTreeItem(m_cameraTree, "Make", m_metadata.cameraMake, "Camera manufacturer");
    addTreeItem(m_cameraTree, "Model", m_metadata.cameraModel, "Camera model");
    addTreeItem(m_cameraTree, "Lens", m_metadata.lensModel, "Lens model used");
    addTreeItem(m_cameraTree, "Focal Length", m_metadata.focalLength, "Focal length of the lens");
    addTreeItem(m_cameraTree, "Aperture", m_metadata.aperture, "Aperture setting");
    addTreeItem(m_cameraTree, "Shutter Speed", m_metadata.shutterSpeed, "Shutter speed setting");
    addTreeItem(m_cameraTree, "ISO", m_metadata.iso, "ISO sensitivity setting");
    addTreeItem(m_cameraTree, "Exposure Mode", m_metadata.exposureMode, "Exposure mode used");
    addTreeItem(m_cameraTree, "White Balance", m_metadata.whiteBalance, "White balance setting");
    addTreeItem(m_cameraTree, "Flash", m_metadata.flash, "Flash setting");
}

void MetadataViewer::populateSoftwareInfo()
{
    m_softwareTree->clear();
    
    addTreeItem(m_softwareTree, "Software", m_metadata.software, "Software used to create/edit the image");
    addTreeItem(m_softwareTree, "Version", m_metadata.version, "Software version");
    addTreeItem(m_softwareTree, "Copyright", m_metadata.copyright, "Copyright information");
    addTreeItem(m_softwareTree, "Artist", m_metadata.artist, "Creator/artist name");
}

QString MetadataViewer::formatFileSize(qint64 bytes) const
{
    if (bytes < 1024) {
        return QString::number(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return QString::number(bytes / 1024.0, 'f', 1) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MB";
    } else {
        return QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 1) + " GB";
    }
}

QString MetadataViewer::formatDateTime(const QDateTime &dateTime) const
{
    if (dateTime.isValid()) {
        return dateTime.toString("yyyy-MM-dd hh:mm:ss");
    }
    return "Unknown";
}

QString MetadataViewer::formatGpsCoordinate(double coordinate, bool isLatitude) const
{
    if (coordinate == 0.0) {
        return "Not available";
    }
    
    int degrees = static_cast<int>(coordinate);
    double minutes = (coordinate - degrees) * 60;
    int minutesInt = static_cast<int>(minutes);
    double seconds = (minutes - minutesInt) * 60;
    
    QString direction;
    if (isLatitude) {
        direction = coordinate >= 0 ? "N" : "S";
    } else {
        direction = coordinate >= 0 ? "E" : "W";
    }
    
    return QString("%1° %2' %3\" %4")
        .arg(qAbs(degrees))
        .arg(minutesInt)
        .arg(seconds, 0, 'f', 2)
        .arg(direction);
}

QString MetadataViewer::formatExifValue(const QString &key, const QString &value) const
{
    // Add descriptions for common EXIF tags
    if (key == "ImageWidth") return "Width of the image in pixels";
    if (key == "ImageHeight") return "Height of the image in pixels";
    if (key == "BitsPerSample") return "Number of bits per color component";
    if (key == "Compression") return "Compression scheme used";
    if (key == "PhotometricInterpretation") return "Color space of the image data";
    if (key == "Orientation") return "Orientation of the image";
    if (key == "SamplesPerPixel") return "Number of components per pixel";
    if (key == "XResolution") return "Horizontal resolution in pixels per unit";
    if (key == "YResolution") return "Vertical resolution in pixels per unit";
    if (key == "ResolutionUnit") return "Unit for X and Y resolution";
    
    return "EXIF metadata value";
}

void MetadataViewer::addTreeItem(QTreeWidget *tree, const QString &key, const QString &value)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(tree);
    item->setText(0, key);
    item->setText(1, value);
    item->setText(2, "");
}

void MetadataViewer::addTreeItem(QTreeWidget *tree, const QString &key, const QString &value, const QString &description)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(tree);
    item->setText(0, key);
    item->setText(1, value);
    item->setText(2, description);
}

void MetadataViewer::onExportMetadata()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Metadata",
        QString(),
        "JSON Files (*.json);;Text Files (*.txt);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        QJsonObject json;
        json["fileName"] = m_metadata.fileName;
        json["filePath"] = m_metadata.filePath;
        json["imageSize"] = QJsonObject{
            {"width", m_metadata.imageSize.width()},
            {"height", m_metadata.imageSize.height()}
        };
        json["bitDepth"] = m_metadata.bitDepth;
        json["colorSpace"] = m_metadata.colorSpace;
        json["hasAlpha"] = m_metadata.hasAlpha;
        json["fileSize"] = m_metadata.fileSize;
        json["created"] = m_metadata.created.toString(Qt::ISODate);
        json["modified"] = m_metadata.modified.toString(Qt::ISODate);
        json["accessed"] = m_metadata.accessed.toString(Qt::ISODate);
        json["dpiX"] = m_metadata.dpiX;
        json["dpiY"] = m_metadata.dpiY;
        json["format"] = m_metadata.format;
        json["compression"] = m_metadata.compression;
        json["colorCount"] = m_metadata.colorCount;
        json["cameraMake"] = m_metadata.cameraMake;
        json["cameraModel"] = m_metadata.cameraModel;
        json["lensModel"] = m_metadata.lensModel;
        json["focalLength"] = m_metadata.focalLength;
        json["aperture"] = m_metadata.aperture;
        json["shutterSpeed"] = m_metadata.shutterSpeed;
        json["iso"] = m_metadata.iso;
        json["exposureMode"] = m_metadata.exposureMode;
        json["whiteBalance"] = m_metadata.whiteBalance;
        json["flash"] = m_metadata.flash;
        json["software"] = m_metadata.software;
        json["version"] = m_metadata.version;
        json["copyright"] = m_metadata.copyright;
        json["artist"] = m_metadata.artist;
        
        QJsonObject exifJson;
        for (auto it = m_metadata.exifData.begin(); it != m_metadata.exifData.end(); ++it) {
            exifJson[it.key()] = it.value();
        }
        json["exifData"] = exifJson;
        
        QJsonDocument doc(json);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            QMessageBox::information(this, "Export Complete", "Metadata exported successfully.");
        } else {
            QMessageBox::warning(this, "Export Error", "Failed to save metadata file.");
        }
    }
}

void MetadataViewer::onCopyMetadata()
{
    QString text;
    text += "Image Metadata\n";
    text += "==============\n\n";
    
    text += "Basic Information:\n";
    text += QString("File Name: %1\n").arg(m_metadata.fileName);
    text += QString("Dimensions: %1 × %2\n").arg(m_metadata.imageSize.width()).arg(m_metadata.imageSize.height());
    text += QString("Bit Depth: %1\n").arg(m_metadata.bitDepth);
    text += QString("Color Space: %1\n").arg(m_metadata.colorSpace);
    text += QString("Has Alpha: %1\n").arg(m_metadata.hasAlpha ? "Yes" : "No");
    text += QString("Format: %1\n\n").arg(m_metadata.format);
    
    text += "File Information:\n";
    text += QString("File Size: %1\n").arg(formatFileSize(m_metadata.fileSize));
    text += QString("Created: %1\n").arg(formatDateTime(m_metadata.created));
    text += QString("Modified: %1\n").arg(formatDateTime(m_metadata.modified));
    text += QString("Accessed: %1\n\n").arg(formatDateTime(m_metadata.accessed));
    
    text += "Technical Information:\n";
    text += QString("DPI X: %1\n").arg(m_metadata.dpiX);
    text += QString("DPI Y: %1\n").arg(m_metadata.dpiY);
    text += QString("Compression: %1\n").arg(m_metadata.compression);
    text += QString("Color Count: %1\n\n").arg(m_metadata.colorCount);
    
    if (!m_metadata.cameraMake.isEmpty()) {
        text += "Camera Information:\n";
        text += QString("Make: %1\n").arg(m_metadata.cameraMake);
        text += QString("Model: %1\n").arg(m_metadata.cameraModel);
        text += QString("Lens: %1\n").arg(m_metadata.lensModel);
        text += QString("Focal Length: %1\n").arg(m_metadata.focalLength);
        text += QString("Aperture: %1\n").arg(m_metadata.aperture);
        text += QString("Shutter Speed: %1\n").arg(m_metadata.shutterSpeed);
        text += QString("ISO: %1\n").arg(m_metadata.iso);
        text += QString("Exposure Mode: %1\n").arg(m_metadata.exposureMode);
        text += QString("White Balance: %1\n").arg(m_metadata.whiteBalance);
        text += QString("Flash: %1\n\n").arg(m_metadata.flash);
    }
    
    if (!m_metadata.software.isEmpty()) {
        text += "Software Information:\n";
        text += QString("Software: %1\n").arg(m_metadata.software);
        text += QString("Version: %1\n").arg(m_metadata.version);
        text += QString("Copyright: %1\n").arg(m_metadata.copyright);
        text += QString("Artist: %1\n").arg(m_metadata.artist);
    }
    
    QApplication::clipboard()->setText(text);
    QMessageBox::information(this, "Copy Complete", "Metadata copied to clipboard.");
}

void MetadataViewer::onRefreshMetadata()
{
    updateDisplay();
}

// MetadataViewerDialog implementation

MetadataViewerDialog::MetadataViewerDialog(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

MetadataViewerDialog::~MetadataViewerDialog()
{
}

void MetadataViewerDialog::setupUI()
{
    setWindowTitle("Image Metadata");
    // setModal(true); // Not needed for QWidget
    resize(800, 600);
    
    m_mainLayout = new QVBoxLayout(this);
    
    m_metadataViewer = new MetadataViewer();
    m_mainLayout->addWidget(m_metadataViewer);
    
    m_buttonsLayout = new QHBoxLayout();
    m_closeButton = new QPushButton("Close");
    m_buttonsLayout->addStretch();
    m_buttonsLayout->addWidget(m_closeButton);
    
    m_mainLayout->addLayout(m_buttonsLayout);
    
    connect(m_closeButton, &QPushButton::clicked, this, &MetadataViewerDialog::onClose);
}

void MetadataViewerDialog::setImage(const QImage &image, const QString &filePath)
{
    m_metadataViewer->setImage(image, filePath);
}

void MetadataViewerDialog::onClose()
{
    close();
}
