#include "usbcameras.h"


Q_DECLARE_METATYPE(CustomComboBoxData)

USBCameras::USBCameras(QObject *parent) : QThread(parent)
{
    //m_ctx = Cap_createContext();

    //CapDeviceID deviceID = 0;
    //CapFormatID formatID = 0;

    //m_streamID = Cap_openStream(m_ctx, deviceID, formatID);
    //Cap_getFormatInfo(m_ctx, deviceID, formatID, &m_finfo);

    //m_frameData.resize(m_finfo.width*m_finfo.height*3);


    /*
    connect(ui->cameraChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCamera()));

    // add exposure and white balance checkboxes
    connect(ui->autoExposure, SIGNAL(toggled(bool)), this, SLOT(onAutoExposure(bool)));
    connect(ui->autoWhiteBalance, SIGNAL(toggled(bool)),this, SLOT(onAutoWhiteBalance(bool)));
    connect(ui->exposureSlider, SIGNAL(valueChanged(int)),this, SLOT(onExposureSlider(int)));
    connect(ui->whitebalanceSlider, SIGNAL(valueChanged(int)),this, SLOT(onWhiteBalanceSlider(int)));
    connect(ui->autoGain, SIGNAL(toggled(bool)), this, SLOT(onAutoGain(bool)));
    connect(ui->autoFocus, SIGNAL(toggled(bool)), this, SLOT(onAutoFocus(bool)));
    connect(ui->gainSlider, SIGNAL(valueChanged(int)), this, SLOT(onGainSlider(int)));
    connect(ui->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(onContrastSlider(int)));
    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(onBrightnessSlider(int)));
    connect(ui->gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(onGammaSlider(int)));
    connect(ui->focusSlider, SIGNAL(valueChanged(int)), this, SLOT(onFocusSlider(int)));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(onZoomSlider(int)));
    connect(ui->hueSlider, SIGNAL(valueChanged(int)), this, SLOT(onHueSlider(int)));
    connect(ui->backlightSlider, SIGNAL(valueChanged(int)), this, SLOT(onBacklightSlider(int)));
    connect(ui->sharpnessSlider, SIGNAL(valueChanged(int)), this, SLOT(onSharpnessSlider(int)));
    connect(ui->powerlineFreqSlider, SIGNAL(valueChanged(int)), this, SLOT(onColorEnableSlider(int)));
    connect(ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(onSaturationSlider(int)));
    */
    // add timer to refresh the frame display
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(doFrameUpdate()));
    // m_refreshTimer->start(50);

    // update GUI to reflect the actual camera settings
    //readCameraSettings();

}

USBCameras::~USBCameras()
{
    m_refreshTimer->stop();
    delete m_refreshTimer;

    Cap_closeStream(m_ctx, m_streamID);
    Cap_releaseContext(m_ctx);
}

void USBCameras::listCameras()
{
    m_ctx = Cap_createContext();

    emit addCamera("Select camera...", QVariant("null"));

    for(uint32_t device=0; device<Cap_getDeviceCount(m_ctx); device++)
    {
        QString deviceName = Cap_getDeviceName(m_ctx, device);
        for(int32_t format=0; format<Cap_getNumFormats(m_ctx, device); format++)
        {
            CapFormatInfo finfo;
            Cap_getFormatInfo(m_ctx, device, format, &finfo);

            QString fourcc;
            for(uint32_t i=0; i<4; i++)
            {
                fourcc += (char)(finfo.fourcc & 0xFF);
                finfo.fourcc >>= 8;
            }

            QString formatName = QString::asprintf("%dx%d %s", finfo.width, finfo.height, fourcc.toLatin1().data());
            QString total = deviceName + " " + formatName;
            CustomComboBoxData customData;
            customData.m_device = device;
            customData.m_format = format;
            QVariant v;
            v.setValue(customData);

            m_cameraList.append(v);
            emit addCamera(total, v);
            //m_mainWindow->ui->cameraChooser->addItem(total, v);
        }
    }
}

void USBCameras::doFrameUpdate()
{
    if (Cap_hasNewFrame(m_ctx, m_streamID))
    {
        Cap_captureFrame(m_ctx, m_streamID, &m_frameData[0], m_frameData.size());
        QImage img((const uint8_t*)&m_frameData[0],
                m_finfo.width,
                m_finfo.height,
                //m_finfo.width*3,
                QImage::Format_RGB888);

        if (img.isNull())
        {
            qDebug() << "Cannot create image..";
            return;
        }
        // m_scene->m_image->fromImage(img);

        // ui->frameDisplay->setPixmap(QPixmap::fromImage(img));
        // previewWidget->updateFrame(img);
        emit frameImage(img);

        QString frameInfo = QString::asprintf("%d x %d frames:%d",
                                              m_finfo.width,
                                              m_finfo.height,
                                              Cap_getStreamFrameCount(m_ctx, m_streamID));

        //ui->statusBar->showMessage(frameInfo);
        emit updateStatusBar(frameInfo);
    }

    // also update the log messages, if there are some
    // updateLogMessages();
}

