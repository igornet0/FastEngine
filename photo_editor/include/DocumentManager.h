#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QWidget>
#include <QString>
#include <QList>
#include <QSignalMapper>
#include <QAction>

class ImageDocument;
class CanvasWidget;
class LayerPanel;

struct DocumentTab {
    ImageDocument *document;
    CanvasWidget *canvas;
    LayerPanel *layerPanel;
    QString fileName;
    QString displayName;
    bool isModified;
    int tabIndex;
};

class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(QObject *parent = nullptr);
    ~DocumentManager();

    // Document management
    int createNewDocument(int width = 800, int height = 600, const QColor &backgroundColor = Qt::white);
    int openDocument(const QString &fileName);
    bool closeDocument(int index);
    bool closeAllDocuments();
    bool saveDocument(int index);
    bool saveDocumentAs(int index, const QString &fileName);
    bool exportDocument(int index, const QString &fileName, const QString &format = "PNG");

    // Current document
    int getCurrentDocumentIndex() const;
    void setCurrentDocument(int index);
    ImageDocument* getCurrentDocument() const;
    CanvasWidget* getCurrentCanvas() const;
    LayerPanel* getCurrentLayerPanel() const;

    // Document information
    int getDocumentCount() const;
    DocumentTab* getDocumentTab(int index) const;
    QString getDocumentFileName(int index) const;
    QString getDocumentDisplayName(int index) const;
    bool isDocumentModified(int index) const;
    void setDocumentModified(int index, bool modified);

    // Tab management
    QTabWidget* getTabWidget() const { return m_tabWidget; }
    void updateTabTitle(int index);
    void updateTabIcon(int index);

    // Actions
    QAction* getNewAction() const { return m_newAction; }
    QAction* getOpenAction() const { return m_openAction; }
    QAction* getCloseAction() const { return m_closeAction; }
    QAction* getCloseAllAction() const { return m_closeAllAction; }
    QAction* getSaveAction() const { return m_saveAction; }
    QAction* getSaveAsAction() const { return m_saveAsAction; }
    QAction* getExportAction() const { return m_exportAction; }

signals:
    void documentCreated(int index);
    void documentOpened(int index);
    void documentClosed(int index);
    void documentChanged(int index);
    void currentDocumentChanged(int index);
    void documentModifiedChanged(int index, bool modified);
    void tabTitleChanged(int index, const QString &title);

private slots:
    void onTabChanged(int index);
    void onTabCloseRequested(int index);
    void onNewDocument();
    void onOpenDocument();
    void onCloseDocument();
    void onCloseAllDocuments();
    void onSaveDocument();
    void onSaveAsDocument();
    void onExportDocument();

private:
    void setupTabWidget();
    void setupActions();
    void setupConnections();
    void updateActions();
    QString generateDocumentName() const;
    QString getFileBaseName(const QString &fileName) const;
    void updateWindowTitle();

    QTabWidget *m_tabWidget;
    QList<DocumentTab*> m_documents;
    int m_currentIndex;

    // Actions
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_closeAction;
    QAction *m_closeAllAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exportAction;
    QAction *m_nextTabAction;
    QAction *m_prevTabAction;
};

#endif // DOCUMENTMANAGER_H







