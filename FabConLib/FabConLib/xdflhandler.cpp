#include "xdflhandler.h"
#include "testing/util.h"
#include <QApplication>
#include <QDebug>

XDFLHandler::XDFLHandler():handlerstate_(XDFLHandler::Stopped),estimationDone_(false),
    estimatedTime_(0),estimatedVolume_(0),current_material_(0),current_command_(0),needMaterial_(false),current_bay_(-1)

{
    last_end_point_ = FabPoint();
}

XDFLHandler::XDFLHandler(VMPrototype* vm, QDomDocument xdfl) {
    XDFLHandler();
    setVM(vm);
    loadFromDom(xdfl);
    current_bay_ = 0;
}

void XDFLHandler::updateState(){
    if (vm_->isInitialized()&&!commands_.isEmpty()){
        handlerstate_=XDFLHandler::Ready;
    }else{
        handlerstate_=XDFLHandler::Stopped;
    }
}

double XDFLHandler::getEstimatedTime() {
    if (!estimationDone_) {
        estimate();
    }
    return estimatedTime_;
}

double XDFLHandler::getEstimatedVolume() {
    if (!estimationDone_) {
        estimate();
    }
    return estimatedVolume_;
}

QMap<int,Material> XDFLHandler::getMaterials() {
    return mat_map;
}

// Threadsafe slot. Processing can only be paused if it is currently running.
void XDFLHandler::pause() {
    QMutexLocker locker(&mutex_);

    if (handlerstate_ == XDFLHandler::Running) {
        qDebug()<<"Pausing...";
        handlerstate_ = XDFLHandler::Paused;
        vm_->pauseprint();
    }
//    vm_->moveToThread(QApplication::instance()->thread());
    // (mutex automatically released upon locker destruction)
}

// Threadsafe slot. Processing can only be resumed if it currently paused.
void XDFLHandler::resume() {
    QMutexLocker locker(&mutex_);
    ///UPDATE THE MATERIAL MAPINGS
    updateInfo();
    if (handlerstate_ == XDFLHandler::Paused) {
        qDebug()<<"Resuming...";
        vm_->startprint();
        handlerstate_ = XDFLHandler::Running;
        resumed_.wakeAll();
    }
    // (mutex automatically released upon locker destruction)
}

// Threadsafe slot. Processing can be cancelled if it is running or paused.
void XDFLHandler::cancel() {
    QMutexLocker locker(&mutex_);

    qDebug()<<"canceling...";
    vm_->cancelprint();
    if (handlerstate_ == XDFLHandler::Running) {
        handlerstate_ = XDFLHandler::Stopped;
    }
    else if (handlerstate_ == XDFLHandler::Paused) {
        handlerstate_ = XDFLHandler::Stopped;
        // Additionally wake the thread so that it can be stopped.
        resumed_.wakeAll();
    }
//    vm_->moveToThread(QApplication::instance()->thread());
//    exit(0);
    //    terminate();
    // (mutex automatically released upon locker destruction)
}

void XDFLHandler::forceStop(){
//    vm_->moveToThread(QApplication::instance()->thread());
//    cancel();
//    deleteLater();
//    quit();
    terminate();
}


int  XDFLHandler::getState(){
    return handlerstate_;
}

void  XDFLHandler::setState(HandlerState i){
    handlerstate_ =i;
}

int XDFLHandler::getNumberOfCommands() {
    return commands_.length();
}

void XDFLHandler::setVM(VMPrototype* vm) {
    vm_ = vm;

    updateState();
}

