#ifndef DOG_LCD_h
#define DOG_LCD_h

#include <inttypes.h>
#include "Print.h"

/** Define the available models */
#define DOG_LCD_M081 1
#define DOG_LCD_M162 2
#define DOG_LCD_M163 3

/** define the supply voltage for the display */
#define DOG_LCD_VCC_5V 0
#define DOG_LCD_VCC_3V3 1

/**
 * A class for Dog text LCD's using the 
 * SPI-feature of the controller.
 */
class DogLcd : public Print {
 private:
    /** The model-type of the display connected */
    int model;
    /** The number of lines on the display */
    int rows;
    /** The number of vivible columns on the display */
    int cols;
    /** The size of the character memory on each row */
    int memSize;
    /** The starting address of each row */
    int startAddress[3];
    
    /** The (arduino-)pin used for the serial data */
    int SI;
    /** The (arduino-)pin used for the serial clock */
    int CLK;
    /** 
     * The (arduino-)pin that toggles between sending a command
     * or character data to the display
     */
    int RS;
    /** The (arduino-)pin used selecting the display */
    int CSB;
    /** The (arduino-)pin used for resetting the dislay */
    int RESET;
    /** The (arduino-)pin used for switching the backlight */
    int backLight;
    /** 
     * The supply voltage used for the display. This is
     * one of the constants <code>DOG_LCD_VCC_5V</code> 
     * or <code>DOG_LCD_VCC_3V3</code>.
     */
    int vcc;
    /** The contrast setting for the dipslay */
    int contrast;

    /** the mode the cursor is in */
    int cursorMode;
    /** the mode the display is in */
    int displayMode;
    /** the blink setting for the cursor */
    int blinkMode;

    /** the entrymode currently used */
    int entryMode;

 public:
    /**
     * Creates a new instance of DogLcd and asigns the (arduino-)pins
     * used to control the display.
     * @param SI The (arduino-)pin connected to the SI-pin on the display
     * @param CLK The (arduino-)pin connected to the CLK-pin on the display
     * @param RS The (arduino-)pin connected to the RS-pin on the display
     * @param CSB The (arduino-)pin connected to the CSB-pin on the display
     * @param backLight If you hardware supports switching the backlight 
     * on the display from software this is the (arduino-)pin to be used.
     * @param RESET If you want your code to reset the display from
     * software this is the (arduino-)pin where the RESET-pin of 
     * the display is connected. If you don't need this feature simply 
     * connect the RESET-pin on the display to VCC.
     */
    DogLcd(int SI, int CLK, int RS, int CSB, int RESET=-1, int backLight=-1);
    
    /**
     * Resets and initializes the Display.
     * @param model the type of display that is connected.
     * This must be one of the constants <code>DOG_LCD_M081</code>, 
     * <code>OG_LCD_M162</code> or <code>DOG_LCD_M163</code> 
     * defined in this class.
     * @param contrast the contrast setting for the display. Values
     * between 0x00 and 0x3f are allowed. 
     * @param vcc the supply voltage on which the display runs. This must
     * one of the constants <code>DOG_LCD_VCC_5V</code> 
     * or <code>DOG_LCD_VCC_3V3</code>  
     * @return 0 if the display was sucessfully initialized, 
     * -1 otherwise.
     */
    int begin(int model, int contrast, int vcc=DOG_LCD_VCC_5V);

    /**
     * Reset the display.
     */
    void reset();

    /**
     * Clears the display and moves the cursor back to 
     * the start of the first line.
     */
    void clear();

    /**
     * Moves the cursor back to the start of the first line 
     * without clearing the display.
     */
    void home();

    /**
     * Switches the display off 
     */
    void noDisplay();

    /**
     * Switches the display on 
     */
    void display();

    /**
     * Disables blinking of the cursor 
     */
    void noBlink();

    /**
     *Enables blinking of the cursor 
     */
    void blink();

    /**
     * Disables the (underline-)cursor
     */
    void noCursor();

    /**
     * Enables the (underline-)cursor
     */
    void cursor();

    /**
     * Scroll the contents of the display to the left
     */
    void scrollDisplayLeft();

    /**
     * Scroll the contents of the display to the right
     */
    void scrollDisplayRight();

    /**
     * Calling this method will change the way new characters 
     * are printed at the current cursor-position.
     * The character is printed and then the cursor is advanced to the 
     * column on the right of the character. 
     * This is "sort of" the standard editor behaviour we are 
     * used to with western european languages.
     * It is also the standard behaviuor when the display is reset.
     */
    void leftToRight();

    /**
     * Calling this method will change the way new characters 
     * are printed at the current cursor-position.
     * The character is printed and then the cursor is advanced to the 
     * column on the left of the character. 
     */
    void rightToLeft();

    void autoscroll();

    void noAutoscroll();

    /**
     * Set one of the eight user-defineable chars
     * @param charCode the code of the char you want to define.
     * Values from 0..7 are allowed here
     * @param charMap an array of 8 bytes that contains the char
     * definition. 
     */
    void createChar(int charCode, int charMap[]);

    /**
     * Set the cursor to a new loaction.
     * @param col the column to move the cursor to 
     * @param row the row to move the cursor to 
     * If the column- or row-index does exceed
     * the number of columns/rows on the hardware 
     * the cursor stays where it is.
     */
    void setCursor(int col, int row); 
    
    /**
     * Implements the write()-method from the base-class that
     * is called whenever a character is to be printed to the
     * display. 
     * @param c the character to be printed. 
     */ 
    virtual void write(uint8_t c);
    
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

 private:
    /**
     * Call the displaymode function when cursor settings
     * have been changed
     */
    void writeDisplayMode();
    /**
     * Send a command to the display
     * @param cmd the command to send.
     * @param executionTime the hardware needs some time to
     * execute the instruction just send. This is the time in
     * microseconds the code should wait after trandd´sfrerring the data
     */
    void writeCommand(int cmd, int executionTime);

    /**
     * Send a character to the display 
     * @param c the character to send.
     */
    void writeChar(int c);

    /**
     * Implements the low-level transfer of the data
     * to the hardware.
     * @param executionTime the hardware needs some time to
     * execute the instruction just send. This is the time in
     * microseconds the code should wait after trandd´sfrerring the data
     */
    void spiTransfer(int c,int executionTime);
};

#endif