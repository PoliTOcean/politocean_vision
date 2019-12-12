#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <opencv2/highgui.hpp>

#include <iostream>
#include <exception>
#include <thread>
#include <chrono>

class CaptureException : public std::exception
{
    std::string message;

public:
    CaptureException(std::string message) : message(message) {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }
};

class Capture
{
    cv::VideoCapture cap;
    cv::Mat frame;

    std::thread *captureThread_;
    bool isCapturing_;

public:
    void init();
    void start();
    cv::Mat getFrame();

    bool isCapturing();
};

#endif //CAPTURE_HPP