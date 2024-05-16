#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void updateLabel();
    void initialiseVideoCapture();
    int countPeopleCrossing(const Mat &frame);
    VideoCapture videoCapture{0};  //My default Cam
    Mat frame;
    cv::Mat previousFrame;
    int peopleCount = 0;
    bool armed = 0;
    cv::VideoWriter videoWriter;
};
#endif // MAINWINDOW_H
