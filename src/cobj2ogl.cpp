/*!
 * \file cobj2ogl.cpp
 *
 * \author David Smejkal
 * \date 22.4.2009
 *
 * Converter from .obj to openGL.
 */


#include "myinclude.h"
#include "cobj2ogl.h"

#include <cmath>

#include <QtGui>
#include <QProgressDialog>

/*!
 * \brief Just NULLing some pointers in constructor of cObj2OGL.
 *
 * Pointers needs to be NULLed. This needs to be done in order to prevent
 * deleting untaken memory since deleting of these pointers will be done in some
 * other methods of this class (NULL pointers don't get freed in C++, even if
 * delete is explicitly called on them).
 */
cObj2OGL::cObj2OGL()
{
    vertices = NULL;
    textures = NULL;
    normals = NULL;
    faces = NULL;
    bParsed = false;
}

/*!
 * \brief Uses method for freeing memory.
 *
 * Freeing memory is now implemented in other method of this application, here
 * in destructor, this method is just called and memory will be freed.
 *
 */
cObj2OGL::~cObj2OGL()
{
    freeMemory();
}

/*!
 * \brief Free some fields of allocated memory.
 *
 * This is the method called from destructor and several other places in this
 * class. This method just free some fields of allocated memory. Notice that
 * delete [] is used since we want to free a field of allocated memory, not just
 * one variable.
 */
void cObj2OGL::freeMemory()
{
    delete [] vertices;
    delete [] textures;
    delete [] normals;
    delete [] faces;
}

/*!
 * \brief Creates display list from obj file specified by str parameter.
 *
 * Parsing obj file without normals. So vertex normals will be calculated and
 * ofcourse normalized.
 *
 * \return ID of compiled display list.
 * \sa cObj2OGL::makeObjectFromObjFileWithNormals()
 */
int cObj2OGL::makeObjectFromObjFile(QString str)
{
    QFile objFile(str);
    if(!objFile.exists()) return 0;

    /* First pass - Count vertices and faces. */
    if(!objFile.open(QIODevice::ReadOnly)) return 0;

    // its important to zero these before while loop
    numVertices = 0; 
    numNormals = 0;
    numFaces = 0;
    QRegExp regV("^v\\s.*");
    QRegExp regF("^f\\s.*");
    QString line;
    while (!objFile.atEnd())
    {
        line = objFile.readLine();
        if(line.contains(regV)) numVertices++;
        else if(line.contains(regF)) numFaces++;
    }

    objFile.close();

    /* Second pass - Now when numbers of vertices and faces are known create
       arrays and feed them with values from object file.*/
    if(!objFile.open(QIODevice::ReadOnly)) return 0;

    vertices = new sPoint3[numVertices];
    normals = new sPoint3[numVertices];
    faces = new QList<sFace>[numFaces];

    for(int i=0; i < numVertices; i++)
    {
        normals[i].x = 0.0;
        normals[i].y = 0.0;
        normals[i].z = 0.0;
    }

    int i = 0;
    int j = 0;
    QStringList tempLineSegments;
    QStringList tempFaceSegments;
    sPoint3 tempPoint;
    sFace tempFace;
    while(i < numVertices || j < numFaces)
    {
        line = objFile.readLine();
        if(line.contains(regV))
        {
            tempLineSegments = line.split(" ");
            vertices[i].x = tempLineSegments.at(1).toFloat();
            vertices[i].y = tempLineSegments.at(2).toFloat();
            vertices[i].z = tempLineSegments.at(3).toFloat();
            i++;
        } else if(line.contains(regF))
        {
            tempLineSegments = line.split(" ");
            for(int k = 1; k < tempLineSegments.size(); k++)
            {
                tempFaceSegments = tempLineSegments.at(k).split("/");
                tempFace.vertIndex = tempFaceSegments.at(0).toInt()-1;
                tempFace.normIndex = tempFaceSegments.at(0).toInt()-1;
                faces[j].append(tempFace);

            }
            tempPoint = computeFaceNormal(faces[j].at(0).vertIndex, \
                                          faces[j].at(1).vertIndex, \
                                          faces[j].at(2).vertIndex);
            for(int k = 0; k < faces[j].size(); k++)
            {
                normals[faces[j].at(k).vertIndex].x += tempPoint.x;
                normals[faces[j].at(k).vertIndex].y += tempPoint.y;
                normals[faces[j].at(k).vertIndex].z += tempPoint.z;
            }

            j++;
        }
    }

    objFile.close();

    normalizeVertexNormals();

    bParsed = true;

    return 1;
}

