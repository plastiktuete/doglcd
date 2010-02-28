#include "GDogLcd.h"
#include "WProgram.h"
 
GDogLcd::GDogLcd(int lcdSI, int lcdCLK, int lcdRS, int lcdCSB, 
		 int lcdRESET=-1, int backLight=-1) {
    this->lcdSI=lcdSI;
    this->lcdCLK=lcdCLK;
    this->lcdRS=lcdRS;
    this->lcdCSB=lcdCSB;
    this->lcdRESET=lcdRESET;
    this->backLight=backLight;
    pinMode(this->lcdSI,OUTPUT);
    pinMode(this->lcdCLK,OUTPUT);
    pinMode(this->lcdRS,OUTPUT);
    pinMode(this->lcdCSB,OUTPUT);
    if(this->lcdRESET>=0) {
	pinMode(this->lcdRESET,OUTPUT);
	digitalWrite(this->lcdRESET,HIGH);
    }
    else {
	this->lcdRESET=-1;
    }
    if(this->backLight>=0) {
	pinMode(this->backLight,OUTPUT);
    }
    else {
	this->backLight=-1;
    }
}

int GDogLcd::begin(int model) {
    if(model==GDOG_LCD_132_5) {
	data=NULL;
	data=(uint8_t)malloc(132*4);
	if(!data)
	    return -1;
    }
    else if(model==GDOG_LCD_128_6) {
	data=NULL;
	data=(uint8_t)malloc(128*8);
	if(!data)
	    return -1;
    }
    else {
	return -1;
    }
    reset();
    return 0;
}

void GDogLcd::reset() {
    if(lcdReset!=-1) {
	digitalWrite(lcdReset,LOW);
	delay(1);
	digitalWrite(lcdReset,HIGH);
	delay(1);
    }
    if(model==GDOG_LCD_132_5) {
	writeCommand(0x40); //Display Startline 0
	writeCommand(0xA1); //Display Orientation
	writeCommand(0xC0); //Output Mode select
 	writeCommand(0xA6); //Display mode normal/inverse
	writeCommand(0xA2); //Lcd Bias
	writeCommand(0x2F); //Power Control
	writeCommand(0xF8); //Booster ratio set cmd
	writeCommand(0x00); //Booster ratio data
	writeCommand(0x23); //V0 voltage regulator
	setContrast(0x1F);
	writeCommand(0xAC); //Static indicator off
	writeCommand(0x00); //Static indicator data
    }
    else if(model==GDOG_LCD_128_6) {
	writeCommand(0x40); //Display Startline 0
	writeCommand(0xA1); //Display Orientation
	writeCommand(0xC0); //Output Mode select
	writeCommand(0xA6); //Display mode normal/inverse
	writeCommand(0xA2); //Lcd Bias
	writeCommand(0x2F); //Power Control
	writeCommand(0xF8); //Booster ratio set cmd
	writeCommand(0x00); //Booster ratio data
	writeCommand(0x27); //V0 voltage regulator
	setContrast(0x16);
	writeCommand(0xAC); //Static indicator off
	writeCommand(0x00); //Static indicator data
    }
    clear();
    display();
}


void GDogLcd::setContrast(int contrast) {
    writeCommand(0x81);
    writeCommand(contrast &0x3F);
}

void GDogLcd::clear() {
    memset(data,0,displayBufferSize);
}

void GDogLcd::noDisplay() {
    writeCommand(0xAE);
}

void GDogLcd::display() {
    writeCommand(0xAF);
}

void GDogLcd::setBacklight(int value,bool PWM=false) {
    if(backLight!=-1) {
	if(PWM) {
	    analogWrite(backLight,value);
	}
	else {
	    digitalWrite(value);
	}
    }
}
	    
void GDogLcd::putPixel(int x, int y, int color=GDOG_LCD_FG_COLOR) {
    uint8_t *dptr=getDisplayPagePointer(x,y);
    if(!dptr) {
	//pixel coordiantes are not on the screen
	return;
    }
    y&=7;//the number of the bit on the the page to be set 
    if(color==GDOG_LCD_FG_COLOR) {
	*dptr|=(uint8_t)(1<<y);
    }
    else {
	*dptr&=(uint8_t)(~(1<<y));
    }
}


void GDogLcd::getDisplayPagePointer(int x, int y) {
    if(x<0 || x>=width || y<0 || y>=width)
	return NULL;
    uint8_t *retval=data+((y>>3)*width)+x;
    return retval;
}

void GDogLcd::writeCommand(uinit8_t cmd) {
    digitalWrite(lcdRS,LOW);
    spiTransfer(cmd);
    digitalWrite(lcdCSB,HIGH);
}

void GDogLcd::writeData(uint_8t data) {
    digitalWrite(lcdRS,HIGH);
    writeCommand(data);
}
    
void GDogLcd::spiTransfer(uint8_t data) {
    digitalWrite(lcdCLK,HIGH);
    digitalWrite(lcdCSB,LOW);
    for(int i=7;i>=0;i--) {
	if(bitRead(value,i)) {
	    digitalWrite(lcdSI,HIGH);
	}
	else {
	    digitalWrite(lcdSI,LOW);
	}
	digitalWrite(lcdCLK,LOW);
	digitalWrite(lcdCLK,HIGH);
    }
    digitalWrite(lcdCSB,HIGH);
}

	    



