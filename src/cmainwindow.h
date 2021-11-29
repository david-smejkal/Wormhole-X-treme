/*!
 * \file cmainwindow.h
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Base widget (window), declaration.
 */

#ifndef WINDOW_H
#define WINDOW_H


#include <QMainWindow>

class cGLWidget;
class QGLFormat;
class QSettings;
class QSlider;
class QGroupBox;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;
class QToolButton;
class QProgressBar;
class QLabel;
class QAction;
class QToolBar;
class QMenu;

class cDSettings;

/*!
 * \class cMainWindow
 * \brief Base window contains opengl widget and GUI.
 *
 * Without this particular class, notohing is displayed. It's alpha and omega of
 * this Qt application. Even settings are stored in this class and glWidget
 * communicates through this class with them.
 */
class cMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    cMainWindow();
    ~cMainWindow();
    void resizeEvent(QResizeEvent * event);
    void moveEvent(QMoveEvent * event);
    void writeSettings();
    void readSettings();

    // publicly accesible application settings
    QSettings * settings;

    QSize sizeCWidget;
    QPoint posCWidget;
    Qt::WindowStates stateCWidget;
    int screenNum;

    // object parsing progress bar
    QProgressBar * progressbar_glWidget;
    QVBoxLayout * vboxlayout_glWidget;
    QLabel * label_progressbar;

    // statusbar widgets
    QLabel * label_bestscore;

    // actions changed from glWidgets
    QAction * playPauseAction;
    QAction * fullscreenAction;

    // SETTINGS - VARIABLES
    int settings_bestscore;
    int settings_antialiasing;
    int settings_multisampling;
    bool settings_recreateGL;
    bool settings_navigation;
    QString settings_difficulty;
    QString settings_object;
    int settings_polygons;
//    GLenum settings_polygonMode;

signals:
    void startGameLoop();

public slots:
    void execSettingsDialog();

private slots:
    void reCreateGLWidget();
    void about();
    void aboutOpenGL();
    void help();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void evalSettings(cDSettings * dialog_settings);
    void createNonGLWidgetConnections();
    void createGLWidgetConnections();

    QSlider * createWhSectorsSlider();
    QSlider * createCircleSectorsSlider();

    QGLFormat * glWidget_format;
    cGLWidget * glWidget;
    cDSettings * dialog_settings;

    QSlider * circleSectorsSlider;
    QSlider * whSectorsSlider;

    QComboBox * polygonModeComboBox;

    QWidget * widget_progressBar;

    QMenu * gameMenu;
    QMenu * settingsMenu;
    QMenu * helpMenu;
    QToolBar * wormholeToolBar;
    QToolBar * gameToolBar;
    QAction * resetAction;
    QAction * settingsAction;
    QAction * aboutAction;
    QAction * aboutQtAction;
    QAction * aboutOpenGLAction;
    QAction * helpAction;
    QAction * exitAction;

};


#endif