void XDFLHandler::loadFromDom(QDomDocument xdfl) {

    QDomElement root = xdfl.documentElement();
    if (root.isNull()) {
        qDebug()<<"NULL ROOT";
        return;
    }
    QDomNode paletteNode = root.namedItem("palette");
    if (paletteNode.isNull()) {
        paletteNode = root.namedItem("Palette");// its case sensitive
        if (paletteNode.isNull()) {
            qDebug()<<"NULL PALETTE";
            return;
        }
    }
    QDomNode commandsNode = root.namedItem("commands");
    if (commandsNode.isNull()) {
        commandsNode = root.namedItem("Commands");
        if (commandsNode.isNull()) {
            qDebug()<<"NULL COMMANDS";
            return;
        }
    }

    commands_ = commandsNode.childNodes();
    if (commands_.isEmpty()) {
        qDebug()<<"NO COMMANDS";
        // throw exceptions? Return false?
        return;
    }
    current_command_ = 0;


    mat_map.clear();
    QDomNodeList materialTags  = paletteNode.childNodes();
    for (unsigned int k = 0; k < materialTags.length(); k++) {
        if ("material"==materialTags.at(k).nodeName().toLower()) {
            Material m = materialFromDom(materialTags.at(k));
            mat_map[m.id]=m;
        }
    }
    updateState();
    estimate();
}

/**
  Calculates the estimated time and volume of the currently set XDFL document.
  If there is none, estimatedTime_ and estimatedVolume_ are set to -1 and estimationDone is set to false.
  **/
void XDFLHandler::estimate() {

    // This method could be modified to find the estimated remaining time and volume by
    // starting calculation from current_command_ and using last_end_point_. Threadsafe
    // behavior would be required to obtain synchronized values for those variables.

    unsigned int current_command_est = 0; // = current_command_;
    FabPoint last_end_point_est = FabPoint(); // = last_end_point_;
    FabPoint new_start_point;
    double speed;

    estimatedTime_   = 0;
    estimatedVolume_ = 0;



    qDebug()<<"current cmds"<<current_command_est;
    while (current_command_est < commands_.length()) {

        QDomElement commandTag = commands_.at(current_command_est).toElement();

        if ("path" == commandTag.nodeName().toLower()) {
            XDFLPath p = pathFromQDom(commandTag);

            if (p.isNull()) { continue; }// check to see if the path is valid

            // If the start of this XDFL path is not close to the end of the last one,
            // generate a transition path.
            // Set the speed of the queued XDFL path for the transition path.
            if (p.speed > 0) {
                speed = p.speed;
            } else {
                speed = mat_map[p.materialID].property["pathspeed"].toDouble();
            }
            new_start_point = p.start();

            if(!pointsEqual(new_start_point,last_end_point_est,0.1)) { // ensures that the machine doesn't jump between points
                // Add the transition path time but NOT the path volume (no extrusion)
                estimatedTime_ += distance(new_start_point,last_end_point_est) / speed;
            }

            // Add the XDFL path time and IF extruding the XDFL path volume.
            estimatedTime_ += p.length() / speed;

            if (!(p.materialID == 0 && p.speed != 0)) { // extrusion path
                QMap<QString,QString> properties = mat_map[p.materialID].property;
                estimatedVolume_ += properties.value("pathwidth").toDouble()
                                  * properties.value("pathheight").toDouble()
                                  * properties.value("areaconstant").toDouble()
                                  * p.length();
            }

            last_end_point_est = p.end();

        } else if ("voxel" == commandTag.nodeName().toLower()) {
            // If the XDFL voxel is not close to the end of the last movement,
            // we need to move between the points.
            // TODO: Add the transition path time.

            XDFLVoxel v = voxFromQDom(commandTag);

            // Add the XDFL voxel volume.
            estimatedVolume_ += v.volume;

            new_start_point.x = v.x;
            new_start_point.y = v.y;
            new_start_point.z = v.z;

            //SET LAST_END_POINT to location of voxel in ABS coordinates
            last_end_point_est = new_start_point;

        } else if ("dwell" == commandTag.nodeName().toLower()) {
            double dwelltime = commandTag.text().toDouble();

            // dwelltime is given in ms; add to estimatedTime_ in seconds.
            estimatedTime_ += dwelltime / 1000;
        } else {
        }
        current_command_est++;
    }
    estimationDone_ = true;
    emit estimated(estimatedTime_, estimatedVolume_, commands_.length());
}

