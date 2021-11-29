/*!
 * \file cglwidget.h
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * OpenGL widget, declaration.  This widget is heart of the application.
 */

#ifndef CGLWIDGET_H
#define CGLWIDGET_H

#include "myinclude.h"

#include <QGLWidget>
#include <QTimer>
#include <QTime>

class cMainWindow;
class cGLObject;
class cWormhole;
class cUfo;

/*!
 * \class cGLWidget
 * \brief OpenGL widget, heart of the application. Calculations, painting, etc.
 *
 * This class is of the upmost immportance. Whole engine and graphical
 * OpenGL operations are implemented within this class. This class closely
 * interacts with it's friend class, cMainWindow.
 */
class cGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    cGLWidget(QGLFormat fmt, cMainWindow *parent = 0);
    ~cGLWidget();

    QSize minimumSizeHint() const {return QSize(50, 50);}
    QSize sizeHint() const {return QSize(400, 400);}

    friend class cMainWindow;

public slots:
    void recreateWormhole();
    void setCircleSectors(int sectors);
    void setWhSectors(int sectors);
    void setPolygonMode(QString mode);

    void playPause();
    void reset();
    void toggleFullScreen();
//    void gameLoop();

private slots:
    void animate();

signals:
    void circleSectorsChanged(int sectors);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void setAAMS();
    void myglAlignVectorToVector(float servant_x, float servant_y,
                                 float servant_z, float master_x,
                                 float master_y, float master_z);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void moveObjects();
    void checkCollisions();
    void setScore();
    float pointToLineDistance(sPoint3 point, sPoint3 A, sPoint3 B);
    void resetCamera(bool perspective = false);
    void resetUfo();

    int collision;
    float distance;
    float score;

private:
    int LoadTextureFromBMP(char *filename, GLuint texture_name);
    void checkWormhole();

    GLuint textureWormhole;

    int width; // width of glWidget (useful when rendering textures)
    int height;

    cUfo *obj1;
    cGLObject *obj2;
    cGLObject *obj3;
    cWormhole *wormhole;
    cUfo *ufo;

    int xRot;
    int yRot;
    int zRot;
    int key_code;

    QTime lastPaintTime;

    QTime fpsTime;
    int fps;
    QString strFps;
    QString strScore;

    bool bPause;

    bool bFullScreen;
    GLboolean bFirstInit;

    GLenum polygonMode;
    cMainWindow * parentCWidget;
    QGLFormat format;

    float flmXrot, flmYrot, flmZrot; // free look mode rotations
    float flmX, flmY, flmZ; // free look mode camera position

    float objCamZoom; // play camera zoom
    float objCamXrot, objCamYrot;

    float objForward;
    float objXrot, objZrot; // object local rotations
    float effectXrot, effectZrot; // effect of rotation on object


    float camX, camY, camZ; // point of camera (eye) view (gluLookAt())
    float upX, upY, upZ; // up vector in gluLookAt()


    bool bUp, bDown, bLeft, bRight, bSpace; // key down booleans

    float piover180;
    QPoint lastPos;
    QTimer animationTimer;
};


#endif
