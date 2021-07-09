#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "usbcameras.h"
#include "previewscenewidget.h"

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

    // QThread* usbCamerasThread;
    USBCameras* usbCameras;
    PreviewSceneWidget* previewScene;

public slots:
    void addCamera(QString name, QVariant v);
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

};
#endif // MAINWINDOW_H