void XDFLHandler::run()
{
    // When this method is called, the current state should be Ready.

    qDebug()<<"Initializing the XDFL handler thread.";

    vm_->startprint();
    QMutexLocker locker(&mutex_);
    current_command_ = 0;
    current_material_ = 0;
//    last_end_point_ = FabPoint();
    updateInfo();
    if (handlerstate_ == XDFLHandler::Ready) {
        // Set up for processing.
        updateInfo();
        handlerstate_ = XDFLHandler::Running;
    }
    else {
        qDebug()<<"ERROR: XDFL handler was not ready.";
        handlerstate_ = XDFLHandler::Stopped;
    }

    while (handlerstate_ != XDFLHandler::Stopped) {
        if (handlerstate_ == XDFLHandler::Running) {
            // Allow state changes [pausing/stopping] while processing one path
            locker.unlock();
            if(vm_->getBufferSize()<10){processCommand();}
            locker.relock();
            if (current_command_ == commands_.length()) {
                // XDFL file completed.
                qDebug()<<"XDFL file processing completed.";
                handlerstate_ = XDFLHandler::Stopped;
            }
        }
        else if (handlerstate_ == XDFLHandler::Paused) {
            // Block thread until woken by resumed_.wakeAll().
            // The waking function is responsible for changing the state.
            resumed_.wait(&mutex_);
        }
        else {
            qDebug()<<"ERROR: XDFL handler state was incorrect.";
            handlerstate_ = XDFLHandler::Stopped;
        }
    }
    // XDFLHandler::Stopped:
    // Stop processing and reset progress.

    //    qDebug()<<"\nExiting the XDFL handler thread.";
//    vm_->moveToThread(QApplication::instance()->thread());
    // (mutex automatically released upon locker destruction)
}

void XDFLHandler::  processCommand() {

    FabPoint new_start_point;
    double speed=0;


    emit startingCommand(current_command_);

    QDomElement commandTag = commands_.at(current_command_).toElement();

    if ("path" == commandTag.nodeName().toLower()) {
#ifdef DEBUGGING
        qDebug()<<"PATH:"<<current_command_;
#endif
        XDFLPath p = pathFromQDom(commandTag);
        p.toAbsolute();

        if (p.isNull()) { return; }// check to see if the path is valid

        // Check to see if the material is listed in the header or if we are switching bays.
        if(!setMaterial(p.materialID)){return;}

        // If the start of this XDFL path is not close to the end of the last one,
        // generate a transition path.
        // Set the speed of the queued XDFL path for the transition path.
        if (p.speed > 0) {
            speed = p.speed;
        } else {
            speed = mat_map[p.materialID].property["pathspeed"].toDouble();
        }
        QStringList n;
        if (p.materialID == 0 && p.speed != 0) {
            if(current_bay_<0){
                current_bay_ = 0;
            }
            // if the path is not an extrusion path we move along it
            if (commands_.length()>(current_command_+1)){
                QDomElement nextCommandTag = commands_.at(current_command_+1).toElement();
                if ("path" == nextCommandTag.nodeName().toLower()){
                    XDFLPath next = pathFromQDom(nextCommandTag);
                    int nextMatId = next.materialID;
                    Bay* nextBay = vm_->bays[current_bay_];
                    Bay* currentBay = vm_->bays[current_bay_];
                    // && (nextMatId !=0)
                    if(current_bay_!=material_bay_mapping_[nextMatId]->getId() && (nextMatId !=0) ){
                        nextBay = material_bay_mapping_[nextMatId];
                        currentBay = vm_->bays[current_bay_];
                    }
                    p.points.last().x = p.points.last().x-nextBay->location_.at(0);
                    p.points.last().y = p.points.last().y-nextBay->location_.at(1);
                    p.points.last().z = p.points.last().z-nextBay->location_.at(2);

                    p.points.first().x = p.points.first().x-currentBay->location_.at(0);
                    p.points.first().y = p.points.first().y-currentBay->location_.at(1);
                    p.points.first().z = p.points.first().z-currentBay->location_.at(2);
                }
            }
            n = vm_->xyzmotion->pathAlong(p,speed);
        } else { // if it is an extrusion path we feed it to the proper bay.

            if(current_bay_!=material_bay_mapping_[p.materialID]->getId()){
                current_bay_ = material_bay_mapping_[p.materialID]->getId();
                QStringList sl;
                sl.append("T"+QString::number(current_bay_));
                vm_->runCmds(sl);
            }
            n = material_bay_mapping_[p.materialID]->onPath(p);
        }

        vm_->runCmds(n);
        last_end_point_ = p.end();

    } else if ("voxel" == commandTag.nodeName().toLower()) {

        // If the XDFL voxel is not close to the end of the last movement,
        // we need to move between the points.

        XDFLVoxel v = voxFromQDom(commandTag);

        // Check to see if the material is listed in the header.
        if(!setMaterial(v.id)){return;}
        new_start_point.x = v.x;
        new_start_point.y = v.y;
        new_start_point.z = v.z;

        //MAKE RELATIVE VOXEL to send to bay
        //FabPoint delta = last_end_point_;//subtractpoints(new_start_point,last_end_point_);
        //v.x = delta.x;
        //v.y = delta.y;
        //v.z = delta.z;

        //run NPATH from bay
//        runNPath(material_bay_mapping_[v.id]->onVoxel(v));
        if(current_bay_!=material_bay_mapping_[v.id]->getId()){
            //current_bay_ = material_bay_mapping_[v.id]->getId();
            //QStringList sl;
            //sl.append("T"+QString::number(current_bay_));
            //vm_->runCmds(sl);
        }
        vm_->runCmds(material_bay_mapping_[v.id]->onVoxel(v));

        //SET LAST_END_POINT to location of voxel in ABS coordinates
        last_end_point_ = new_start_point;

    } else if ("dwell" == commandTag.nodeName().toLower()) {

        double dwelltime = commandTag.text().toDouble();
        vm_->runCmds(dwell(dwelltime));

    } else{
    }
    current_command_++;
    return;
}

