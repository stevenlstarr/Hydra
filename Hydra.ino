/* MIT License

Copyright (c) 2021 Steven Starr

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <Arduino.h>
#include <lvgl.h>                 // Graphic User Interface
#include <TFT_eSPI.h>             // ILI9341 2.8 Inch Touch LCD
#include <XPT2046_Touchscreen.h>  // ILI9341 Touch Support
#include <SPI.h>
//#include <SD.h>                 // Micro SD Card
#include "BluetoothSerial.h"      // Bluetooth over Serial
#include "MPU9250.h"              // MPU-9250 (Motion Sensor)
//#include "MCP23017.h"           // MCP23017 (MCP23017 16 channel I2C port expander)
#include "Wire.h"                 // MCP23017 (MCP23017 16 channel I2C port expander)


// Motion Control
MPU9250 mpu;
//MCP23017 MCP(0x38);

// Serial Communication
#define LED_PIN 32
bool led_status = false;
String command;



// ILI9341 2.8 Inch Touch LCD
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}



void setup() {
    Serial.begin(115200);

    // Serial.println(MCP23017_LIB_VERSION);
    
    pinMode(LED_PIN, OUTPUT);



    // Graphics Library Setup
    lv_init();
    tft.fillScreen(TFT_BLACK); 

    tft.begin(); /* TFT init */
    tft.setRotation(0); /* Landscape orientation */

    uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
    tft.setTouch(calData);
  
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 320;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // Change Screen BG Color to Black
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(lv_scr_act(), LV_OBJ_PART_MAIN, &style_screen);



    // Label Style
    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    
    // Create the main label
    lv_obj_t * main_label = lv_label_create(lv_scr_act(), NULL);
    //lv_label_set_long_mode(main_label, LV_LABEL_LONG_SROLL_CIRC);
    //lv_obj_set_width(main_label, 100);
    lv_label_set_text(main_label, "Activity");

    // Position the main label
    lv_obj_align(main_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_add_style(main_label, LV_LABEL_PART_MAIN, &label_style);




    
}

void send_led_status(){
  if ( led_status ) {
    Serial.println("LED is ON");
  } else {
    Serial.println("LED is OFF");
  }
} 
void loop() {

    // Gui Library
    lv_task_handler(); /* let the GUI do its work */
    delay(5);


    // Serial Library
    if(Serial.available()){
        command = Serial.readStringUntil('\n');

        Serial.printf("Command received %s \n", command);

        if(command.equals("led=on")){
            digitalWrite(LED_PIN, HIGH);
            led_status = 1;
        } else if(command.equals("led=off")){
            digitalWrite(LED_PIN, LOW);
            led_status = 0;
        } else if(command.equals("ledstatus")){
            send_led_status();
        } else{
            Serial.println("Invalid command");
        }
    }
}
