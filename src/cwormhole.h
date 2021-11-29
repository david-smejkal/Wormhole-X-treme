/*!
 * \file cwormhole.h
 *
 * \author David Smejkal
 * \date 4.4.2009
 *
 * Unpredictably curved "tube's" declaration.
 */

#ifndef CWORMHOLE_H
#define CWORMHOLE_H


#include "cglobject.h"
#include "myinclude.h"

struct sSector {
    sPoint3 * circle;
    sPoint3 * normals;
    sPoint3 splinePoint;
    float radius;
};

/*!
 * \class cWormhole
 * \brief Unpredictably curved "tube". Object of high importance in application.
 *
 * This so called wormhole is textured and can be customized by some of the
 * cMainWindow object GUI. Some times (like in restart after collision) wormhole
 * needs to be recreated. For these cases, using updateObject() method is
 * adviced.
 */
class cWormhole : public cGLObject
{
public:
    cWormhole();
    ~cWormhole();
    void makeObject(QProgressBar * progress_bar = NULL,
                    QLabel * progress_label = NULL);
    void initializeWormholeCoordinates();
    void updateObject(int newWhSectors, int newCircleSectors);
    GLuint makeDisplayList(int polygons = 0);

    sSector * sectors;
    QList<sPoint3> listControlPoints;
    QList<sPoint3> listControlRadiusPoints;

    int nControlPoints;
    int whSectors;
    int circleSectors;
    int t;           // degree of polynomial = t-1

private:
    void compute_intervals(int *u, int n, int t);
    double blend(int k, int t, int *u, double v);
    void compute_point(int *u, int n, int t, double v,
                       QList<sPoint3> listPoints,
                       sPoint3 *output);
    void bsplineSectorPoints(int n, int t, QList<sPoint3> listPoints,
                             sSector *sectors, int whSectors);
    void bsplineSectorRadius(int n, int t, QList<sPoint3> listPoints,
                             sSector *sectors, int whSectors);
    sPoint3 computeFaceNormal(sPoint3 point0, sPoint3 point1, sPoint3 point2);
    void normalizeVertexNormals();
    void genPoints();
    void genCircles(sSector *sectors);
    void genNormals(sSector *sectors);


};


#endif // CWORMHOLE_H
