/*!
 * \file cglwidget.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * OpenGL widget, definition. This widget is heart of the application.
 */

#include <QtGui>
#include <QtOpenGL>
//#include <QTime>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#ifndef GL_RESCALE_NORMAL
#define GL_RESCALE_NORMAL 0x803A
#endif

#include "vec3.h"
#include "cmainwindow.h"
#include "cglobject.h"
#include "cufo.h"
#include "cwormhole.h"
#include "cglwidget.h"

// Space Ship Mode matrix
GLdouble ssmMatrix[16] = {1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1};
// Free Look Mode matrix
GLdouble flmMatrix[16] = {1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1};
// Temporary matrix used for some auxiliary calculations
GLdouble tmpMatrix[16] = {1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1};

/*!
 * \brief One of the most important consructors in this application.
 *
 * Initializing some variables, setting timers and creating objects for
 * rendering.
 */
cGLWidget::cGLWidget(QGLFormat fmt, cMainWindow *parent)
    : QGLWidget(fmt, parent)
{
    setWindowTitle(tr("Wormhole X-treme FullScreen"));
    
    // setting focus policy (to accept key pressing in this widget)
    setFocusPolicy(Qt::StrongFocus);

    // catching parent parameter
    parentCWidget = parent;
    format = fmt;

    polygonMode = GL_FILL;

    bFullScreen = false;
    bPause = true;
    bDown = false;
    bUp = false;
    bLeft = false;
    bRight = false;
    bSpace = false;

    score = 0;
    collision = 0;

    fps = 0;
    strFps = QString(" FPS: %1").arg(fps, 0, 10);
    strScore = QString(" SCORE: %1").arg(fps, 0, 10);
    fpsTime.start();

    key_code = 64;

    piover180= 0.01745329252;

    // movement
    animationTimer.start(10);
    lastPaintTime.start();

    // ufo camera data
    objCamZoom = 0.0;
    objCamXrot = objCamYrot = 0.0;

    // free look mode data
    flmX = flmY = flmZ = 0.0;
    flmXrot = flmYrot = flmZrot = 0.0;

    // object data
    objForward = 0.0;
    objXrot = objZrot = 0.0;
    effectXrot = effectZrot = 0.0;

    // real camera coordinates
    camX = camY = camZ = 0.0;

    // up vector (of object)
    upX = 0.0;
    upY = 1.0;
    upZ = 0.0;

    // random seed
    QTime midnight(0,0,0);
    //midnight.setHMS(0, 0, 0, 0); // .setHMS(0, 0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));


    /* SETTING TIMERS */
    // animation timer (start/stop in playPause() and reset() methods)
    animationTimer.setSingleShot(false); // repeat timer over and over
    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));

    /* GAME OBJECTS */
    wormhole = new cWormhole;
    ufo = new cUfo(parentCWidget->settings_object);

    ufo->radius = 0.01;
    ufo->pos.x = ufo->pos.y = ufo->pos.z = 0.0;

    bFirstInit = GL_TRUE;

    wormhole->makeObject();

    ufo->makeObject(parentCWidget->progressbar_glWidget,
                    parentCWidget->label_progressbar);

    //obj1->makeObject(parentCWidget->progressbar_glWidget,
    //                 parentCWidget->label_progressbar);

    // setting object comunicating with widgets
    //parentCWidget->whSectorsSlider->setValue(wormhole->whSectors);
    //parentCWidget->circleSectorsSlider->setValue(wormhole->circleSectors);

}

/*!
 * \brief Destroys textures, display lists and scene objects.
 *
 * Very importatnt destructor. Freeing memory in a place such as this is duty.
 * Not freeing memory in a place such as this may result in serious application
 * malfunctions. Textures, display lists and instances of cUfo and cWormhole
 * are properly freed in this method.
 */
cGLWidget::~cGLWidget()
{
    makeCurrent();

    // textures
    glDeleteTextures( 1, &textureWormhole);

    // display lists
    glDeleteLists(wormhole->object, 1);
    glDeleteLists(ufo->object, 1);
    //glDeleteLists(obj1->object, 1);

    delete wormhole;
    delete ufo;
    //delete obj1;

}

/*!
 * \brief Initializes GL.
 *
 * Creating display lists, setting shaders, lights, etc.
 *
 * \sa cGLWidget::paintGL(), cGLWidget::resizeGL()
 */
