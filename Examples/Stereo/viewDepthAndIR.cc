#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<opencv2/core/core.hpp>
#include<time.h>
#include<librealsense2/rs.hpp>

#include<System.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {

	// RealSense Pipeline encapsulating actual device and sensors
	rs2::pipeline pipe;

	//Create a configuration for configuring the pipeline with a non default profile
	rs2::config cfg;

	//Add desired streams to configuration
	cfg.enable_stream(RS2_STREAM_INFRARED, 1);
	cfg.enable_stream(RS2_STREAM_INFRARED, 2);

	// Start streaming from the pipeline
	rs2::pipeline_profile selection = pipe.start(cfg);
	rs2::device selected_device = selection.get_device();
	auto depth_sensor = selected_device.first<rs2::depth_sensor>();

	// Disbale the projection pattern
	if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
	{
	    depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f); // Disable emitter
	}
	if (depth_sensor.supports(RS2_OPTION_LASER_POWER))
	{
	    depth_sensor.set_option(RS2_OPTION_LASER_POWER, 0.f); // Disable laser
	}

	for(;;){

		rs2::frameset data = pipe.wait_for_frames();
		rs2::frame ir1 = data.get_infrared_frame(1);
		rs2::frame ir2 = data.get_infrared_frame(2);

		//Find the size of the frame
		int widthIR1 = ir1.as<rs2::video_frame>().get_width();
		int heightIR1 = ir1.as<rs2::video_frame>().get_height();
		int widthIR2 = ir2.as<rs2::video_frame>().get_width();
		int heightIR2 = ir2.as<rs2::video_frame>().get_height();

		// Create OpenCV matrix of size (w,h) from the data
        Mat irFrame1(Size(widthIR1, heightIR1), CV_8UC1, (void*)ir1.get_data(), Mat::AUTO_STEP);
        Mat irFrame2(Size(widthIR2, heightIR2), CV_8UC1, (void*)ir2.get_data(), Mat::AUTO_STEP);

        resize(irFrame1, irFrame1, cv::Size(), 0.5, 0.5);
        resize(irFrame2, irFrame2, cv::Size(), 0.5, 0.5);

        imshow("IR1", irFrame1);
        imshow("IR2", irFrame2);
        waitKey(1);

        if ( (char)27 == (char) waitKey(1) ) break;
	}
	return 0;
} 