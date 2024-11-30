#ifdef USE_WIFI_WEB

#include "web-images.h"

////////////////////////////////
// List of available sequences by name and matching id
enum
{
    kMAX_FADE = 15,
    kMAX_DELAY = 500,
    kMIN_DELAY = 10,
    kMIN_BRI = 10,

    kMAX_ADJLOOP = 90000,
    kMIN_ADJLOOP = 500,
};

WMenuData mainMenu[] = {
    { "Logics", "/logics" },
    { "Setup", "/setup" }
};

WMenuData setupMenu[] = {
    { "Home", "/" },
    { "Logics", "/setuplogics" },
    { "Marcduino", "/marcduino" },
    { "WiFi", "/wifi" },
    { "Remote", "/remote" },
    { "Firmware", "/firmware" },
    { "Back", "/" }
};

WElement mainContents[] = {
    WVerticalMenu("menu", mainMenu, SizeOfArray(mainMenu)),
    rseriesSVG
};

WElement setupContents[] = {
    WVerticalMenu("setup", setupMenu, SizeOfArray(setupMenu)),
    rseriesSVG
};

String fldBoards[] = {
    "None",
    "RSeries 40 LED",
    "RSeries 40 LED Inverted",
    "C3PO 40 LED",
    "Naboo 48 LED",
    "RGBW 40 LED"
};

String rldBoards[] = {
    "None",
    "Imahara 112 LED",
    "Imahara 112 LED Inverted",
    "DeathStar 96 LED",
    "DeathStar 96 LED StaggerOdd",
    "DeathStar 96 LED Inverted",
    "DeathStar 96 LED Inverted StaggerOdd",
    "Kenny 96 LED",
    "Naboo 96 LED",
    "Super 256 LED",
};

String logicsSeq[] = {
#define LOGICENGINE_SEQ(nam, val) \
    BUILTIN_SEQ(nam, LogicEngineDefaults::val)
#define BUILTIN_SEQ(nam, val) \
    nam,

#include "logic-sequences.h"

#undef BUILTIN_SEQ
#undef LOGICENGINE_SEQ
};

unsigned logicsSeqNumber[] = {
#define LOGICENGINE_SEQ(nam, val) \
    BUILTIN_SEQ(nam, LogicEngineDefaults::val)
#define BUILTIN_SEQ(nam, val) \
    val,

#include "logic-sequences.h"

#undef BUILTIN_SEQ
#undef LOGICENGINE_SEQ
};

String logicsColors[] = {
    "Default",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Purple",
    "Magenta",
    "Pink"
};

bool sFLDChanged = true;
bool sRLDChanged = true;

int sFLDSequence;
int sRLDSequence;

String sFLDText = "";
String sRLDText = "";
String sFLDDisplayText;
String sRLDDisplayText;

int sFLDColor = LogicEngineRenderer::kDefault;
int sRLDColor = LogicEngineRenderer::kDefault;

int sFLDSpeedScale;
int sRLDSpeedScale;

int sFLDNumSeconds;
int sRLDNumSeconds;

