// Hide.cpp
// 
// This file contains all the routines to Embed/Extract Message Data
//

#include "include\Hide.h"
#include <cstdint>

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

typedef struct MESSAGE_SIZE
{
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
} msg_sz;



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
int msgSize = 0;

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
				//set isHiding = true so that we only hide on coefficients that are > 1 and < 0 
				isHiding = true;
				//increment gBitCapacity one byte at a time to messure total capacity of the file
				gBitCapacity += 1;
				
				//grab a byte from message to be hidden only when count is 0
				if( n < msgSize) {
					if(count == 0){
					
						//grab a byte and put it into a character pointer then do pointer arithmetic to advance the pointer to the next byte
						aChar = (unsigned char*) (gMsgBuffer + n++ );
						count = total = 8; //set count and total to 8, we will split the byte into 8 bits

						//split the byte we grabed into 8 bits and for any bit > 1 we will shift to the left i times
						//so that on bits[i] we only have 1's and 0's
						for(int i = 0; i < count; i++) {
							bits[i] = *aChar & lsbMsk3[i];
							if( i > 0 ){
								bits[i] >>= i;
							}
						}
						//set first bit to embed 
						tmpLsb = bits[0];
						count--;
					}
					else {
						//now that we already have all the bits in bits[i] we just iterate through the next bits and embed them
						//in the subsecuent bytes of the cover image
						tmpLsb = bits[total-count];
						count--;
					}
				}
			}
			
			if(isHiding){
				//get the quantatization table
				qt.GetDataValue(row*JpegSampleWidth+col);
				(*data)[row][col] &= -2;		// and the coefficient with -2 which is the same as 11111110
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
	//pointer for holding first 4 bytes of message
	unsigned int *buff;
	//an int is 4 bytes long so if we read from buffer into an int we will read the first 4 bytes of the message
	buff = (unsigned int*) (gMsgBuffer);
	//the message buffer is little endian so we must convert to big endian or else we will get the wrong number
	*buff = endian_swap(*buff);
	//assign our unsigned int to buffSize an now we have or message size
	msgSize = *buff;

	return;
} // setMsgSize

//inline function that swaps endianess on an unsiged int
inline unsigned int endian_swap(unsigned int& x)  
{
return ( ( (x & 0x000000FF) << 24 ) | 
         ( (x & 0x0000FF00) << 8  ) |
         ( (x & 0x00FF0000) >> 8  ) |
         ( (x & 0xFF000000) >> 24 ) );
}



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

