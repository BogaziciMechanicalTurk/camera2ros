#include <ros/ros.h>
#include <QDebug>
#include <QVector>
#include <QObject>


#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace std;

class RosThread:public QObject
{
    Q_OBJECT

public:
    RosThread(int argc,char** argv);
    ~RosThread();

private:
    ros::NodeHandle n;
    int argc;
    char** argv;

    char* sh_command_to_find_idx;

public slots:
     void work();
     void shutdownROS();

 signals:
     void rosFinished();
     void rosStarted();
     void rosStartFailed();

};