/////////////////////////////////////////////////////////////////////////
// Web Interface for logic engine animation sequences
WElement logicsContents[] = {
    WSelect("Front Logic Sequence", "frontseq",
        logicsSeq, SizeOfArray(logicsSeq),
        []() { return sFLDSequence; },
        [](int val) { sFLDSequence = val; sFLDChanged = true; } ),
    WSelect("Front Color", "frontcolor",
        logicsColors, SizeOfArray(logicsColors),
        []() { return sFLDColor; },
        [](int val) { sFLDColor = val; sFLDChanged = true; } ),
    WSlider("Animation Speed", "fldspeed", 0, 9,
        []()->int { return sFLDSpeedScale; },
        [](int val) { sFLDSpeedScale = val; sFLDChanged = true; } ),
    WSlider("Number of seconds", "fldseconds", 0, 99,
        []()->int { return sFLDNumSeconds; },
        [](int val) { sFLDNumSeconds = val; sFLDChanged = true; } ),
    WTextField("Front Text:", "fronttext",
        []()->String { return sFLDText; },
        [](String val) { sFLDText = val; sFLDChanged = true; } ),
    WSelect("Rear Logic Sequence", "rearseq",
        logicsSeq, SizeOfArray(logicsSeq),
        []() { return sRLDSequence; },
        [](int val) { sRLDSequence = val; sRLDChanged = true; } ),
    WSelect("Rear Color", "rearcolor",
        logicsColors, SizeOfArray(logicsColors),
        []() { return sRLDColor; },
        [](int val) { sRLDColor = val; sRLDChanged = true; } ),
    WSlider("Animation Speed", "rldspeed", 0, 9,
        []()->int { return sRLDSpeedScale; },
        [](int val) { sRLDSpeedScale = val; sRLDChanged = true; } ),
    WSlider("Number of seconds", "rldseconds", 0, 99,
        []()->int { return sRLDNumSeconds; },
        [](int val) { sRLDNumSeconds = val; sRLDChanged = true; } ),
    WTextField("Rear Text:", "reartext",
        []()->String { return sRLDText; },
        [](String val) { sRLDText = val; sRLDChanged = true; } ),
    WButton("Run", "run", []() {
        if (sFLDChanged)
        {
            sFLDDisplayText = sFLDText;
            sFLDDisplayText.replace("\\n", "\n");
            if (FLD != nullptr)
                FLD->resetEffect();
            FLD_selectSequence(logicsSeqNumber[sFLDSequence], (LogicEngineRenderer::ColorVal)sFLDColor, sFLDSpeedScale, sFLDNumSeconds);
            FLD_setTextMessage(sFLDDisplayText.c_str());
            sFLDChanged = false;
        }
        if (sRLDChanged)
        {
            sRLDDisplayText = sRLDText;
            sRLDDisplayText.replace("\\n", "\n");
            if (RLD != nullptr)
                RLD->resetEffect();
            RLD_selectSequence(logicsSeqNumber[sRLDSequence], (LogicEngineRenderer::ColorVal)sRLDColor, sRLDSpeedScale, sRLDNumSeconds);
            RLD_setTextMessage(sRLDDisplayText.c_str());
            sRLDChanged = false;
        }
    }),
    WHorizontalAlign(),
    WButton("Back", "back", "/"),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    rseriesSVG
};

////////////////////////////////

String swBaudRates[] = {
    "2400",
    "9600",
    "57600",
    "115200",
};

int swBaudRatesInt[] = {
    2400,
    9600,
    57600,
    115200,
};

bool isValidBaudRate(int baudRate) {
    for (int i = 0; i < SizeOfArray(swBaudRatesInt); i++)
    {
        if (baudRate == swBaudRatesInt[i]) {
            return true;
        }
    }
    return false;
}

int swBaudRateToIndex(int baudRate)
{
    for (int i = 0; i < SizeOfArray(swBaudRatesInt); i++)
    {
        if (baudRate == swBaudRatesInt[i])
            return i;
    }
    return 0;
}

int marcSerial1Baud;
int marcSerial2Baud;
bool marcSerialPass;
bool marcSerialEnabled;
bool marcWifiEnabled;
bool marcWifiSerialPass;

