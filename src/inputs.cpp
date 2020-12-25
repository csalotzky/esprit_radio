#include "inputs.h"

#define I2CADDR         0x20
#define ROWS            4
#define COLS            4




/*Gui *guiRef;
Radio *radioRef;*/
i2ckeypad kpd = i2ckeypad(I2CADDR, ROWS, COLS);

char currentKey;
char lastKey;
bool currentHold;
bool lastHold;

EasyButton teszt(12);

//VirtualButton* currentMatrixBtn;



    //bool teszt;
    //EasyButtonVirtual testbtn(teszt);







void ButtonExecute() {
    int8_t matrixKey = -1;
    switch (currentKey) {
 /*       // HARDCODED KEYS
        case DEFAULT_KEY_POWER: {
            // short press: switch source
            if (!currentHold) PowerSwitch();
            // long press (execute only once): power off
            else if (!lastHold) PowerOff();
            return;
        } 
        case DEFAULT_KEY_OK: return;*/
        case KEY_MATRIX_1: matrixKey = 1; break;
        case KEY_MATRIX_2: matrixKey = 2; break;
        case KEY_MATRIX_3: matrixKey = 3; break;
        case KEY_MATRIX_4: matrixKey = 4; break;
        case KEY_MATRIX_5: matrixKey = 5; break;
        case KEY_MATRIX_6: matrixKey = 6; break;
        case KEY_MATRIX_7: matrixKey = 7; break;
        case KEY_MATRIX_8: matrixKey = 8; break;
        case KEY_MATRIX_9: matrixKey = 9; break;
        case KEY_MATRIX_0: matrixKey = 0; break;
        // CONFIGURABLE KEYS
        default: {
            for (size_t i = 0; i < TOTAL_BUTTONS; i++)
            {
                KeyMap tmp = radioRef->config.KeyMaps[i];
                if (tmp.LongPress == currentHold && tmp.Key == currentKey) {
                    Commands[tmp.CmdIndex].Fn();
                    Serial.print(Commands[tmp.CmdIndex].DisplayName);
                    Serial.println(" called");
                    return;
                }
            }
            return;
        } 
    }
    
    // Num pressed
    if (matrixKey >= 0) {
        // Enter freq mode is active
        if (guiRef->display == GUI_ENTERFREQ) radioRef->TuneEnter(matrixKey);

        // short press
        else if (!currentHold) {
            if (radioRef->config.IsNumKeyDefaultPreset) radioRef->RecallPreset(matrixKey ? matrixKey : 10);
            else radioRef->TuneEnter(matrixKey);
        }
        // long press (execute only once)
        else if (!lastHold) radioRef->SavePreset(matrixKey ? matrixKey : 10);
    }
}

void GetRotary(Rotary *rot, bool *up, bool *down) {
	// Read the current state of CLK
    uint8_t currentStateClk = digitalRead(rot->Clk);

    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (LOW == rot->lastStateClk && currentStateClk == 1){

        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(rot->Dt) == LOW) {
            *down = true;
        } else {
            *up = true;
        }
    }

    // Remember last CLK state
    rot->lastStateClk=currentStateClk;
}

void ShortNumCallback() {
    Serial.println("short");
    Serial.println(currentKey,DEC);
}

void LongNumCallback() {
    Serial.println("long");
    Serial.println(currentKey,DEC);
}

