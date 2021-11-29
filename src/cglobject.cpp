/*!
 * \file cglobject.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Basic globject definition. This class offers virtual functions for cUfo and
 * cWormhole classes.
 */

#include "myinclude.h"
#include "cobj2ogl.h"
#include "cglobject.h"

/*!
 * \brief Constructor of cGLObject.
 *
 * Just initializing some variables.
 */
cGLObject::cGLObject()
{
    // initializing data
    object = 0;

    green = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    purple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    obj2OGL = new cObj2OGL();
}

/*!
 * \brief Destructor of cGLObject.
 *
 * Deleting obj2OGL object.
 */
cGLObject::~cGLObject()
{
    delete obj2OGL;
}

/*!
 * \brief Another constructor of cGLObject.
 *
 * If QString parameter is supplied, version of cGLObject with cObj2OGL object
 * will be construted.
 *
 * \sa cGLObject::cGLObject()
 */
cGLObject::cGLObject(QString str)
{
    // initializing data
    object = 0;
    objFile = str;

    green = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    purple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    obj2OGL = new cObj2OGL();
}

/*!
 * \brief Auxiliary method.
 *
 * Helpful method for rendering quad. Will be used in cWormhole.
 */
void cGLObject::quad(sPoint3 normal1, sPoint3 point1,
                     sPoint3 normal2, sPoint3 point2,
                     sPoint3 normal3, sPoint3 point3,
                     sPoint3 normal4, sPoint3 point4)
{
    glNormal3f(normal1.x, normal1.y, normal1.z);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(point1.x, point1.y, point1.z);

    glNormal3f(normal2.x, normal2.y, normal2.z);
    glTexCoord2f(0.0, 2.0);
    glVertex3f(point2.x, point2.y, point2.z);

    glNormal3f(normal3.x, normal3.y, normal3.z);
    glTexCoord2f(2.0, 2.0);
    glVertex3f(point3.x, point3.y, point3.z);

    glNormal3f(normal4.x, normal4.y, normal4.z);
    glTexCoord2f(2.0, 0.0);
    glVertex3f(point4.x, point4.y, point4.z);
}

/*!
 * \brief Auxiliary method.
 *
 * Helpful method for rendering triangle. Will be used in cWormhole.
 */
void cGLObject::triangle(sPoint3 normal1, sPoint3 point1,
                         sPoint3 normal2, sPoint3 point2,
                         sPoint3 normal3, sPoint3 point3)
{
    glNormal3f(normal1.x, normal1.y, normal1.z);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(point1.x, point1.y, point1.z);

    glNormal3f(normal2.x, normal2.y, normal2.z);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(point2.x, point2.y, point2.z);

    glNormal3f(normal3.x, normal3.y, normal3.z);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(point3.x, point3.y, point3.z);
}
