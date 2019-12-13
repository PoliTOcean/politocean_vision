#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "capture.hpp"

#define LIVE_WIN_NAME "LIVE"
#define RENDER_WIN_NAME "RENDER"

#define RED Scalar(0, 0, 255)
#define PINK Scalar(230, 130, 255)
#define BLUE Scalar(255, 0, 0)
#define LIGHTBLUE Scalar(255, 255, 160)
#define GREEN Scalar(0, 255, 0)
#define BGD_KEY EVENT_FLAG_CTRLKEY
#define FGD_KEY EVENT_FLAG_SHIFTKEY

using namespace std;
using namespace cv;

static void getBinMask(const Mat &comMask, Mat &binMask)
{
    if (comMask.empty() || comMask.type() != CV_8UC1)
        CV_Error(Error::StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
    if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
        binMask.create(comMask.size(), CV_8UC1);
    binMask = comMask & 1;
}

class Grabcut
{
public:
    Grabcut();

    void showImage();
    void mouseClick(int event, int x, int y, int flags, void *param);
    void reset();
    int nextIter();
    int getIterCount() const { return iterCount; }
    void getRect() { cout << rect << endl; }

private:
    void setRectInMask();
    void setLblsInMask(int flags, Point p, bool isPr);
    Capture cap;
    Mat mask;
    Mat bgdModel, fgdModel;
    Rect rect;
    uchar rectState, lblsState, prLblsState;
    bool isInitialized;
    vector<Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
    int iterCount;

    enum
    {
        NOT_SET = 0,
        IN_PROCESS = 1,
        SET = 2
    };
    static const int radius = 2;
    static const int thickness = -1;
};

Grabcut::Grabcut() : cap()
{
    cap.init();
    cap.start();

    while (cap.getFrame().empty())
        ;

    mask.create(cap.getFrame().size(), CV_8UC1);
    reset();
}

void Grabcut::reset()
{
    if (!mask.empty())
        mask.setTo(Scalar::all(GC_BGD));

    bgdPxls.clear();
    fgdPxls.clear();
    prBgdPxls.clear();
    prFgdPxls.clear();
    isInitialized = false;
    rectState = NOT_SET;
    lblsState = NOT_SET;
    prLblsState = NOT_SET;
    iterCount = 0;
}

void Grabcut::showImage()
{
    Mat frame = cap.getFrame();
    if (frame.empty())
        return;

    Mat res, binMask;
    if (!isInitialized)
        frame.copyTo(res);
    else
    {
        getBinMask(mask, binMask);
        frame.copyTo(res, binMask);
    }

    vector<Point>::const_iterator it;
    for (it = bgdPxls.begin(); it != bgdPxls.end(); ++it)
        circle(res, *it, radius, BLUE, thickness);
    for (it = fgdPxls.begin(); it != fgdPxls.end(); ++it)
        circle(res, *it, radius, RED, thickness);
    for (it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it)
        circle(res, *it, radius, LIGHTBLUE, thickness);
    for (it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it)
        circle(res, *it, radius, PINK, thickness);

    if (rectState == IN_PROCESS || rectState == SET)
        rectangle(res, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), GREEN, 2);

    imshow(LIVE_WIN_NAME, res);
}

void Grabcut::setRectInMask()
{
    Mat frame = cap.getFrame();

    CV_Assert(!mask.empty());
    mask.setTo(GC_BGD);
    rect.x = max(0, rect.x);
    rect.y = max(0, rect.y);
    rect.width = min(rect.width, frame.cols - rect.x);
    rect.height = min(rect.height, frame.rows - rect.y);
    (mask(rect)).setTo(Scalar(GC_PR_FGD));
}

void Grabcut::setLblsInMask(int flags, Point p, bool isPr)
{
    vector<Point> *bpxls, *fpxls;
    uchar bvalue, fvalue;
    if (!isPr)
    {
        bpxls = &bgdPxls;
        fpxls = &fgdPxls;
        bvalue = GC_BGD;
        fvalue = GC_FGD;
    }
    else
    {
        bpxls = &prBgdPxls;
        fpxls = &prFgdPxls;
        bvalue = GC_PR_BGD;
        fvalue = GC_PR_FGD;
    }
    if (flags & BGD_KEY)
    {
        bpxls->push_back(p);
        circle(mask, p, radius, bvalue, thickness);
    }
    if (flags & FGD_KEY)
    {
        fpxls->push_back(p);
        circle(mask, p, radius, fvalue, thickness);
    }
}

void Grabcut::mouseClick(int event, int x, int y, int flags, void *)
{
    switch (event)
    {
    case EVENT_LBUTTONDOWN:
    {
        bool isb = (flags & BGD_KEY) != 0,
             isf = (flags & FGD_KEY) != 0;
        if (rectState == NOT_SET && !isb && !isf)
        {
            rectState = IN_PROCESS;
            rect = Rect(x, y, 1, 1);
        }
        if ((isb || isf) && rectState == SET)
            lblsState = IN_PROCESS;
    }
    break;

    case EVENT_LBUTTONUP:
        if (rectState == IN_PROCESS)
        {
            rect = Rect(Point(rect.x, rect.y), Point(x, y));
            rectState = SET;
            setRectInMask();
            CV_Assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
        }
        if (lblsState == IN_PROCESS)
        {
            setLblsInMask(flags, Point(x, y), false);
            lblsState = SET;
        }
        break;

    case EVENT_MOUSEMOVE:
        if (rectState == IN_PROCESS)
        {
            rect = Rect(Point(rect.x, rect.y), Point(x, y));
            CV_Assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
        }
        else if (lblsState == IN_PROCESS)
        {
            setLblsInMask(flags, Point(x, y), false);
        }
        else if (prLblsState == IN_PROCESS)
        {
            setLblsInMask(flags, Point(x, y), true);
        }
        break;
    }
}

int Grabcut::nextIter()
{
    Mat frame = cap.getFrame();
    if (isInitialized)
        grabCut(frame, mask, rect, bgdModel, fgdModel, 1);
    else
    {
        if (rectState != SET)
            return iterCount;
        if (lblsState == SET || prLblsState == SET)
            grabCut(frame, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
        else
            grabCut(frame, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
        isInitialized = true;
    }
    iterCount++;
    bgdPxls.clear();
    fgdPxls.clear();
    prBgdPxls.clear();
    prFgdPxls.clear();
    return iterCount;
}

Grabcut gc;

static void on_mouse(int event, int x, int y, int flags, void *param)
{
    gc.mouseClick(event, x, y, flags, param);
}

int main(int argc, char **argv)
{
    namedWindow(LIVE_WIN_NAME, WINDOW_AUTOSIZE);
    setMouseCallback(LIVE_WIN_NAME, on_mouse, 0);

    for (;;)
    {
        gc.showImage();
        int c = waitKey(30);
        switch ((char)c)
        {
        case '\x1b':
            cout << "Exiting ..." << endl;
            goto exit_main;
        case 'r':
            cout << endl;
            gc.reset();
            break;
        case 'n':
            int iterCount = gc.getIterCount();
            cout << "<" << iterCount << "... ";
            int newIterCount = gc.nextIter();
            if (newIterCount > iterCount)
            {
                gc.showImage();
                cout << iterCount << ">" << endl;
            }
            else
                cout << "rect must be determined>" << endl;
            break;
        }
    }
exit_main:
    destroyWindow(LIVE_WIN_NAME);
}