QStringList XDFLHandler::dwell(double time_in_ms) {
    QStringList returnlist;
    QString dwell = "G4 P"+QString::number(time_in_ms);
    returnlist.append(dwell);
    return returnlist;
}




void XDFLHandler::updateInfo(){
    material_bay_mapping_.clear();
    foreach (Bay* b,vm_->bays) {
        material_bay_mapping_[b->getMaterial().id] = b;
    }
//    laststate_ = vm_->currentState();
    last_end_point_ = pointFromQVector(vm_->currentPosition());
//    current_material_=0;

}


bool XDFLHandler::setMaterial(int id) {
    qDebug()<<"\nid: "<<id<<"\tCurrrent id: "<<current_material_;
    if (0==id){return true;}
//    foreach (Bay* b,vm_->bays) {
//        material_bay_mapping_[b->getMaterial().id] = b;
//    }

    if (!mat_map.keys().contains(id)) {
            qDebug()<<"Material not found in XDFL";
            emit needMaterialChange(id);
            // This should be checked at loading of commands
            pause();
            return false;

    }else if(!material_bay_mapping_.keys().contains(id)) {
        qDebug()<<"Material "<<id<<" not loaded";
        emit needMaterialChange(id);
        pause();

        return false;
    }

    if (current_material_ == 0) {
        current_material_ = id;

    }else if ((current_material_ == id)||(id == 0)) {
        return true;
    }else{
        // THIS MAY BE A PROBLEMATIC WAY OF HANDELING SHIFTING OF THE TOOL BAYS
//        FabPoint Pnew = pointFromQVector(material_bay_mapping_[id]->getLocation());
//        FabPoint Pold = pointFromQVector(material_bay_mapping_[current_material_]->getLocation());
//        FabPoint delta = subtractpoints(Pnew,Pold);
//        double speed = material_bay_mapping_[id]->getMaterial().property["pathspeed"].toDouble();
//        current_material_ = id;
//        qDebug()<<"Changing materials.";
//        QStringList cmds;
//        QString rel = "G91";
//        QString abs = "G90";
//        QString move = "G1 X"+QString::number(delta.x)+
//                         " Y"+QString::number(delta.y)+
//                         " Z"+QString::number(delta.z)+
//                         " F"+QString::number(speed*60);
//        cmds.append(rel);
//        cmds.append(move);
//        cmds.append(abs);
        //vm_->runCmds(cmds);
        //qDebug()<<"move switch: "<<move;
//        runNPath(vm_->xyzmotion->pathTo(delta.x,delta.y,delta.z,speed));

    }
    return true;
}