void cGLWidget::initializeGL()
{
    qglClearColor(wormhole->purple.dark());
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    /* normalizing */
    //glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_NORMALIZE);

    /* flat / smooth */
    //glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);

    /* Anti-aliasing / Multisampling */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    setAAMS();

    /* fill / lines / points */
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    /* MATERIAL */
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    // glCullFace(GL_BACK);
    // glEnable(GL_CULL_FACE);

    /* LIGHTS */
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    GLfloat light1_ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light1_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light1_position[] = {0.0, 0.0, 0.0, 1.0};
    //GLfloat spot_direction[] = {0.0, 0.0, -0.2};

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    //glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
    //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.9);
    //glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 2.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);

    //glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 135.0);
    //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    //glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

    glEnable(GL_LIGHT1);

    // set point light in 0,0,0 (viewport)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);


    /* TEXTURES */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);        /* zpusob ulozeni bytu v texture */
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the original
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glEnable(GL_TEXTURE_2D);

    /* initializeGL() is called once whenever the widget has been assigned a
    new QGLContext (e.g. toggle fullscreen). However making display lists from
    obj files takes some time and therefore will be called only once. We don't
    need to load textures either.*/
    if(bFirstInit)
    {
        // textures
        glGenTextures(1, &textureWormhole);
        if(LoadTextureFromBMP("./images/wormhole_texture.bmp", textureWormhole))
            glBindTexture(GL_TEXTURE_2D, textureWormhole);

        wormhole->object =
            wormhole->makeDisplayList(parentCWidget->settings_polygons);
        ufo->object = ufo->makeDisplayList();
        //obj1->object = obj1->makeDisplayList();
        //obj2->object = obj2->makeDisplayList();
        //obj3->object = obj3->makeDisplayList();
        bFirstInit = GL_FALSE;

        resetUfo();
        resetCamera(true);
    }
}

/*!
 * \brief Resizes GL.
 *
 * Setting viewport and mapping xyz axes.
 *
 * \sa cGLWidget::initializeGL(), cGLWidget::paintGL()
 */
