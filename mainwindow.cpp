#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <string>
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialise Video Capture
    initialiseVideoCapture();

    //Frame Update timer, 20ms
    QTimer *timer = new QTimer(this);
    timer->start(20);

    connect(timer, &QTimer::timeout, [=](){
        updateLabel();
    });
}

void MainWindow::updateLabel()
{
    //Capture initial frame
    videoCapture >> frame;

    if (!frame.empty()) {

        // Draw a line vertically down the middle of the image
        line(frame, Point(frame.cols / 2, 0), Point(frame.cols / 2, frame.rows), Scalar(0, 255, 0), 2);

        // Count people crossing the line
        int crossingCount = countPeopleCrossing(frame);

        // Update count
        peopleCount += crossingCount;

        // Display the count on the frame
        putText(frame, "Object Count: " + std::to_string(peopleCount), Point(10, 120), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        // Display my Logo on the frame
        putText(frame, "GitDhamani", Point(450, 120), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        // Get the size of the label
        QSize labelSize = ui->label->size();

        // Write the frame to the video file in native size, before I resize
        videoWriter.write(frame);

        //Resize the captured Frame to fit inside the Label instead of its regular resolution size
        cv::resize(frame, frame, Size(labelSize.width(), labelSize.height()));

        //Place the frame into an image
        QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        //Load the image into a Pixmap and load the Pixmap into the Label.
        ui->label->setPixmap(QPixmap::fromImage(img.rgbSwapped()));
    }
}

void MainWindow::initialiseVideoCapture()
{
    videoCapture >> frame;
   // Define the codec and create the video writer
   int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
   int fps = 30;
   cv::Size frameSize(frame.cols, frame.rows);

   //Filename must have a number in it.
   videoWriter.open("output_01.avi", fourcc, fps, frameSize, true);

   if (videoWriter.isOpened())
       qDebug() << "Video opened for writing.";
   else
       qDebug() << "Error: Could not open video file for writing.";
}

MainWindow::~MainWindow()
{
    frame.release();
    previousFrame.release();
    videoWriter.release();
    cv::destroyAllWindows();
    delete ui;
}

int MainWindow::countPeopleCrossing(const Mat &frame)
{
    // Convert the frame to grayscale
    Mat grayFrame;
    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);

    if (previousFrame.empty()) {
        // Initialize previousFrame on first iteration
        previousFrame = grayFrame.clone();
        return 0;
    }

    // Compute the absolute difference between the current frame and the previous frame
    Mat frameDiff;
    absdiff(previousFrame, grayFrame, frameDiff);

    // Apply blur to reduce noise
    GaussianBlur(frameDiff, frameDiff, Size(5, 5), 0);

    // Apply thresholding to obtain a binary image
    threshold(frameDiff, frameDiff, 50, 255, THRESH_BINARY);

    // Find contours in the thresholded image
    std::vector<std::vector<Point>> contours;
    findContours(frameDiff, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int crossingCount = 0;
    int linePosition = frame.cols / 2;

    // Draw the line for visualization
    line(frame, Point(linePosition, 0), Point(linePosition, frame.rows), Scalar(0, 0, 255), 2);

    // Iterate through the contours and draw bounding boxes around them
    for (const auto &contour : contours)
    {
        // Get bounding box for each contour
        Rect boundingBox = boundingRect(contour);

        // Draw a bounding box around each contour
        rectangle(frame, boundingBox, Scalar(0, 255, 0), 2);

        //Detect Crossing
        if (boundingBox.tl().x < (linePosition + 10) && boundingBox.tl().x > (linePosition - 10))
        {
            if (armed == 0)
            {
                armed = 1;
                // Draw the line for visualization
                line(frame, Point(linePosition, 0), Point(linePosition, frame.rows), Scalar(0, 255, 0), 2);
                QTimer::singleShot(1000, NULL, [=](){
                            line(frame, Point(linePosition, 0), Point(linePosition, frame.rows), Scalar(0, 255, 0), 2);
                        });
                crossingCount++;
                QTimer::singleShot(4000, NULL, [this](){
                            armed = 0;
                        });
            break;
            }

        }
    }

    // Update the previous frame
    previousFrame = grayFrame.clone();

    return crossingCount;
}



