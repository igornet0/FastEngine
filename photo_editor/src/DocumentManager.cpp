#include "DocumentManager.h"
#include "ImageDocument.h"
#include "CanvasWidget.h"
#include "LayerPanel.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QIcon>

DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent)
    , m_tabWidget(nullptr)
    , m_currentIndex(-1)
{
    setupTabWidget();
    setupActions();
    setupConnections();
}

DocumentManager::~DocumentManager()
{
    closeAllDocuments();
    delete m_tabWidget;
}

void DocumentManager::setupTabWidget()
{
    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setUsesScrollButtons(true);
}

void DocumentManager::setupActions()
{
    m_newAction = new QAction("&New", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setIcon(QIcon(":/icons/new.png"));
    m_newAction->setToolTip("Create a new document");

    m_openAction = new QAction("&Open", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setIcon(QIcon(":/icons/open.png"));
    m_openAction->setToolTip("Open an existing document");

    m_closeAction = new QAction("&Close", this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setIcon(QIcon(":/icons/close.png"));
    m_closeAction->setToolTip("Close the current document");

    m_closeAllAction = new QAction("Close &All", this);
    m_closeAllAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_W);
    m_closeAllAction->setToolTip("Close all documents");

    m_saveAction = new QAction("&Save", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setIcon(QIcon(":/icons/save.png"));
    m_saveAction->setToolTip("Save the current document");

    m_saveAsAction = new QAction("Save &As...", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setToolTip("Save the current document with a new name");

    m_exportAction = new QAction("&Export...", this);
    m_exportAction->setShortcut(Qt::CTRL | Qt::Key_E);
    m_exportAction->setIcon(QIcon(":/icons/export.png"));
    m_exportAction->setToolTip("Export the current document");

    m_nextTabAction = new QAction("Next Tab", this);
    m_nextTabAction->setShortcut(Qt::CTRL | Qt::Key_Tab);
    m_nextTabAction->setToolTip("Switch to next tab");

    m_prevTabAction = new QAction("Previous Tab", this);
    m_prevTabAction->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab);
    m_prevTabAction->setToolTip("Switch to previous tab");
}

void DocumentManager::setupConnections()
{
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &DocumentManager::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &DocumentManager::onTabCloseRequested);
    
    connect(m_newAction, &QAction::triggered, this, &DocumentManager::onNewDocument);
    connect(m_openAction, &QAction::triggered, this, &DocumentManager::onOpenDocument);
    connect(m_closeAction, &QAction::triggered, this, &DocumentManager::onCloseDocument);
    connect(m_closeAllAction, &QAction::triggered, this, &DocumentManager::onCloseAllDocuments);
    connect(m_saveAction, &QAction::triggered, this, &DocumentManager::onSaveDocument);
    connect(m_saveAsAction, &QAction::triggered, this, &DocumentManager::onSaveAsDocument);
    connect(m_exportAction, &QAction::triggered, this, &DocumentManager::onExportDocument);
    
    connect(m_nextTabAction, &QAction::triggered, [this]() {
        if (m_documents.size() > 1) {
            int nextIndex = (m_currentIndex + 1) % m_documents.size();
            setCurrentDocument(nextIndex);
        }
    });
    
    connect(m_prevTabAction, &QAction::triggered, [this]() {
        if (m_documents.size() > 1) {
            int prevIndex = (m_currentIndex - 1 + m_documents.size()) % m_documents.size();
            setCurrentDocument(prevIndex);
        }
    });
}

int DocumentManager::createNewDocument(int width, int height, const QColor &backgroundColor)
{
    // Create new document
    ImageDocument *document = new ImageDocument(this);
    if (!document->newDocument(width, height, backgroundColor)) {
        delete document;
        return -1;
    }

    // Create canvas
    CanvasWidget *canvas = new CanvasWidget();
    canvas->setDocument(document);

    // Create layer panel
    LayerPanel *layerPanel = new LayerPanel();
    layerPanel->setDocument(document);

    // Create tab widget
    QWidget *tabWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(canvas);

    // Create document tab
    DocumentTab *tab = new DocumentTab;
    tab->document = document;
    tab->canvas = canvas;
    tab->layerPanel = layerPanel;
    tab->fileName = QString();
    tab->displayName = generateDocumentName();
    tab->isModified = false;
    tab->tabIndex = m_tabWidget->count();

    // Add to tab widget
    m_tabWidget->addTab(tabWidget, tab->displayName);
    m_documents.append(tab);

    // Set as current
    setCurrentDocument(m_documents.size() - 1);

    // Connect document signals
    connect(document, &ImageDocument::modifiedChanged, [this, tab](bool modified) {
        setDocumentModified(tab->tabIndex, modified);
    });

    updateActions();
    emit documentCreated(tab->tabIndex);
    
    return tab->tabIndex;
}

int DocumentManager::openDocument(const QString &fileName)
{
    // Check if document is already open
    for (int i = 0; i < m_documents.size(); ++i) {
        if (m_documents[i]->fileName == fileName) {
            setCurrentDocument(i);
            return i;
        }
    }

    // Create new document
    ImageDocument *document = new ImageDocument(this);
    if (!document->openDocument(fileName)) {
        delete document;
        return -1;
    }

    // Create canvas
    CanvasWidget *canvas = new CanvasWidget();
    canvas->setDocument(document);

    // Create layer panel
    LayerPanel *layerPanel = new LayerPanel();
    layerPanel->setDocument(document);

    // Create tab widget
    QWidget *tabWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(canvas);

    // Create document tab
    DocumentTab *tab = new DocumentTab;
    tab->document = document;
    tab->canvas = canvas;
    tab->layerPanel = layerPanel;
    tab->fileName = fileName;
    tab->displayName = getFileBaseName(fileName);
    tab->isModified = false;
    tab->tabIndex = m_tabWidget->count();

    // Add to tab widget
    m_tabWidget->addTab(tabWidget, tab->displayName);
    m_documents.append(tab);

    // Set as current
    setCurrentDocument(m_documents.size() - 1);

    // Connect document signals
    connect(document, &ImageDocument::modifiedChanged, [this, tab](bool modified) {
        setDocumentModified(tab->tabIndex, modified);
    });

    updateActions();
    emit documentOpened(tab->tabIndex);
    
    return tab->tabIndex;
}

bool DocumentManager::closeDocument(int index)
{
    if (index < 0 || index >= m_documents.size()) {
        return false;
    }

    DocumentTab *tab = m_documents[index];
    
    // Check if document is modified
    if (tab->isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            m_tabWidget,
            "Save Document",
            QString("The document '%1' has been modified. Do you want to save it?")
                .arg(tab->displayName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );

        if (reply == QMessageBox::Save) {
            if (!saveDocument(index)) {
                return false; // User cancelled save
            }
        } else if (reply == QMessageBox::Cancel) {
            return false; // User cancelled close
        }
    }

    // Remove from tab widget
    m_tabWidget->removeTab(index);

    // Clean up
    delete tab->canvas;
    delete tab->layerPanel;
    delete tab->document;
    delete tab;

    // Remove from list
    m_documents.removeAt(index);

    // Update tab indices
    for (int i = index; i < m_documents.size(); ++i) {
        m_documents[i]->tabIndex = i;
    }

    // Update current index
    if (m_documents.isEmpty()) {
        m_currentIndex = -1;
    } else if (m_currentIndex >= m_documents.size()) {
        m_currentIndex = m_documents.size() - 1;
    }

    updateActions();
    emit documentClosed(index);
    
    return true;
}

bool DocumentManager::closeAllDocuments()
{
    bool allClosed = true;
    
    // Close documents in reverse order to avoid index issues
    for (int i = m_documents.size() - 1; i >= 0; --i) {
        if (!closeDocument(i)) {
            allClosed = false;
            break;
        }
    }
    
    return allClosed;
}

bool DocumentManager::saveDocument(int index)
{
    if (index < 0 || index >= m_documents.size()) {
        return false;
    }

    DocumentTab *tab = m_documents[index];
    
    if (tab->fileName.isEmpty()) {
        // Save as new file
        QString fileName = QFileDialog::getSaveFileName(
            m_tabWidget,
            "Save Document",
            QString(),
            "Photo Editor Files (*.qphoto);;All Files (*)"
        );
        
        if (fileName.isEmpty()) {
            return false;
        }
        
        tab->fileName = fileName;
        tab->displayName = getFileBaseName(fileName);
        updateTabTitle(index);
    }

    bool success = tab->document->saveDocument(tab->fileName);
    if (success) {
        setDocumentModified(index, false);
    }
    
    return success;
}

bool DocumentManager::saveDocumentAs(int index, const QString &fileName)
{
    if (index < 0 || index >= m_documents.size()) {
        return false;
    }

    DocumentTab *tab = m_documents[index];
    tab->fileName = fileName;
    tab->displayName = getFileBaseName(fileName);
    updateTabTitle(index);

    bool success = tab->document->saveDocument(fileName);
    if (success) {
        setDocumentModified(index, false);
    }
    
    return success;
}

bool DocumentManager::exportDocument(int index, const QString &fileName, const QString &format)
{
    if (index < 0 || index >= m_documents.size()) {
        return false;
    }

    DocumentTab *tab = m_documents[index];
    return tab->document->exportImage(fileName, format);
}

int DocumentManager::getCurrentDocumentIndex() const
{
    return m_currentIndex;
}

void DocumentManager::setCurrentDocument(int index)
{
    if (index >= 0 && index < m_documents.size() && index != m_currentIndex) {
        m_currentIndex = index;
        m_tabWidget->setCurrentIndex(index);
        updateActions();
        emit currentDocumentChanged(index);
    }
}

ImageDocument* DocumentManager::getCurrentDocument() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_documents.size()) {
        return m_documents[m_currentIndex]->document;
    }
    return nullptr;
}

