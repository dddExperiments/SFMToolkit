/*
 * File:	exif.h
 * Purpose:	cpp EXIF reader
 * 16/Mar/2003 <ing.davide.pizzolato@libero.it>
 * based on jhead-1.8 by Matthias Wandel <mwandel(at)rim(dot)net>
 * http://www.codeproject.com/KB/graphics/cexif.aspx
 */

#pragma once

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#define MAX_COMMENT 1000
#define MAX_SECTIONS 20

typedef struct tag_ExifInfo {
	char  Version      [5];
    char  CameraMake   [32];
    char  CameraModel  [40];
    char  DateTime     [20];
    int   Height, Width;
    int   Orientation;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ISOequivalent;
    int   CompressionLevel;
	float FocalplaneXRes;
	float FocalplaneYRes;
	float FocalplaneUnits;
	float Xresolution;
	float Yresolution;
	float ResolutionUnit;
	float Brightness;
    char  Comments[MAX_COMMENT];

    unsigned char * ThumbnailPointer;  /* Pointer at the thumbnail */
    unsigned ThumbnailSize;     /* Size of thumbnail. */

	bool  IsExif;
} EXIFINFO;

//--------------------------------------------------------------------------
// JPEG markers consist of one or more 0xFF unsigned chars, followed by a marker
// code unsigned char (which is not an FF).  Here are the marker codes of interest
// in this program.  (See jdmarker.c for a more complete list.)
//--------------------------------------------------------------------------

#define M_SOF0  0xC0            // Start Of Frame N
#define M_SOF1  0xC1            // N indicates which compression process
#define M_SOF2  0xC2            // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5            // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            // Start Of Image (beginning of datastream)
#define M_EOI   0xD9            // End Of Image (end of datastream)
#define M_SOS   0xDA            // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0            // Jfif marker
#define M_EXIF  0xE1            // Exif marker
#define M_COM   0xFE            // Comment 

class Cexif
{
typedef struct tag_Section_t
{
    unsigned char*    Data;
    int      Type;
    unsigned Size;
} Section_t;

public:
	EXIFINFO* m_exifinfo;
	char m_szLastError[256];
	Cexif(EXIFINFO* info = NULL);
	~Cexif();
	bool DecodeExif(FILE* hFile);

protected:
	bool process_EXIF(unsigned char * CharBuf, unsigned int length);
	void process_COM (const unsigned char * Data, int length);
	void process_SOFn (const unsigned char * Data, int marker);
	int Get16u(void * Short);
	int Get16m(void * Short);
	long Get32s(void * Long);
	unsigned long Get32u(void * Long);
	double ConvertAnyFormat(void * ValuePtr, int Format);
	bool ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength,
                           EXIFINFO * const pInfo, unsigned char ** const LastExifRefdP, int NestingLevel=0);
	int ExifImageWidth;
	int MotorolaOrder;
	Section_t Sections[MAX_SECTIONS];
	int SectionsRead;
	bool freeinfo;
};