/*!
 * \file cmainwindow.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Base widget (window), definition.
 */

#include <QtGui>

#include "cmainwindow.h"
#include "cglwidget.h"
#include "cdsettings.h"

/*!
 * \brief Constructs the main window of the application.
 *
 * Creates components (e.g. glWidget) and layout components for whole window.
 * GUI consists of one menubar, one statusbar, two toolbars and one settings
 * dialog. Title of the main window is also specified in this method.
 */
cMainWindow::cMainWindow()
{
    // specified for QSettings to work properly
    QCoreApplication::setOrganizationName("FIT");
    QCoreApplication::setOrganizationDomain("fit.vutbr.com");
    QCoreApplication::setApplicationName("Wormhole X-treme");
    settings = new QSettings();
    readSettings();

    glWidget_format = new QGLFormat();
    glWidget = NULL;

    dialog_settings = new cDSettings(this);
    dialog_settings->setWindowTitle(tr("cDSettings"));

    this->setWindowTitle(tr("Wormhole X-treme"));

    stateCWidget = this->windowState();
    sizeCWidget = this->size();
    posCWidget = this->pos();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    widget_progressBar = new QWidget;
    progressbar_glWidget = new QProgressBar;
    label_progressbar = new QLabel;
    vboxlayout_glWidget = new QVBoxLayout;
    label_progressbar->setText(QString("Loading..."));
    label_progressbar->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    vboxlayout_glWidget->addStretch();
    vboxlayout_glWidget->addWidget(label_progressbar);
    vboxlayout_glWidget->addWidget(progressbar_glWidget);
    vboxlayout_glWidget->addStretch();
    progressbar_glWidget->setValue(0);
    widget_progressBar->setLayout(vboxlayout_glWidget);
    setCentralWidget(widget_progressBar);

    createNonGLWidgetConnections(); // except connections to glWidgets

    QTimer::singleShot(100, this, SLOT(reCreateGLWidget()));
//    //reCreateGLWidget();
}

/*!
 * \brief Destroys everything.
 *
 * This time freeing allocated memory is not so crucial as in other destructors,
 * since life of whole application comes to an end after this particular method.
 * Qt can and propably will free every pice of forgoten memory.
 * However a good habit is a good habit. Why shall Qt do the dirty job for
 * us and what would happen if it would not? After all at least settings need
 * to be saved.
 */
cMainWindow::~cMainWindow()
{
    writeSettings();

    delete glWidget_format;
    delete dialog_settings;
    delete glWidget;   
}

/*!
 * \brief This method handles creation of actions.
 *
 * Making actions instead of buttons is very handy. Action can be used in
 * toolbar and then again in menubar or practicly anywhere else. If we use
 * action in toolbar it gets a look and feel of a button and so on. Notice
 * that for each action a status tip and icon is specified. Icon will be used in
 * menubar and toolbar and statustip will be displayed in statusbar on mouseover
 * over the action.
 */
void cMainWindow::createActions()
{
    resetAction = new QAction(QIcon("./images/reset.png"), tr("&Reset"), this);
    resetAction->setShortcut(tr("Ctrl+R"));
    resetAction->setStatusTip(tr("Reset game"));

    playPauseAction = new QAction(QIcon("./images/play.png"),
                                  tr("&Play"), this);
    playPauseAction->setShortcut(tr("Ctrl+P"));
    playPauseAction->setStatusTip(tr("Play game / Pause game"));

    fullscreenAction = new QAction(QIcon("./images/window_fullscreen.png"),
                                   tr("&Fullscreen"), this);
    fullscreenAction->setShortcut(tr("Ctrl+F"));
    fullscreenAction->setStatusTip(tr("Toggle fullscreen"));

    settingsAction = new QAction(QIcon("./images/settings.png"),
                                 tr("&Settings"), this);
    settingsAction->setShortcut(tr("Ctrl+S"));
    settingsAction->setStatusTip(tr("Launch settings dialog"));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));

    aboutOpenGLAction = new QAction(tr("About Open&GL"), this);
    aboutOpenGLAction->setStatusTip(tr("Show info about OpenGL."));

    helpAction = new QAction(QIcon("./images/help.png"), tr("&Help"), this);
    helpAction->setStatusTip(tr("Show the help box"));

    exitAction = new QAction(QIcon("./images/exit.png"), tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));

}

/*!
 * \brief This method handles creation of menubar.
 *
 * Create and fill 3 menu items (gameMenu, settingsMenu and helpMenu).
 * This method is implemented for limpidity reasons. Actions get inserted in
 * each menu item.
 */