void USBCameras::changeCamera(QVariant v)
{
    // QVariant v = ui->cameraChooser->currentData();
    // QVariant v = m_cameraList.at(index+1);
    CustomComboBoxData data = v.value<CustomComboBoxData>();

    // kill currently running stream
    m_refreshTimer->stop();
    Cap_closeStream(m_ctx, m_streamID);

    qDebug() << "Opening new device/format: " << data.m_device << data.m_format;

    // open new stream
    m_streamID = Cap_openStream(m_ctx, data.m_device, data.m_format);

    qDebug() << "New stream ID" << m_streamID;

    // resize the display window
    Cap_getFormatInfo(m_ctx, data.m_device, data.m_format, &m_finfo);
    m_frameData.resize(m_finfo.width*m_finfo.height*3);

    //ui->frameDisplay->setFixedSize(QSize(m_finfo.width, m_finfo.height));
    //ui->frameDisplay->setStyleSheet("border: 1px solid red");

    readCameraSettings();

    m_refreshTimer->start(50);
}

void USBCameras::closeCamera()
{
    qDebug() << "Closing camera stream...";
    m_refreshTimer->stop();
    Cap_closeStream(m_ctx, m_streamID);
}

void USBCameras::onAutoExposure(bool state)
{
    qDebug() << "Auto exposure set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, state ? 1 : 0);
    // emit hasAutoExposure((!state), m_hasExposure);
    // ui->exposureSlider->setEnabled((!state) & m_hasExposure);
}

void USBCameras::onAutoWhiteBalance(bool state)
{
    qDebug() << "Auto white balance set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, state ? 1 : 0);
    //ui->whitebalanceSlider->setEnabled((!state) & m_hasWhiteBalance);
    // emit hasAutoWhiteBalance(!state, m_hasWhiteBalance);
}

void USBCameras::onAutoGain(bool state)
{
    qDebug() << "Auto gain set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_GAIN, state ? 1 : 0);
    //ui->gainSlider->setEnabled((!state) & m_hasGain);
    //emit hasAutoGain(!state, m_hasGain);
}

void USBCameras::onAutoFocus(bool state)
{
    qDebug() << "Auto focus set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, state ? 1 : 0);
    //ui->gainSlider->setEnabled((!state) & m_hasGain);
    // emit hasAutoFocus(!state, m_hasFocus);
}

