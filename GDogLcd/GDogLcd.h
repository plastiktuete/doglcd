#ifndef GDOG_LCD_h
#define GDOG_LCD_h

#include <inttypes.h>
#include "Print.h"

/** Define the available models */
#define GDOG_LCD_132_5 1
#define GDOG_LCD_128_6 2

/** Define available colors on the display */
#define GDOG_LCD_BG_COLOR 0 
#define GDOG_LCD_FG_COLOR 1 

/**
 * A class for Dog graphic LCD's
 */
class GDogLcd : public Print {
 private:
    /** The model-type of the display */
    int model;
    /** The width of the display in pixels */
    int width;
    /** The height of the display in pixels */
    int height;
    
    /** The (arduino-)pin used for the serial data */
    int lcdSI;
    /** The (arduino-)pin used for the serial clock */
    int lcdSCL;
    /** 
     * The (arduino-)pin that toggles between sending a command
     * or character data to the display
     */
    int lcdA0;
    /** The (arduino-)pin used selecting the display */
    int lcdCSB;
    /** The (arduino-)pin used for resetting the dislay */
    int lcdRESET;
    /** The (arduino-)pin used for switching the backlight */
    int backLight;
    /** The contrast setting for the dipslay */
    int contrast;

    /** This is true if the display rotates the view by 180 degrees */
    int rotatedView;
    /** This is true if the display is painting in inverse mode */
    int paintInvers;

    /** A pointer to the display data */
    uint8_t *data;

 public:
    /**
     * Creates a new instance of GDogLcd and asigns the (arduino-)pins
     * used to control the display.
     * @param lcdSI The (arduino-)pin connected to the SI-pin on the display
     * @param lcdCLK The (arduino-)pin connected to the CLK-pin on the display
     * @param lcdRS The (arduino-)pin connected to the RS-pin on the display
     * @param lcdCSB The (arduino-)pin connected to the CSB-pin on the display
     * @param lcdRESET If you want your code to reset the display from
     * software this is the (arduino-)pin where the RESET-pin of 
     * the display is connected. If you don't need this feature simply 
     * connect the RESET-pin on the display to VCC.
     * @param backLight If you hardware supports switching the backlight 
     * on the display from software this is the (arduino-)pin to be used.
     */
    GDogLcd(int lcdSI, int lcdCLK, int lcdRS, int lcdCSB, 
	    int lcdRESET=-1, int backLight=-1);
    
    /**
     * Resets and initializes the Display.
     * @param model the type of display that is connected.
     * This must be one of the constants 
     * <code>GDOG_LCD_132_5</code> or 
     * <code>GDOG_LCD_128_6</code>  
     * defined in this class.
     * @return 0 if the display was sucessfully initialized, 
     * -1 otherwise.
     */
    int begin(int model);

    /**
     * Reset the display.
     */
    void reset();

    /**
     * Set the contrast for the display.
     * @param contrast the contrast to be used for the display. Setting 
     * the contrast to a value < 32 will probably give the impression
     * that the display isn't working because nothing is printed.  
     * The displays that I tested worked firn with a contrast >= 0x28.
     * The valid range for the contrast value is 0.63 (0x00..0x3F hex).
     * If the value is outside the valid range the method does nothing.
     */
    void setContrast(int contrast);

    /**
     * Clears all pixels on the display.
     */
    void clear();

    /**
     * Switches the display off 
     */
    void noDisplay();

    /**
     * Switches the display on 
     */
    void display();

    /**
     * Set the backlight. This is obviously only possible
     * if you have build a small circuit for switching/dimming the 
     * backlight of the display. 
     * @param value the new value for the backlight. If the 
     * backlight driver circuit is connected to a normal digital IO-pin
     * on the arduino using value=LOW or value=HIGH will switch
     * off/on as expected. 
     * @param usePWM set this to true if the driver is connected to  
     * a PWM pin on the arduino you can set any dim the backlight
     * with values from 0..255
     */
    void setBacklight(int value,bool PWM=false);

    /**
     * Paint a pixel on the display.
     * @param x the x-coordiante of the pixel to be painted
     * @param y the y-coordiante of the pixel to be painted
     * @param color the color to be used for painting. 
     * Use the constants GDOG_LCD_FG_COLOR and  
     * GDOG_LCD_BG_COLOR. 
     * Default is GDOG_LCD_FG_COLOR 
     */
    void putPixel(int x, int y, int color=GDOG_LCD_FG_COLOR);

 private:

    /**
     * Returns a pointer to the display databyte for a specific location 
     * on the screen.
     * @param x the x-coordiante of the pixel to be painted
     * @param y the y-coordiante of the pixel to be painted
     * @return a pointer to the display data byte  
     */
    uint8_t *getDisplayPagePointer(int x, int y);
     
    /**
     * Send a command to the display
     * @param cmd the command to send.
     */
    void writeCommand(uinit8_t cmd);

    /**
     * Send pixeldata to the display 
     * @param data the data for 8 pixels.
     */
    void writeData(uint_8t data);

    /**
     * Implements the low-level transfer of the data
     * to the hardware.
     * @param data the 1 byte data to send
     */
    void spiTransfer(uint8_t data);
};

#endif