void cMainWindow::createMenus()
{
    gameMenu = menuBar()->addMenu(tr("&Game"));
    gameMenu->addAction(playPauseAction);
    gameMenu->addAction(resetAction);
    gameMenu->addSeparator();
    gameMenu->addAction(exitAction);

    settingsMenu = menuBar()->addMenu(tr("&Settings"));
    settingsMenu->addAction(fullscreenAction);
    settingsMenu->addAction(settingsAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
    helpMenu->addAction(aboutOpenGLAction);
    helpMenu->addAction(helpAction);
}

/*!
 * \brief This method handles creation of toolbars.
 *
 * Making the core transparent, toolbars are created in a separate method.
 * Actions are also used to fill this toolbars and this time some widgets get
 * inserted as well (e.g. polygonModeComboBox to wormholeToolBar).
 */
void cMainWindow::createToolBars()
{
    circleSectorsSlider = createCircleSectorsSlider();
    circleSectorsSlider->setToolTip(tr("Wormhole circle sectors"));
    circleSectorsSlider->setStatusTip(tr("Wormhole circle sectors"));
    //circleSectorsSlider->setTracking(false);

    whSectorsSlider = createWhSectorsSlider();
    whSectorsSlider->setToolTip(tr("Wormhole sectors"));
    whSectorsSlider->setStatusTip(tr("Wormhole sectors"));
    //whSectorsSlider->setTracking(false);

    polygonModeComboBox = new QComboBox;
    polygonModeComboBox->setToolTip(tr("Polygon mode"));
    polygonModeComboBox->addItem("GL_FILL");
    polygonModeComboBox->addItem("GL_LINE");
    polygonModeComboBox->addItem("GL_POINT");

    QGroupBox *groupBox = new QGroupBox(tr("Wormhole"));
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(whSectorsSlider);
    vbox->addWidget(circleSectorsSlider);
    groupBox->setMaximumWidth(100);
    groupBox->setLayout(vbox);

    wormholeToolBar = addToolBar(tr("Wormhole"));
    addToolBar(Qt::RightToolBarArea, wormholeToolBar);
    wormholeToolBar->addWidget(groupBox);
    wormholeToolBar->addWidget(polygonModeComboBox);
    //addToolBar ( Qt::ToolBarArea area, QToolBar * toolbar )

    gameToolBar = addToolBar(tr("Game"));
    addToolBar(Qt::TopToolBarArea, gameToolBar);
    gameToolBar->addAction(resetAction);
    gameToolBar->addAction(playPauseAction);
    gameToolBar->addSeparator();
    gameToolBar->addAction(fullscreenAction);
    gameToolBar->addAction(settingsAction);
    gameToolBar->addSeparator();
    gameToolBar->addAction(helpAction);
}

/*!
 * \brief Shor and simple method for creating statusbar.
 *
 * Statusbar shows tooltips and score.
 * Some things are in Qt so simple that maybe an own method for creating GUI
 * widgets makes code less transparent. But who knows if this statusbar method
 * won't expand in future development.
 */
void cMainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    label_bestscore = new QLabel(QString("Best score: ") +
                                 QString().number(settings_bestscore, 10) +
                                 QString(" "));
    statusBar()->addPermanentWidget(label_bestscore);
}

/*!
 * \brief Saves chosen variables for future execution of the application.
 *
 * This method is extremely handy. XML parsers are needed no more.
 * Cross-platform settings by this and cMainWindow::readSettings() method are
 * guaranteed. The only things apart this two methods is to fill some basic
 * information (rganization name, domain and application name) in constructor
 * of the main window.
 *
 * \sa cMainWindow::readSettings()
 */
void cMainWindow::writeSettings()
{
    settings->beginGroup("MainWindow");
        settings->setValue("size", size());
        settings->setValue("pos", pos());
    settings->endGroup();

    settings->beginGroup("GLWidget");
        settings->setValue("recreateGL", settings_recreateGL);
        settings->setValue("antialiasing", settings_antialiasing);
        settings->setValue("multisampling", settings_multisampling);
        settings->setValue("bestscore", settings_bestscore);
    settings->endGroup();

    settings->beginGroup("Difficulty");
        settings->setValue("navigation",  settings_navigation);
        settings->setValue("difficulty",  settings_difficulty);
    settings->endGroup();

    settings->beginGroup("SpaceShip");
        settings->setValue("object",  settings_object);
    settings->endGroup();

    settings->beginGroup("Wormhole");
        settings->setValue("polygons",  settings_polygons);
    settings->endGroup();

//    settings->beginGroup("ToolBar");
//        settings->setValue("polygonMode",  settings_polygonMode);
//    settings->endGroup();
}

