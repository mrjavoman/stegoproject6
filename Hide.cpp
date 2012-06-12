// Hide.cpp
//
// this file contains the embedding/extraction functions
//

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include "Steg_LSB.h"

// embed the source file into the target file
// will use the numLSB to determine how many MSBs of the source, to embed in the LSBs of the target
int embedImageIntoFile(unsigned char *pTgt, unsigned char *pSrc, int imageLength)
{
	int i;
	unsigned char tmp;

printf("Embedding the data ...\n\n");
printf("gMask=%02x, gShift=%02x, len=%d\n", gMask, gShift, imageLength);

	// embed each source pixel into each target pixel
	// assuming same sized images
	for(i = 0; i < imageLength; i++)
	{
		// zero the bits we will use for concealment
		*pTgt = *pTgt & gMask;

		// shift the source image to fit within the bit range
		// do not change origianl source image
		tmp = *pSrc;
		tmp = tmp >> gShift;

//printf("tmp=%02x\n", tmp);

		// add the source image to the target image
		*pTgt = *pTgt | tmp;

		// increment the data pointers
		pSrc++;
		pTgt++;
	}
	return(SUCCESS);
} // embedImageIntoFile

// extract lower bits from source file
int extractImageFromFile(unsigned char *pTgt, unsigned char *pSrc, int imageLength)
{
	int i;
	unsigned char tmp;

printf("Extracting the data and saving to an image ...\n\n");
printf("gMask=%02x, gShift=%02x, imageLength = %d\n", gMask, gShift, imageLength);

	for(i = 0; i < imageLength; i++)
	{
		tmp = *pSrc;
		tmp = tmp << gShift;
//printf("tmp=%02x\n", tmp);
		*pTgt = tmp;
		pSrc++;
		pTgt++;
	}
	return(SUCCESS);
} // extractImageFromFile

// extract the least significant bits and put them in a file
void extractBitsFromFile(unsigned char *pBin, unsigned char *pSrc, int imageLength)
{
	int i;
	unsigned char tmp, shift;

	printf("Extracting the data ...\n\n");
	printf("gShift=%02x, imageLength = %d\n", gMask, gShift, imageLength);

	shift = 0;
	for(i = 0; i < imageLength; i++)
	{
		tmp = *pSrc;
		tmp = tmp << gShift;

//		tmp = tmp & gAndMask[shift][gShift];

		*pBin = tmp;
		pSrc++;
		pBin++;
		shift++;
		if(shift == 8) shift = 0;
	}
	return;
} // extractBitsFromFile

// this function will extract the specified number of LSBs from the specified source file
// then it will scale those bits to be the MSBs and create a new image file
int extractDataFromSourceFile(char *srcFileName)
{
	unsigned char *pExtractedFile, *pExtractedFileData;
	char ex_filename[260];

	// Allocate Memory to hold Extracted File
	pExtractedFile = (unsigned char *) malloc(srcFileSize);
	if(pExtractedFile == NULL)
	{
		printf("Error allocating memory for source file extracted data.\n\n");
		return(FAILURE);
	}

printf("Getting data from source file size = %d, %d\n", srcFileSize, pSrcInfoHdr->biSizeImage);

	// Save the source file to memory allocated for extracted file
	memcpy(pExtractedFile, pSrcFile, srcFileSize);
	pExtractedFileData = pExtractedFile + pSrcFileHdr->bfOffBits;

	// extract the specified number of bits, scale to be MSB
	extractImageFromFile(pExtractedFileData, pSrcData, pSrcInfoHdr->biSizeImage);

printf("size=%d, filehdr-off=%d, srcinfosize=%d, size=%d\n", 
	   srcFileSize, pSrcFileHdr->bfOffBits, pSrcInfoHdr->biSizeImage, pSrcInfoHdr->biSize);

	strcpy(ex_filename, "ex__");
	strcat(ex_filename, srcFileName);
	writeFile(pExtractedFile, srcFileSize, ex_filename);
	return(SUCCESS);
} // extractDataFromSourceFile