WElement marcduinoContents[] = {
    WSelect("Serial1 Baud Rate", "serial1baud",
        swBaudRates, SizeOfArray(swBaudRates),
        []() { return (marcSerial1Baud = swBaudRateToIndex(preferences.getInt(PREFERENCE_MARCSERIAL1, MARC_SERIAL1_BAUD_RATE))); },
        [](int val) { marcSerial1Baud = swBaudRatesInt[val]; }),
    WVerticalAlign(),
    WSelect("Serial2 Baud Rate", "serial2baud",
        swBaudRates, SizeOfArray(swBaudRates),
        []() { return (marcSerial2Baud = swBaudRateToIndex(preferences.getInt(PREFERENCE_MARCSERIAL2, MARC_SERIAL2_BAUD_RATE))); },
        [](int val) { marcSerial2Baud = swBaudRatesInt[val]; }),
    WVerticalAlign(),
    WCheckbox("Serial1 pass-through to Serial2", "serialpass",
        []() { return (marcSerialPass = (preferences.getBool(PREFERENCE_MARCSERIAL_PASS, MARC_SERIAL_PASS))); },
        [](bool val) { marcSerialPass = val; } ),
    WVerticalAlign(),
    WCheckbox("Marcduino on Serial1", "enabled",
        []() { return (marcSerialEnabled = (preferences.getBool(PREFERENCE_MARCSERIAL_ENABLED, MARC_SERIAL_ENABLED))); },
        [](bool val) { marcSerialEnabled = val; } ),
    WVerticalAlign(),
    WCheckbox("Marcduino on Wifi (port 2000)", "wifienabled",
        []() { return (marcWifiEnabled = (preferences.getBool(PREFERENCE_MARCWIFI_ENABLED, MARC_WIFI_ENABLED))); },
        [](bool val) { marcWifiEnabled = val; } ),
    WVerticalAlign(),
    WCheckbox("Marcduino Wifi pass-through to Serial2", "wifipass",
        []() { return (marcWifiSerialPass = (preferences.getBool(PREFERENCE_MARCWIFI_SERIAL_PASS, MARC_WIFI_SERIAL_PASS))); },
        [](bool val) { marcWifiSerialPass = val; } ),
    WVerticalAlign(),
    WButton("Save", "save", []() {
        preferences.putInt(PREFERENCE_MARCSERIAL1, marcSerial1Baud);
        preferences.putInt(PREFERENCE_MARCSERIAL2, marcSerial2Baud);
        preferences.putBool(PREFERENCE_MARCSERIAL_PASS, marcSerialPass);
        preferences.putBool(PREFERENCE_MARCSERIAL_ENABLED, marcSerialEnabled);
        preferences.putBool(PREFERENCE_MARCWIFI_ENABLED, marcWifiEnabled);
        preferences.putBool(PREFERENCE_MARCWIFI_SERIAL_PASS, marcWifiSerialPass);
    }),
    WHorizontalAlign(),
    WButton("Back", "back", "/setup"),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    WVerticalAlign(),
    rseriesSVG
};

////////////////////////////////

WElement setupLogicsContents[] = {
    WSelect("Front Logic Engine Type", "fld",
        fldBoards, SizeOfArray(fldBoards),
        []() { return (int)sFLDType; },
        [](int val) { FLD_selectType((LogicEngineFLDType)val); } ),
    WSelect("Rear Logic Engine Type", "rld",
        rldBoards, SizeOfArray(rldBoards),
        []() { return (int)sRLDType; },
        [](int val) { RLD_selectType((LogicEngineRLDType)val); } ),
#ifdef USE_CONTROLLER
    WSlider("Front Brightness", "fldbri", 0, 255,
        []()->int { return sController.getSettings(0).fBri; },
        [](int val) { sController.changeSetting(0, 1, val); } ),
    WSlider("Front Hue", "fldhue", 0, 255,
        []()->int { return sController.getSettings(0).fHue; },
        [](int val) { sController.changeSetting(0, 2, val); } ),
    WSlider("Front Fade", "fldfad", 0, 255,
        []()->int { return sController.getSettings(0).fFade; },
        [](int val) { sController.changeSetting(0, 3, val); } ),
    WSlider("Front Pause", "fldpau", 0, 255,
        []()->int { return sController.getSettings(0).fDelay; },
        [](int val) { sController.changeSetting(0, 4, val); } ),
    WSlider("Front Palette", "fldpal", 0, LogicEngineDefaults::PAL_COUNT,
        []()->int { return sController.getSettings(0).fPalNum; },
        [](int val) { sController.changeSetting(0, 5, val); } ),
    WSlider("Rear Brightness", "rearbri", 0, 255,
        []()->int { return sController.getSettings(1).fBri; },
        [](int val) { sController.changeSetting(1, 1, val); } ),
    WSlider("Rear Hue", "rearhue", 0, 255,
        []()->int { return sController.getSettings(1).fHue; },
        [](int val) { sController.changeSetting(1, 2, val); } ),
    WSlider("Rear Fade", "rearfae", 0, 255,
        []()->int { return sController.getSettings(1).fFade; },
        [](int val) { sController.changeSetting(1, 3, val); } ),
    WSlider("Rear Pause", "rearpau", 0, 255,
        []()->int { return sController.getSettings(1).fDelay; },
        [](int val) { sController.changeSetting(1, 4, val); } ),
    WSlider("Rear Palette", "rearpal", 0, LogicEngineDefaults::PAL_COUNT,
        []()->int { return sController.getSettings(1).fPalNum; },
        [](int val) { sController.changeSetting(1, 5, val); } ),
#endif
    WButton("Save", "save", []() {
        preferences.putUChar(PREFERENCE_FLD, sFLDType);
        preferences.putUChar(PREFERENCE_RLD, sRLDType);
    #ifdef USE_CONTROLLER
        sController.commit();
    #endif
    }),
    WHorizontalAlign(),
    WButtonReload("Defaults", "default", []() {
    #ifdef USE_CONTROLLER
        sController.restoreFactoryDefaults(0);
        sController.restoreFactoryDefaults(1);
    #endif
    }),
    WHorizontalAlign(),
    WButton("Back", "back", "/setup"),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    WVerticalAlign(),
    rseriesSVG
};

