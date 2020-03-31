/**
* @brief
* WinTelloAR common header
* std.h
*/
#pragma once
#pragma warning(disable:4819)
#pragma warning(disable:4996)

// use socket
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

// use std,boost
#include <iostream>
#include <boost/timer/timer.hpp>
#include <boost/version.hpp>
#include <boost/filesystem.hpp>

// use OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#ifdef _DEBUG
#pragma comment(lib, "opencv_world420d.lib")
#else
#pragma comment(lib, "opencv_world420.lib")
#endif

// use FFmpeg
extern "C" {
	#include <libavutil/imgutils.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}
#pragma comment(lib, "libswscale.dll.a")
#pragma comment(lib, "libavcodec.dll.a")
#pragma comment(lib, "libavfilter.dll.a")
#pragma comment(lib, "libavformat.dll.a")
#pragma comment(lib, "libavutil.dll.a")
#pragma comment(lib, "libswresample.dll.a")

// EOF