void cGLWidget::resizeGL(int w, int h)
{
    width = w;
    height = h;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (GLdouble) width/height, 0.0001, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

/*!
 * \brief Actual painting.
 *
 * Setting scene and calling display lists. Propably the longest method in
 * application, various improvements were made to make application run smoother.
 * But there is always something that can be improved. The decision on us is to
 * consider development of these improvements beneficial or not, compared to the
 * invested energy and time.
 *
 * \sa cGLWidget::initializeGL(), cGLWidget::resizeGL()
 */
void cGLWidget::paintGL()
{
    /* FPS calculation */
    if(fpsTime.elapsed() < 1000)
    {
        fps += 1;
    }
    else
    {
        strFps = QString(" FPS: %1").arg(fps, 0, 10);
        fps = 0;
        fpsTime.restart();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* DYNAMIC SETTINGS >> */

    // polygone mode (fill / line / point)
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    if(polygonMode == GL_POINT) glPointSize(4.0);
    else glPointSize(1.0);

    // Anti-aliasing / Multisampling
    setAAMS();

    /* << DYNAMIC SETTINGS */

    if(bPause) // free look mode
    {
        glLoadIdentity();
        if ((flmXrot != 0.0) || (flmYrot != 0.0) || (flmZrot != 0.0) ||
            (flmX != 0.0) || (flmY != 0.0) || (flmZ != 0.0)) {
            if (flmXrot != 0.0)
            {
                glRotatef(flmXrot/16., 1,0,0);
                flmXrot = 0.0;
            }
            if (flmYrot != 0.0)
            {
                glRotatef(flmYrot/16., 0,1,0);
                flmYrot = 0.0;
            }
            if (flmZrot != 0.0)
            {
                glRotatef(flmZrot/16., 0,0,1);
                flmZrot = 0.0;
            }
            //glTranslatef (0, 0, 0.1);
            glTranslatef(flmX, flmY, flmZ);
            flmX = flmY = flmZ = 0.0;
            //glTranslatef (0, 0, -0.1);
            glMultMatrixd(flmMatrix);
            glGetDoublev(GL_MODELVIEW_MATRIX, flmMatrix);
        }

        // paint scene
        glLoadIdentity();
        glMultMatrixd (flmMatrix);

        // draw ufo
        glPushMatrix();
//            glTranslatef (0.0, 0.0, -0.1); // move object away from camera
            glMultMatrixd(ssmMatrix);
            qglColor(QColor::fromRgb(150, 150, 150));
            glCallList(ufo->object);
        glPopMatrix();

        // draw wormhole
        glBindTexture(GL_TEXTURE_2D, textureWormhole);
        glEnable(GL_TEXTURE_2D);
            glCallList(wormhole->object);
        glDisable(GL_TEXTURE_2D);

        // draw spline dots in wormhole (navigation)
        if(parentCWidget->settings_navigation)
        {

            glPointSize(5.0);
            qglColor(QColor::fromRgb(255, 0, 0));
            glBegin(GL_POINTS);
                for (int i=0; i<wormhole->whSectors; i++)
                {
                    glVertex3f(wormhole->sectors[i].splinePoint.x,
                               wormhole->sectors[i].splinePoint.y,
                               wormhole->sectors[i].splinePoint.z);
                }
            glEnd();
            glPointSize(1.0);
        }

    }
    else // space ship mode
    {
        // local rotation and translation of objects (ufo, ..) and camera
        glLoadMatrixd(ssmMatrix);
            glRotatef(objXrot, 1.0, 0.0, 0.0);
            glRotatef(objZrot, 0.0, 0.0, 1.0);
            glTranslatef(0,0,objForward);
        glGetDoublev(GL_MODELVIEW_MATRIX, ssmMatrix);

        // object position
        ufo->pos.x = ssmMatrix[12];
        ufo->pos.y = ssmMatrix[13];
        ufo->pos.z = ssmMatrix[14];

        glLoadMatrixd(ssmMatrix);
            glTranslatef(0.0, 0.0, 0.001);
        glGetDoublev(GL_MODELVIEW_MATRIX, tmpMatrix);

        // eye position (used in gluLookAt() function)
        camX = tmpMatrix[12];
        camY = tmpMatrix[13];
        camZ = tmpMatrix[14];

        glLoadMatrixd(ssmMatrix);
            glTranslatef(0.0, 1.00, 0.0);
        glGetDoublev(GL_MODELVIEW_MATRIX, tmpMatrix);

        // up vector shall not move with object, just rotate
        upX = tmpMatrix[12] - ufo->pos.x;
        upY = tmpMatrix[13] - ufo->pos.y;
        upZ = tmpMatrix[14] - ufo->pos.z;

        objXrot = objZrot = 0.0;
        objForward = 0.0;

        // paint scene
        glLoadIdentity();

        // draw ufo
        glTranslatef (0.0, 0.0, -(ufo->radius*12.0)+objCamZoom);
        glRotatef(objCamXrot/16, 1.0, 0.0, 0.0);
        glRotatef(objCamYrot/16, 0.0, 1.0, 0.0);
        glPushMatrix();
            glRotatef(effectXrot, 1.0, 0.0, 0.0);
            glRotatef(effectZrot, 0.0, 0.0, 1.0);
            qglColor(QColor::fromRgb(150, 150, 150));
            glCallList(ufo->object);
        glPopMatrix();

        // set camera
        gluLookAt(camX, camY, camZ, ufo->pos.x, ufo->pos.y, ufo->pos.z, upX, upY, upZ);

        // draw wormhole
        glBindTexture(GL_TEXTURE_2D, textureWormhole);
        glEnable(GL_TEXTURE_2D);
            glCallList(wormhole->object);
        glDisable(GL_TEXTURE_2D);

        // draw spline dots in wormhole (avigation)
        if(parentCWidget->settings_navigation)
        {
            glPointSize(5.0);
            qglColor(QColor::fromRgb(255, 0, 0));
            glBegin(GL_POINTS);
                for (int i=0; i<wormhole->whSectors; i++)
                {
                    glVertex3f(wormhole->sectors[i].splinePoint.x,
                               wormhole->sectors[i].splinePoint.y,
                               wormhole->sectors[i].splinePoint.z);
                }
            glEnd();
            glPointSize(1.0);
        }
    }
    // text rendering
    strScore = QString(" SCORE: %1").arg(score, 10);
    glColor3f(1.0, 0.0, 0.0);
    renderText(10, 10, strFps);
    glColor3f(1.0, 1.0, 0.0);
    renderText(width - 10 - strScore.size()*4, 10, strScore);
}

/*!
 * \brief Calculates distance between line and point.
 *
 * Calculates distance between line and point using cross product technique.
 */
float cGLWidget::pointToLineDistance(sPoint3 point, sPoint3 A, sPoint3 B)
{
    float d;
    //d = fabs((x0 - x1) x (x0 - x2))/fabs(x2 - x1)
    vec3 x01(point.x - A.x, point.y - A.y, point.z - A.z);
    vec3 x02(point.x - B.x, point.y - B.y, point.z - B.z);
    vec3 x21(B.x - A.x, B.y - A.y, B.z - A.z);
    d = fabs(x01.CrossProduct(x02).Magnitude())/fabs(x21.Magnitude());
    return d;
}

/*!
 * \brief Aligns vector1 to vector2.
 *
 * Rotates servant vector to mach master vector orientation.
 */
void cGLWidget::myglAlignVectorToVector(float servant_x, float servant_y,
                                        float servant_z, float master_x,
                                        float master_y, float master_z)
{
    vec3 u(servant_x, servant_y, servant_z);
    vec3 v(master_x, master_y, master_z);
    vec3 n = u.CrossProduct(v);
    n = n / n.Magnitude();
    float phi = acos(u.Dot(v)/(u.Magnitude()*v.Magnitude()));

    /* n    - unit vector along the axis of rotation
       phi  - angle of rotation */
    glRotatef(phi/piover180, n.x, n.y, n.z);

//    // rotation matrix
//    float t = 1-cos(phi);
//    float a = n.x;
//    float b = n.y;
//    float c = n.z;
//    float s=sin(phi);
//    float d=cos(phi);
//    GLdouble rot[16] = {t*a*a+d,     t*a*b + s*c, t*a*c - s*b, 0,
//                        t*a*b-s*c,   t*b*b+d,     t*b*c+s*a,   0,
//                        t*a*c + s*b, t*b*c - a*s, t*c*c + d,   0,
//                        0,           0,           0,           1};
//
//    glMultMatrixd(rot);
}

/*!
 * \brief Set Anti-aliasing / Multisampling.
 *
 * Sets anti-aliasing (AA) and multisampling (MS) according to settings.
 */
void cGLWidget::setAAMS()
{
    if(parentCWidget->settings_multisampling != 0 && format.sampleBuffers())
    {
        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POINT_SMOOTH);
        glEnable(GL_MULTISAMPLE);
    } else
    {
        glDisable(GL_MULTISAMPLE);
        // Anti-aliasing
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        if(parentCWidget->settings_antialiasing == 1)
        {
            glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
            glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
            glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
        } else
        if(parentCWidget->settings_antialiasing == 2)
        {
            glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
            glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
            glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
        } else
        if(parentCWidget->settings_antialiasing == 3)
        {
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
        } else
        {
            glDisable(GL_BLEND);
            glDisable(GL_POLYGON_SMOOTH);
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POINT_SMOOTH);
        }
    }
}

