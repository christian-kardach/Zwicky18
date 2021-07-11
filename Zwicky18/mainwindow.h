#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QFileDialog>

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"

#include "usbcameras.h"
#include "previewscenewidget.h"
#include "previewscene.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct CachedImages
{
    CachedImages() {}
    CachedImages(cv::Mat *_sourceImage, cv::Mat *_stabilizedImage)
    {
        sourceImage = _sourceImage;
        stabilizedImage = _stabilizedImage;
    }
    bool cached = false;
    bool cachedStabalized = false;
    cv::Mat *sourceImage;
    cv::Mat *stabilizedImage;
    cv::Mat T;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // QThread* usbCamerasThread;
    USBCameras* usbCameras;
    PreviewSceneWidget* previewScene;

    std::vector<CachedImages> cachedImages;
    cv::VideoCapture* videoSource;
    int activeFrame = 0;

public slots:
    void addCamera(QString name, QVariant v);
    void changeCamera(int index);

    void hasBrightness(bool enabled, int emin, int emax);
    void hasExposure(bool enabled, int emin, int emax);
    void hasGamma(bool enabled, int emin, int emax);
    void hasWhiteBalance(bool enabled, int emin, int emax);
    void hasGain(bool enabled, int emin, int emax);
    void hasContrast(bool enabled, int emin, int emax);
    void hasSaturation(bool enabled, int emin, int emax);
    void hasFocus(bool enabled, int emin, int emax);
    void hasZoom(bool enabled, int emin, int emax);
    void hasHue(bool enabled, int emin, int emax);
    void hasSharpness(bool enabled, int emin, int emax);
    void hasBacklightcomp(bool enabled, int emin, int emax);
    void hasPowerlineFreq(bool enabled, int emin, int emax);

    void hasAutoExposure(bool enabled, int checked);
    void hasAutoGain(bool enabled, int checked);
    void hasAutoWhiteBalance(bool enabled, int checked);
    void hasAutoFocus(bool enabled, int checked);

    void updateStatusBar(QString message);

    QImage cvMatToQImageVideo(const cv::Mat *inMat);
    QImage cvMatToQImage( const cv::Mat *inMat );

    void actionOpenVideoFile();
    void getSourceVideoInfo();
    void frameChanged(int);



    void mainUpdateLoop();

};
#endif // MAINWINDOW_H
