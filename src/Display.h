#ifndef Display_H
#define Display_H

#include <SPI.h>
#include <TFT.h>


#define TFT_CS_PIN 10
#define SPI_DC_PIN 9
#define SPI_RST_PIN 8

#define TFTROTATE0		1
#define TFTROTATE90		2
#define TFTROTATE180	3
#define TFTROTATE270	4

// Red+Blue 5bits Green 6 bits
#define BR 0
#define BG 0
#define BB 255

#define FR 255
#define FG 0
#define FB 0


#define	DISPW  160
#define	DISPH  128

class Display {
private:
public:

	int bb=BB, bg=BG, br=BR, fb=FB, fg=FG, fr=FR;
	TFT tftScreen = TFT(TFT_CS_PIN, SPI_DC_PIN, SPI_RST_PIN); // @suppress("Abstract class cannot be instantiated")


	Display();
	Display(int br,int bg, int bb);
	Display(int br,int bg, int bb, int fr, int fg, int fb);

	void setFg(int r, int g, int b);
	void setBg(int r, int g, int b);
};

// indexes into chrSize
#define CHRW 0
#define CHRH 1
#define YLINES 2
#define XCOLUMNS 3
#define MAXCHRSIZE 5

class DisplayText{
private:
public:
	const uint8_t chrSize[6][4] = { // Ypix,Xpix,YL,XC
		{6,8,16,26},	// Width in pixels	128*160 Yielding 16 lines of 26 characters at size0
		{6,8,16,26},	// Width in pixels	128*160 Yielding 16 lines of 26 characters at size1
		{13,16,8,12},	// Width in pixels	128*160 Yielding 8 lines of 12 characters at size2
		{17,22,5,9},	// Width in pixels	128*160 Yielding 5 lines of 9 characters at size3
		{22,32,4,7},	// Width in pixels	128*160 Yielding 4 lines of 7 characters at size4
		{32,42,8,5}		// Width in pixels	128*160 Yielding 8 lines of 5 characters at size5
	};
	Display *disp;
//	int textLength;
	int col, row;			// Column Number, Line Number

	int bb=BB, bg=BG, br=BR, fb=FB, fg=FG, fr=FR;
	int textSize = 1;
	char *text;			// Space to be allocated by caller

//	DisplayText(char *txtBuffer, int length, Display *disp, int col, int row, int textSize);
//	DisplayText(char *txtBuffer, int length, Display *disp, int col, int row, int textSize, int br, int bg, int bb, int fr, int fg, int fb);
	DisplayText(char *txtBuffer, Display *disp, int col, int row, int textSize);
	DisplayText(char *txtBuffer, Display *disp, int col, int row, int textSize, int br, int bg, int bb, int fr, int fg, int fb);
	void setFg(int r, int g, int b);
	void setBg(int r, int g, int b);
	void show();
	bool show(char *newText);
	void invert();
	void move(int row, int col);
	void remove();
	bool setText(char *newText);
	void setCol(int col);
	void setRow(int row);
	void setTextSize(int size);
	/*
	#define rgb565(r,g,b) ( ((r>>3) << 11) | ((g>>2) << 5) | b >> 3)
    private int toInt(Color c) {
        return
                (                      255  << 24) |
                ((int) (c.getRed()   * 255) << 16) |
                ((int) (c.getGreen() * 255) << 8)  |
                ((int) (c.getBlue()  * 255));
    }
    */

};

#endif
