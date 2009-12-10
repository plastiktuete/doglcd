#include "DogLcd.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "WProgram.h"

DogLcd::DogLcd(int SI, int CLK, int RS, int CSB, int RESET, int backLight) {
    this->SI=SI;
    this->CLK=CLK;
    this->RS=RS;
    this->CSB=CSB;
    this->RESET=RESET;
    this->backLight=backLight;
    //the first pin to go HIGH, we dont want to send any commands by accident
    pinMode(this->SI,OUTPUT);
    pinMode(this->CLK,OUTPUT);
    pinMode(this->RS,OUTPUT);
    pinMode(this->CSB,OUTPUT);
    pinMode(this->RESET,OUTPUT);
   
    digitalWrite(this->CSB,HIGH);
    digitalWrite(this->RESET,HIGH);
    digitalWrite(this->CLK,HIGH);
    digitalWrite(this->SI,HIGH);
    digitalWrite(this->RS,HIGH);
    if(this->backLight!=-1) {
	pinMode(this->backLight,OUTPUT);
	digitalWrite(this->backLight,LOW);
    }
}

int DogLcd::begin(int model, int contrast, int vcc) {
    if(model==DOG_LCD_M081) {
	this->model=model;
	rows=1;
	cols=8;
	memSize=80;
	startAddress[0]=0;
	startAddress[1]=-1;
	startAddress[2]=-1;
    }
    else if(model==DOG_LCD_M162) {
	this->model=model;
	rows=2;
	cols=16;
	memSize=40;
	startAddress[0]=0;
	startAddress[1]=0x40;
	startAddress[2]=-1;
    }
    else if(model==DOG_LCD_M163) {
	this->model=model;
	rows=3;
	cols=16;
	memSize=16;
	startAddress[0]=0;
	startAddress[1]=0x10;
	startAddress[2]=0x20;
    }
    else {
	//unknown or unsupported model
	return -1;
    }
    if(contrast < 0 || contrast> 0x3F) {
	//contrast is outside the valid range
	return -1;
    }
    this->contrast=contrast;
    if(vcc==DOG_LCD_VCC_5V || vcc==DOG_LCD_VCC_3V3) {
	this->vcc=vcc;
    }
    else {
	//unknown or unsupported supply voltage
	return -1;
    }
    reset();
    return 0;
}

void DogLcd::reset() {
    if(RESET!=-1) {
	//If user wired the reset line, pull it low and wait for 40 millis
	digitalWrite(RESET,LOW);
	delay(40);
	digitalWrite(RESET,HIGH);
	delay(40);
    }
    else {
	//User wants software reset, we simply wait a bit for stable power
	delay(50);
    }
    if(model==DOG_LCD_M081) {
	//8-bit,1-line,switch to instruction set 1
	writeCommand(0x31,30);
	//bias 1/4 
	writeCommand(0x1C,30);
    }
    else if(model==DOG_LCD_M162) {
	//8-bit,2-line,switch to instruction set 1
	writeCommand(0x39,30);
	//bias 1/4
	writeCommand(0x1C,30);
    }
    else if(model==DOG_LCD_M163) {
	//8-bit,2-line,switch to instruction set 1
	writeCommand(0x39,30);
	//bias 1/4
	writeCommand(0x1D,30);
    }

    //follower control
    writeCommand(0x69,30);

    if(this->vcc==DOG_LCD_VCC_5V) { 
	/*
	  For 5v operation the booster must be off
	  set (2-bit) high-nibble of contrast
	*/
	writeCommand((0x50 | ((contrast>>4)&0x03)),30);
    }
    else {
	/*
	  For 3.3v operation the booster must be on
	  set (2-bit) high-nibble of contrast
	*/
	writeCommand((0x54 | ((contrast>>4)&0x03)),30);
    }	
    //set low-byte of contrast
    writeCommand((0x70 | (contrast & 0x0F)),30);
    //Standard setting is : display on, cursor on, no blink
    displayMode=0x04;
    cursorMode=0x02;
    blinkMode=0x00;
    writeDisplayMode();
    entryMode=0x04;
    clear();
    leftToRight();
}
    
