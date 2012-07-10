// Hide.cpp
// 
// This file contains all the routines to Embed/Extract Message Data
//

#include "include\Hide.h"


typedef struct _MESSAGE
{
	union
	{
		unsigned char bit1:1;
		unsigned char bit2:2;
		unsigned char bit3:3;
		unsigned char bit4:4;
		unsigned char bit5:5;
		unsigned char bit6:6;
		unsigned char bit7:7;
		unsigned char bit8:8;
	} BYTE;
} MESSAGE;

//bitfield read message
void readMsg(MESSAGE *tMsg)
{
	
	//tMsg.BYTE.bit8 = 0xE7;

	printf("1=%c, 2=%c, 3=%c, 4=%c, 5=%c, 6=%c, 7=%c, 8=%c \n", 
		tMsg->BYTE.bit1, tMsg->BYTE.bit2, tMsg->BYTE.bit3, tMsg->BYTE.bit4, 
		tMsg->BYTE.bit5, tMsg->BYTE.bit6, tMsg->BYTE.bit7, tMsg->BYTE.bit8);
	return;
}
//*/


void applyBitMask(MESSAGE *);


char tmpMessage[] = "Hello World! This is my message. Want it to be long to test the program.";

char gOutputFileName[260];

unsigned char lsbMsk1[4] = { 0xfc, 0xf3, 0xcf, 0x3f };
unsigned char lsbMsk2[4] = { 0x03, 0x0c, 0x30, 0xc0 };
unsigned char lsbMsk3[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x30, 0x40, 0x80 };
unsigned char bits[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
int count, total = 0;
unsigned char gBitMask1[8] = { 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
unsigned short gBitMask1_2[8] = { 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
unsigned short gBitMask2_2[8] = { 0xFFFE, 0xFFFC, 0xFFF8, 0xFFF0, 0xFFE0, 0xFFC0, 0xFF80, 0xFF00 };
//unsigned char gBitMask3[8] = { 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };
unsigned int gBitCapacity = 0;

char *gMsgBuffer = NULL;

MESSAGE *tMsg;
int n = 0;
unsigned char *aChar = 0;
unsigned char currentData;
unsigned char tmpLsb;
bool isHiding = false;

unsigned int gMsgSize = 0;
double gAlpha = 1.0;			// jpenquan.h
double gUniformityFactor = 1.2;
unsigned int gImageQuality = 50;
bool gExtractMsg = false;
bool gDestroyMsg = false;
bool gWipeMsg = false;
bool gHideMsg = false;
double lg2; // lg2 = natural log 2
double lgQ; // lg2 of quality factor

// encoding/decoding objects
JpegEncoder gJpgEncoder;
JpegDecoder gJpgDecoder;
BmpEncoder gBitmapEncoder;
BmpDecoder gBitmapDecoder;
BitmapImage gImage;

// This function gets numBits from the message starting from the last position
// up until the end of the message (numBits range from 1 to 8)
unsigned char getBitsFromBuffer(unsigned int numBits, unsigned char *inBuffer, unsigned int inBufferLength)
{

	return(0);
} // getBitsFromBuffer

// this function gets the Uniformity Factor
double getUniformity_E(JpegEncoderCoefficientBlock data)
{
	int row, col;
	short tmpData;

	for(row = 0; row < JpegSampleWidth; row++)
		for(col = 0; col < JpegSampleWidth; col++)
			tmpData = data[row][col];

	return(0);
} // getUniformity_E

// hide the data in a block of coefficients
void hideInBlock(JpegEncoderCoefficientBlock *data, JpegEncoderQuantizationTable &qt)
{
	unsigned int row, col;

	// check for simple conversions - no hiding/extracting
	if(gHideMsg == false) return;

	for(row = 0; row < JpegSampleWidth; row++)
		for(col = 0; col < JpegSampleWidth; col++)
		{
			if((*data)[row][col] > 1 || (*data)[row][col] < 0){
				currentData = (*data)[row][col];
				isHiding = true;
				gBitCapacity += 1;
				
				if(count == 0){
					aChar = (unsigned char*) (gMsgBuffer + n++ );
					count = total = 4;

					for(int i = 0; i < count; i++) {
						bits[i] = *aChar & lsbMsk3[i];
						if( i > 0 ){
							bits[i] >>= i;
						}
					}
					tmpLsb = bits[0];
					count--;
				}
				else {
					tmpLsb = bits[total-count];
					count--;
				}
								
			}
			if(isHiding){
				qt.GetDataValue(row*JpegSampleWidth+col);
				(*data)[row][col] &= -2;
				(*data)[row][col] |= tmpLsb;	// hide data in coefficients
				isHiding = false;
			}
		}
	return;
} // hideInBlock

// takes the biffer, extracts a filename, and writes the rest of the data to disk
void writeMsg()
{
	return;
} // writeMsg

// uses the first 4 bytes in the message buffer to set the actual size of the message
void setMsgSize()
{
	return;
} // setMsgSize

// takes some number of bits and places them in a inBufferfer
int putBitsInBuffer(unsigned int numBits, unsigned char bits, unsigned char *outBuffer, unsigned int outBufferLength)
{
	return(SUCCESS);
} // putBitsInBuffer

// this function gets the Uniformity Factor
double getUniformity_D(JpegDecoderCoefficientBlock data)
{
	return(0);
} // getUniformity_D

// this function removes the bits from a block
void extractFromBlock(JpegDecoderCoefficientBlock data, const JpegDecoderQuantizationTable &qt)
{

	return;
} // extractFromBlock

void applyBitMask(MESSAGE *msg)
{
	msg->BYTE.bit1 &= gBitMask1[0];
	msg->BYTE.bit2 &= gBitMask1[0];
	msg->BYTE.bit3 &= gBitMask1[0];
	msg->BYTE.bit4 &= gBitMask1[0];
	msg->BYTE.bit5 &= gBitMask1[0];
	msg->BYTE.bit6 &= gBitMask1[0];
	msg->BYTE.bit7 &= gBitMask1[0];
	msg->BYTE.bit8 &= gBitMask1[0];

	return;
}


/*
typedef struct _MESSAGE
{
	union
	{
		unsigned char bit1:1;
		unsigned char bit2:2;
		unsigned char bit3:3;
		unsigned char bit4:4;
		unsigned char bit5:5;
		unsigned char bit6:6;
		unsigned char bit7:7;
		unsigned char bit8:8;
	} BYTE;
} MESSAGE;

 bitfield read message
void readMsg()
{
	MESSAGE tMsg;

	tMsg.BYTE.bit8 = 0xE7;

	printf("1=%x, 2=%x, 3=%x, 4=%x, 5=%x, 6=%x, 7=%x, 8=%x \n", 
		tMsg.BYTE.bit1, tMsg.BYTE.bit2, tMsg.BYTE.bit3, tMsg.BYTE.bit4, 
		tMsg.BYTE.bit5, tMsg.BYTE.bit6, tMsg.BYTE.bit7, tMsg.BYTE.bit8);
	return;
}
//*/