/*!
 * \brief Toggle Pause/Play.
 *
 * Toggle through pause and play state of the game.
 *
 * \note public slot
 */
void cGLWidget::playPause()
{
    bPause=!bPause;
    if(bPause)
    { // PAUSE
        parentCWidget->playPauseAction->setIcon(QIcon("./images/play.png"));
        parentCWidget->playPauseAction->setToolTip(tr("Play"));

        resetCamera();

        animationTimer.start(10);
    }
    else
    { // PLAY
        parentCWidget->playPauseAction->setIcon(QIcon("./images/pause.png"));
        parentCWidget->playPauseAction->setToolTip(tr("Pause"));

        resetCamera();

        lastPaintTime.restart();
        animationTimer.start(0);
    }
    this->setFocus();
}

/*!
 * \brief Resets the game.
 *
 * Create new wormhole and reset position of spaceship. This method is usualy
 * called upon detected collision.
 *
 * \note public slot
 */
void cGLWidget::reset()
{
    bPause = true;

    wormhole->initializeWormholeCoordinates();
    recreateWormhole();

    resetUfo();
    resetCamera(true);

    parentCWidget->playPauseAction->setIcon(QIcon("./images/play.png"));
    parentCWidget->playPauseAction->setToolTip(tr("Play"));
}

/*!
 * \brief Calculates movement of objects.
 *
 * Gravitation, velocity, etc.
 */
