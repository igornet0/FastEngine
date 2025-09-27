#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QDockWidget>
#include <QLabel>

class SimpleMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("FastEngine Qt Editor - Simple Test");
        setMinimumSize(800, 600);
        
        // Центральный виджет
        QTextEdit *centralWidget = new QTextEdit(this);
        centralWidget->setText("FastEngine Qt Editor - Simple Test\n\nThis is a basic test of the main window structure.");
        setCentralWidget(centralWidget);
        
        // Меню
        QMenuBar *menuBar = this->menuBar();
        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("&New", this, &QWidget::close);
        fileMenu->addAction("&Open", this, &QWidget::close);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close);
        
        // Статусная строка
        statusBar()->showMessage("Ready");
        
        // Dock панель
        QDockWidget *dockWidget = new QDockWidget("Test Panel", this);
        QLabel *dockLabel = new QLabel("This is a test dock widget", dockWidget);
        dockWidget->setWidget(dockLabel);
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimpleMainWindow window;
    window.show();
    
    return app.exec();
}

#include "simple_main.moc"