/*!
 * \brief Creates display list from obj file specified by str parameter.
 *
 * Parsing obj file with normals. No vertex normals will get calculated this
 * time. Trusted ones from obj file will be used instead.
 *
 * \return ID of compiled display list.
 * \sa cObj2OGL::makeObjectFromObjFile()
 */
int cObj2OGL::makeObjectFromObjFileWithNormals(QString str,
                                               QProgressBar * progress_bar,
                                               QLabel * progress_label)
{
    QFile objFile(str);
    if(!objFile.exists()) return 0;

    /* First pass - Count vertices and faces. */
    if(!objFile.open(QIODevice::ReadOnly)) return 0;

    // its important to zero these before while loop
    numVertices = 0;
    numTextures = 0;
    numNormals = 0;
    numFaces = 0;
    QRegExp regV("^v\\s.*");
    QRegExp regT("^vt\\s.*");
    QRegExp regN("^vn\\s.*");
    QRegExp regF("^f\\s.*");
    QString line;
    while (!objFile.atEnd())
    {
        line = objFile.readLine();
        if(line.contains(regV)) numVertices++;
        else if(line.contains(regT)) numTextures++;
            else if(line.contains(regN)) numNormals++;
                else if(line.contains(regF)) numFaces++;
    }

    objFile.close();

    /* Second pass - Now when numbers of vertices and faces are known create
       arrays and feed them with values from object file.
       This operation takes some time so progress bar will be displayed.*/
    if(!objFile.open(QIODevice::ReadOnly)) return 0;

    vertices = new sPoint3[numVertices];
    textures = new sPoint3[numTextures];
    normals = new sPoint3[numNormals];
    faces = new QList<sFace>[numFaces];

    if(progress_bar != NULL)
        progress_bar->setRange(0, numVertices+numTextures+numNormals+numFaces);
    if(progress_label != NULL)
    {
        progress_label->setText(str);
        progress_label->repaint();
    }

    /*for(int i=0; i < numVertices; i++)
    {
        normals[i].x = 0.0;
        normals[i].y = 0.0;
        normals[i].z = 0.0;
    }*/

    // its important to zero these before while loop
    int nV = 0;
    int nT = 0;
    int nN = 0;
    int nF = 0;
    QStringList tempLineSegments;
    QStringList tempFaceSegments;
    sFace tempFace;
    while(nV < numVertices || nT < numTextures || nN < numNormals ||
          nF < numFaces)
    {

        if(progress_bar != NULL)
        {
            //qApp->processEvents();
            progress_bar->setValue(nV+nT+nN+nF);
        }

        line = objFile.readLine();
        if(line.contains(regV))
        {
            tempLineSegments = line.split(" ");
            if(tempLineSegments.size() > 3)
            {
                vertices[nV].x = tempLineSegments.at(1).toFloat();
                vertices[nV].y = tempLineSegments.at(2).toFloat();
                vertices[nV].z = tempLineSegments.at(3).toFloat();
            } else
            {
                vertices[nV].x = 0.0;
                vertices[nV].y = 0.0;
                vertices[nV].z = 0.0;
            }
            nV++;
        } else
        if(line.contains(regT))
        {
            tempLineSegments = line.split(" ");
            if(tempLineSegments.size() > 2)
            {
                textures[nT].x = tempLineSegments.at(1).toFloat();
                textures[nT].y = tempLineSegments.at(2).toFloat();
                textures[nT].z = 0.0;
                if(tempLineSegments.size() > 3)
                    textures[nT].z = tempLineSegments.at(3).toFloat();
            } else
            {
                textures[nT].x = 0.0;
                textures[nT].y = 0.0;
                textures[nT].z = 0.0;
            }
            nT++;
        } else
        if(line.contains(regN))
        {
            tempLineSegments = line.split(" ");
            if(tempLineSegments.size() > 3)
            {
                normals[nN].x = tempLineSegments.at(1).toFloat();
                normals[nN].y = tempLineSegments.at(2).toFloat();
                normals[nN].z = tempLineSegments.at(3).toFloat();
            } else
            {
                normals[nN].x = 1.0;
                normals[nN].y = 0.0;
                normals[nN].z = 0.0;
            }
            nN++;
        } else
        if(line.contains(regF))
        {
            tempLineSegments = line.split(" ");
            for(int i = 1; i < tempLineSegments.size(); i++)
            {
                tempFaceSegments = tempLineSegments.at(i).split("/");
                if(tempFaceSegments.size() == 3)
                {
                    tempFace.vertIndex = tempFaceSegments.at(0).toInt()-1;
                    tempFace.textIndex = tempFaceSegments.at(1).toInt()-1;
                    tempFace.normIndex = tempFaceSegments.at(2).toInt()-1;
                    faces[nF].append(tempFace);
                } else
                {
                    tempFace.vertIndex = 0;
                    tempFace.textIndex = 0;
                    tempFace.normIndex = 0;
                    faces[nF].append(tempFace);
                }
            }
            nF++;
        }
    }

    // end progress
    if(progress_bar != NULL)
        progress_bar->setValue(numVertices+numTextures+numNormals+numFaces);

    objFile.close();

    //normalizeVertexNormals();

    bParsed = true;

    return 1;
}

