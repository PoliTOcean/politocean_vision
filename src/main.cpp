

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;
int main(int argc, char **argv)
{
    // Declare the output variables
    Mat dst, cdst, cdstP;
    int i, flag;
    flag = 0; //ti find first value min and max
    Point pm1, pm2, pM1, pM2, medio1, medio2;
    //points to draw lines: pm min x, pM max x so I have the first and last vertical lines

    const char *default_file = "grid.jpeg";
    const char *filename = argc >= 2 ? argv[1] : default_file;
    // Loads an image
    Mat src = imread(samples::findFile(filename));

    Mat blanc = Mat::ones(src.rows, src.cols, CV_64F);

    //at the end i want only three lines
    Mat blanclast = Mat::ones(src.rows, src.cols, CV_64F);
    //CV_64F -> bianco      CV_8UC3 nero

    // Check if image is loaded fine
    if (src.empty())
    {
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default %s] \n", default_file);
        return -1;
    }

    //_____________________________________________________________________
    //I create a mask to found blue
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);

    Mat mask1;
    // Creating masks to detect the upper and lower blue color.
    inRange(hsv, Scalar(110, 50, 50), Scalar(130, 255, 255), mask1);

    imshow("magic", mask1);

    //_____________________________________________________________________

    // Edge detection-> on the mask!
    Canny(mask1, dst, 50, 200, 3);

    // Copy edges to the images that will display the results in BGR
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    cdstP = cdst.clone();
    // Standard Hough Line Transform
    vector<Vec2f> lines;                               // will hold the results of the detection
    HoughLines(dst, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection
    // Draw the lines
    Mat cdstclone = cdst.clone();
    for (i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 100 * (a));
        line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        // line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);

        //i use blanc to find just vertical lines
        line(blanc, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);

        if (flag == 0)
        {
            pM1 = pt1;
            pM2 = pt2;

            pm1 = pt1;
            pm2 = pt2;
            //printf(" FIRSTTTT Point: pm1x:%d  pm1y:%d     pmx2:%d pmy2:%d    pMx1:%d  pMy1:%d    pMx2:%d  pMy2:%d\n", pm1.x, pm1.y, pm2.x, pm2.y, pM1.x, pM1.y, pM2.x, pM2.y);
            flag = 1;
        }
        else
        {
            if (pt1.x < pm1.x)
            {
                pm1 = pt1;
                pm2 = pt2;
            }
            else if (pt1.x > pM1.x)
            {
                pM1 = pt1;
                pM2 = pt2;
            }
        }

        // printf("Point: x0:%f  y0:%f     x1:%d y1:%d    x2:%d  y2:%d\n", x0, y0, pt1.x, pt1.y, pt2.x, pt2.y);
    }
    /*
    // Probabilistic Line Transform
    // The best is HoughLines, I decided to not use Probabilistic HLT
    vector<Vec4i> linesP;                               // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI / 180, 50, 50, 10); // runs the actual detection
    // Draw the lines
    for (size_t i = 0; i < linesP.size(); i++)
    {
        Vec4i l = linesP[i];
        line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
    }
    */

    //Points of lines
    printf("Point: pm1x:%d  pm1y:%d     pmx2:%d pmy2:%d    pMx1:%d  pMy1:%d    pMx2:%d  pMy2:%d\n", pm1.x, pm1.y, pm2.x, pm2.y, pM1.x, pM1.y, pM2.x, pM2.y);

    line(blanclast, pm1, pm2, Scalar(0, 0, 255), 3);
    line(blanclast, pM1, pM2, Scalar(0, 0, 255), 3);

    //calculate the third line
    medio1.x = (pm1.x + pM1.x) / 2;
    medio1.y = (pm1.y + pM2.y) / 2;
    medio2.x = (pm2.x + pM2.x) / 2;
    medio2.y = (pm2.y + pM2.y) / 2;

    line(blanclast, medio1, medio2, Scalar(0, 0, 255), 3);
    // Show results
    imshow("Source", src);

    imshow("dst", dst);
    imshow("Bianco", blanc);

    imshow("Bianco Ultimo", blanclast);
    imshow("Detected Lines (in red) - Standard Hough Line Transform   ", cdst);

    //imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
    // Wait and Exit
    waitKey();
    return 0;
}