void cGLWidget::moveObjects()
{
    float lpTime = lastPaintTime.elapsed();

    if(bSpace)
    {
        //lpTime += 15;
        objForward -= 0.002 * lpTime;
    }

    if(bDown)
    {
        objXrot += 0.1 * lpTime;
        flmZ -= 0.001 * lpTime;
        if(effectXrot < 30.0) // cca 30°
            effectXrot += 0.1 * lpTime;
    } else if(!bUp && effectXrot > 0.0)
    {
        effectXrot -= 0.1 * lpTime;
    }

    if(bUp)
    {
        objXrot -= 0.1 * lpTime;
        flmZ += 0.001 * lpTime;
        if(effectXrot > -30.0) // cca -30°
            effectXrot -= 0.1 * lpTime;
    } else if(!bDown && effectXrot < 0.0)
    {
        effectXrot += 0.1 * lpTime;
    }

    if(bLeft)
    {
        objZrot += 0.1 * lpTime;
        flmX += 0.001 * lpTime;
        if(effectZrot < 30.0) // cca 30°
            effectZrot += 0.1 * lpTime;
    } else if(!bRight && effectZrot > 0.0)
    {
        effectZrot -= 0.1 * lpTime;
    }

    if(bRight)
    {
        objZrot -= 0.1 * lpTime;
        flmX -= 0.001 * lpTime;
        if(effectZrot > -30.0) // cca -30°
            effectZrot -= 0.1 * lpTime;
    } else if(!bLeft && effectZrot < 0.0)
    {
        effectZrot += 0.1 * lpTime;
    }

    objForward -= 0.0005 * lpTime;
    lastPaintTime.restart();
}

/*!
 * \brief Animates scene.
 *
 * This private slot is binded to a timer that forces it to do its duty
 * everytime it's prime time finishes. On paused mode timer is set to 10ms and
 * on play mode timer is set to 0ms. So this method will be called on each
 * event loop. On paused mode, it will get called cca 100 times per second.
 * Method ensures object movement recalculation, collision detection, wormhole
 * generation, score calculation and REPAINT on each pass!
 *
 * \sa cGLWidget::animationTimer
 * \note private slot
 */
void cGLWidget::animate()
{
    moveObjects();
    if(!bPause)
    {
        checkCollisions();
        checkWormhole(); // whether new sector needs to be generated
        setScore();
    }
    updateGL();
}

/*!
 * \brief Check for collisions.
 *
 * Collision detection is implemented in a simple manner. Basicly in each
 * collision check, distance of the space ship from the nearest line created by
 * two spline point is compared to the radius of the corresponding wormhole
 * sector. The distance is adjusted to capture marginal collisions of spaceship.
 */
void cGLWidget::checkCollisions()
{
    collision = 0;
    for (int j=1; j<this->wormhole->whSectors-1; j++)
    {
        if((this->wormhole->sectors[j-1].splinePoint.x < this->ufo->pos.x) &&
           (this->ufo->pos.x < this->wormhole->sectors[j].splinePoint.x))
        {

             //d = fabs((x0 - x1) x (x0 - x2))/fabs(x2 - x1)
             distance = pointToLineDistance(ufo->pos,
                                            wormhole->sectors[j-1].splinePoint,
                                            wormhole->sectors[j].splinePoint);
            if(wormhole->sectors[j-1].radius <= distance + ufo->radius)
            {
                collision += 1;
            }
        }
    }

    if(collision != 0)
    {
        if(parentCWidget->settings_bestscore < score)
        {
            parentCWidget->settings_bestscore = (int) score;
            parentCWidget->label_bestscore->setText(
                QString("Best score: ") +
                QString().number(parentCWidget->settings_bestscore, 10) +
                QString(" "));
        }
        score = 0;
        reset();
        reset();
    }
}

/*!
 * \brief Set game score.
 *
 * Calculate game score. The multiplication by 10 is a psychological reason. It
 * gives player more satisfaction to see a big score like 100 or 1000.
 */
void cGLWidget::setScore()
{
    if(ufo->pos.x * 10 > score)
    {
        score = (int) ufo->pos.x * 10;
    }
}

/*!
 * \brief Reset camera.
 *
 * Resets camera position and rotation. Look at ufo from perspective.
 *
 * \sa cGLWidget::resetObject()
 */
void cGLWidget::resetCamera(bool perspective)
{
    // stop moving
    bUp = false;
    bDown = false;
    bLeft = false;
    bRight = false;
    bSpace = false;

    flmXrot = flmYrot = flmZrot = 0.0;
    flmX = flmY = flmZ = 0.0;
    objCamZoom = 0.0;
    objCamXrot = objCamYrot = 0.0;

    objXrot = objZrot = 0.0;
    objForward = 0.0;

    glLoadIdentity();
        if(perspective)
        {
            gluLookAt(ufo->pos.x - 5*ufo->radius,
                      ufo->pos.y + 5*ufo->radius,
                      ufo->pos.z + 5*ufo->radius,
                      ufo->pos.x, ufo->pos.y, ufo->pos.z,
                      0.0, 1.0, 0.0);
        }
        else
        {
            glTranslatef(0.0, 0.0, -(ufo->radius*12.0));
            gluLookAt(camX, camY, camZ,
                      ufo->pos.x, ufo->pos.y, ufo->pos.z,
                      upX, upY, upZ);
        }
    glGetDoublev(GL_MODELVIEW_MATRIX, flmMatrix);
}

