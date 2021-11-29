/*!
 * \file cglobject.h
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Globject declaration. This class offers virtual functions for cUfo and
 * cWormhole classes.
 */

#include "myinclude.h"
#include "cobj2ogl.h"

#ifndef CGLOBJECT_H
#define CGLOBJECT_H


#include <QGLWidget>

//class cObj2OGL;


/*!
 * \class cGLObject
 * \brief Basic model for every openGL object in scene (wormhole, ufo, etc.).
 *
 * This object supplies 2 virtual functions for its descendants. One for
 * just creating display list and second for making object by parsing obj file.
 * This so called parsing will be achieved by creating and using cObj2OGL
 * instance.
 */
class cGLObject : public QGLWidget
{
public:
    cGLObject();
    ~cGLObject();
    cGLObject(QString str);
    //virtual GLuint makeObject() =0;
    virtual void makeObject(QProgressBar * progress_bar = NULL,
                            QLabel * progress_label = NULL) =0;
    virtual GLuint makeDisplayList(int polygons = 0) =0;
    //virtual GLuint makeObject(QProgressDialog * progress_dialog = NULL) =0;
    //virtual void updateObject() =0;

    int nLists;
    QString objFile;
    GLuint object;
    cObj2OGL *obj2OGL;

    QColor green;
    QColor purple;

//protected:
    void quad(sPoint3 normal1, sPoint3 point1,\
              sPoint3 normal2, sPoint3 point2,\
              sPoint3 normal3, sPoint3 point3,\
              sPoint3 normal4, sPoint3 point4);
    void triangle(sPoint3 normal1, sPoint3 point1,\
                  sPoint3 normal2, sPoint3 point2,\
                  sPoint3 normal3, sPoint3 point3);

};


#endif // CGLOBJECT_H