CanvasWidget* DocumentManager::getCurrentCanvas() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_documents.size()) {
        return m_documents[m_currentIndex]->canvas;
    }
    return nullptr;
}

LayerPanel* DocumentManager::getCurrentLayerPanel() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_documents.size()) {
        return m_documents[m_currentIndex]->layerPanel;
    }
    return nullptr;
}

int DocumentManager::getDocumentCount() const
{
    return m_documents.size();
}

DocumentTab* DocumentManager::getDocumentTab(int index) const
{
    if (index >= 0 && index < m_documents.size()) {
        return m_documents[index];
    }
    return nullptr;
}

QString DocumentManager::getDocumentFileName(int index) const
{
    if (index >= 0 && index < m_documents.size()) {
        return m_documents[index]->fileName;
    }
    return QString();
}

QString DocumentManager::getDocumentDisplayName(int index) const
{
    if (index >= 0 && index < m_documents.size()) {
        return m_documents[index]->displayName;
    }
    return QString();
}

bool DocumentManager::isDocumentModified(int index) const
{
    if (index >= 0 && index < m_documents.size()) {
        return m_documents[index]->isModified;
    }
    return false;
}

void DocumentManager::setDocumentModified(int index, bool modified)
{
    if (index >= 0 && index < m_documents.size()) {
        DocumentTab *tab = m_documents[index];
        if (tab->isModified != modified) {
            tab->isModified = modified;
            updateTabTitle(index);
            emit documentModifiedChanged(index, modified);
        }
    }
}

