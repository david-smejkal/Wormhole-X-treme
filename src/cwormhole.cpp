/*!
 * \file cwormhole.cpp
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Unpredictably curved "tube's" definition.
 */

#include "cwormhole.h"

#include <iostream>
#include <cmath>
#include <time.h>

/*!
 * \brief Constructor of cWormhole.
 *
 * Just initializing some variables.
 */
cWormhole::cWormhole()
{
    nLists = 1;
    whSectors = 200;
    circleSectors = 25; // circleSectors + 1
    nControlPoints = 20;
    t=4;           // degree of polynomial = t-1

    makeObject();


}

/*!
 * \brief Destructor of cWormhole.
 *
 * Freeing display list. This is a must, otherwise stability of the entire
 * application will be compromised. Since sliders in cMainWindow force this
 * class to recreate itself.
 */
cWormhole::~cWormhole()
{
    glDeleteLists(object, nLists);
    for (int i = 0; i < whSectors; i++)
    {
        delete[] sectors[i].circle;
        delete[] sectors[i].normals;
    }
    delete[] sectors;
}

/*!
 * \brief Creates cWormhole data.
 *
 * Ensures proper first creation of cWormhole.
 *
 * \sa cUfo::makeObject()
 * \note pure virtual method
 */
void cWormhole::makeObject(QProgressBar * progress_bar, QLabel * progress_label)
{
    sectors = new sSector[whSectors];
    for (int i = 0; i < whSectors; i++)
    {
        sectors[i].circle = new sPoint3[circleSectors];
        sectors[i].normals = new sPoint3[circleSectors];
    }

    initializeWormholeCoordinates();

    bsplineSectorPoints(nControlPoints, t, listControlPoints, sectors,
                        whSectors);
    bsplineSectorRadius(nControlPoints, t, listControlRadiusPoints, sectors,
                        whSectors);
    genCircles(sectors);
    genNormals(sectors);
}

/*!
 * \brief Generate wormhole coordinates from (0,0,0).
 *
 * Initialize not only wormhole coordinates, but also wormhole radius. First
 * 4 control points of wormhole are linear with z axis and radius is bigger to
 * simulate wormhole entrance.
 */
void cWormhole::initializeWormholeCoordinates()
{
//    listControlPoints.clear();
//    listControlRadiusPoints.clear();

    srand ( time(NULL) );
    sPoint3 tmpPoint;
    for(int i=-(nControlPoints+1); i < 0 ; i++)
    {
        tmpPoint.x = i;
        listControlPoints.append(tmpPoint);
        tmpPoint.x = i;
        listControlRadiusPoints.append(tmpPoint);
    }

    int n = listControlPoints.size();
    for(int i=0; i<n; i++)
    {
        listControlPoints.removeFirst();
        tmpPoint.x = listControlPoints.last().x + 1.0;
        tmpPoint.y = (rand() % 2000 - 1000) / 1000.0;
        tmpPoint.z = (rand() % 2000 - 1000) / 1000.0;
        if(i<4)
        {
            tmpPoint.y = 0.0;
            tmpPoint.z = 0.0;
        }
        listControlPoints.append(tmpPoint);

        listControlRadiusPoints.removeFirst();
        tmpPoint.x = listControlRadiusPoints.last().x + 1.0;
        tmpPoint.y = (rand() % 400 + 200) / 1000.0;
        tmpPoint.z = 0.0;
        listControlRadiusPoints.append(tmpPoint);
    }
}

/*!
 * \brief Updates cWormhole object.
 *
 * Ensures that preferences of cWormhole object can be modified during runtime.
 *
 * \sa cUfo::updateObject()
 * \note pure virtual method
 */
void cWormhole::updateObject(int newWhSectors, int newCircleSectors)
{
    for (int i = 0; i < whSectors; i++)
    {
        delete[] sectors[i].circle;
        delete[] sectors[i].normals;
    }
    delete[] sectors;

    whSectors = newWhSectors;
    circleSectors = newCircleSectors;

    sectors = new sSector[whSectors];
    for (int i = 0; i < whSectors; i++)
    {
        sectors[i].circle = new sPoint3[circleSectors];
        sectors[i].normals = new sPoint3[circleSectors];
    }

    bsplineSectorPoints(nControlPoints, t, listControlPoints, sectors,
                        whSectors);
    bsplineSectorRadius(nControlPoints, t, listControlRadiusPoints, sectors,
                        whSectors);
    genCircles(sectors);
    genNormals(sectors);
}


