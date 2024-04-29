#include "Display.h"

Display:: Display():Display(0,0,0,255,255,255){}
Display:: Display(int br,int bg, int bb):Display(br,bg,bb,255,255,255){}
Display:: Display(int br,int bg, int bb, int fr, int fg, int fb){
	this->setFg(fr, fg, fb);
	this->setBg(br,bg,bb);
}
void Display::setup(){
	this->tftScreen.begin();
	this->tftScreen.setRotation(TFTROTATE180);
	this->tftScreen.background(br, bg, bb);
}
void Display::setFg(int r, int g, int b){
	this->fr = r;
	this->fg = g;
	this->fb = b;
}
void Display::setBg(int r, int g, int b){
	//Serial.print(r); Serial.print(':'); Serial.print(g); Serial.print(':'); Serial.println(b);
	this->br = r;
	this->bg = g;
	this->bb = b;
}

//DisplayText::DisplayText(char *txtBuffer, int length,Display *disp, int col, int row, int textSize):
//		DisplayText(txtBuffer, length,disp, col, row, textSize, disp->br, disp->bg, disp->bb, disp->fr, disp->fg, disp->fb){ }
DisplayText::DisplayText(char *txtBuffer, Display *disp, int col, int row, int textSize):
		DisplayText(txtBuffer, disp, col, row, textSize, disp->br, disp->bg, disp->bb, disp->fr, disp->fg, disp->fb){ }
//DisplayText::DisplayText(char *txtBuffer, int length, Display *disp, int col, int row, int textSize, int br, int bg, int bb, int fr, int fg, int fb){
DisplayText::DisplayText(char *txtBuffer, Display *disp, int col, int row, int textSize, int br, int bg, int bb, int fr, int fg, int fb){
//	this->textLength = length;
	this->disp = disp;
	this->col = col;
	this->row = row;
	this->textSize = textSize;
	this->setFg(fr, fg, fb);
	this->setBg(br, bg, bb);
	this->text = txtBuffer;
//	Serial.println(F("Display Text Object Created"));
}
void DisplayText::setFg(int r, int g, int b){
	this->fr = r;
	this->fg = g;
	this->fb = b;
}
void DisplayText::setBg(int r, int g, int b){
	this->br = r;
	this->bg = g;
	this->bb = b;
}

void DisplayText::show(){
//	int pixsz=strlen(this->text)*this->chrSize[this->textSize][CHRW];

	if(this->textSize > MAXCHRSIZE) return;

	int xpix = this->col*this->chrSize[this->textSize][CHRW];
	int ypix = this->row*this->chrSize[this->textSize][CHRH];

	this->disp->tftScreen.setTextSize(this->textSize);
//	this->disp->tftScreen.stroke(this->fr, this->fg, this->fb);
	this->disp->tftScreen.setTextColor(
			disp->tftScreen.Color565(this->fr, this->fg, this->fb),
			disp->tftScreen.Color565(this->br, this->bg, this->bb));

	this->disp->tftScreen.setCursor(xpix, ypix);
	this->disp->tftScreen.print(this->text);
	//this->disp->tftScreen.text(this->text, xpix, ypix);
}

bool DisplayText::show(char *newText){
//	if((int)strlen(newText) > this->textLength)
//		return false;

	if(strcmp(this->text, newText) != 0){
//		int pixsz=strlen(this->text)*this->chrSize[this->textSize][CHRW];

		int xpix = this->col*this->chrSize[this->textSize][CHRW];
		int ypix = this->row*this->chrSize[this->textSize][CHRH];

				// Wipe out previous
		this->disp->tftScreen.setTextSize(this->textSize);
		this->disp->tftScreen.stroke(this->br, this->bg, this->bb);
		this->disp->tftScreen.text(this->text, xpix, ypix);

				// Write new
		this->disp->tftScreen.stroke(this->fr, this->fg, this->fb);
		this->disp->tftScreen.text(newText, xpix,ypix);
		this->text = newText;
	}

	return true;
}

void DisplayText::invert(){
	int br = this->br;
	int bg = this->bg;
	int bb = this->bb;
	this->br = this->fr;
	this->bg = this->fg;
	this->bb = this->fb;

	this->fr = br;
	this->fg = bg;
	this->fb = bb;
}
void DisplayText::move(int col, int row){
	if(this->textSize > MAXCHRSIZE) return;

	int pixsz=strlen(this->text)*this->chrSize[this->textSize][CHRW];

	int origXpix = this->col*this->chrSize[this->textSize][CHRW];
	int origYpix = this->row*this->chrSize[this->textSize][CHRH];
	int newXpix = col*this->chrSize[this->textSize][CHRW];
	int newYpix = row*this->chrSize[this->textSize][CHRH];

			// Wipe out previous
	this->disp->tftScreen.setTextSize(this->textSize);
	this->disp->tftScreen.stroke(this->br, this->bg, this->bb);
	this->disp->tftScreen.text(this->text, origXpix, origYpix);

			// Write new
	this->disp->tftScreen.stroke(this->fr, this->fg, this->fb);
	this->disp->tftScreen.text(this->text, newXpix,newYpix);
	this->col = col;
	this->row = row;
}
void DisplayText::remove(){
	if(this->textSize > MAXCHRSIZE) return;

	int pixsz=strlen(this->text)*this->chrSize[this->textSize][CHRW];

	int origXpix = this->col*this->chrSize[this->textSize][CHRW];
	int origYpix = this->row*this->chrSize[this->textSize][CHRH];
	int newXpix = col*this->chrSize[this->textSize][CHRW];
	int newYpix = row*this->chrSize[this->textSize][CHRH];

			// Wipe out previous
	this->disp->tftScreen.setTextSize(this->textSize);
	this->disp->tftScreen.stroke(this->br, this->bg, this->bb);
	this->disp->tftScreen.text(this->text, origXpix, origYpix);
}

bool DisplayText::setText(char *newText){
//	if((int)strlen(newText) > this->textLength)
//		return false;
	this->text = newText;
	return true;
}
void DisplayText::setCol(int col){
	this->col = col;
}
void DisplayText::setRow(int row){
	this->row = row;
}
void DisplayText::setTextSize(int size){
	if(size > MAXCHRSIZE ||  size < 0 ) return;
	this->textSize = size;
}
