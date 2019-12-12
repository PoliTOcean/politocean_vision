#include "capture.hpp"

void Capture::init()
{
    if (!cap.open(0))
        throw CaptureException("Error opening webcam");
}

void Capture::start()
{
    if (isCapturing_)
        return;

    isCapturing_ = true;
    captureThread_ = new std::thread([&]() {
        for (;;)
        {
            cap.read(frame);
            if (frame.empty())
                break;
        }
    });
}

cv::Mat Capture::getFrame()
{
    return frame;
}

bool Capture::isCapturing()
{
    return isCapturing_;
}