/*!
 * \brief Creates openGL display list for wormhole.
 *
 * Vertex data and OpenGL commands are cached in the display list so calling
 * it in cGLWidget::paintGL() is much faster than creating wormhole object
 * by calling glVertex functions, etc.
 *
 * \return ID of compiled display list.
 * \sa cUfo::makeDisplayList()
 * \note pure virtual method
 */
GLuint cWormhole::makeDisplayList(int polygons)
{
    GLuint list = glGenLists(nLists);
    glNewList(list, GL_COMPILE);

    qglColor(QColor::fromRgb(145, 44, 238));
    //glClear(GL_COLOR_BUFFER_BIT);
    if(polygons)
        glBegin(GL_QUADS);
    else
        glBegin(GL_TRIANGLES);
        for (int j=0; j<whSectors-1; j++)
        {
            for(int i=0; i<circleSectors-1; i++)
            {
                /*qglColor(trolltechGreen.dark((int)(250 +\
                                              sin(1.5*sectors[j].circle[i].x) *\
                                              100)));*/
                if(polygons)
                {
                    quad(sectors[j].normals[i], sectors[j].circle[i],
                         sectors[j+1].normals[i], sectors[j+1].circle[i],
                         sectors[j+1].normals[i+1], sectors[j+1].circle[i+1],
                         sectors[j].normals[i+1], sectors[j].circle[i+1]);
                } else
                {
                    triangle(sectors[j].normals[i], sectors[j].circle[i],
                         sectors[j+1].normals[i], sectors[j+1].circle[i],
                         sectors[j+1].normals[i+1], sectors[j+1].circle[i+1]);
                    triangle(sectors[j+1].normals[i+1], sectors[j+1].circle[i+1],
                         sectors[j].normals[i+1], sectors[j].circle[i+1],
                         sectors[j].normals[i], sectors[j].circle[i]);
                }
            }
            int last = circleSectors-1;
            if(polygons)
            {
                quad(sectors[j].normals[last], sectors[j].circle[last],
                     sectors[j+1].normals[last], sectors[j+1].circle[last],
                     sectors[j+1].normals[0], sectors[j+1].circle[0],
                     sectors[j].normals[0], sectors[j].circle[0]);
            } else
            {
                triangle(sectors[j].normals[last],sectors[j].circle[last],
                     sectors[j+1].normals[last], sectors[j+1].circle[last],
                     sectors[j+1].normals[0], sectors[j+1].circle[0]);
                triangle(sectors[j+1].normals[0], sectors[j+1].circle[0],
                     sectors[j].normals[0], sectors[j].circle[0],
                     sectors[j].normals[last], sectors[j].circle[last]);
            }
        }
    glEnd();

    glEndList();
    
    return list;
}

/*void cWormhole::genPoints()
{
  //int n,t,i;
  //n=nControlPoints;          // number of control points = n+1
  int t=4;           // degree of polynomial = t-1

  bspline(nControlPoints, t, listControlPoints, sector, whSectors);
}*/

/*!
 * \brief Generate circle sectors for every spline point in wormhole.
 *
 * For each spline point there has to be a circle. This is needed for rendering
 * the wormhole around these spline points. Each circle has its own radius.
 */
