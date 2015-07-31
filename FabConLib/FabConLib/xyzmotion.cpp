#include "xyzmotion.h"
//#include "jsnpath.h"
#include <math.h>

XYZMotion::XYZMotion(){}

XYZMotion::XYZMotion(const QDomNode& sourceDomNode) {
    QDomNodeList bayChildren = sourceDomNode.childNodes();
    for(uint i=0; i<bayChildren.length();i++){
        QDomNode bchild = bayChildren.at(i);
        if (bchild.isComment()) {continue;}
        if ("axis" == bchild.nodeName().toLower()) {
            axis a;
            QDomNodeList bchildren = bchild.childNodes();
            for (int j=0; j < bchildren.size(); j++) {
                 QDomNode achild = bchildren.at(j);
                if (achild.isComment()) {continue;}

                if ("range"==achild.nodeName().toLower()) {
                    a.range = achild.toElement().text().toDouble();
                } else if ("name"== achild.nodeName().toLower()) {
                    a.name = achild.toElement().text().toLower();
                }
            }
            axismap_[a.name] = a;
        }
    }
}



QStringList XYZMotion::pathAlong(XDFLPath path,double speed) {


    QStringList returnlist;

    speed = fabs(speed);


    if(path.relative_){ returnlist.append("G91");} //to relative
    foreach(FabPoint p, path.getPoints()){
        QString s = "G1 X"+QString::number(p.x)+
                      " Y"+QString::number(p.y)+
                      " Z"+QString::number(p.z)+
                      " F"+QString::number(speed*60.0);
        returnlist.append(s);
    }
    if(path.relative_){ returnlist.append("G90");} //to Aboslute
    return returnlist;
}

QStringList XYZMotion::pathTo(double x, double y, double z, double speed, bool isrelative) {
    QStringList returnlist;
    speed = fabs(speed);
    if(isrelative){ returnlist.append("G91");} //to relative
    QString s = "G1 X"+QString::number(x)+
                  " Y"+QString::number(y)+
                  " Z"+QString::number(z)+
                  " F"+QString::number(speed*60.0);
    returnlist.append(s);
    if(isrelative){ returnlist.append("G90");} //to Aboslute
    return returnlist;
}



QList<double> XYZMotion::buildSpace(){
    QList<double> a;
    a[0]=axismap_["x"].range;
    a[1]=axismap_["y"].range;
    a[2]=axismap_["z"].range;
    return a;
}
