// Hiding in the LSB
//
// Steg_LSB.exe
//

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>
#include "Steg_LSB.h"

// Global Variables for File Data Pointers
BITMAPFILEHEADER *pSrcFileHdr, *pTgtFileHdr;
BITMAPINFOHEADER *pSrcInfoHdr, *pTgtInfoHdr;
RGBQUAD *pSrcColorTable, *pTgtColorTable;
unsigned char *pSrcFile, *pTgtFile, *pSrcData, *pTgtData;
int srcFileSize, tgtFileSize;

unsigned char toCGC[256];
unsigned char toPBC[256];

// default values
unsigned char gNumLSB = 1, gMask = 0xfe, gShift = 7;
unsigned char gAndMask[8][8] =
{
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }
};

// this routine fills the gray code array with corresponding values
void buildGrayCode()
{
	int i, length, posUp, posDw, cnt;

	length = 1;
	toCGC[0] = 0;
	toPBC[0] = 0;
	cnt = 1;

	while(length < 256)
	{
		posUp = length - 1;
		posDw = length;
		for(i = 0; i < length; i++)
		{
			toCGC[i + posDw] = toCGC[posUp - i] + length;
			toPBC[toCGC[i + posDw]] = cnt++;
		}
		length = length * 2;
	}
	//for(i=0; i< 256;i++)
	//	printf("CGC:%02x, PBC:%02x\n", toCGC[i], toPBC[i]);
	return;
} // buildGrayCode

// Show the various bitmap header bytes primarily for debugging
void displayFileInfo(char *pFileName,
					 BITMAPFILEHEADER *pFileHdr,
					 BITMAPINFOHEADER *pFileInfo,
					 RGBQUAD *pColorTable,
					 unsigned char *pData)
{
	// first two characters in bitmap file should be "BM"
	char *pp = (char *) &(pFileHdr->bfType);
	int numColors, i, j;
	float mean=0, variance=0, stddev=0, sum, sum2;
	int N, M;	// image dimensions
	RGBQUAD *pCT = pColorTable;

	printf("\nFile Info for %s: \n\n", pFileName);

	printf("File Header Info:\n");
	printf("File Type: %c%c\nFile Size:%d\nData Offset:%d\n\n",
		*pp, *(pp + 1), pFileHdr->bfSize, pFileHdr->bfOffBits);

	switch(pFileInfo->biBitCount)
	{
	case 1:
		numColors = 2;
		break;
	case 4:
		numColors = 16;
		break;
	case 8:
		numColors = 256;
		break;
	case 16:
		numColors = 65536;
		break;
	case 24:
		numColors = 16777216;
		break;
	default:
		numColors = 16777216;
	}

	printf("Bit Map Image Info:\n\nSize:%d\nWidth:%d\nHeight:%d\nPlanes:%d\n"
		"Bits/Pixel:%d ==> %d colors\n"
		"Compression:%d\nImage Size:%d\nRes X:%d\nRes Y:%d\nColors:%d\nImportant Colors:%d\n\n",
		pFileInfo->biSize,
		pFileInfo->biWidth,
		pFileInfo->biHeight,
		pFileInfo->biPlanes,
		pFileInfo->biBitCount, numColors,
		pFileInfo->biCompression,
		pFileInfo->biSizeImage,
		pFileInfo->biXPelsPerMeter,
		pFileInfo->biYPelsPerMeter,
		pFileInfo->biClrUsed,
		pFileInfo->biClrImportant);

	/* Display Color Tables if Desired

	//	There are no color tables
	if(pFileInfo->biBitCount > 16) return;

	//	only needed for debugging
	printf("Color Table:\n\n");

	for(i = 0; i < numColors; i++)
	{
		printf("R:%02x   G:%02x   B:%02x\n", pCT->rgbRed, pCT->rgbGreen, pCT->rgbBlue);
		pCT++;
	}
	//*/

	// Compute the mean, variance, and standard deviation of an N X M image
	// calculated with nested for statements for clarity
	printf("Calculating the mean, variance, and standard deviation.\n\n");

	N = pFileInfo->biHeight;
	M = pFileInfo->biWidth;
	sum = 0;

	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
		{
			sum = sum + *(pData + i * M + j);
		}
	mean = sum/(N * M);

	sum2 = 0;
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
		{	float tmp;
			tmp =  (*(pData + i * M + j) - mean);
			sum2 = sum2 + tmp * tmp;
		}
	variance  = sum2/(N * M);
	stddev = sqrt(variance);

	printf("Mean:%f\nVariance:%f\nStandard Deviation:%f\n\n", mean, variance, stddev);
	printf("\n\n");
	return;
} // displayFileInfo

// prints help message to the screen
void Usage()
{
	printf("Usage: Steg_LSB 'source filename' 'target filename' [bits to use] \n\n");
	printf("Where 'source filename' is the name of the bitmap file to hide.\n");
	printf("Where 'target filename' is the name of the bitmap file to conceal the source.\n");
	printf("To extract data from the source, name the target file \"ex\".\n");
	printf("To bit slice the source, name the target file \"bs\".\n");
	printf("To make a random picture for testing, name the target file \"rnd\".\n");// jao1
	printf("The number of bits to hide or extract, range is (1 - 7).\n");
	printf("If not specified, 1 bit is used as the default.\n\n");
	return;
} // Usage

