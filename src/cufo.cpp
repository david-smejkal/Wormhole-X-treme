/*!
 * \file cufo.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Unidentified Flying Object's definition.
 */

#include "cufo.h"

#include <cmath>

/*!
 * \brief Constructor of cUfo.
 *
 * Just initializing some variables.
 */
cUfo::cUfo()
{
    nLists = 1;
    pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;
    radius = 0.01;
}

cUfo::cUfo(QString str) : cGLObject(str)
{
}

/*!
 * \brief Destructor of cUfo.
 *
 * Freeing display lists. Important method! Without freeing display lists the
 * application will loose track of memory that was once used for this object.
 * Since other cUfo object will be created when changin model in settings.
 * Freeing memory whenever it is not needed anymore is not only a good habit,
 * but also a must do for a skilled programer.
 */
cUfo::~cUfo()
{
    glDeleteLists(object, nLists);
}

/*!
 * \brief Creates openGL display list for ufo.
 *
 * Get data from objFile. Use obj2OGL parser.
 *
 * \sa cWormhole::makeObject()
 * \note pure virtual method
 */

void cUfo::makeObject(QProgressBar * progress_bar, QLabel * progress_label)
{
    obj2OGL->makeObjectFromObjFileWithNormals(objFile,
                                              progress_bar,
                                              progress_label);
}

/*!
 * \brief Make display list of cUfo object.
 *
 * Create display list from parsed data. Attention, call cUfo::makeObject() for
 * the first time before calling this method.
 *
 * \return ID of compiled display list.
 * \sa cWormhole::makeDisplayList()
 * \note pure virtual method
 */
GLuint cUfo::makeDisplayList(int polygons)
{
    return obj2OGL->createDisplayList();
}

/*!
 * \brief Updates cUfo object.
 *
 * Ensures that preferences of cUfo object can be modified during runtime.
 *
 * \sa cWormhole::updateObject()
 * \note pure virtual method
 */
void cUfo::updateObject()
{
    return;
}
