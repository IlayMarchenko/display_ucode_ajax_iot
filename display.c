#include <stdint.h>
#include <unistd.h>
#include <ti/drivers/GPIO.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplayExt.h>
#include <ti/display/AnsiColor.h>

#include "ti_drivers_config.h"
#include "splash_image.h"

void draw_splash_image(Display_Handle hLcd, Graphics_Image image, uint8_t time);

void *mainThread(void *arg0) {
    /*unsigned int ledPinValue;*/
    unsigned int ledPinValue;
    unsigned int loopCount = 0;
    char *currLedState;

    GPIO_init();
    Display_init();

    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    Display_Handle hLcd = Display_open(Display_Type_LCD, &params);
    Display_Handle hSerial = Display_open(Display_Type_UART, &params);

    if (hSerial == NULL && hLcd == NULL) {
        while (1) {}
    }

    draw_splash_image(hLcd, splashImage, 30);

    char *serialLedOn = "On";
    char *serialLedOff = "Off";

    if (Display_getType(hSerial) & Display_Type_ANSI) {
        serialLedOn = ANSI_COLOR(FG_GREEN, ATTR_BOLD) "On" ANSI_COLOR(ATTR_RESET);
        serialLedOff = ANSI_COLOR(FG_RED, ATTR_UNDERLINE) "Off" ANSI_COLOR(ATTR_RESET);
    }

    while (1) {

        ledPinValue = GPIO_read(CONFIG_GPIO_LED_0);

        Display_clearLine(hLcd, ledPinValue ? 1 : 0);
        Display_printf(hLcd, ledPinValue ? 0 : 1, 0, "LED: %s", (ledPinValue == CONFIG_GPIO_LED_ON) ? "On!":"Off!");

        Display_clearLine(hSerial, ledPinValue ? 1 : 0);
        Display_printf(hSerial, ledPinValue ? 0 : 1, 0, "LED: %s", (ledPinValue == CONFIG_GPIO_LED_ON) ? serialLedOn : serialLedOff);

        if (Display_getType(hSerial) & Display_Type_ANSI) {
            currLedState = (ledPinValue == CONFIG_GPIO_LED_ON) ? serialLedOn : serialLedOff;
            Display_printf(hSerial, DisplayUart_SCROLLING, 0, "[ %d ] LED: %s", loopCount++, currLedState);
        }

        sleep(1);
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }
}

void draw_splash_image(Display_Handle hLcd, Graphics_Image image, uint8_t time) {

    if (hLcd) {
        Display_printf(hLcd, 5, 3, "Hello LCD!");
        sleep(3);
        Graphics_Context *context = DisplayExt_getGraphicsContext(hLcd);

        if (context) {
            int splashX = (Graphics_getDisplayWidth(context) - Graphics_getImageWidth(&image)) / 2;
            int splashY = (Graphics_getDisplayHeight(context) - Graphics_getImageHeight(&image)) / 2;

            Graphics_drawImage(context, &image, splashX, splashY);
            Graphics_flushBuffer(context);
        }

        sleep(time);
        Display_clear(hLcd);
    }
}