// scramble the color table
unsigned char *scramble(unsigned char *pSrcFile, RGBQUAD *pSrcColorTable, int fileSize)
{
	BITMAPINFOHEADER *pDstInfoHdr;
	RGBQUAD *pDstColorTable;
	int i, cnt = 0, last = 0;
	unsigned char *pDstFile;

	pDstFile = (unsigned char *) malloc(fileSize);

	if(pDstFile == NULL)
	{
		printf("\n\nError allocating memory for scrambled file.\n\n");
		return(NULL);
	}

	// copy the source file to memory
	memcpy(pDstFile, pSrcFile, fileSize);

	pDstInfoHdr = (BITMAPINFOHEADER *) (pDstFile + sizeof(BITMAPFILEHEADER));
	// pointer to first RGB color palette, follows file header and bitmap header
	pDstColorTable = (RGBQUAD *) (pDstFile + sizeof(BITMAPFILEHEADER) + pDstInfoHdr->biSize);

	// the color table has 256 entries
	// will scramble by choosing the next color to swap, using a prime number
	// by using a number relatively prime to 256, we will hit each and every color
	// table entry exactly once
	for(i = 0; i < 256; i++)
	{
		cnt += 67;	// using 211 as our number relatively prime to 256
		if(cnt > 255) cnt -= 256;
printf("color %d == color %d\n", last,cnt);
		pDstColorTable[last].rgbBlue = pSrcColorTable[cnt].rgbBlue;
		pDstColorTable[last].rgbGreen = pSrcColorTable[cnt].rgbGreen;
		pDstColorTable[last].rgbRed = pSrcColorTable[cnt].rgbRed;

		last += 1;
		if(last > 255) last -= 256;
	}
	return(pDstFile);
} // scramble

// this function will slice an image into 8 bitplanes
void BitSlice(char *fileName, int fileSize)
{
	BITMAPINFOHEADER *pDstInfoHdr;
	BITMAPFILEHEADER *pDstFileHdr;
	char bs_filename[260];
	int dataSize, mult;
	unsigned char *pDstFile, *pDstData;
//	unsigned short tmpW;

	pDstFile = (unsigned char *) malloc(fileSize);

	if(pDstFile == NULL)
	{
		printf("\n\nError allocating memory for bit sliced file.\n\n");
		return;
	}

	// copy the source file to memory
	memcpy(pDstFile, pSrcFile, fileSize);

	// Set up pointers to various parts of the source file
	pDstFileHdr = (BITMAPFILEHEADER *) pDstFile;
	pDstInfoHdr = (BITMAPINFOHEADER *) (pDstFile + sizeof(BITMAPFILEHEADER));

	// file header indicates where image data begins
	pDstData = pDstFile + pDstFileHdr->bfOffBits;
	dataSize = (int) (pDstFile + fileSize - pDstData);

	mult = 256;
	gMask = 1;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < dataSize; j++)
		{
			// for regular bit planes
			*(pDstData + j) = *(pDstData + j) & gMask;

			// For canonical gray code bit planes
			//*(pDstData + j) = toCGC[*(pDstData + j)] & gMask; // get gray code bit planes


			if(*(pDstData + j) != 0) *(pDstData + j) = 255; // this line works for 2-color transform
			//tmpW = *(pDstData + j) * mult;
			//if(tmpW != 0) tmpW = tmpW - 1;
			//*(pDstData + j) = (unsigned char) tmpW;
		}
		strcpy(bs_filename, "bs_");
		strcat(bs_filename, fileName);

		writeFile(pDstFile, pDstFileHdr->bfSize, bs_filename);
		gMask = gMask << 1;
		mult = mult >> 1;

		// copy the source file to memory
		memcpy(pDstFile, pSrcFile, fileSize);
	}
	return;
} // BitSlice

// Randomize  ---  jao1
// makes a picture the size of the source with random values
void randomize(char *fileName, int fileSize)
{
	BITMAPINFOHEADER *pDstInfoHdr;
	BITMAPFILEHEADER *pDstFileHdr;
	char rnd_filename[260];
	int dataSize;
	WORD w1;
	BYTE b1;
	unsigned char *pDstFile, *pDstData;

	pDstFile = (unsigned char *) malloc(fileSize);

	if(pDstFile == NULL)
	{
		printf("\n\nError allocating memory for randomized file.\n\n");
		return;
	}

	// copy the source file to memory
	memcpy(pDstFile, pSrcFile, fileSize);

	// seed the random number generator
	srand(GetTickCount());

	// Set up pointers to various parts of the source file
	pDstFileHdr = (BITMAPFILEHEADER *) pDstFile;
	pDstInfoHdr = (BITMAPINFOHEADER *) (pDstFile + sizeof(BITMAPFILEHEADER));

	// file header indicates where image data begins
	pDstData = pDstFile + pDstFileHdr->bfOffBits;
	dataSize = (int) (pDstFile + fileSize - pDstData);

	for(int j = 0; j < dataSize; j++)
	{
		// get random number from built-in C function
		w1 = rand();
		b1 = w1 & 0xff;
		*(pDstData + j) = b1;
	}
	strcpy(rnd_filename, "rnd_");
	strcat(rnd_filename, fileName);
	writeFile(pDstFile, pDstFileHdr->bfSize, rnd_filename);

	return;
} // randomize