/*!
 * \brief Loads chosen variables with saved values.
 *
 * This method is extremely handy. XML parsers are needed no more.
 * Cross-platform settings by this and cMainWindow::writeSettings() method are
 * guaranteed. The only things apart this two methods is to fill some basic
 * information (rganization name, domain and application name) in constructor
 * of the main window.
 *
 * \sa cMainWindow::writeSettings()
 */
void cMainWindow::readSettings()
{
    settings->beginGroup("MainWindow");
        resize(settings->value("size", QSize(500, 400)).toSize());
        move(settings->value("pos", QPoint(100, 100)).toPoint());
    settings->endGroup();

    settings->beginGroup("GLWidget");
        settings_recreateGL = settings->value("recreateGL", false).toBool();
        settings_antialiasing = settings->value("antialiasing", 0).toInt();
        settings_multisampling = settings->value("multisampling", 0).toInt();
        settings_bestscore = settings->value("bestscore", 0).toInt();
    settings->endGroup();

    settings->beginGroup("Difficulty");
        settings_navigation = settings->value("navigation", true).toBool();
        settings_difficulty = settings->value("difficulty", QString("Easy")).toString();
    settings->endGroup();

    settings->beginGroup("SpaceShip");
        settings_object = settings->value("object", QString("small_ship.obj")).toString();
    settings->endGroup();

    settings->beginGroup("Wormhole");
        // 0 - triangles, 1 - quads
        settings_polygons = settings->value("polygons", 0).toInt();
    settings->endGroup();

//    settings->beginGroup("ToolBar");
//        settings_polygonMode = settings->value("plygonMode", 0).toInt();
//    settings->endGroup();
}

/*!
 * \brief Simple method for creating particular slider.
 *
 * This method specifies properties for creation of whSectorsSlider.
 */
QSlider *cMainWindow::createWhSectorsSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(100, 400);
    slider->setSingleStep(1);
    slider->setPageStep(10);
    slider->setTickInterval(30);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setValue(200);
    return slider;
}

/*!
 * \brief Simple method for creating particular slider.
 *
 * This method specifies properties for creation of circleSectorsSlider.
 */
QSlider *cMainWindow::createCircleSectorsSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(20, 70);
    slider->setSingleStep(1);
    slider->setPageStep(5);
    slider->setTickInterval(5);
    slider->setTickPosition(QSlider::TicksRight);
    slider->setValue(25);
    return slider;
}

/*!
 * \brief Method that recreates OpenGL kontext (glWidget).
 *
 * First of all, why do we need to recreate glWidget? Well the main reason is
 * for making it possible to turn of and on multisampling. Multisampling needs
 * to have special format of QGLWidget window and this format must be specified
 * before creation of such widget. So in order to support turning multisample
 * technique on and off, the recreation of glWidget is needed.
 * Furthermore while parsing an object in glWidget constructor a progressbar is
 * shown.
 */
void cMainWindow::reCreateGLWidget()
{
    // Try to enable MultiSampling 2x 4x 8x?
    if(this->settings_multisampling != 0 && glWidget_format->doubleBuffer())
    {
        // Try to enable DoubleBuffering
        glWidget_format->setSampleBuffers(true);
        glWidget_format->setDoubleBuffer(true);
        glWidget_format->setSamples(this->settings_multisampling * 2);
    } else
    {
        // Try to enable DoubleBuffering
        glWidget_format->setDoubleBuffer(true);
        glWidget_format->setSampleBuffers(false);
    }

    if(glWidget != NULL)
    {
        glWidget->close();
        delete glWidget;

        widget_progressBar = new QWidget;
        progressbar_glWidget = new QProgressBar;
        label_progressbar = new QLabel;
        vboxlayout_glWidget = new QVBoxLayout;
        label_progressbar->setText(QString("Loading..."));
        label_progressbar->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        vboxlayout_glWidget->addStretch();
        vboxlayout_glWidget->addWidget(label_progressbar);
        vboxlayout_glWidget->addWidget(progressbar_glWidget);
        vboxlayout_glWidget->addStretch();
        progressbar_glWidget->setValue(0);
        widget_progressBar->setLayout(vboxlayout_glWidget);
        setCentralWidget(widget_progressBar);

        qApp->processEvents();
    }

    this->setDisabled(true);
    glWidget = new cGLWidget(*glWidget_format, this);
    this->setDisabled(false);

    label_progressbar->close();
    delete label_progressbar;
    progressbar_glWidget->close();
    delete progressbar_glWidget;
    delete vboxlayout_glWidget;
    delete widget_progressBar;

    createGLWidgetConnections();
    setCentralWidget(glWidget);

    glWidget->setFocus();

    // if not double buffer then no sample buffer
    if(glWidget_format->sampleBuffers() && !glWidget_format->doubleBuffer())
        reCreateGLWidget();
}

