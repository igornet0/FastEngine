#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "ModernPhotoEditor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Modern Photo Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("FastEngine");
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Create and show main window
    ModernPhotoEditor window;
    window.show();
    
    return app.exec();
}