void cWormhole::genCircles(sSector *sectors)
{
    for(int i=0; i < whSectors; i++)
    {
        const double Pi = 3.14159265358979323846;
        float ax = sectors[i].splinePoint.x;//1000.0;
        float ay = sectors[i].splinePoint.y;//1000.0;
        float az = sectors[i].splinePoint.z;//1000.0;
        float bx = sectors[i+1].splinePoint.x;//1000.0;
        float by = sectors[i+1].splinePoint.y;//1000.0;
        float bz = sectors[i+1].splinePoint.z;//1000.0;

        float w0 = bx-ax;
        float w1 = by-ay;
        float w2 = bz-az;
        float factor;
        float u0, u1, u2;
        float v0, v1, v2;

        float w3d = sqrt(w0*w0+w1*w1+w2*w2);
        w0 = w0/w3d; // normalizing
        w1 = w1/w3d; // normalizing
        w2 = w2/w3d; // normalizing

//        if (fabs(w0) >= fabs(w1)) {
            factor = 1/sqrt(w0*w0+w2*w2);
            u0 = -w2*factor;
            u1 = 0;
            u2 = w0*factor;
/*        } else {
            factor = 1/sqrt(w1*w1+w2*w2);
            u0 = 0;
            u1 = w2*factor;
            u2 = -w1*factor;
        }*/

        /*float u3d = sqrt(u0*u0+u1*u1+u2*u2);
        u0 = u0/u3d; // normalizing
        u1 = u1/u3d; // normalizing
        u2 = u2/u3d; // normalizing*/
        //a x b = (a2b3 - a3b2)i + (a3b1 - a1b3)j + (a1b2 - a2b1)k
        //V = Cross(W,U);
        v0 = w1*u2 - w2*u1;
        v1 = w2*u0 - w0*u2;
        v2 = w0*u1 - w1*u0;

        /*float v3d = sqrt(v0*v0+v1*v1+v2*v2);
        v0 = v0/v3d; // normalizing
        v1 = v1/v3d; // normalizing
        v2 = v2/v3d; // normalizing*/
        //X(t) = C + (r*cos(t))*U + (r*sin(t))*V
        int j;
        float circleRadius = sectors[i].radius;
        for (j = 0; j < circleSectors; ++j) {
            double angle1 = (j * 2 * Pi) / (circleSectors);
            sectors[i].circle[j].x = ax + (circleRadius*cos(angle1))*u0 \
                                        + (circleRadius*sin(angle1))*v0;
            sectors[i].circle[j].y = ay + (circleRadius*cos(angle1))*u1 \
                                        + (circleRadius*sin(angle1))*v1;
            sectors[i].circle[j].z = az + (circleRadius*cos(angle1))*u2 \
                                        + (circleRadius*sin(angle1))*v2;
        }
        /*sectors[i].circle[circleSectors-1].x = ax + (circleRadius*cos(0))*u0 \
                                    + (circleRadius*sin(0))*v0;
        sectors[i].circle[circleSectors-1].y = ay + (circleRadius*cos(0))*u1 \
                                    + (circleRadius*sin(0))*v1;
        sectors[i].circle[circleSectors-1].z = az + (circleRadius*cos(0))*u2 \
                                    + (circleRadius*sin(0))*v2;*/
        /*sectors[i].w.x = ax+w0/2.;
        sectors[i].w.y = ay+w1/2.;
        sectors[i].w.z = az+w2/2.;
        sectors[i].u.x = ax+u0/2.;
        sectors[i].u.y = ay+u1/2.;
        sectors[i].u.z = az+u2/2.;
        sectors[i].v.x = ax+v0/2.;
        sectors[i].v.y = ay+v1/2.;
        sectors[i].v.z = az+v2/2.;
        sectors[i].a.x = ax;
        sectors[i].a.y = ay;
        sectors[i].a.z = az;*/
    }
}

/*!
 * \brief Generate vertex normals for wormhole.
 *
 * Method cWormhole::computeFaceNormal() gets called on every face of the
 * wormhole. Face normal is than added to every point in face. By the end of
 * this process vertex normals are calculated. There is only one thing left
 * to do. Normalizing them, which is done by calling
 * cWormhole::normalizeVertexNormals() method.
 */