/*!
 * \brief Reset ufo.
 *
 * Set ufo to face the entrance of wormhole.
 *
 * \sa cGLWidget::resetCamera()
 */
void cGLWidget::resetUfo()
{
    objXrot = objZrot = 0.0;
    objForward = 0.0;
    camY = camZ = 0.0;
    camX = -0.001;

    ufo->pos.x = ufo->pos.y = ufo->pos.z = 0.0;
    collision = 0;

    glLoadIdentity();
    glRotatef(-90, 0.0, 1.0, 0.0);
    glGetDoublev(GL_MODELVIEW_MATRIX, ssmMatrix);
}

//void cGLWidget::gameLoop()
//{
//    /*
//    while( user doesn't exit )
//        check for user input
//        run AI
//        move enemies
//        resolve collisions
//        draw graphics
//        play sounds
//    end while
//    */
//    //playPause();
//    //while(1)
//    //{
//    //    qApp->processEvents();
//    //    update();
//    //}
//
//}

/*!
 * \brief Load texture to opengl memory.
 *
 * After we call gluBuild2DMipmaps, the image data are copied into the OpenGL
 * system, and possibly into the video card's memory. This ensures faster
 * performance.
 *
 */
int cGLWidget::LoadTextureFromBMP(char *filename, GLuint texture_name)
{
  FILE *fin;
  int size,width,height;
  unsigned short bitcount = 0;
  unsigned char *pixels;
  unsigned char bmpHeader[54];

  if ((fin = fopen(filename, "rb")) == NULL)
    return 1;

  if (fread(bmpHeader, 1, 54, fin) != 54) { // nacitanie BMP hlavicky
    fclose(fin);
    return 1;
  }

  memcpy(&bitcount, &(bmpHeader[28]), 2); // kontrola poctu bodov
  if (bitcount != 24) {
    fclose(fin);
    return 1;
  }

  memcpy(&width, &(bmpHeader[18]), 4); // rozmer obrazu
  memcpy(&height, &(bmpHeader[22]), 4);
  size = width*height*3;

  if ((pixels = (unsigned char *) malloc(size)) == NULL) {
    fclose(fin);
    return 1;
  }

  if (fread(pixels, 1, size, fin) != (unsigned) size) { // nacitanie obrazku
    fclose(fin);
    free(pixels);
    return 1;
  }

  fclose(fin);

  // build our texture mipmaps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                     GL_RGB, GL_UNSIGNED_BYTE, pixels);

  glBindTexture(GL_TEXTURE_2D, texture_name); // pripojenie textur
  // nastavenie parametrov texturi
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, pixels);

  free(pixels); // uvolnenie allocovanej pamate
  return 0;
}

/*!
 * \brief Checks whether new sectors need to be generated.
 *
 * If object coordinates pass through middle sector in wormhole, new sectors
 * are generated and appended. Old ones from beginning are removed. Number of
 * generated and removed sectors is quarter from number of control points.
 */
void cGLWidget::checkWormhole()
{
    if(ufo->pos.x > wormhole->sectors[wormhole->whSectors/2].splinePoint.x)
    {
        sPoint3 tmpPoint;
        for(int i = 0; i < wormhole->nControlPoints/4; i++)
        {
            wormhole->listControlPoints.removeFirst();
            tmpPoint.x = wormhole->listControlPoints.last().x + 1.0;
            tmpPoint.y = (rand() % 2000 - 1000) / 1000.0;
            tmpPoint.z = (rand() % 2000 - 1000) / 1000.0;
            wormhole->listControlPoints.append(tmpPoint);

            wormhole->listControlRadiusPoints.removeFirst();
            tmpPoint.x = wormhole->listControlRadiusPoints.last().x + 1.0;
            tmpPoint.y = (rand() % 400 + 200) / 1000.0;
            tmpPoint.z = 0.0;
            wormhole->listControlRadiusPoints.append(tmpPoint);
        }
        recreateWormhole();
//        lastPaintTime.restart();
    }
}

