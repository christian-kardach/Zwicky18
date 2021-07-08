#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "openpnp-capture.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // USB Cameras
    usbCameras = new USBCameras(this);
    // usbCamerasThread = new QThread();
    usbCameras->moveToThread(QApplication::instance()->thread());

    connect(ui->cameraChooser, SIGNAL(currentIndexChanged(int)), usbCameras, SLOT(changeCamera(int)));

    // add exposure and white balance checkboxes

    connect(ui->autoExposure, SIGNAL(toggled(bool)), usbCameras, SLOT(onAutoExposure(bool)));
    connect(ui->autoWhiteBalance, SIGNAL(toggled(bool)), usbCameras, SLOT(onAutoWhiteBalance(bool)));
    connect(ui->exposureSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onExposureSlider(int)));
    connect(ui->whitebalanceSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onWhiteBalanceSlider(int)));
    connect(ui->autoGain, SIGNAL(toggled(bool)), usbCameras, SLOT(onAutoGain(bool)));
    connect(ui->autoFocus, SIGNAL(toggled(bool)), usbCameras, SLOT(onAutoFocus(bool)));
    connect(ui->gainSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onGainSlider(int)));
    connect(ui->contrastSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onContrastSlider(int)));
    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onBrightnessSlider(int)));
    connect(ui->gammaSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onGammaSlider(int)));
    connect(ui->focusSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onFocusSlider(int)));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onZoomSlider(int)));
    connect(ui->hueSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onHueSlider(int)));
    connect(ui->backlightSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onBacklightSlider(int)));
    connect(ui->sharpnessSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onSharpnessSlider(int)));
    connect(ui->powerlineFreqSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onColorEnableSlider(int)));
    connect(ui->saturationSlider, SIGNAL(valueChanged(int)), usbCameras, SLOT(onSaturationSlider(int)));

    connect(usbCameras, SIGNAL(addCamera(QString, QVariant)), this, SLOT(addCamera(QString, QVariant)));
    connect(usbCameras, SIGNAL(hasBrightness(bool, int, int)), this, SLOT(hasBrightness(bool, int, int)));
    connect(usbCameras, SIGNAL(hasExposure(bool, int, int)), this, SLOT(hasExposure(bool, int, int)));
    connect(usbCameras, SIGNAL(hasGamma(bool, int, int)), this, SLOT(hasGamma(bool, int, int)));
    connect(usbCameras, SIGNAL(hasWhiteBalance(bool, int, int)), this, SLOT(hasWhiteBalance(bool, int, int)));
    connect(usbCameras, SIGNAL(hasGain(bool, int, int)), this, SLOT(hasGain(bool, int, int)));
    connect(usbCameras, SIGNAL(hasContrast(bool, int, int)), this, SLOT(hasContrast(bool, int, int)));
    connect(usbCameras, SIGNAL(hasSaturation(bool, int, int)), this, SLOT(hasSaturation(bool, int, int)));
    connect(usbCameras, SIGNAL(hasFocus(bool, int, int)), this, SLOT(hasFocus(bool, int, int)));
    connect(usbCameras, SIGNAL(hasZoom(bool, int, int)), this, SLOT(hasZoom(bool, int, int)));
    connect(usbCameras, SIGNAL(hasHue(bool, int, int)), this, SLOT(hasHue(bool, int, int)));
    connect(usbCameras, SIGNAL(hasSharpness(bool, int, int)), this, SLOT(hasSharpness(bool, int, int)));
    connect(usbCameras, SIGNAL(hasBacklightcomp(bool, int, int)), this, SLOT(hasBacklightcomp(bool, int, int)));

    connect(usbCameras, SIGNAL(setBrightness(int)), ui->brightnessSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setExposure(int)), ui->exposureSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setGamma(int)), ui->gammaSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setWhiteBalance(int)), ui->whitebalanceSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setGain(int)), ui->gainSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setContrast(int)), ui->contrastSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setSaturation(int)), ui->saturationSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setFocus(int)), ui->focusSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setZoom(int)), ui->zoomSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setHue(int)), ui->hueSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setSharpness(int)), ui->sharpnessSlider, SLOT(setValue(int)));
    connect(usbCameras, SIGNAL(setBacklightcomp(int)), ui->backlightSlider, SLOT(setValue(int)));

    usbCameras->listCameras();

    // Graphics Scene
    previewScene = new PreviewSceneWidget(this);
    ui->liveViewLayout->insertWidget(0, previewScene);
    connect(usbCameras, SIGNAL(frameImage(QImage)), previewScene, SLOT(updateFrame(QImage)));

    // Splitter
    ui->splitter->setSizes(QList<int>({400, 100}));
    ui->splitter_2->setSizes(QList<int>({800, 100}));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addCamera(QString name, QVariant v)
{
   ui->cameraChooser->addItem(name, v);
}

