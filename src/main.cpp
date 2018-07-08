#include <ros/ros.h>
#include <rosThread.h>
#include <QApplication>
#include <QThread>

/// USAGE: rosrun camera2ros camera2ros_node {video_device_index(/dev/video{})}

int main(int argc,char** argv){
    QApplication app(argc,argv);

    ros::init(argc,argv,"camera2ros");

    RosThread* rosthread  = new RosThread(argc, argv);
    QThread* worker = new QThread(&app);
    rosthread->moveToThread(worker);

    QObject::connect(rosthread,SIGNAL(rosFinished()),worker,SLOT(quit()));
    QObject::connect(worker,SIGNAL(finished()),&app,SLOT(quit()));
    QObject::connect(worker,SIGNAL(finished()),rosthread,SLOT(deleteLater()));
    QObject::connect(worker,SIGNAL(started()),rosthread,SLOT(work()));

    worker->start();

    return app.exec();
}