////////////////////////////////

String wifiSSID;
String wifiPass;
bool wifiAP;

WElement wifiContents[] = {
    W1("WiFi Setup"),
    WCheckbox("WiFi Enabled", "enabled",
        []() { return wifiEnabled; },
        [](bool val) { wifiEnabled = val; } ),
    WCheckbox("Access Point", "apmode",
        []() { return (wifiAP = preferences.getBool(PREFERENCE_WIFI_AP, WIFI_ACCESS_POINT)); },
        [](bool val) { wifiAP = val; } ),
    WTextField("WiFi:", "wifi",
        []()->String { return (wifiSSID = preferences.getString(PREFERENCE_WIFI_SSID, WIFI_AP_NAME)); },
        [](String val) { wifiSSID = val; } ),
    WPassword("Password:", "password",
        []()->String { return (wifiPass = preferences.getString(PREFERENCE_WIFI_PASS, WIFI_AP_PASSPHRASE)); },
        [](String val) { wifiPass = val; } ),
    WButton("Save", "save", []() {
        DEBUG_PRINTLN("WiFi Changed");
        preferences.putBool(PREFERENCE_WIFI_ENABLED, wifiEnabled);
        preferences.putBool(PREFERENCE_WIFI_AP, wifiAP);
        preferences.putString(PREFERENCE_WIFI_SSID, wifiSSID);
        preferences.putString(PREFERENCE_WIFI_PASS, wifiPass);
        reboot();
    }),
    WHorizontalAlign(),
    WButton("Back", "back", "/setup"),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    WVerticalAlign(),
    rseriesSVG
};

////////////////////////////////

String remoteHostName;
String remoteSecret;

WElement remoteContents[] = {
    W1("Droid Remote Setup"),
    WCheckbox("Droid Remote Enabled", "remoteenabled",
        []() { return remoteEnabled; },
        [](bool val) { remoteEnabled = val; if (remoteEnabled) wifiEnabled = false; } ),
    WLabel("Droid Remote Disables WiFi", "label1"),
    WHR(),
    WTextField("Device Name:", "hostname",
        []()->String { return (remoteHostName = preferences.getString(PREFERENCE_REMOTE_HOSTNAME, SMQ_HOSTNAME)); },
        [](String val) { remoteHostName = val; } ),
    WPassword("Secret:", "secret",
        []()->String { return (remoteSecret = preferences.getString(PREFERENCE_REMOTE_SECRET, SMQ_SECRET)); },
        [](String val) { remoteSecret = val; } ),
    WButton("Save", "save", []() {
        DEBUG_PRINTLN("Remote Changed");
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, remoteEnabled);
        preferences.putBool(PREFERENCE_WIFI_ENABLED, wifiEnabled);
        preferences.putString(PREFERENCE_REMOTE_HOSTNAME, remoteHostName);
        preferences.putString(PREFERENCE_REMOTE_SECRET, remoteSecret);
        reboot();
    }),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    WVerticalAlign(),
    rseriesSVG
};

