#ifndef USBCAMERAS_H
#define USBCAMERAS_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QVariant>
#include <QThread>

#include <vector>
#include "openpnp-capture.h"

struct CustomComboBoxData
{
    uint32_t m_device;
    uint32_t m_format;
};

class USBCameras : public QThread
{
    Q_OBJECT
public:
    explicit USBCameras(QObject *parent = nullptr);
    ~USBCameras();

    void readCameraSettings();

public slots:
    void doFrameUpdate();
    // void updateLogMessages();
    void changeCamera(QVariant v);
    void closeCamera();
    void listCameras();

    void onAutoExposure(bool state);
    void onAutoWhiteBalance(bool state);
    void onAutoGain(bool state);
    void onAutoFocus(bool state);
    void onExposureSlider(int value);
    void onWhiteBalanceSlider(int value);
    void onGainSlider(int value);
    void onContrastSlider(int value);
    void onBrightnessSlider(int value);
    void onSaturationSlider(int value);
    void onFocusSlider(int value);
    void onZoomSlider(int value);
    void onGammaSlider(int value);
    void onHueSlider(int value);
    void onBacklightSlider(int value);
    void onSharpnessSlider(int value);
    void onColorEnableSlider(int value);

private:
    CapFormatInfo           m_finfo;
    std::vector<uint8_t>    m_frameData;

    QList<QVariant> m_cameraList;

    bool        m_hasBrightness;
    bool        m_hasExposure;
    bool        m_hasGamma;
    bool        m_hasWhiteBalance;
    bool        m_hasGain;
    bool        m_hasContrast;
    bool        m_hasSaturation;
    bool        m_hasFocus;
    bool        m_hasZoom;
    bool        m_hasHue;
    bool        m_hasSharpness;
    bool        m_hasBacklightcomp;
    bool        m_hasColorEnable;
    bool        m_hasPowerlineFreqEnable;

    QTimer*     m_refreshTimer;
    CapContext  m_ctx;
    int32_t     m_streamID;

signals:
    void addCamera(QString name, QVariant );
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

    void setBrightness(int val);
    void setExposure(int val);
    void setGamma(int val);
    void setWhiteBalance(int val);
    void setGain(int val);
    void setContrast(int val);
    void setSaturation(int val);
    void setFocus(int val);
    void setZoom(int val);
    void setHue(int val);
    void setSharpness(int val);
    void setBacklightcomp(int val);
    void setPowerlineFreq(int val);

    void frameImage(QImage frame);
    void updateStatusBar(QString message);
};

#endif // USBCAMERAS_H