/*!
 * \brief Method ensures proper recreation of a wormhole.
 *
 * First of all, wormhole data are updated, old display list is freed and new
 * one is created.
 *
 * \note public slot
 */
void cGLWidget::recreateWormhole()
{
    wormhole->updateObject(wormhole->whSectors, wormhole->circleSectors);
    makeCurrent();
    glDeleteLists(wormhole->object, wormhole->nLists);
    wormhole->object =
        wormhole->makeDisplayList(parentCWidget->settings_polygons);
//    updateGL();
}

/*!
 * \brief Recreation of a wormhole with new sectors specified.
 *
 * First of all, wormhole data are updated, old display list is freed and new
 * one is created. Solot manipulated from cMainWindow.
 *
 * \note public slot
 */
void cGLWidget::setCircleSectors(int sectors)
{
    if (sectors >= 3 && 200 >= sectors) {
        wormhole->updateObject(wormhole->whSectors, sectors);
        makeCurrent();
        glDeleteLists(wormhole->object, wormhole->nLists);
        wormhole->object =
            wormhole->makeDisplayList(parentCWidget->settings_polygons);
        updateGL();
    }
}

/*!
 * \brief Recreation of a wormhole with new circle sectors specified.
 *
 * First of all, wormhole data are updated, old display list is freed and new
 * one is created. Solot manipulated from cMainWindow.
 *
 * \note public slot
 */
void cGLWidget::setWhSectors(int sectors)
{
    if (sectors >= 20 && 400 >= sectors) {
        wormhole->updateObject(sectors, wormhole->circleSectors);
        makeCurrent();
        glDeleteLists(wormhole->object, wormhole->nLists);
        wormhole->object =
            wormhole->makeDisplayList(parentCWidget->settings_polygons);
        updateGL();
    }
}

/*!
 * \brief Slot that changes polygon mode.
 *
 * This slot is manipulated from cMainWindow's polygonModeComboBox.
 *
 * \note public slot
 */
void cGLWidget::setPolygonMode(QString mode)
{
    if(mode == "GL_FILL") polygonMode = GL_FILL;
    else if(mode == "GL_LINE") polygonMode = GL_LINE;
        else if(mode == "GL_POINT") polygonMode = GL_POINT;
            else polygonMode = GL_FILL;
    updateGL();
}

/*!
 * \brief Handles key pressing.
 *
 * Reimplements event for handlin key pressings. Implemented as one long switch.
 *
 * \sa cGLWidget::mousePressEvent()
 */
void cGLWidget::keyPressEvent(QKeyEvent *event){
    key_code = event->key();
    switch(event->key())
    {
        case Qt::Key_Space :
            bSpace = true;
            break;
        case Qt::Key_Up :
        case Qt::Key_W :
            bUp = true;
            //flmZ += 0.01;
            break;
        case Qt::Key_Down :
        case Qt::Key_S :
            bDown = true;
            //flmZ -= 0.01;
            break;
        case Qt::Key_Left :
        case Qt::Key_A :
            bLeft = true;
            //flmX += 0.01;
            break;
        case Qt::Key_Right :
        case Qt::Key_D :
            bRight = true;
            //flmX -= 0.01;
            break;
        case Qt::Key_F : // fullscreen
            toggleFullScreen();
            break;
        case Qt::Key_C : // reset camera on flm and center on play mode
            resetCamera();
            break;
        case Qt::Key_P : // Play / Pause
            playPause();
            break;
        case Qt::Key_R : // Restart
            reset();
            break;
        case Qt::Key_Q :
        case Qt::Key_Escape :
            QCoreApplication::quit();
            break;
        default:
            break;
    }

    event->ignore();
}

/*!
 * \brief Handles key releasing.
 *
 * Simulate keyDown event (not in Qt) by setting booleans on keyPressEvent and
 * keyReleaseEvent.
 *
 * \sa cGLWidget::keyPressEvent(), cGLWidget::focusOutEvent()
 * \note virtual method of QWidget
 */
void cGLWidget::keyReleaseEvent(QKeyEvent *event){
    key_code = event->key();
    switch(event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_W:
            bUp = false;
            break;
        case Qt::Key_Down:
        case Qt::Key_S :
            bDown = false;
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            bLeft = false;
            break;
        case Qt::Key_Right:
        case Qt::Key_D :
            bRight = false;
            break;
        case Qt::Key_Space :
            bSpace = false;
        default:
            break;
    }

    event->ignore();
}