void DogLcd::clear() {
    writeCommand(0x01,1080);
}

void DogLcd::home() {
    writeCommand(0x02,1080);
}

void DogLcd::setCursor(int col, int row) {
    if(col>=memSize || row>=rows) {
	//not a valid cursor position
	return;
    }
    int address=(startAddress[row]+col) & 0x7F;
    Serial.println(address,HEX);
    writeCommand(0x80|address,30);
}

void DogLcd::noDisplay() {
    displayMode=0x00;
    writeDisplayMode();
}

void DogLcd::display() {
    displayMode=0x04;
    writeDisplayMode();
}

void DogLcd::noCursor() {
    cursorMode=0x00;
    writeDisplayMode();
}

void DogLcd::cursor() {
    cursorMode=0x02;
    writeDisplayMode();
}

void DogLcd::noBlink() {
    blinkMode=0x00;
    writeDisplayMode();
}

void DogLcd::blink() {
    blinkMode=0x01;
    writeDisplayMode();
}

void DogLcd::scrollDisplayLeft(void) {
    writeCommand(0x18,30);
}

void DogLcd::scrollDisplayRight(void) {
    writeCommand(0x1C,30);
}

void DogLcd::leftToRight(void) {
    entryMode|=0x02;
    writeCommand(entryMode,30);
}

void DogLcd::rightToLeft(void) {
    entryMode&=~0x02;
    writeCommand(entryMode,30);
}

void DogLcd::autoscroll(void) {
    entryMode|=0x01;
    writeCommand(entryMode,30);
}

void DogLcd::noAutoscroll(void) {
    entryMode&=~0x01;
    writeCommand(entryMode,30);
}

void DogLcd::createChar(int charPos, int charMap[]) {
    int baseAddress;
    if(charPos<0 || charPos>7)
	return;
    baseAddress=charPos*8;
    writeCommand(0x30,30);
    for (int i=0; i<8; i++) {
    	writeCommand((0x40|(baseAddress+i)),30);
	writeChar(charMap[i]);
    }
    /*
      The numner of lines onn the display belong to the 'Function Set'.
      We have to restore this withn different value for each model
    */
    if(model==DOG_LCD_M081) {
	//8-bit,1-line,switch to instruction set 0
	writeCommand(0x30,30);
    }
    else if(model==DOG_LCD_M162) {
	//8-bit,2-line,switch to instruction set 0
	writeCommand(0x38,30);
    }
    else if(model==DOG_LCD_M163) {
	//8-bit,2-line,switch to instruction set 0
	writeCommand(0x38,30);
    }
}

void DogLcd::writeDisplayMode() {
    writeCommand((0x08 | displayMode | cursorMode | blinkMode),30);
}


void DogLcd::setBacklight(int value, bool usePWM) {
    if(backLight!=-1 && value>=0) {
	if(!usePWM) {
	    if(value==LOW) {
		digitalWrite(backLight,LOW);
	    }
	    else {	    
		digitalWrite(backLight,HIGH);
	    }
	}
	else {	    
	    if(value>255)
		value=255;
	    analogWrite(backLight,value);
	}
    }
}

void DogLcd::writeChar(int value) {
    digitalWrite(RS,HIGH);
    spiTransfer(value,30);
}

void DogLcd::writeCommand(int value,int executionTime) {
    digitalWrite(RS,LOW);
    spiTransfer(value,executionTime);
}

void DogLcd::spiTransfer(int value, int executionTime) {
    digitalWrite(CLK,HIGH);
    digitalWrite(CSB,LOW);
    for(int i=7;i>=0;i--) {
	if(bitRead(value,i)) {
	    digitalWrite(SI,HIGH);
	}
	else {
	    digitalWrite(SI,LOW);
	}
	digitalWrite(CLK,LOW);
	digitalWrite(CLK,HIGH);
    }
    digitalWrite(CSB,HIGH);
    delayMicroseconds(executionTime);
}

void DogLcd::write(uint8_t c) {
    writeChar(c);
}