// Main function in LSB Steg
// Parameters are used to indicate the input file and available options
int main(int argc, char *argv[])
{
	int x;

	if(argc < 3 || argc > 4)
	{
		Usage();
		return -1;
	}

	// initialize gray code conversion table
	buildGrayCode();

	// get the number of bits to use for data hiding or data extracting
	// if not specified, default to one
	if(argc == 4)
	{
		// the range for gNumLSB is 1 - 7;  if gNumLSB == 0, then the mask would be 0xFF and the
		// shift value would be 8, leaving the target unmodified during embedding or extracting
		// if gNumLSB == 8, then the source would completely replace the target
		gNumLSB = *(argv[3]) - 48;
		if(gNumLSB < 1 || gNumLSB > 7)
		{
			gNumLSB = 1;
			printf("The number specified for LSB was invalid, using the default value of '1'.\n\n");
		}
		gMask = 256 - (int) pow(2, gNumLSB);
		gShift = 8 - gNumLSB;
	}

	// read the source file
	pSrcFile = readFile(argv[1], &srcFileSize);
	if(pSrcFile == NULL) return -1;

	// Set up pointers to various parts of the source file
	pSrcFileHdr = (BITMAPFILEHEADER *) pSrcFile;
	pSrcInfoHdr = (BITMAPINFOHEADER *) (pSrcFile + sizeof(BITMAPFILEHEADER));
	// pointer to first RGB color palette, follows file header and bitmap header
	pSrcColorTable = (RGBQUAD *) (pSrcFile + sizeof(BITMAPFILEHEADER) + pSrcInfoHdr->biSize);
	// file header indicates where image data begins
	pSrcData = pSrcFile + pSrcFileHdr->bfOffBits;

	// for debugging purposes, show file info on the screen
	displayFileInfo(argv[1], pSrcFileHdr, pSrcInfoHdr, pSrcColorTable, pSrcData);

	// if the target file is "ex" then we will extract data from the source file instead of embedding
	// the source file into the target file
	if(strcmp(argv[2], "ex") == 0)
	{
		// extract gNumLSB from source file, scale the data to the upper bits
		x = extractDataFromSourceFile(argv[1]);

		if(x == SUCCESS)
		{
			printf("Extraction successful!\n");
		}
		return 1;
	}

	// if target file is "bs" then we will slice the image into 8 pieces, one for each bit plane
	if(strcmp(argv[2], "bs") == 0)
	{
		BitSlice(argv[1], srcFileSize);
		return 1;
	} // bit slice

	// if target file is "rnd" we will produce a picture composed of pseudo-random numbers
	if(strcmp(argv[2], "rnd") == 0)
	{
		randomize(argv[1], srcFileSize);
		return 1;
	} // bit slice

	// scramble the color table
	if(strcmp(argv[2], "sc") == 0)
	{
		char nm[128];
		unsigned char *pFile;
		FILE *ptrFile;

		strcpy(nm, "sc_");
		strcat(nm, argv[1]);

		// scramble the entire color table
		pFile = scramble(pSrcFile, pSrcColorTable, srcFileSize);
		if(pFile == NULL) return 1;

		// open the new file, MUST set binary format (text format will add line feed characters)
		ptrFile = fopen(nm, "wb+");
		if(ptrFile == NULL)
		{
			printf("Error opening new file for writing.\n\n");
			return -1;
		}

		// write the file
		x = (int) fwrite(pFile, sizeof(unsigned char), srcFileSize, ptrFile);

		// check for success
		if(x != srcFileSize)
		{
			printf("Error writing file %s.\n\n", nm);
			return -1;
		}
		fclose(ptrFile); // close file

		return 1;
	}

	// read the target file
	pTgtFile = readFile(argv[2], &tgtFileSize);
	if(pTgtFile == NULL) return 1;

	// Set up pointers to various parts of file
	pTgtFileHdr = (BITMAPFILEHEADER *) pTgtFile;
	pTgtInfoHdr = (BITMAPINFOHEADER *) (pTgtFile + sizeof(BITMAPFILEHEADER));
	pTgtColorTable = (RGBQUAD *) (pTgtFile + sizeof(BITMAPFILEHEADER) + pTgtInfoHdr->biSize);
	pTgtData = pTgtFile + pTgtFileHdr->bfOffBits;

	// for debugging purposes, show file info on the screen
	displayFileInfo(argv[2], pTgtFileHdr, pTgtInfoHdr, pTgtColorTable, pTgtData);

	// embed the source file into the target file
	x = embedImageIntoFile(pTgtData, pSrcData, pSrcInfoHdr->biSizeImage);
	if(x == FAILURE) return 1;

	// write the file to disk
	x = writeFile(pTgtFile, pTgtFileHdr->bfSize, argv[2]);

	return 0;
} // main