void cWormhole::genNormals(sSector *sectors)
{
    for (int j=0; j<whSectors; j++)
    {
        for(int i=0; i<circleSectors; i++)
        {
            sectors[j].normals[i].x = 0.0;
            sectors[j].normals[i].y = 0.0;
            sectors[j].normals[i].z = 0.0;
        }
    }
    
    sPoint3 tempPoint;
    for (int j=0; j<whSectors-1; j++)
    {
        for(int i=0; i < circleSectors-1; i++)
        {
            tempPoint = computeFaceNormal(sectors[j].circle[i],\
                                          sectors[j+1].circle[i],\
                                          sectors[j+1].circle[i+1]);
            sectors[j].normals[i].x += tempPoint.x;
            sectors[j].normals[i].y += tempPoint.y;
            sectors[j].normals[i].z += tempPoint.z;
            sectors[j+1].normals[i].x += tempPoint.x;
            sectors[j+1].normals[i].y += tempPoint.y;
            sectors[j+1].normals[i].z += tempPoint.z;
            sectors[j+1].normals[i+1].x += tempPoint.x;
            sectors[j+1].normals[i+1].y += tempPoint.y;
            sectors[j+1].normals[i+1].z += tempPoint.z;
            sectors[j].normals[i+1].x += tempPoint.x;
            sectors[j].normals[i+1].y += tempPoint.y;
            sectors[j].normals[i+1].z += tempPoint.z;
        }
        tempPoint = computeFaceNormal(sectors[j].circle[circleSectors-1],\
                                      sectors[j+1].circle[circleSectors-1],\
                                      sectors[j+1].circle[0]);
        sectors[j].normals[circleSectors-1].x += tempPoint.x;
        sectors[j].normals[circleSectors-1].y += tempPoint.y;
        sectors[j].normals[circleSectors-1].z += tempPoint.z;
        sectors[j+1].normals[circleSectors-1].x += tempPoint.x;
        sectors[j+1].normals[circleSectors-1].y += tempPoint.y;
        sectors[j+1].normals[circleSectors-1].z += tempPoint.z;
        sectors[j+1].normals[0].x += tempPoint.x;
        sectors[j+1].normals[0].y += tempPoint.y;
        sectors[j+1].normals[0].z += tempPoint.z;
        sectors[j].normals[0].x += tempPoint.x;
        sectors[j].normals[0].y += tempPoint.y;
        sectors[j].normals[0].z += tempPoint.z;
    }

    normalizeVertexNormals();
}

/*!
 * \brief Compute normal for face defined by 3 points in 3D space.
 *
 * This method is used on each face of the wormhole. Each partial normal is then
 * added to point in face. By this process vertex normals are calculated.
 *
 * \return Computed face normal defined in 3D space.
 */
