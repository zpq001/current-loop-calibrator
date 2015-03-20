
#include <QSignalMapper.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "textdisplay.h"
#include "keydriver.h"
#include "gui_wrapper.h"
#include "buttons.h"
#include "tdd_verification.h"

// Internal button codes
enum buttons {
    APP_KEY_OK = 1,
    APP_KEY_ESC,
    APP_KEY_CONST,
    APP_KEY_ALTERN,
    APP_KEY_NUM0,   APP_KEY_NUM1,       APP_KEY_NUM2,
    APP_KEY_NUM3,   APP_KEY_NUM4,       APP_KEY_NUM5,
    APP_KEY_NUM6,   APP_KEY_NUM7,       APP_KEY_NUM8,
    APP_KEY_NUM9,   APP_KEY_BKSPACE,    APP_KEY_DOT
};


typedef struct {
    int app_key;
    uint32_t btn_code;
} keyCodeRecord_t;


keyCodeRecord_t keyCodeTable[] = {
    { APP_KEY_OK,       KEY_OK},
    { APP_KEY_ESC,      KEY_ESC},
    { APP_KEY_CONST,    KEY_OUTPUT_CONST},
    { APP_KEY_ALTERN,   KEY_OUTPUT_ALTERN},
    { APP_KEY_NUM0,     KEY_NUM0},
    { APP_KEY_NUM1,     KEY_NUM1},
    { APP_KEY_NUM2,     KEY_NUM2},
    { APP_KEY_NUM3,     KEY_NUM3},
    { APP_KEY_NUM4,     KEY_NUM4},
    { APP_KEY_NUM5,     KEY_NUM5},
    { APP_KEY_NUM6,     KEY_NUM6},
    { APP_KEY_NUM7,     KEY_NUM7},
    { APP_KEY_NUM8,     KEY_NUM8},
    { APP_KEY_NUM9,     KEY_NUM9},
    { APP_KEY_NUM0,     KEY_NUM0},
    { APP_KEY_BKSPACE,  KEY_BACKSPACE},
    { APP_KEY_DOT,      KEY_DOT}
};



MainWindow* pt2Myself;        // Global variable which points to this.
                              // Used for C callbacks.