void InputsInit(Gui *gui, Radio *radio){
    guiRef = gui;
    radioRef = radio;
    Wire.begin();
    kpd.init();
    InterfaceInit(gui,radio);


    #ifdef TUNE_ROTARY
    rotVol.lastStateClk = digitalRead(rotVol.Clk);
    pinMode(ROT1_CLK,INPUT);
	pinMode(ROT1_DT,INPUT);

    #endif

    #ifdef VOL_ROTARY
    rotTune.lastStateClk = digitalRead(rotTune.Clk);
    pinMode(ROT2_CLK,INPUT);
	pinMode(ROT2_DT,INPUT);

    #endif

    //teszt.begin();
    
    /*testbtn.begin();
    testbtn.onPressed(ShortNumCallback);
    testbtn.onPressedFor(1000,LongNumCallback);
    */

    //delay(5000);
   

    // Programmable keys
    for (size_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        KeyMap tmp = radioRef->config.KeyMaps[i];
        for (size_t j = 0; j < HwButtonsSize; j++)
        {
            if (!tmp.LongPress && tmp.Key == HwButtons[j].KeyId) HwButtons[j].Btn.onPressed(Commands[tmp.CmdIndex].Fn);
            if (tmp.LongPress && tmp.Key == HwButtons[j].KeyId)
            {
                if (Commands[tmp.CmdIndex].RepeatableExecute) HwButtons[j].RepeatableLongFn = Commands[tmp.CmdIndex].Fn;
                else HwButtons[j].Btn.onPressedFor(1000,Commands[tmp.CmdIndex].Fn);
            } 

        }

        for (size_t j = 0; j < VirtButtonsMatrixSize; j++)
        {
            if (!tmp.LongPress && tmp.Key == VirtButtonsMatrix[j].KeyId) VirtButtonsMatrix[j].Btn.onPressed(Commands[tmp.CmdIndex].Fn);
            if (tmp.LongPress && tmp.Key == VirtButtonsMatrix[j].KeyId)
            {
                if (Commands[tmp.CmdIndex].RepeatableExecute) VirtButtonsMatrix[j].RepeatableLongFn = Commands[tmp.CmdIndex].Fn;
                else VirtButtonsMatrix[j].Btn.onPressedFor(1000,Commands[tmp.CmdIndex].Fn);
            } 
        }

        for (size_t j = 0; j < VirtButtonsRotarySize; j++)
        {
            if (!tmp.LongPress && tmp.Key == VirtButtonsRotary[j].KeyId) VirtButtonsRotary[j].Btn.onPressed(Commands[tmp.CmdIndex].Fn);
            //if (tmp.LongPress && tmp.Key == VirtButtonsRotary[j].KeyId) VirtButtonsRotary[j].Btn.onPressedFor(1000,Commands[tmp.CmdIndex].Fn);
        }        
    }

    // Hardcoded keys
    // todoooo
    VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.onPressed(ShortNumCallback);
    VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.onPressedFor(1000,LongNumCallback);
    for (size_t i = 0; i < HwButtonsSize; i++) {
        if (HwButtons[i].KeyId == DEFAULT_KEY_OK) HwButtons[i].Btn.onPressed(Ok);
        if (HwButtons[i].KeyId == DEFAULT_KEY_BACK) HwButtons[i].Btn.onPressed(Back);
        if (HwButtons[i].KeyId == DEFAULT_KEY_LEFT) HwButtons[i].Btn.onPressed(MenuLeft);
        if (HwButtons[i].KeyId == DEFAULT_KEY_RIGHT) HwButtons[i].Btn.onPressed(MenuRight);
        if (HwButtons[i].KeyId == DEFAULT_KEY_UP) HwButtons[i].Btn.onPressed(MenuUp);
        if (HwButtons[i].KeyId == DEFAULT_KEY_DOWN) HwButtons[i].Btn.onPressed(MenuDown);
        if (HwButtons[i].KeyId == DEFAULT_KEY_PWR_TGL) HwButtons[i].Btn.onPressedFor(100,PowerOff);
        if (HwButtons[i].KeyId == DEFAULT_KEY_PWR_BTN) HwButtons[i].Btn.onPressed(PowerSwitch);
    }

    for (size_t i = 0; i < HwButtonsSize; i++)
    {
        HwButtons[i].Btn.begin();
        delay(5);
        // EasyButton bug, todo(?)
        HwButtons[i].Btn.begin();
    }

    for (size_t i = 0; i < VirtButtonsMatrixSize; i++)
    {
        VirtButtonsMatrix[i].Btn.begin();
    }

    for (size_t i = 0; i < VirtButtonsRotarySize; i++)
    {
        VirtButtonsRotary[i].Btn.begin();
    }

    // Tasks
    #if defined(TUNE_ROTARY) || defined(VOL_ROTARY) || defined(NAVI_BUTTONS) || defined(POWER_SWITCH)
    xTaskCreate(ButtonLoop,"Read buttons and rotary",8192,NULL,0,NULL);
    #endif

    #ifdef I2C_KEYPAD
    xTaskCreate(KeypadLoop,"Read Keypad",8192,NULL,0,NULL);
    #endif
}
  