void MainWindow::hasAutoExposure(bool enabled, int checked)
{
    ui->autoExposure->setEnabled(enabled);
    ui->autoExposure->setCheckState((checked==0) ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::hasAutoGain(bool enabled, int checked)
{
    ui->autoGain->setEnabled(enabled);
    ui->autoGain->setCheckState((checked==0) ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::hasAutoWhiteBalance(bool enabled, int checked)
{
    ui->autoWhiteBalance->setEnabled(enabled);
    ui->autoWhiteBalance->setCheckState((checked==0) ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::hasAutoFocus(bool enabled, int checked)
{
    ui->autoFocus->setEnabled(enabled);
    ui->autoFocus->setCheckState((checked==0) ? Qt::Unchecked : Qt::Checked);
}

void MainWindow::hasBrightness(bool enabled, int emin, int emax)
{
    ui->brightnessSlider->setEnabled(enabled);
    ui->brightnessSlider->setRange(emin, emax);
    ui->brightnessSpinBox->setRange(emin, emax);
    ui->brightnessSpinBox->setEnabled(enabled);
}

void MainWindow::hasExposure(bool enabled, int emin, int emax)
{
    ui->exposureSlider->setEnabled(enabled);
    ui->exposureSlider->setRange(emin, emax);
    ui->exposureSpinBox->setRange(emin, emax);
    ui->exposureSpinBox->setEnabled(enabled);
}

void MainWindow::hasGamma(bool enabled, int emin, int emax)
{
    ui->gammaSlider->setEnabled(enabled);
    ui->gammaSlider->setRange(emin, emax);
    ui->gammaSpinBox->setRange(emin, emax);
    ui->gammaSpinBox->setEnabled(enabled);
}

void MainWindow::hasWhiteBalance(bool enabled, int emin, int emax)
{
    ui->whitebalanceSlider->setEnabled(enabled);
    ui->whitebalanceSlider->setRange(emin, emax);
    ui->whitebalanceSpinBox->setRange(emin, emax);
    ui->whitebalanceSpinBox->setEnabled(enabled);
}

void MainWindow::hasGain(bool enabled, int emin, int emax)
{
    ui->gainSlider->setEnabled(enabled);
    ui->gainSlider->setRange(emin, emax);
    ui->gainSpinBox->setRange(emin, emax);
    ui->gainSpinBox->setEnabled(enabled);
}

void MainWindow::hasContrast(bool enabled, int emin, int emax)
{
    ui->contrastSlider->setEnabled(enabled);
    ui->contrastSlider->setRange(emin, emax);
    ui->contrastSpinBox->setRange(emin, emax);
    ui->contrastSpinBox->setEnabled(enabled);
}

void MainWindow::hasSaturation(bool enabled, int emin, int emax)
{
    ui->saturationSlider->setEnabled(enabled);
    ui->saturationSlider->setRange(emin, emax);
    ui->saturationSpinBox->setRange(emin, emax);
    ui->saturationSpinBox->setEnabled(enabled);
}

void MainWindow::hasFocus(bool enabled, int emin, int emax)
{
    ui->focusSlider->setEnabled(enabled);
    ui->focusSlider->setRange(emin, emax);
    ui->focusSpinBox->setRange(emin, emax);
    ui->focusSpinBox->setEnabled(enabled);
}

void MainWindow::hasZoom(bool enabled, int emin, int emax)
{
    ui->zoomSlider->setEnabled(enabled);
    ui->zoomSlider->setRange(emin, emax);
    ui->zoomSpinBox->setRange(emin, emax);
    ui->zoomSpinBox->setEnabled(enabled);
}
void MainWindow::hasHue(bool enabled, int emin, int emax)
{
    ui->hueSlider->setEnabled(enabled);
    ui->hueSlider->setRange(emin, emax);
    ui->hueSpinBox->setRange(emin, emax);
    ui->hueSpinBox->setEnabled(enabled);
}
void MainWindow::hasSharpness(bool enabled, int emin, int emax)
{
    ui->sharpnessSlider->setEnabled(enabled);
    ui->sharpnessSlider->setRange(emin, emax);
    ui->sharpnessSpinBox->setRange(emin, emax);
    ui->sharpnessSpinBox->setEnabled(enabled);
}
void MainWindow::hasBacklightcomp(bool enabled, int emin, int emax)
{
    ui->backlightSlider->setEnabled(enabled);
    ui->backlightSlider->setRange(emin, emax);
    ui->backlightSpinBox->setRange(emin, emax);
    ui->backlightSpinBox->setEnabled(enabled);
}