/*!
 * \brief Compute face normal.
 *
 * Face normal for a face specified by 3 points in 3D space will be calculated
 *
 * \return Calculated face normal.
 */
sPoint3 cObj2OGL::computeFaceNormal(int index0, int index1, int index2)
{
    sPoint3 retPoint;

    float dx1 = vertices[index1].x - vertices[index0].x;
    float dy1 = vertices[index1].y - vertices[index0].y;
    float dz1 = vertices[index1].z - vertices[index0].z;
    float dx2 = vertices[index2].x - vertices[index0].x;
    float dy2 = vertices[index2].y - vertices[index0].y;
    float dz2 = vertices[index2].z - vertices[index0].z;
    retPoint.x = dy1*dz2 - dz1*dy2;
    retPoint.y = dz1*dx2 - dx1*dz2;
    retPoint.z = dx1*dy2 - dy1*dx2;
    float d = sqrt(retPoint.x*retPoint.x + retPoint.y*retPoint.y + \
                   retPoint.z*retPoint.z);

    if (d == 0.0)
    {
      retPoint.x = 1;
      retPoint.y = 0;
      retPoint.z = 0;
    }
    else
    {
      retPoint.x /= d;
      retPoint.y /= d;
      retPoint.z /= d;
    }
    return retPoint;
}

/*!
 * \brief Calculate vertex normals.
 *
 * Normalizing vertex and face normals is good habit. Some implementations of
 * OpenGL may have problem with unnormalized vertex normals. After all the name
 * normal speaks for itself.
 */
void cObj2OGL::normalizeVertexNormals()
{
    float d;
    for(int i=0; i < numVertices; i++)
    {
     d = sqrt(normals[i].x*normals[i].x + normals[i].y*normals[i].y + \
              normals[i].z*normals[i].z);

      if (d == 0.0)
      {
        //normals[i].x = 1;
        //normals[i].y = 0;
        //normals[i].z = 0;
      }
      else
      {
        normals[i].x /= d;
        normals[i].y /= d;
        normals[i].z /= d;
      }

    }
}

/*!
 * \brief Creates display list.
 *
 * Create display list from data geathered by obj2OGL parser. This method gets
 * called from within cObj2OGL::makeObjectFromObjFileWithNormals() and
 * cObj2OGL::makeObjectFromObjFileWithNormals() methods. It's purpose is to keep
 * things simple. Or it may be also used by object that uses this parser.
 *
 * \return ID of compiled display list.
 */
GLuint cObj2OGL::createDisplayList()
{
    if(!bParsed) return 0;
    if(vertices == NULL || normals == NULL) return 0;
    
    GLuint displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);

    int indexV;
    //int indexT;
    int indexN;
    for(int i = 0; i < numFaces; i++)
    {
        if(faces[i].size() == 3) glBegin(GL_TRIANGLES);
        else if (faces[i].size() == 4) glBegin(GL_QUADS);
        else glBegin(GL_POLYGON);
        for(int k = 0; k < faces[i].size(); k++)
        {
            indexV = faces[i].at(k).vertIndex;
            //indexT = faces[i].at(k).textIndex;
            indexN = faces[i].at(k).normIndex;
            glNormal3f(normals[indexN].x,
                       normals[indexN].y,
                       normals[indexN].z);
            glVertex3f(vertices[indexV].x,
                       vertices[indexV].y,
                       vertices[indexV].z);
        }
        glEnd();
    }

    glEndList();

    return displayList;
}