sPoint3 cWormhole::computeFaceNormal(sPoint3 point0, sPoint3 point1,
                                     sPoint3 point2)
{
    sPoint3 retPoint;

    float dx1 = point1.x - point0.x;
    float dy1 = point1.y - point0.y;
    float dz1 = point1.z - point0.z;
    float dx2 = point2.x - point0.x;
    float dy2 = point2.y - point0.y;
    float dz2 = point2.z - point0.z;
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
 * \brief Normalize vertex normals.
 *
 * Normalize vertex normals of wormhole. Bad normals may result in glitches on
 * rendered objects. OpenGL can handel normalizing of normals on it's own, but
 * who knows. It's basicly a good habbit to normalize these little dummies.
 */
void cWormhole::normalizeVertexNormals()
{
    float d;

    for (int j=0; j<whSectors; j++)
    {
        for(int i=0; i<circleSectors; i++)
        {
            d = sqrt(sectors[j].normals[i].x*sectors[j].normals[i].x +
                     sectors[j].normals[i].y*sectors[j].normals[i].y +
                     sectors[j].normals[i].z*sectors[j].normals[i].z);
            if (d == 0.0)
            {
                sectors[j].normals[i].x = 1;
                sectors[j].normals[i].y = 0;
                sectors[j].normals[i].z = 0;
            }
            else
            {
                sectors[j].normals[i].x = sectors[j].normals[i].x / d;
                sectors[j].normals[i].y = sectors[j].normals[i].y / d;
                sectors[j].normals[i].z = sectors[j].normals[i].z / d;
            }
        }
    }
}

/*!
 * \brief Calculate spline points for control points.
 *
 * n > the number of control points minus 1.
 * t > the degree of the polynomial plus 1.
 * listPoints > control point array made up of point stucture
 * sectors > array of sStruct where calculated spline points will be stored
 * num_output > number of points on the spline
 *
 * \sa cWormhole::bsplineSectorRadius()
 */
void cWormhole::bsplineSectorPoints(int n, int t,
                                    QList<sPoint3> listPoints,
                                    sSector *sectors, int num_sectors)
{
  int *u;
  double increment,interval;
  sPoint3 calcxyz;
  int output_index;

  u = new int[n+t+1];
  compute_intervals(u, n, t);

  increment = (double) (n-t+2)/(num_sectors-1);  // how much parameter goes up each time
  interval = 0;

  for(output_index=0; output_index<num_sectors-1; output_index++)
  {
    compute_point(u, n, t, interval, listPoints, &calcxyz);
    sectors[output_index].splinePoint.x = calcxyz.x;
    sectors[output_index].splinePoint.y = calcxyz.y;
    sectors[output_index].splinePoint.z = calcxyz.z;
    interval=interval+increment;  // increment our parameter
  }
  // last point
  sectors[num_sectors-1].splinePoint.x = listPoints.at(n).x;
  sectors[num_sectors-1].splinePoint.y = listPoints.at(n).y;
  sectors[num_sectors-1].splinePoint.z = listPoints.at(n).z;

  delete u;
}

/*!
 * \brief Calculate spline points for control points.
 *
 * n >> the number of control points minus 1.
 * t >> the degree of the polynomial plus 1.
 * listPoints >> list of control points (sPoint3)
 * sectors >> array of sStruct where calculated spline points will be stored
 * num_output >> number of points on the spline
 *
 * \sa cWormhole::bsplineSectorPoints()
 */
void cWormhole::bsplineSectorRadius(int n, int t,
                                    QList<sPoint3> listPoints,
                                    sSector *sectors, int num_sectors)
{
  int *u;
  double increment,interval;
  sPoint3 calcxyz;
  int output_index;

  u = new int[n+t+1];
  compute_intervals(u, n, t);

  increment = (double) (n-t+2)/(num_sectors-1);  // how much parameter goes up each time
  interval = 0;

  for(output_index=0; output_index<num_sectors-1; output_index++)
  {
    compute_point(u, n, t, interval, listPoints, &calcxyz);
    sectors[output_index].radius = calcxyz.y;
    interval=interval+increment;  // increment our parameter
  }
  // last point
  sectors[num_sectors-1].radius = listPoints.at(n).y;

  delete [] u;
}

double cWormhole::blend(int k, int t, int *u, double v)  // calculate the blending value
{
  double value;

  if (t==1)			// base case for the recursion
  {
    if ((u[k]<=v) && (v<u[k+1]))
      value=1;
    else
      value=0;
  }
  else
  {
    if ((u[k+t-1]==u[k]) && (u[k+t]==u[k+1]))  // check for divide by zero
      value = 0;
    else
    if (u[k+t-1]==u[k]) // if a term's denominator is zero,use just the other
      value = (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
    else
    if (u[k+t]==u[k+1])
      value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v);
    else
      value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, u, v) +
          (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, u, v);
  }
  return value;
}

void cWormhole::compute_intervals(int *u, int n, int t)   // figure out the knots
{
  int j;

  for (j=0; j<=n+t; j++)
  {
    if (j<t)
      u[j]=0;
    else
    if ((t<=j) && (j<=n))
      u[j]=j-t+1;
    else
    if (j>n)
      u[j]=n-t+2;  // if n-t=-2 then we're screwed, everything goes to 0
  }
}

void cWormhole::compute_point(int *u, int n, int t, double v,
                              QList<sPoint3> listPoints, sPoint3 *output)
{
  int k;
  double temp;

  // initialize the variable that will hold outputted point
  output->x=0;
  output->y=0;
  output->z=0;

  for (k=0; k<=n; k++)
  {
    temp = blend(k,t,u,v);  // same blend is used for each dimension coordinate
    output->x = output->x + listPoints.at(k).x * temp;
    output->y = output->y + listPoints.at(k).y * temp;
    output->z = output->z + listPoints.at(k).z * temp;
  }
}