////////////////////////////////

WElement firmwareContents[] = {
    W1("Firmware Setup"),
    WFirmwareFile("Firmware:", "firmware"),
    WFirmwareUpload("Reflash", "firmware"),
    WLabel("Current Firmware Build Date:", "label"),
    WLabel(__DATE__, "date"),
#ifdef BUILD_VERSION
    WHRef(BUILD_VERSION, "Sources"),
#endif
    WButton("Clear Prefs", "clear", []() {
        DEBUG_PRINTLN("Clear all preference settings");
        preferences.clear();
    }),
    WHorizontalAlign(),
    WButton("Reboot", "reboot", []() {
        reboot();
    }),
    WHorizontalAlign(),
    WButton("Back", "back", "/setup"),
    WHorizontalAlign(),
    WButton("Home", "home", "/"),
    WVerticalAlign(),
    rseriesSVG
};

//////////////////////////////////////////////////////////////////

WPage pages[] = {
    WPage("/", mainContents, SizeOfArray(mainContents)),
      WPage("/logics", logicsContents, SizeOfArray(logicsContents)),
    WPage("/setup", setupContents, SizeOfArray(setupContents)),
      WPage("/setuplogics", setupLogicsContents, SizeOfArray(setupLogicsContents)),
      WPage("/marcduino", marcduinoContents, SizeOfArray(marcduinoContents)),
      WPage("/wifi", wifiContents, SizeOfArray(wifiContents)),
      WPage("/remote", remoteContents, SizeOfArray(remoteContents)),
      WPage("/firmware", firmwareContents, SizeOfArray(firmwareContents)),
        WUpload("/upload/firmware",
            [](Client& client)
            {
                if (Update.hasError())
                    client.println("HTTP/1.0 200 FAIL");
                else
                    client.println("HTTP/1.0 200 OK");
                client.println("Content-type:text/html");
                client.println("Vary: Accept-Encoding");
                client.println();
                client.println();
                client.stop();
                if (!Update.hasError())
                {
                    reboot();
                }
            #ifdef USE_LOGICS
                FLD_selectSequence(LogicEngineDefaults::FAILURE);
                FLD_setTextMessage("Flash Fail");
                FLD_selectSequence(LogicEngineDefaults::TEXTSCROLLLEFT, LogicEngineRenderer::kRed, 1, 0);
                FLD_setEffectWidthRange(1.0);
                FLD_setEffectWidthRange(1.0);
            #endif
                otaInProgress = false;
            },
            [](WUploader& upload)
            {
                if (upload.status == UPLOAD_FILE_START)
                {
                    otaInProgress = true;
                    unmountFileSystems();
                #ifdef USE_LOGICS
                    FLD_selectSequence(LogicEngineDefaults::NORMAL);
                    RLD_selectSequence(LogicEngineDefaults::NORMAL);
                    FLD_setEffectWidthRange(0);
                    RLD_setEffectWidthRange(0);
                #endif
                    Serial.printf("Update: %s\n", upload.filename.c_str());
                    if (!Update.begin(upload.fileSize))
                    {
                        //start with max available size
                        Update.printError(Serial);
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE)
                {
                    float range = (float)upload.receivedSize / (float)upload.fileSize;
                    DEBUG_PRINTLN("Received: "+String(range*100)+"%");
                   /* flashing firmware to ESP*/
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                    {
                        Update.printError(Serial);
                    }
                #ifdef USE_LOGICS
                    FLD_setEffectWidthRange(range);
                    RLD_setEffectWidthRange(range);
                #endif
                }
                else if (upload.status == UPLOAD_FILE_END)
                {
                    DEBUG_PRINTLN("GAME OVER");
                    if (Update.end(true))
                    {
                        //true to set the size to the current progress
                        Serial.printf("Update Success: %u\nRebooting...\n", upload.receivedSize);
                    }
                    else
                    {
                        Update.printError(Serial);
                    }
                }
            })
};

WifiWebServer<10,SizeOfArray(pages)> webServer(pages, wifiAccess);
#endif