void KeypadLoop(void* params){
    while(true) {
           
        bool error = false;
        //currentMatrixBtn->Btn.read();

        if(i2cSemaphore != NULL ) {
            if(xSemaphoreTake(i2cSemaphore, (TickType_t) 100) == pdTRUE) {
                bool result = kpd.get_key_v2(&currentKey);
                state_KEY_MATRIX_A = 0, state_KEY_MATRIX_B = 0, state_KEY_MATRIX_C = 0, state_KEY_MATRIX_D = 0, state_KEY_MATRIX_ASTERISK = 0, state_KEY_MATRIX_SHARP = 0, state_KEY_MATRIX_NUM = 0;
                xSemaphoreGive(i2cSemaphore);
                if(result) {
                    Serial.print("[");
                    Serial.print(currentKey,DEC);
                    Serial.println("]");
                    /*ButtonExecute();
                    lastKey = currentKey;
                    lastHold = currentHold;*/
                    switch (currentKey)
                    {
                        case KEY_MATRIX_A: state_KEY_MATRIX_A = true; break;
                        case KEY_MATRIX_B: state_KEY_MATRIX_B = true; break;
                        case KEY_MATRIX_C: state_KEY_MATRIX_C = true; break;
                        case KEY_MATRIX_D: state_KEY_MATRIX_D = true; break;
                        case KEY_MATRIX_ASTERISK: state_KEY_MATRIX_ASTERISK = true; break;
                        case KEY_MATRIX_SHARP: state_KEY_MATRIX_SHARP = true; break;
                        default: state_KEY_MATRIX_NUM = true; break;
                    }
                }
                else {
                      /*lastKey = '\0';
                      lastHold = false;*/
                }
                for (size_t i = 0; i < VirtButtonsMatrixSize; i++) VirtButtonsMatrix[i].Btn.read();
                //VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.read();
                //Serial.println(teszt);
                //testbtn.read();
                //Serial.println(VirtButtonsMatrix[VirtButtonsMatrixSize-1].KeyId);
                //if (VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.isPressed()) Serial.println("PRESSED");
                /*if (VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.isPressed()) {
                    if (guiRef->display == GUI_ENTERFREQ || !radioRef->config.IsNumKeyDefaultPreset) radioRef->TuneEnter(currentKey);
                    else radioRef->RecallPreset(currentKey ? currentKey : 10);
                } 
                else if (VirtButtonsMatrix[VirtButtonsMatrixSize-1].Btn.pressedFor(1000) && (guiRef->display == GUI_RADIOMAIN || guiRef->display == GUI_RADIOSIGNAL)) radioRef->SavePreset(currentKey ? currentKey : 10);*/
            }
            else {
                error = true;
            }
        } 
        else {
            error = true;
        }

        if(error) Serial.println("INPUTS | ERROR While fetching keypad button key");
        
   
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void ButtonLoop(void* params) {
    while(true) {
        #ifdef TUNE_ROTARY
        state_KEY_ROT_ROT1_UP=0, state_KEY_ROT_ROT1_DOWN=0;
        GetRotary(&rotTune,&state_KEY_ROT_ROT1_UP,&state_KEY_ROT_ROT1_DOWN);

        #endif
        #ifdef VOL_ROTARY
        state_KEY_ROT_ROT2_UP=0, state_KEY_ROT_ROT2_DOWN=0;
        GetRotary(&rotVol,&state_KEY_ROT_ROT2_UP,&state_KEY_ROT_ROT2_DOWN);
        #endif     

        //if (state_KEY_ROT_ROT1_UP) Serial.println("state_KEY_ROT_ROT1_UP");
        for (size_t i = 0; i < VirtButtonsRotarySize; i++) VirtButtonsRotary[i].Btn.read();
        for (size_t i = 0; i < HwButtonsSize; i++)
        {
            if (HwButtons[i].Btn.read() && HwButtons[i].RepeatableLongFn != NULL && HwButtons[i].Btn.pressedFor(1000)) {
                HwButtons[i].RepeatableLongFn();
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        } 
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

