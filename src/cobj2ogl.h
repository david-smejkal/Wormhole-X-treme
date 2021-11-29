/*!
 * \file cobj2ogl.h
 *
 * \author David Smejkal
 * \date 22.4.2009
 *
 * Converter from .obj to openGL.
 */

#ifndef COBJ2OGL_H
#define COBJ2OGL_H

#include <QGLWidget>
#include <QFile>
#include <QProgressDialog>

struct sFace {
    int vertIndex;
    int textIndex;
    int normIndex;
};

/*!
 * \class cObj2OGL
 * \brief Obj file parser.
 *
 * This class offeres methods that read obj fles. It converts obj to OpenGL
 * applicable commands, creating display list in this process.
 */
class cObj2OGL
{
public:
    cObj2OGL();
    ~cObj2OGL();
    void freeMemory();
    int makeObjectFromObjFile(QString str);
    int makeObjectFromObjFileWithNormals(QString str,
                                         QProgressBar * progress_bar,
                                         QLabel * progress_label);
    sPoint3 computeFaceNormal(int index0, int index1, int index2);
    void normalizeVertexNormals();
    GLuint createDisplayList();

    int numVertices;
    int numTextures;
    int numNormals;
    int numFaces;
    sPoint3 * vertices;
    sPoint3 * textures;
    sPoint3 * normals;
    QList<sFace> * faces;
    bool bParsed;

};

#endif // COBJ2OGL_H
