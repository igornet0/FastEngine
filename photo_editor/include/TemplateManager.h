#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSize>
#include <QColor>
#include <QJsonObject>
#include <QHash>
#include <QList>
#include <QImage>

class QWidget;

struct TemplateLayer {
    QString name;
    QString type; // "raster", "text", "shape"
    QRect geometry;
    QColor backgroundColor;
    QString text;
    QString fontFamily;
    int fontSize;
    QColor textColor;
    QString imagePath;
    float opacity;
    bool visible;
    
    TemplateLayer() 
        : fontSize(12)
        , opacity(1.0f)
        , visible(true)
    {}
};

struct ProjectTemplate {
    QString name;
    QString displayName;
    QString description;
    QString category;
    QSize size;
    QColor backgroundColor;
    QList<TemplateLayer> layers;
    QString thumbnailPath;
    QStringList tags;
    QString author;
    QString version;
    QDateTime created;
    QDateTime modified;
    bool isCustom;
    
    ProjectTemplate() 
        : isCustom(false)
    {}
};

class TemplateManager : public QObject
{
    Q_OBJECT

public:
    explicit TemplateManager(QObject *parent = nullptr);
    ~TemplateManager();

    // Template management
    void loadTemplates();
    void saveTemplates();
    
    QStringList getAvailableTemplates() const;
    QStringList getTemplatesByCategory(const QString &category) const;
    QStringList getCategories() const;
    
    ProjectTemplate getTemplate(const QString &name) const;
    void addTemplate(const ProjectTemplate &template_);
    void removeTemplate(const QString &name);
    void updateTemplate(const ProjectTemplate &template_);
    
    // Template creation
    ProjectTemplate createTemplate(const QString &name, const QString &description, 
                                 const QSize &size, const QColor &backgroundColor = Qt::white);
    void addLayerToTemplate(ProjectTemplate &template_, const TemplateLayer &layer);
    
    // Template application
    bool applyTemplate(const QString &templateName, QWidget *targetWidget);
    QImage generateThumbnail(const ProjectTemplate &template_, const QSize &thumbnailSize = QSize(200, 200));
    
    // Import/Export
    bool exportTemplate(const ProjectTemplate &template_, const QString &filePath);
    bool importTemplate(const QString &filePath);
    bool exportTemplateSet(const QStringList &templateNames, const QString &filePath);
    bool importTemplateSet(const QString &filePath);
    
    // Search and filter
    QStringList searchTemplates(const QString &query) const;
    QStringList getTemplatesByTag(const QString &tag) const;
    QStringList getRecentTemplates(int count = 10) const;
    
    // Categories
    void addCategory(const QString &category);
    void removeCategory(const QString &category);
    void setTemplateCategory(const QString &templateName, const QString &category);
    
    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void templateAdded(const QString &templateName);
    void templateRemoved(const QString &templateName);
    void templateUpdated(const QString &templateName);
    void templatesLoaded();
    void templatesSaved();

private:
    void setupDefaultTemplates();
    void loadTemplateFromFile(const QString &filePath);
    void saveTemplateToFile(const ProjectTemplate &template_, const QString &filePath);
    QString getTemplateFilePath(const QString &templateName) const;
    QString getTemplatesDirectory() const;
    
    QHash<QString, ProjectTemplate> m_templates;
    QStringList m_categories;
    QString m_templatesDirectory;
    QStringList m_recentTemplates;
};

class TemplateSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateSelectorWidget(QWidget *parent = nullptr);
    ~TemplateSelectorWidget();

    void setTemplateManager(TemplateManager *manager);
    TemplateManager* getTemplateManager() const { return m_templateManager; }

signals:
    void templateSelected(const ProjectTemplate &template_);
    void templateDoubleClicked(const ProjectTemplate &template_);

private slots:
    void onCategoryChanged();
    void onSearchChanged();
    void onTemplateClicked();
    void onTemplateDoubleClicked();
    void onNewTemplate();
    void onImportTemplate();
    void onExportTemplate();
    void onDeleteTemplate();

private:
    void setupUI();
    void setupConnections();
    void updateTemplateList();
    void updateTemplateList(const QStringList &templateNames);
    void createTemplateItem(const ProjectTemplate &template_);
    
    TemplateManager *m_templateManager;
    
    // UI components
    QVBoxLayout *m_mainLayout;
    
    // Search and filter
    QHBoxLayout *m_searchLayout;
    QLineEdit *m_searchEdit;
    QComboBox *m_categoryCombo;
    QPushButton *m_clearButton;
    
    // Template grid
    QScrollArea *m_scrollArea;
    QWidget *m_scrollWidget;
    QGridLayout *m_gridLayout;
    
    // Actions
    QHBoxLayout *m_actionsLayout;
    QPushButton *m_newButton;
    QPushButton *m_importButton;
    QPushButton *m_exportButton;
    QPushButton *m_deleteButton;
    QPushButton *m_refreshButton;
    
    // Data
    QStringList m_displayedTemplates;
    QString m_currentCategory;
    QString m_searchQuery;
};

class TemplateItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateItemWidget(const ProjectTemplate &template_, QWidget *parent = nullptr);
    ~TemplateItemWidget();

    ProjectTemplate getTemplate() const { return m_template; }

signals:
    void clicked(const ProjectTemplate &template_);
    void doubleClicked(const ProjectTemplate &template_);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    void updateDisplay();
    
    ProjectTemplate m_template;
    bool m_hovered;
    
    // UI components
    QVBoxLayout *m_layout;
    QLabel *m_thumbnailLabel;
    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_sizeLabel;
};

#endif // TEMPLATEMANAGER_H