void DocumentManager::updateTabTitle(int index)
{
    if (index >= 0 && index < m_documents.size()) {
        DocumentTab *tab = m_documents[index];
        QString title = tab->displayName;
        if (tab->isModified) {
            title += " *";
        }
        m_tabWidget->setTabText(index, title);
        emit tabTitleChanged(index, title);
    }
}

void DocumentManager::updateTabIcon(int index)
{
    if (index >= 0 && index < m_documents.size()) {
        DocumentTab *tab = m_documents[index];
        QIcon icon = tab->isModified ? QIcon(":/icons/modified.png") : QIcon(":/icons/document.png");
        m_tabWidget->setTabIcon(index, icon);
    }
}

void DocumentManager::onTabChanged(int index)
{
    if (index != m_currentIndex) {
        m_currentIndex = index;
        updateActions();
        emit currentDocumentChanged(index);
    }
}

void DocumentManager::onTabCloseRequested(int index)
{
    closeDocument(index);
}

void DocumentManager::onNewDocument()
{
    createNewDocument();
}

void DocumentManager::onOpenDocument()
{
    QString fileName = QFileDialog::getOpenFileName(
        m_tabWidget,
        "Open Document",
        QString(),
        "Photo Editor Files (*.qphoto);;PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)"
    );
    
    if (!fileName.isEmpty()) {
        openDocument(fileName);
    }
}

void DocumentManager::onCloseDocument()
{
    if (m_currentIndex >= 0) {
        closeDocument(m_currentIndex);
    }
}

void DocumentManager::onCloseAllDocuments()
{
    closeAllDocuments();
}

void DocumentManager::onSaveDocument()
{
    if (m_currentIndex >= 0) {
        saveDocument(m_currentIndex);
    }
}

void DocumentManager::onSaveAsDocument()
{
    if (m_currentIndex >= 0) {
        QString fileName = QFileDialog::getSaveFileName(
            m_tabWidget,
            "Save Document As",
            QString(),
            "Photo Editor Files (*.qphoto);;PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)"
        );
        
        if (!fileName.isEmpty()) {
            saveDocumentAs(m_currentIndex, fileName);
        }
    }
}

void DocumentManager::onExportDocument()
{
    if (m_currentIndex >= 0) {
        QString fileName = QFileDialog::getSaveFileName(
            m_tabWidget,
            "Export Document",
            QString(),
            "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp);;TIFF Files (*.tiff);;All Files (*)"
        );
        
        if (!fileName.isEmpty()) {
            QString format = QFileInfo(fileName).suffix().toUpper();
            exportDocument(m_currentIndex, fileName, format);
        }
    }
}

void DocumentManager::updateActions()
{
    bool hasDocuments = !m_documents.isEmpty();
    bool hasCurrentDocument = m_currentIndex >= 0 && m_currentIndex < m_documents.size();
    bool isCurrentModified = hasCurrentDocument && m_documents[m_currentIndex]->isModified;

    m_closeAction->setEnabled(hasCurrentDocument);
    m_closeAllAction->setEnabled(hasDocuments);
    m_saveAction->setEnabled(hasCurrentDocument && isCurrentModified);
    m_saveAsAction->setEnabled(hasCurrentDocument);
    m_exportAction->setEnabled(hasCurrentDocument);
    m_nextTabAction->setEnabled(m_documents.size() > 1);
    m_prevTabAction->setEnabled(m_documents.size() > 1);
}

QString DocumentManager::generateDocumentName() const
{
    int count = m_documents.size() + 1;
    return QString("Untitled %1").arg(count);
}

QString DocumentManager::getFileBaseName(const QString &fileName) const
{
    return QFileInfo(fileName).baseName();
}







