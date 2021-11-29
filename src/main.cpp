/*!
 * \file main.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Main function passes control to QT.
 */

#include <QtGui/QApplication>
#include <QtOpenGL>

#include "cmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    cMainWindow window;
    window.show(); // displays the main window of the applicaton
    // QT takes control by calling app.exec()
    return app.exec();
}
