#include "rosThread.h"
#include <signal.h>
#include <QRegExp>
#include <std_msgs/Int16.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <opencv2/core/core.hpp>

using namespace std;

string exec(const char* cmd) {
    char buffer[128];
    string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

RosThread::RosThread(int argc,char** argv)
{
    this->argc = argc;
    this->argv = argv;

    sh_command_to_find_idx = "#!/bin/bash\n\nfor sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do\n(\nsyspath=\"${sysdevpath%/dev}\"\ndevname=\"$(udevadm info -q name -p $syspath)\"\neval \"$(udevadm info -q property --export -p $syspath)\"\necho \"/dev/$devname - $ID_SERIAL\"\n)\ndone";
}

RosThread::~RosThread()
{
}

void RosThread::shutdownROS()
{
    ros::shutdown();
}

bool capture_image = false;

void imageCaptureCallback(const std_msgs::Int16 imageCaptureSignal){

    if(imageCaptureSignal.data == 1){
        capture_image = true;
    }

}



bool stop = false;
void sigIntHandler(int signal) {
    stop = true;
}
void RosThread::work(){
    int video_idx = 0;
    /*
    if(argc < 2){
        cout << "No video device idx entered. Trying to find index of Magewell USB-HDMI!" << endl << endl;

        QString regex_query = "/dev/video([^/]*) - Magewell.*USB-HDMI";
        QRegExp regexp(regex_query);
        regexp.setMinimal(true);

        QString str = QString::fromStdString(exec(sh_command_to_find_idx));
        int pos = regexp.indexIn(str);
        if(pos == -1){
            cout << str.toStdString() << endl << endl;

            cout << "ERROR couldn't find Magewell USB-HDMI!" << endl;
            exit(1);
        }

        QString idx_str = regexp.cap(1);
        video_idx = idx_str.toInt();

        cout << "Found: /dev/video" << video_idx << endl;
    }
    else
        video_idx = atoi(argv[1]);
        */

    signal(SIGINT, sigIntHandler);

    image_transport::ImageTransport it(n);
    image_transport::Publisher pub = it.advertise("camera/image", 0);
    ros::Subscriber sssub = n.subscribe("camera2ros/captureImage",1, imageCaptureCallback);

    cout << "Opening /dev/video" << video_idx << endl;
    cv::VideoCapture cap(video_idx); /// "/dev/videoX"

    //cap.set(CV_CAP_PROP_MODE,3);

    image_transport::Subscriber sub;

    // Check if video device can be opened with the given index
    if(!cap.isOpened()){
        cout << "ERROR capture cannot be opened!!" << endl;
        exit(1);
    }
    cv::Mat frame;
    sensor_msgs::ImagePtr msg;


    cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
    cap.set(CV_CAP_PROP_FPS,30);


    //cv::Mat image;

    //image = cv::imread("/home/naps/Pictures/term_project.png",CV_LOAD_IMAGE_COLOR);


    //cv::imshow("test",image);



    //cv::waitKey(30);

    ros::Rate loop(100);
    int idx = 0;
    while(ros::ok() && !stop)
    {
        cap>>frame;

        //std::cout << cap.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
        //std::cout << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;
        // Check if grabbed frame is actually full with some content
        if(!frame.empty()) {

            msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
            pub.publish(msg);

            if(capture_image){
                stringstream ss;
                ss << "/home/naps/capturedImages/image/image_0" << idx++ << ".jpg";
                cv::imwrite(ss.str(), frame);
                capture_image = false;
            }
        }
        else{
            cout << "WARNING Empty frame!!" << endl;
        }

        //std::cout << frame.rows << " " << frame.cols << std::endl;

        //cv::resize(frame,image,cv::Size(100,100));

        //cv::imshow("test",image);


        //loop.sleep();

        ros::spinOnce();
        loop.sleep();
    }

    qDebug()<<"I am quitting";

    cap.release();
    ros::shutdown();

    exit(0);
}


/*bool stop = false;
void sigIntHandler(int signal) {
    stop = true;
}
void RosThread::work(){
    signal(SIGINT, sigIntHandler);
    image_transport::ImageTransport it(n);
    image_transport::Publisher pub = it.advertise("camera/image", 0);

    cv::VideoCapture cap(1);

    ros::Rate loop(5);
    while(ros::ok() && !stop)
    {
        cout << "asd" << endl;

        cv::Mat frame = cv::imread("/dev/video0", CV_LOAD_IMAGE_COLOR); /// CHANGE THE PARAMETER 0 /// "/dev/videoX"

        // Check if grabbed frame is actually full with some content
        if(!frame.empty()) {
            cout << "cool!" << endl;
            cv::imshow("video", frame);

            sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
            pub.publish(msg);
            cv::waitKey(1);
        }

        ros::spinOnce();
        loop.sleep();
    }

    qDebug()<<"I am quitting";

    cap.release();
    ros::shutdown();

    exit(0);
}*/