/*!
 * \brief Handles focus out event.
 *
 * Release all keydown booleans on focus out event.
 *
 * \sa cGLWidget::keyReleaseEvent()
 * \note virtual method of QWidget
 */
void cGLWidget::focusOutEvent(QFocusEvent * event)
{
    bUp = false;
    bDown = false;
    bLeft = false;
    bRight = false;
    bSpace = false;

    event->ignore();
}

/*!
 * \brief Handles mouse pressing.
 *
 * Storing position on left and right mouse button pressing. Reset camera modes
 * on mid mouse button press.
 *
 * \sa cGLWidget::mouseMoveEvent(), cGLWidget::wheelEvent()
 */
void cGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton ||
        event->buttons() & Qt::RightButton)
    {
        lastPos = event->pos();
    } else if(event->buttons() & Qt::MidButton)
    {
        resetCamera();
    }

    event->ignore();
}

/*!
 * \brief Handles mouse movement.
 *
 * In case of left or right mouse button is pressed set rotations.
 *
 * \sa cGLWidget::mousePressEvent
 */
void cGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        flmXrot += 8 * dy;
        flmYrot += 8 * dx;
        objCamXrot += 8 * dy;
        objCamYrot += 8 * dx;
    } else if (event->buttons() & Qt::RightButton)
    {
        flmZrot += 8 * dx;
    }
    lastPos = event->pos();

    event->ignore();
}

/*!
 * \brief Handles mouse wheel.
 *
 * In case of mouse wheel is rotated.
 *
 * \sa cGLWidget::mousePressEvent, cGLWidget::mouseMoveEvent()
 */
void cGLWidget::wheelEvent(QWheelEvent *event)
{
    /* Most mouse types work in steps of 15 degrees, in which case the delta
       value is a multiple of 120 */
    int numDegrees = event->delta() / 120;
    float numSteps = numDegrees / 10.0;

    if (event->orientation() == Qt::Horizontal) {
        flmZ += numSteps;
        if((objCamZoom + numSteps / 10.0) < 0.05 &&
           (objCamZoom + numSteps / 10.0) > -0.2)
            objCamZoom += numSteps / 10.0;
    } else {
        flmZ += numSteps;
        if((objCamZoom + numSteps / 10.0) < 0.05 &&
           (objCamZoom + numSteps / 10.0) > -0.2)
            objCamZoom += numSteps / 10.0;
    }

    event->ignore();
}

/*!
 * \brief Handles mouse doubleclick event.
 *
 * In case of mouse doubleclick on OpenGL kontext, toggle fullscreen.
 *
 * \sa cGLWidget::mousePressEvent, cGLWidget::wheelEvent()
 */
void cGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        toggleFullScreen();
    }
}

/*!
 * \brief Inteligent method for toggling fullscreen.
 *
 * Method is implemented for n number of screens. It will span properly on them.
 * It will pause the game if needed.
 *
 * \note public slot
 */
void cGLWidget::toggleFullScreen()
{
    bFullScreen = !bFullScreen;
    if(bPause == false)
        this->playPause();

    if(bFullScreen)
    { // go full screen
        parentCWidget->fullscreenAction->\
                setIcon(QIcon("./images/window_nofullscreen.png"));
        parentCWidget->stateCWidget = parentCWidget->windowState();
        if(!(parentCWidget->stateCWidget & 7)) // not (minimized or maximized
        {                                      //      or fullscreen)
            parentCWidget->sizeCWidget = parentCWidget->size();
            parentCWidget->posCWidget = parentCWidget->pos();
        }
        parentCWidget->screenNum = QApplication::desktop()->\
                                   screenNumber(parentCWidget);
        parentCWidget->hide();
        this->setParent(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        this->setGeometry(QApplication::desktop()->\
                          screenGeometry(parentCWidget->screenNum));
        this->resize((QApplication::desktop()->\
                      screenGeometry(parentCWidget->screenNum)).size());
        this->show();
        this->setFocus();
    }
    else
    { // go back to windowed mode
        parentCWidget->fullscreenAction->\
                setIcon(QIcon("./images/window_fullscreen.png"));
        parentCWidget->setCentralWidget(this);
        parentCWidget->setGeometry(QApplication::desktop()->\
                                   availableGeometry(parentCWidget->screenNum));
        parentCWidget->move(parentCWidget->posCWidget);
        parentCWidget->resize(parentCWidget->sizeCWidget);

        parentCWidget->setWindowState(parentCWidget->stateCWidget);
        parentCWidget->show();
        this->setFocus();
    }
}
