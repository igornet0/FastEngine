#ifndef METADATAVIEWER_H
#define METADATAVIEWER_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QFormLayout>
#include <QImage>
#include <QString>
#include <QMap>
#include <QJsonObject>

class QTreeWidgetItem;

struct ImageMetadata {
    // Basic information
    QString fileName;
    QString filePath;
    QSize imageSize;
    int bitDepth;
    QString colorSpace;
    bool hasAlpha;
    
    // File information
    qint64 fileSize;
    QDateTime created;
    QDateTime modified;
    QDateTime accessed;
    
    // EXIF data
    QMap<QString, QString> exifData;
    
    // Technical information
    int dpiX;
    int dpiY;
    QString format;
    QString compression;
    
    // Color information
    int colorCount;
    QString dominantColors;
    
    // GPS information
    double latitude;
    double longitude;
    double altitude;
    QString gpsLocation;
    
    // Camera information
    QString cameraMake;
    QString cameraModel;
    QString lensModel;
    QString focalLength;
    QString aperture;
    QString shutterSpeed;
    QString iso;
    QString exposureMode;
    QString whiteBalance;
    QString flash;
    
    // Software information
    QString software;
    QString version;
    QString copyright;
    QString artist;
    
    ImageMetadata() 
        : bitDepth(0)
        , hasAlpha(false)
        , fileSize(0)
        , dpiX(0)
        , dpiY(0)
        , colorCount(0)
        , latitude(0.0)
        , longitude(0.0)
        , altitude(0.0)
    {}
};

class MetadataViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MetadataViewer(QWidget *parent = nullptr);
    ~MetadataViewer();

    void setImage(const QImage &image, const QString &filePath = QString());
    void clearMetadata();
    ImageMetadata getMetadata() const { return m_metadata; }
    
private:
    void loadExifData(const QImage &image, const QString &filePath);

signals:
    void metadataChanged(const ImageMetadata &metadata);

private slots:
    void onExportMetadata();
    void onCopyMetadata();
    void onRefreshMetadata();

private:
    void setupUI();
    void setupConnections();
    void loadMetadata(const QImage &image, const QString &filePath);
    void updateDisplay();
    void populateBasicInfo();
    void populateFileInfo();
    void populateExifData();
    void populateTechnicalInfo();
    void populateColorInfo();
    void populateGpsInfo();
    void populateCameraInfo();
    void populateSoftwareInfo();
    
    QString formatFileSize(qint64 bytes) const;
    QString formatDateTime(const QDateTime &dateTime) const;
    QString formatGpsCoordinate(double coordinate, bool isLatitude) const;
    QString formatExifValue(const QString &key, const QString &value) const;
    
    void addTreeItem(QTreeWidget *tree, const QString &key, const QString &value);
    void addTreeItem(QTreeWidget *tree, const QString &key, const QString &value, const QString &description);

    ImageMetadata m_metadata;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // Basic information tab
    QWidget *m_basicTab;
    QVBoxLayout *m_basicLayout;
    QTreeWidget *m_basicTree;
    
    // File information tab
    QWidget *m_fileTab;
    QVBoxLayout *m_fileLayout;
    QTreeWidget *m_fileTree;
    
    // EXIF data tab
    QWidget *m_exifTab;
    QVBoxLayout *m_exifLayout;
    QTreeWidget *m_exifTree;
    
    // Technical information tab
    QWidget *m_technicalTab;
    QVBoxLayout *m_technicalLayout;
    QTreeWidget *m_technicalTree;
    
    // Color information tab
    QWidget *m_colorTab;
    QVBoxLayout *m_colorLayout;
    QTreeWidget *m_colorTree;
    
    // GPS information tab
    QWidget *m_gpsTab;
    QVBoxLayout *m_gpsLayout;
    QTreeWidget *m_gpsTree;
    
    // Camera information tab
    QWidget *m_cameraTab;
    QVBoxLayout *m_cameraLayout;
    QTreeWidget *m_cameraTree;
    
    // Software information tab
    QWidget *m_softwareTab;
    QVBoxLayout *m_softwareLayout;
    QTreeWidget *m_softwareTree;
    
    // Actions
    QHBoxLayout *m_actionsLayout;
    QPushButton *m_exportButton;
    QPushButton *m_copyButton;
    QPushButton *m_refreshButton;
};

class MetadataViewerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit MetadataViewerDialog(QWidget *parent = nullptr);
    ~MetadataViewerDialog();

    void setImage(const QImage &image, const QString &filePath = QString());

private slots:
    void onClose();

private:
    void setupUI();
    
    MetadataViewer *m_metadataViewer;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonsLayout;
    QPushButton *m_closeButton;
};

#endif // METADATAVIEWER_H