void USBCameras::readCameraSettings()
{
    qDebug() << "readCameraSettings -> Context = " << m_ctx;

    // enable the sliders that also have an AUTO setting.
    // we will disable them later on when we check the
    // AUTO settings.

    m_hasBrightness = false;
    m_hasExposure = false;
    m_hasGamma = false;
    m_hasWhiteBalance = false;
    m_hasGain = false;
    m_hasContrast = false;
    m_hasSaturation = false;
    m_hasFocus = false;
    m_hasZoom = false;
    m_hasColorEnable = false;

    // ********************************************************************************
    //   AUTO EXPOSURE
    // ********************************************************************************

    uint32_t bValue;
    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoExposure(false, 0);
    }
    else
    {
        emit hasAutoExposure(true, bValue);
    }

    // ********************************************************************************
    //   AUTO GAIN
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_GAIN, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoGain(false, 0);
    }
    else
    {
        emit hasAutoGain(true, bValue);
    }

    // ********************************************************************************
    //   AUTO WHITE BALANCE
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoWhiteBalance(false, 0);
    }
    else
    {
        emit hasAutoWhiteBalance(true, bValue);
    }

    // ********************************************************************************
    //   AUTO FOCUS
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, &bValue) != CAPRESULT_OK)
    {
        qDebug() << "Autofocus unsupported";
        emit hasAutoFocus(false, 0);
    }
    else
    {
        qDebug() << "Autofocus supported";
        emit hasAutoFocus(true, bValue);
    }

    // ********************************************************************************
    //   EXPOSURE
    // ********************************************************************************

    int32_t emin,emax,edefault;
    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_EXPOSURE, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Exposure min: " << emin;
        qDebug() << "Exposure max: " << emax;
        qDebug() << "Exposure default: " << edefault;

        emit hasExposure(true, emin, emax);
        m_hasExposure = true;
    }
    else
    {
        qDebug() << "Failed to get exposure limits";
        emit hasExposure(false, 0, 0);
    }

    int32_t exposure;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, &exposure)==CAPRESULT_OK)
    {
        qDebug() << "Exposure: " << exposure;
        emit setExposure((int)exposure);

    }
    else
    {
        qDebug() << "Failed to get exposure value";
        emit hasExposure(false, 0, 0);
    }

    // ********************************************************************************
    //   WHITE BALANCE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "White balance min: " << emin;
        qDebug() << "White balance max: " << emax;
        qDebug() << "White balance default: " << edefault;

        emit hasWhiteBalance(true, emin, emax);
        m_hasWhiteBalance = true;
    }
    else
    {
        qDebug() << "Failed to get white balance limits";
        emit hasWhiteBalance(false, 0, 0);
    }

    int32_t whitebalance;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &whitebalance)==CAPRESULT_OK)
    {
        qDebug() << "White Balance: " << whitebalance;
        emit setWhiteBalance(whitebalance);
    }
    else
    {
        qDebug() << "Failed to get gain value";
        emit hasWhiteBalance(false, 0, 0);
    }

    // ********************************************************************************
    //   GAIN
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_GAIN, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Gain min: " << emin;
        qDebug() << "Gain max: " << emax;
        qDebug() << "Gain default: " << edefault;

        emit hasGain(true, emin, emax);
        m_hasGain = true;
    }
    else
    {
        qDebug() << "Failed to get gain limits";
        emit hasGain(false, 0, 0);
    }

    int32_t gain;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_GAIN, &gain)==CAPRESULT_OK)
    {
        qDebug() << "Gain: " << gain;
        emit setGain(gain);
    }
    else
    {
        qDebug() << "Failed to get gain value";
        emit hasGain(false, 0, 0);
    }

    // ********************************************************************************
    //   CONTRAST
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_CONTRAST, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Contrast min: " << emin;
        qDebug() << "Contrast max: " << emax;
        qDebug() << "Contrast default: " << edefault;

        emit hasContrast(true, emin, emax);
        m_hasContrast = true;
    }
    else
    {
        emit hasContrast(false, 0, 0);
    }

    int32_t contrast;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_CONTRAST, &contrast)==CAPRESULT_OK)
    {
        qDebug() << "Contrast: " << contrast;
        emit setContrast(contrast);
    }
    else
    {
        qDebug() << "Failed to get contrast value";
        emit hasContrast(false, 0, 0);
    }

    // ********************************************************************************
    //   BRIGHTNESS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_BRIGHTNESS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Brightness min: " << emin;
        qDebug() << "Brightness max: " << emax;
        qDebug() << "Brightness default: " << edefault;
        emit hasBrightness(true, emin, emax);
        m_hasBrightness = true;
    }
    else
    {
        emit hasGain(false, 0, 0);
    }

    int32_t brightness;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_BRIGHTNESS, &brightness)==CAPRESULT_OK)
    {
        qDebug() << "Brightness: " << brightness;
        emit setBrightness(brightness);
    }
    else
    {
        qDebug() << "Failed to get brightness value";
        emit hasBrightness(false, 0, 0);
    }

    // ********************************************************************************
    //   GAMMA
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_GAMMA, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Gamma min: " << emin;
        qDebug() << "Gamma max: " << emax;
        qDebug() << "Gamma default: " << edefault;
        emit hasGamma(true, emin, emax);
        m_hasGamma = true;
    }
    else
    {
        emit hasGamma(false, 0, 0);
    }

    int32_t gamma;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_GAMMA, &gamma)==CAPRESULT_OK)
    {
        qDebug() << "Gamma: " << gamma;
        emit setGamma(gamma);
    }
    else
    {
        qDebug() << "Failed to get gamma value";
        emit hasGamma(false, 0, 0);
    }

    // ********************************************************************************
    //   SATURATION
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_SATURATION, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Saturation min: " << emin;
        qDebug() << "Saturation max: " << emax;
        qDebug() << "Saturation default: " << edefault;
        emit hasSaturation(true, emin, emax);
        m_hasSaturation = true;
    }
    else
    {
        emit hasSaturation(false, 0, 0);
    }

    int32_t saturation;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_SATURATION, &saturation)==CAPRESULT_OK)
    {
        qDebug() << "Saturation: " << saturation;
        emit setSaturation(saturation);
    }
    else
    {
        qDebug() << "Failed to get saturation value";
        emit hasSaturation(false, 0, 0);
    }

    // ********************************************************************************
    //   ZOOM
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_ZOOM, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "zoom min: " << emin;
        qDebug() << "zoom max: " << emax;
        qDebug() << "zoom default: " << edefault;
        emit hasZoom(true, emin, emax);
        m_hasZoom = true;
    }
    else
    {
        emit hasZoom(false, 0, 0);
    }

    int32_t zoom;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_ZOOM, &zoom)==CAPRESULT_OK)
    {
        qDebug() << "zoom: " << zoom;
        emit setZoom(zoom);
    }
    else
    {
        qDebug() << "Failed to get zoom value";
        emit hasZoom(false, 0, 0);
    }

    // ********************************************************************************
    //   FOCUS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_FOCUS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "focus min: " << emin;
        qDebug() << "focus max: " << emax;
        qDebug() << "focus default: " << edefault;
        emit hasFocus(true, emin, emax);
        m_hasFocus = true;
    }
    else
    {
        emit hasFocus(false, 0, 0);
    }

    int32_t focus;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, &focus)==CAPRESULT_OK)
    {
        qDebug() << "focus: " << focus;
        emit setFocus(focus);
    }
    else
    {
        qDebug() << "Failed to get focus value";
        emit hasFocus(false, 0, 0);
    }


    // ********************************************************************************
    //   HUE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_HUE, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "hue min: " << emin;
        qDebug() << "hue max: " << emax;
        qDebug() << "hue default: " << edefault;
        emit hasHue(true, emin, emax);
        m_hasHue = true;
    }
    else
    {
        emit hasHue(false, 0, 0);
    }

    int32_t hue;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_HUE, &hue)==CAPRESULT_OK)
    {
        qDebug() << "hue: " << hue;
        emit setHue(hue);
    }
    else
    {
        qDebug() << "Failed to get hue value";
        emit hasHue(false, 0, 0);
    }

    // ********************************************************************************
    //   SHARPNESS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_SHARPNESS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "sharpness min: " << emin;
        qDebug() << "sharpness max: " << emax;
        qDebug() << "sharpness default: " << edefault;
        emit hasSharpness(true, emin, emax);
        m_hasSharpness = true;
    }
    else
    {
        emit hasSharpness(false, 0, 0);
    }

    int32_t sharpness;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_SHARPNESS, &sharpness)==CAPRESULT_OK)
    {
        qDebug() << "sharpness: " << sharpness;
        emit setSharpness(sharpness);
    }
    else
    {
        qDebug() << "Failed to get sharpness value";
        emit hasSharpness(false, 0, 0);
    }

    // ********************************************************************************
    //   BACKLIGHT COMP
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_BACKLIGHTCOMP, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "backlightcomp min: " << emin;
        qDebug() << "backlightcomp max: " << emax;
        qDebug() << "backlightcomp default: " << edefault;
        emit hasBacklightcomp(true, 0, 0);
        m_hasBacklightcomp = true;
    }
    else
    {
        emit hasBacklightcomp(false, 0, 0);
    }

    int32_t backlight;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_BACKLIGHTCOMP, &backlight)==CAPRESULT_OK)
    {
        qDebug() << "backlight: " << backlight;
        emit setBacklightcomp(backlight);
    }
    else
    {
        qDebug() << "Failed to get backlight value";
        emit hasBacklightcomp(false, 0, 0);
    }

    // ********************************************************************************
    //   COLOR ENABLE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_POWERLINEFREQ, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "power line freq min: " << emin;
        qDebug() << "power line freq max: " << emax;
        qDebug() << "power line freq default: " << edefault;
        emit hasPowerlineFreq(true, emin, emax);
        m_hasPowerlineFreqEnable = true;
    }
    else
    {
        emit hasPowerlineFreq(false, 0, 0);
    }

    int32_t powerline;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_POWERLINEFREQ, &powerline)==CAPRESULT_OK)
    {
        qDebug() << "power line : " << powerline;
        emit setPowerlineFreq(powerline);
    }
    else
    {
        qDebug() << "Failed to get power line frequency value";
        emit hasPowerlineFreq(false, 0, 0);
    }
}

void USBCameras::onExposureSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, value);
}

void USBCameras::onWhiteBalanceSlider(int value)
{
    if (Cap_setProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, value) != CAPRESULT_OK)
    {
        qDebug() << "Setting white balance failed";
    }
}

void USBCameras::onGainSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_GAIN, value);
}

void USBCameras::onContrastSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_CONTRAST, value);
}

void USBCameras::onBrightnessSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_BRIGHTNESS, value);
}

void USBCameras::onGammaSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_GAMMA, value);
}

void USBCameras::onSaturationSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_SATURATION, value);
}

void USBCameras::onFocusSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, value);
}

void USBCameras::onZoomSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_ZOOM, value);
}

void USBCameras::onHueSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_HUE, value);
}

void USBCameras::onBacklightSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_BACKLIGHTCOMP, value);
}

void USBCameras::onSharpnessSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_SHARPNESS, value);
}

void USBCameras::onColorEnableSlider(int value)
{
    Cap_setProperty(m_ctx, m_streamID, CAPPROPID_POWERLINEFREQ, value);
}