/*!
 * \brief Creates connection between cMainWindow object and cGLWidget.
 *
 * After each recreation of glWidget, recreation of it's connections is
 * very important. This is what this method is about.
 */
void cMainWindow::createGLWidgetConnections()
{
    connect(circleSectorsSlider, SIGNAL(valueChanged(int)),\
            glWidget, SLOT(setCircleSectors(int)));
    connect(whSectorsSlider, SIGNAL(valueChanged(int)),\
            glWidget, SLOT(setWhSectors(int)));
    connect(polygonModeComboBox, SIGNAL(currentIndexChanged(const QString &)),\
            glWidget, SLOT(setPolygonMode(QString)));

    connect(resetAction, SIGNAL(triggered()), glWidget, SLOT(reset()));
    connect(playPauseAction, SIGNAL(triggered()), glWidget, SLOT(playPause()));
    
    connect(fullscreenAction, SIGNAL(triggered()),
            glWidget, SLOT(toggleFullScreen()));
}

/*!
 * \brief Creates connection of cMainWindow object apart those with cGLWidget.
 *
 * This method needs to be called only once at the end of main window
 * constructor.
 */
void cMainWindow::createNonGLWidgetConnections()
{
    connect(settingsAction, SIGNAL(triggered()),
            this, SLOT(execSettingsDialog()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(aboutOpenGLAction, SIGNAL(triggered()), this, SLOT(aboutOpenGL()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

/*!
 * \brief Shows settings dialog.
 *
 * This slot is bind to an action called settingsAction. This slot shows modal
 * settings dialog uppon it's execution.
 *
 * \note public slot
 */
void cMainWindow::execSettingsDialog()
{
    if(!glWidget->bPause)
        glWidget->playPause();

    if(glWidget->bFullScreen)
        glWidget->toggleFullScreen();

    dialog_settings->setSettingsDialog();
    dialog_settings->exec();

    QTimer::singleShot(10, glWidget, SLOT(recreateWormhole()));

    if(this->settings_recreateGL == true)
    {
        QTimer::singleShot(100, this, SLOT(reCreateGLWidget()));
    }
}

/*!
 * \brief Shows aboutg.
 *
 * Just simple message box with about.
 *
 * \note private slot
 */
void cMainWindow::about()
{
    QMessageBox::about(this, tr("Wormhole X-treme About"),
             tr("<b>Wormhole X-treme</b><br>"
                "is an OpenGL game developed under Qt<br>"
                "(cross-platform application and UI framework).<br><br>"
                "Application version: <b>1.0</b><br><br>"
                "Author: David Smejkal<br>"
                "Brno University of Technology,<br>"
                "<b>F</b>aculty of <b>I</b>nformation <b>T</b>echnology"));

}

/*!
 * \brief Shows message box with OpenGL info.
 *
 * This message box shows preferences of created OpenGL kontext, if some techniq
 * seems to be not working properly, check here, whether it is supported on your
 * hardware / software.
 *
 * \note private slot
 */
void cMainWindow::aboutOpenGL()
{
    QString str = "<b>OpenGL supported Versions:</b><br>";
    int OGLVersion = glWidget_format->openGLVersionFlags();
    if(OGLVersion & QGLFormat::OpenGL_Version_None)
        str += "None, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_1_1)
        str += "1.1, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_1_2)
        str += "1.2, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_1_3)
        str += "1.3, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_1_4)
        str += "1.4, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_1_5)
        str += "1.5, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_2_0)
        str += "2.0, ";
    if(OGLVersion & QGLFormat::OpenGL_Version_2_1)
        str += "2.1, ";
    if(OGLVersion & QGLFormat::OpenGL_ES_CommonLite_Version_1_0)
        str += "ES 1.0 CommonLite, ";
    if(OGLVersion & QGLFormat::OpenGL_ES_Common_Version_1_0)
        str += "ES 1.0 Common, ";
    if(OGLVersion & QGLFormat::OpenGL_ES_CommonLite_Version_1_1)
        str += "ES 1.1 CommonLite, ";
    if(OGLVersion & QGLFormat::OpenGL_ES_Common_Version_1_1)
        str += "ES 1.1 Common, ";
    if(OGLVersion & QGLFormat::OpenGL_ES_Version_2_0)
        str += "ES 2.0, ";
    str.chop(2);
    str += "<br><br>";

    str += "<b>OpenGL active techniques:</b><br>";

    str += "DoubleBuffering - ";
    if(glWidget_format->doubleBuffer()) str += "Yes<br>";
    else str += "No<br>";

    str += "AutoBufferSwap - ";
    if(glWidget->autoBufferSwap()) str += "Yes<br>";
    else str += "No<br>";

    str += "DepthBuffering - ";
    if(glWidget_format->depth()) str += "Yes<br>";
    else str += "No<br>";

    str += "ColorMode - ";
    if(glWidget_format->rgba()) str += "RGBA<br>";
    else str += "ColorIndex.<br>";

    str += "AlphaBuffering - ";
    if(glWidget_format->alpha()) str += "Yes<br>";
    else str += "No<br>";

    str += "AccumulationBuffering - ";
    if(glWidget_format->accum()) str += "Yes<br>";
    else str += "No<br>";

    str += "StencilBuffering - ";
    if(glWidget_format->stencil()) str += "Yes<br>";
    else str += "No<br>";

    str += "StereoBuffering - ";
    if(glWidget_format-> stereo()) str += "Yes<br>";
    else str += "No<br>";

    str += "DirectRendering - ";
    if(glWidget_format->directRendering()) str += "Yes<br>";
    else str += "No<br>";

    str += "OverlayPlane - ";
    if(glWidget_format->hasOverlay()) str += "Yes<br>";
    else str += "No<br>";

    str += "MultisampleBuffering - ";
    if(glWidget_format->sampleBuffers())
        str += QString().number(glWidget_format->samples()) + "x<br>";
    else str += "No<br>";

    QMessageBox::about(this, tr("OpenGL Info"), str);
}

/*!
 * \brief Shows message box with Game info.
 *
 * This info shows basic goal of the game and basic movement bindings.
 *
 * \note private slot
 */
void cMainWindow::help()
{
    QMessageBox::about(this, tr("Wormhole X-treme Help"),
                       tr("<b>Goal:</b><br>"
                          "<br>"
                          "Fly through wormhole and try to survive<br>"
                          "as long as you can. Stay away from deadly<br>"
                          "surface of the wormhole!<br>"
                          "<br>"
                          "Good luck, Commander...<br>"
                          "<br><br>"
                          "<b>Controls:</b><br>"
                          "<br>"
                          "(general bindings)<br>"
                          "Q, Esc         - Exit<br>"
                          "F, DoubleClick - toggle fullscreen<br>"
                          "C              - center camera<br>"
                          "R              - reset game<br>"
                          "P              - toggle pause<br>"
                          "<br>"
                          "(paused - free look mode)<br>"
                          "UpArrow,    W  - camera forwards<br>"
                          "DownArrow,  A  - camera backwards<br>"
                          "LeftArrow,  S  - camera strafe-left<br>"
                          "RightArrow, D  - camera strafe-right<br>"
                          "Scroll         - camera forwards/backwards<br>"
                          "LMB, RMB       - look around<br>"
                          "MMB            - reset camera position<br>"
                          "<br>"
                          "(unpaused - game mode)<br>"
                          "UpArrow,    W  - spaceship twist down<br>"
                          "DownArrow,  A  - spaceship twist up<br>"
                          "LeftArrow,  S  - spaceship twist left<br>"
                          "RightArrow, D  - spaceship twist right<br>"
                          "Space          - turbo<br>"
                          "LMB            - rotate camera around object<br>"
                          "MMB            - center camera<br>"
                          "Scroll         - zoom<br>"
                          ));

}

/*!
 * \brief Reimplementation of window resize event.
 *
 * If this window is not maximized, minimized or fullscreen, capture its
 * size. This will be used in cGLWidget::toggleFullScreen() method.
 */
void cMainWindow::resizeEvent(QResizeEvent * event)
{
    if(!(this->windowState() & 7)) // not (minimized or maximized or fullscreen)
    {
        sizeCWidget = event->size();
    }
}

/*!
 * \brief Reimplementation of window move event.
 *
 * If this window is not maximized, minimized or fullscreen, capture its
 * position. This will be used in cGLWidget::toggleFullScreen() method.
 */
void cMainWindow::moveEvent(QMoveEvent * event)
{
    if(!(this->windowState() & 7)) // not (minimized or maximized or fullscreen)
    {
        posCWidget = event->pos();
    }
}
