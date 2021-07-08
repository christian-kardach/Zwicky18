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
    }

    // also update the log messages, if there are some
    // updateLogMessages();
}

void USBCameras::changeCamera(int index)
{
    if(index == 0)
    {
        return;
    }
    //QVariant v = ui->cameraChooser->currentData();
    QVariant v = m_cameraList.at(index+1);
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

void USBCameras::onAutoExposure(bool state)
{
    qDebug() << "Auto exposure set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, state ? 1 : 0);
    emit hasAutoExposure((!state), m_hasExposure);
    //ui->exposureSlider->setEnabled((!state) & m_hasExposure);
}

void USBCameras::onAutoWhiteBalance(bool state)
{
    qDebug() << "Auto white balance set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, state ? 1 : 0);
    //ui->whitebalanceSlider->setEnabled((!state) & m_hasWhiteBalance);
    emit hasAutoWhiteBalance(!state, m_hasWhiteBalance);
}

void USBCameras::onAutoGain(bool state)
{
    qDebug() << "Auto gain set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_GAIN, state ? 1 : 0);
    //ui->gainSlider->setEnabled((!state) & m_hasGain);
    emit hasAutoGain(!state, m_hasGain);
}

void USBCameras::onAutoFocus(bool state)
{
    qDebug() << "Auto focus set to " << state;
    Cap_setAutoProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, state ? 1 : 0);
    //ui->gainSlider->setEnabled((!state) & m_hasGain);
    emit hasAutoFocus(!state, m_hasFocus);
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

    //ui->exposureSlider->setEnabled(false);
    //ui->gainSlider->setEnabled(false);
    //ui->whitebalanceSlider->setEnabled(false);
    //ui->focusSlider->setEnabled(false);

    // ********************************************************************************
    //   AUTO EXPOSURE
    // ********************************************************************************

    uint32_t bValue;
    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoExposure(false, 0);
        //ui->autoExposure->setEnabled(false);
        //ui->autoExposure->setCheckState(Qt::Unchecked);
    }
    else
    {
        emit hasAutoExposure(true, bValue);
        //ui->autoExposure->setEnabled(true);
        //ui->autoExposure->setCheckState((bValue==0) ? Qt::Unchecked : Qt::Checked);
    }

    // ********************************************************************************
    //   AUTO GAIN
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_GAIN, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoGain(false, 0);
        //ui->autoGain->setEnabled(false);
        //ui->autoGain->setCheckState(Qt::Unchecked);
    }
    else
    {
        emit hasAutoGain(true, bValue);
        //ui->autoGain->setEnabled(true);
        //ui->autoGain->setCheckState((bValue==0) ? Qt::Unchecked : Qt::Checked);
    }

    // ********************************************************************************
    //   AUTO WHITE BALANCE
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &bValue) != CAPRESULT_OK)
    {
        emit hasAutoWhiteBalance(false, 0);
        //ui->autoWhiteBalance->setEnabled(false);
        //ui->autoWhiteBalance->setCheckState(Qt::Unchecked);
    }
    else
    {
        emit hasAutoWhiteBalance(true, bValue);
        //ui->autoWhiteBalance->setEnabled(true);
        //ui->autoWhiteBalance->setCheckState((bValue==0) ? Qt::Unchecked : Qt::Checked);
    }

    // ********************************************************************************
    //   AUTO FOCUS
    // ********************************************************************************

    if (Cap_getAutoProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, &bValue) != CAPRESULT_OK)
    {
        qDebug() << "Autofocus unsupported";
        emit hasAutoFocus(false, 0);
        //ui->autoFocus->setEnabled(false);
        //ui->autoFocus->setCheckState(Qt::Unchecked);
    }
    else
    {
        qDebug() << "Autofocus supported";
        emit hasAutoFocus(true, bValue);
        //ui->autoFocus->setEnabled(true);
        //ui->autoFocus->setCheckState((bValue==0) ? Qt::Unchecked : Qt::Checked);
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
        /*
        ui->exposureSlider->setRange(emin, emax);
        ui->exposureSpinBox->setRange(emin, emax);
        m_hasExposure = true;
        if (!ui->autoExposure->isChecked())
        {
            ui->exposureSlider->setEnabled(true);
            ui->exposureSpinBox->setEnabled(true);
        }
        */
    }
    else
    {
        qDebug() << "Failed to get exposure limits";
        emit hasExposure(false, 0, 0);
        //ui->exposureSlider->setEnabled(false);
        //ui->exposureSpinBox->setEnabled(false);
        //ui->exposureSlider->setRange(0, 0);
    }

    int32_t exposure;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_EXPOSURE, &exposure)==CAPRESULT_OK)
    {
        qDebug() << "Exposure: " << exposure;
        //ui->exposureSlider->setValue(exposure);
        emit setExposure(exposure);

    }
    else
    {
        qDebug() << "Failed to get exposure value";
        emit hasExposure(false, 0, 0);
        //ui->exposureSlider->setEnabled(false);
        //ui->exposureSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   WHITE BALANCE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "White balance min: " << emin;
        qDebug() << "White balance max: " << emax;
        qDebug() << "White balance default: " << edefault;

        emit hasExposure(true, emin, emax);
        m_hasWhiteBalance = true;
        /*
        ui->whitebalanceSlider->setRange(emin, emax);
        ui->whitebalanceSpinBox->setRange(emin, emax);
        m_hasWhiteBalance = true;
        if (!ui->autoWhiteBalance->isChecked())
        {
            ui->whitebalanceSlider->setEnabled(true);
            ui->whitebalanceSpinBox->setEnabled(true);
        }
        */
    }
    else
    {
        qDebug() << "Failed to get white balance limits";
        emit hasWhiteBalance(false, 0, 0);
        //ui->whitebalanceSlider->setRange(0, 0);
        //ui->whitebalanceSlider->setEnabled(false);
        //ui->whitebalanceSpinBox->setEnabled(false);
    }

    int32_t whitebalance;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_WHITEBALANCE, &whitebalance)==CAPRESULT_OK)
    {
        qDebug() << "White Balance: " << whitebalance;
        emit setWhiteBalance(whitebalance);
        //ui->whitebalanceSlider->setValue(whitebalance);
    }
    else
    {
        qDebug() << "Failed to get gain value";
        emit hasWhiteBalance(false, 0, 0);
        //ui->whitebalanceSlider->setEnabled(false);
        //ui->whitebalanceSpinBox->setEnabled(false);
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
        /*
        ui->gainSlider->setRange(emin, emax);
        ui->gainSpinBox->setRange(emin, emax);
        m_hasGain = true;
        if (!ui->autoGain->isChecked())
        {
            ui->gainSlider->setEnabled(true);
            ui->gainSpinBox->setEnabled(true);
        }
        */
    }
    else
    {
        qDebug() << "Failed to get gain limits";
        emit hasGain(false, 0, 0);
        //ui->gainSlider->setRange(0, 0);
        //ui->gainSlider->setEnabled(false);
    }

    int32_t gain;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_GAIN, &gain)==CAPRESULT_OK)
    {
        qDebug() << "Gain: " << gain;
        //ui->gainSlider->setValue(gain);
        emit setGain(gain);
    }
    else
    {
        qDebug() << "Failed to get gain value";
        emit hasGain(false, 0, 0);
        //ui->gainSlider->setEnabled(false);
        //ui->gainSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   CONTRAST
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_CONTRAST, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Contrast min: " << emin;
        qDebug() << "Contrast max: " << emax;
        qDebug() << "Contrast default: " << edefault;
        //ui->contrastSlider->setEnabled(true);
        //ui->contrastSlider->setRange(emin, emax);
        m_hasContrast = true;
    }
    else
    {
        //ui->contrastSlider->setRange(0, 0);
        //ui->contrastSlider->setEnabled(false);
    }

    int32_t contrast;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_CONTRAST, &contrast)==CAPRESULT_OK)
    {
        qDebug() << "Contrast: " << contrast;
        //ui->contrastSlider->setValue(contrast);
    }
    else
    {
        qDebug() << "Failed to get contrast value";
        //ui->contrastSlider->setEnabled(false);
    }

    // ********************************************************************************
    //   BRIGHTNESS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_BRIGHTNESS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Brightness min: " << emin;
        qDebug() << "Brightness max: " << emax;
        qDebug() << "Brightness default: " << edefault;
        //ui->brightnessSlider->setEnabled(true);
        //ui->brightnessSlider->setRange(emin, emax);
        //ui->brightnessSpinBox->setRange(emin, emax);
        //ui->brightnessSpinBox->setEnabled(true);
        m_hasBrightness = true;
    }
    else
    {
        //ui->brightnessSlider->setRange(0, 0);
        //ui->brightnessSlider->setEnabled(false);
        //ui->brightnessSpinBox->setRange(0, 0);
        //ui->brightnessSpinBox->setEnabled(false);
    }

    int32_t brightness;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_BRIGHTNESS, &brightness)==CAPRESULT_OK)
    {
        qDebug() << "Brightness: " << brightness;
        //ui->brightnessSlider->setEnabled(true);
        //ui->brightnessSlider->setValue(brightness);
        //ui->brightnessSpinBox->setEnabled(true);
    }
    else
    {
        qDebug() << "Failed to get brightness value";
        //ui->brightnessSlider->setEnabled(false);
        //ui->brightnessSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   GAMMA
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_GAMMA, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Gamma min: " << emin;
        qDebug() << "Gamma max: " << emax;
        qDebug() << "Gamma default: " << edefault;
        //ui->gammaSlider->setEnabled(true);
        //ui->gammaSlider->setRange(emin, emax);
        //ui->gammaSpinBox->setEnabled(true);
        //ui->gammaSpinBox->setRange(emin, emax);
        m_hasGamma = true;
    }
    else
    {
        //ui->gammaSlider->setRange(0, 0);
        //ui->gammaSlider->setEnabled(false);
        //ui->gammaSpinBox->setRange(0, 0);
        //ui->gammaSpinBox->setEnabled(false);
    }

    int32_t gamma;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_GAMMA, &gamma)==CAPRESULT_OK)
    {
        qDebug() << "Gamma: " << gamma;
        //ui->gammaSlider->setEnabled(true);
        //ui->gammaSpinBox->setEnabled(true);

        //ui->gammaSlider->setValue(gamma);
    }
    else
    {
        qDebug() << "Failed to get gamma value";
        //ui->gammaSlider->setEnabled(false);
        //ui->gammaSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   SATURATION
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_SATURATION, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "Saturation min: " << emin;
        qDebug() << "Saturation max: " << emax;
        qDebug() << "Saturation default: " << edefault;
        //ui->saturationSlider->setEnabled(true);
        //ui->saturationSlider->setRange(emin, emax);
        //ui->saturationSpinBox->setEnabled(true);
        //ui->saturationSpinBox->setRange(emin, emax);
        m_hasSaturation = true;
    }
    else
    {
        //ui->saturationSlider->setRange(0, 0);
        //ui->saturationSlider->setEnabled(false);
        //ui->saturationSpinBox->setRange(0, 0);
        //ui->saturationSpinBox->setEnabled(false);
    }

    int32_t saturation;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_SATURATION, &saturation)==CAPRESULT_OK)
    {
        qDebug() << "Saturation: " << saturation;
        //ui->saturationSlider->setEnabled(true);
        //ui->saturationSpinBox->setEnabled(true);
        //ui->saturationSlider->setValue(saturation);
    }
    else
    {
        qDebug() << "Failed to get saturation value";
        //ui->saturationSlider->setEnabled(false);
        //ui->saturationSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   ZOOM
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_ZOOM, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "zoom min: " << emin;
        qDebug() << "zoom max: " << emax;
        qDebug() << "zoom default: " << edefault;
        //ui->zoomSlider->setEnabled(true);
        //ui->zoomSlider->setRange(emin, emax);
        //ui->zoomSpinBox->setEnabled(true);
        //ui->zoomSpinBox->setRange(emin, emax);
        m_hasZoom = true;
    }
    else
    {
        //ui->zoomSlider->setRange(0, 0);
        //ui->zoomSlider->setEnabled(false);
        //ui->zoomSpinBox->setRange(0, 0);
        //ui->zoomSpinBox->setEnabled(false);
    }

    int32_t zoom;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_ZOOM, &zoom)==CAPRESULT_OK)
    {
        qDebug() << "zoom: " << zoom;
        //ui->zoomSlider->setEnabled(true);
        //ui->zoomSpinBox->setEnabled(true);
        //ui->zoomSlider->setValue(zoom);
    }
    else
    {
        qDebug() << "Failed to get zoom value";
        //ui->zoomSlider->setEnabled(false);
        //ui->zoomSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   FOCUS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_FOCUS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "focus min: " << emin;
        qDebug() << "focus max: " << emax;
        qDebug() << "focus default: " << edefault;
        //ui->focusSlider->setEnabled(true);
        //ui->focusSlider->setRange(emin, emax);
        //ui->focusSpinBox->setEnabled(true);
        //ui->focusSpinBox->setRange(emin, emax);
        m_hasFocus = true;
    }
    else
    {
        //ui->focusSlider->setRange(0, 0);
        //ui->focusSlider->setEnabled(false);
        //ui->focusSpinBox->setRange(0, 0);
        //ui->focusSpinBox->setEnabled(false);
    }

    int32_t focus;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_FOCUS, &focus)==CAPRESULT_OK)
    {
        qDebug() << "focus: " << focus;
        //ui->focusSlider->setEnabled(true);
        //ui->focusSpinBox->setEnabled(true);
        //ui->focusSlider->setValue(focus);
    }
    else
    {
        qDebug() << "Failed to get focus value";
        //ui->focusSlider->setEnabled(false);
        //ui->focusSpinBox->setEnabled(false);
    }


    // ********************************************************************************
    //   HUE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_HUE, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "hue min: " << emin;
        qDebug() << "hue max: " << emax;
        qDebug() << "hue default: " << edefault;
        //ui->hueSlider->setEnabled(true);
        //ui->hueSlider->setRange(emin, emax);
        //ui->hueSpinBox->setEnabled(true);
        //ui->hueSpinBox->setRange(emin, emax);
        m_hasHue = true;
    }
    else
    {
        //ui->hueSlider->setRange(0, 0);
        //ui->hueSlider->setEnabled(false);
        //ui->hueSpinBox->setRange(0, 0);
        //ui->hueSpinBox->setEnabled(false);
    }

    int32_t hue;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_HUE, &hue)==CAPRESULT_OK)
    {
        qDebug() << "hue: " << hue;
        //ui->hueSlider->setEnabled(true);
        //ui->hueSpinBox->setEnabled(true);
        //ui->hueSlider->setValue(hue);
    }
    else
    {
        qDebug() << "Failed to get hue value";
        //ui->hueSlider->setEnabled(false);
        //ui->hueSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   SHARPNESS
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_SHARPNESS, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "sharpness min: " << emin;
        qDebug() << "sharpness max: " << emax;
        qDebug() << "sharpness default: " << edefault;
        //ui->sharpnessSlider->setEnabled(true);
        //ui->sharpnessSlider->setRange(emin, emax);
        //ui->sharpnessSpinBox->setEnabled(true);
        //ui->sharpnessSpinBox->setRange(emin, emax);
        m_hasSharpness = true;
    }
    else
    {
        //ui->sharpnessSlider->setRange(0, 0);
        //ui->sharpnessSlider->setEnabled(false);
        //ui->sharpnessSpinBox->setRange(0, 0);
        //ui->sharpnessSpinBox->setEnabled(false);
    }

    int32_t sharpness;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_SHARPNESS, &sharpness)==CAPRESULT_OK)
    {
        qDebug() << "sharpness: " << sharpness;
        //ui->sharpnessSlider->setEnabled(true);
        //ui->sharpnessSpinBox->setEnabled(true);
        //ui->sharpnessSlider->setValue(sharpness);
    }
    else
    {
        qDebug() << "Failed to get sharpness value";
        //ui->sharpnessSlider->setEnabled(false);
        //ui->sharpnessSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   BACKLIGHT COMP
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_BACKLIGHTCOMP, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "backlightcomp min: " << emin;
        qDebug() << "backlightcomp max: " << emax;
        qDebug() << "backlightcomp default: " << edefault;
        //ui->backlightSlider->setEnabled(true);
        //ui->backlightSlider->setRange(emin, emax);
        //ui->backlightSpinBox->setEnabled(true);
        //ui->backlightSpinBox->setRange(emin, emax);
        m_hasBacklightcomp = true;
    }
    else
    {
        //ui->backlightSlider->setRange(0, 0);
        //ui->backlightSlider->setEnabled(false);
        //ui->backlightSpinBox->setRange(0, 0);
        //ui->backlightSpinBox->setEnabled(false);
    }

    int32_t backlight;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_BACKLIGHTCOMP, &backlight)==CAPRESULT_OK)
    {
        qDebug() << "backlight: " << backlight;
        //ui->backlightSlider->setEnabled(true);
        //ui->backlightSpinBox->setEnabled(true);
        //ui->backlightSlider->setValue(backlight);
    }
    else
    {
        qDebug() << "Failed to get backlight value";
        //ui->backlightSlider->setEnabled(false);
        //ui->backlightSpinBox->setEnabled(false);
    }

    // ********************************************************************************
    //   COLOR ENABLE
    // ********************************************************************************

    if (Cap_getPropertyLimits(m_ctx, m_streamID, CAPPROPID_POWERLINEFREQ, &emin, &emax, &edefault)==CAPRESULT_OK)
    {
        qDebug() << "power line freq min: " << emin;
        qDebug() << "power line freq max: " << emax;
        qDebug() << "power line freq default: " << edefault;
        //ui->powerlineFreqSlider->setEnabled(true);
        //ui->powerlineFreqSlider->setRange(emin, emax);
        //ui->powerlineFreqSpinBox->setEnabled(true);
        //ui->powerlineFreqSpinBox->setRange(emin, emax);
        m_hasBacklightcomp = true;
    }
    else
    {
        //ui->powerlineFreqSlider->setRange(0, 0);
        //ui->powerlineFreqSlider->setEnabled(false);
        //ui->powerlineFreqSpinBox->setRange(0, 0);
        //ui->powerlineFreqSpinBox->setEnabled(false);
    }

    int32_t powerline;
    if (Cap_getProperty(m_ctx, m_streamID, CAPPROPID_POWERLINEFREQ, &powerline)==CAPRESULT_OK)
    {
        qDebug() << "power line : " << powerline;
        //ui->powerlineFreqSlider->setEnabled(true);
        //ui->powerlineFreqSpinBox->setEnabled(true);
        //ui->powerlineFreqSlider->setValue(powerline);
    }
    else
    {
        qDebug() << "Failed to get power line frequency value";
        //ui->powerlineFreqSlider->setEnabled(false);
        //ui->powerlineFreqSpinBox->setEnabled(false);
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
