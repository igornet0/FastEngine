#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    QLabel *label = new QLabel("FastEngine Qt Editor - Test", &window);
    window.setCentralWidget(label);
    window.resize(800, 600);
    window.setWindowTitle("FastEngine Qt Editor - Test");
    window.show();
    
    return app.exec();
}