QSignalMapper *btnPressSignalMapper;
QSignalMapper *btnReleaseSignalMapper;
keyDriver *keyDriver1;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    keyDriver1 = new keyDriver(this, 20, 300, 50);       // size of enum buttons ?

    // Control buttons press
    btnPressSignalMapper = new QSignalMapper(this);
    btnPressSignalMapper->setMapping(ui->pb_num0    , APP_KEY_NUM0);
    btnPressSignalMapper->setMapping(ui->pb_num1    , APP_KEY_NUM1);
    btnPressSignalMapper->setMapping(ui->pb_num2    , APP_KEY_NUM2);
    btnPressSignalMapper->setMapping(ui->pb_num3    , APP_KEY_NUM3);
    btnPressSignalMapper->setMapping(ui->pb_num4    , APP_KEY_NUM4);
    btnPressSignalMapper->setMapping(ui->pb_num5    , APP_KEY_NUM5);
    btnPressSignalMapper->setMapping(ui->pb_num6    , APP_KEY_NUM6);
    btnPressSignalMapper->setMapping(ui->pb_num7    , APP_KEY_NUM7);
    btnPressSignalMapper->setMapping(ui->pb_num8    , APP_KEY_NUM8);
    btnPressSignalMapper->setMapping(ui->pb_num9    , APP_KEY_NUM9);
    btnPressSignalMapper->setMapping(ui->pb_dot     , APP_KEY_DOT);
    btnPressSignalMapper->setMapping(ui->pb_bkspace , APP_KEY_BKSPACE);
    btnPressSignalMapper->setMapping(ui->pb_Ok      , APP_KEY_OK);
    btnPressSignalMapper->setMapping(ui->pb_Esc     , APP_KEY_ESC);
    btnPressSignalMapper->setMapping(ui->pb_Const   , APP_KEY_CONST);
    btnPressSignalMapper->setMapping(ui->pb_Altern  , APP_KEY_ALTERN);
    connect(ui->pb_num0    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num1    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num2    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num3    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num4    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num5    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num6    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num7    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num8    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_num9    , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_dot     , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_bkspace , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_Ok      , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_Esc     , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_Const   , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(ui->pb_Altern  , SIGNAL(pressed()), btnPressSignalMapper, SLOT(map()));
    connect(btnPressSignalMapper, SIGNAL(mapped(const int &)), keyDriver1, SLOT(keyPress(const int &)));

    // Control buttons release
    btnReleaseSignalMapper = new QSignalMapper(this);
    btnReleaseSignalMapper->setMapping(ui->pb_num0    , APP_KEY_NUM0);
    btnReleaseSignalMapper->setMapping(ui->pb_num1    , APP_KEY_NUM1);
    btnReleaseSignalMapper->setMapping(ui->pb_num2    , APP_KEY_NUM2);
    btnReleaseSignalMapper->setMapping(ui->pb_num3    , APP_KEY_NUM3);
    btnReleaseSignalMapper->setMapping(ui->pb_num4    , APP_KEY_NUM4);
    btnReleaseSignalMapper->setMapping(ui->pb_num5    , APP_KEY_NUM5);
    btnReleaseSignalMapper->setMapping(ui->pb_num6    , APP_KEY_NUM6);
    btnReleaseSignalMapper->setMapping(ui->pb_num7    , APP_KEY_NUM7);
    btnReleaseSignalMapper->setMapping(ui->pb_num8    , APP_KEY_NUM8);
    btnReleaseSignalMapper->setMapping(ui->pb_num9    , APP_KEY_NUM9);
    btnReleaseSignalMapper->setMapping(ui->pb_dot     , APP_KEY_DOT);
    btnReleaseSignalMapper->setMapping(ui->pb_bkspace , APP_KEY_BKSPACE);
    btnReleaseSignalMapper->setMapping(ui->pb_Ok      , APP_KEY_OK);
    btnReleaseSignalMapper->setMapping(ui->pb_Esc     , APP_KEY_ESC);
    btnReleaseSignalMapper->setMapping(ui->pb_Const   , APP_KEY_CONST);
    btnReleaseSignalMapper->setMapping(ui->pb_Altern  , APP_KEY_ALTERN);
    connect(ui->pb_num0    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num1    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num2    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num3    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num4    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num5    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num6    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num7    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num8    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_num9    , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_dot     , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_bkspace , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_Ok      , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_Esc     , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_Const   , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(ui->pb_Altern  , SIGNAL(pressed()), btnReleaseSignalMapper, SLOT(map()));
    connect(btnReleaseSignalMapper, SIGNAL(mapped(const int &)), keyDriver1, SLOT(keyRelease(const int &)));

    connect(keyDriver1, SIGNAL(onKeyEvent(int,int)), this, SLOT(onKeyDriverEvent(int, int)));

//    ui->TextDisplay1->putStringXY(0,0,"abcde12345abcde12345");
//    ui->TextDisplay1->putStringXY(0,1,"abcde12345abcde12345");
//    ui->TextDisplay1->putStringXY(0,2,"abcde12345abcde12345");
//    ui->TextDisplay1->putStringXY(0,3,"abcde12345abcde12345");

    //char *myStrings[4];
    //myStrings[0] = "Привет!";
    //myStrings[1] = "abcde12345abcde12345";
    //myStrings[2] = "abcde12345abcde12345";
    //myStrings[3] = "abcde12345abcde12345";
    //ui->TextDisplay1->updateAll(myStrings);

    pt2Myself = this;
    registerLcdUpdateCallback((cbLcdUpdatePtr)&MainWindow::updateDisplayWrapper);
    guiInitialize();

    //TDD_VerifyCalibration();
    //TDD_VerifyWaveformGenerator();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Callback function for LCD update
void MainWindow::updateDisplayWrapper(char **data)
{
    pt2Myself->ui->TextDisplay1->updateAll(data);
}



void MainWindow::on_pb_Update_clicked()
{
    guiUpdate();
}


//-----------------------------------//
// Button and encoder events
//
//  action_down
//  action_up
//  action_up_short
//  action_up_long
//  action_hold


// Encodes simulator key codes into GUI key codes
uint32_t MainWindow::encodeGuiKey(int id)
{
    uint32_t guiKey = 0;
    for (uint8_t i=0; i<sizeof(keyCodeTable)/sizeof(keyCodeRecord_t); i++)
    {
        if (keyCodeTable[i].app_key == id)
        {
            guiKey = keyCodeTable[i].btn_code;
            break;
        }
    }
    return guiKey;
}

// Encodes simulator key events into GUI key events
//void MainWindow::encodeGuiEvent(int keyEventType, uint32_t code)
//{
//
//}



// Key driver to GUI
void MainWindow::onKeyDriverEvent(int id, int keyEventType)
{
    uint32_t guiKeyCode = encodeGuiKey(id);
    switch(keyEventType)
    {
        case keyDriver::KEY_DOWN:   buttons.action_down = guiKeyCode;       break;
        case keyDriver::KEY_UP:   buttons.action_up = guiKeyCode;         break;
        case keyDriver::KEY_UP_SHORT:   buttons.action_up_short = guiKeyCode;   break;
        case keyDriver::KEY_UP_LONG:   buttons.action_up_long = guiKeyCode;    break;
        case keyDriver::KEY_HOLD:   buttons.action_hold = guiKeyCode;       break;
    }
    guiButtonEvent();
}





