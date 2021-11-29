/*!
 * \file cufo.h
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Unidentified Flying Object declaration.
 */

#include "cglobject.h"

#ifndef CUFO_H
#define CUFO_H

#include "myinclude.h"

/*!
 * \class cUfo
 * \brief Unidentified Flying Object.
 *
 * UFO's are constructed from obj files supplied in their constructors. Using
 * cObj2OGL object, parsing of the obj file returns display list, that is
 * used right in cGLWidget class object.
 */
class cUfo : public cGLObject
{
public:
    cUfo();
    cUfo(QString str);
    ~cUfo();
    //GLuint makeObject();
    void makeObject(QProgressBar * progress_bar = NULL,
                    QLabel * progress_label = NULL);
    GLuint makeDisplayList(int polygons = 0);
    void updateObject();

    sPoint3 pos;
    float radius;
};


#endif // CUFO_H
