#ifndef XYZMOTION_H
#define XYZMOTION_H

#include <QObject>
#include <QMap>
#include "xdflpath.h"
#include <QDomNode>
#include <QStringList>
/**
 * A struct for representing hte properties of a machine axis
 */
struct axis{
    QString name;
    double range;
};

class XYZMotion: public QObject
{
    Q_OBJECT
public:
    /**
     * Default constructor of the XYZMotion object.
     * this object proforms the calulations needed for coordinated motion of 3 axes
     */
    XYZMotion();

    /**
     * Constructor of the XYZMotion object from a config file DomNode
     */
    XYZMotion(const QDomNode& sourceDomNode);

    /**
     * Calculates a relative path to a point(x,y,z) from the origin
     * at the given speed using the preset acceleration
     */
    QStringList pathTo(double x, double y, double z, double speed, bool isrelative=false);

    /**
     * Caclulated the relative states for movment along a XDFLPath's relative positions
     * It accelerates ad the preset rate upto the speed and then maintains the speed till it
     * decelerates at the end of the path.
     */
    QStringList pathAlong(XDFLPath path,double speed);


    /**
     * returns the build space of the system
     */
    QList<double> buildSpace();

private:
    QMap < QString, axis > axismap_;
};

#endif // XYZMOTION_H
