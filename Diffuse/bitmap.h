#ifndef bitmap_h
#define bitmap_h

#define BI_RGB 0

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct {
    char id[2];
    int filesize;
    int reserved;
    int headersize;
    int infoSize;
    int width;
    int depth;
    short biPlanes;
    short bits;
    int biCompression;
    int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    int biClrUsed;
    int biClrImportant;
} BMPHEAD;

void saveBMP ( char *filename, float *img, int w, int h )
{
    FILE *fptr = fopen ( filename, "wb" );
    //	BMPHEAD bh;
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER info;
    
    memcpy ( &(header.bfType), "BM", 2 );
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = sizeof ( BITMAPFILEHEADER ) + sizeof ( BITMAPINFOHEADER );
    
    info.biSize = sizeof ( BITMAPINFOHEADER );
    info.biWidth = w;
    info.biHeight = h;
    info.biPlanes = 1;
    info.biBitCount = 24;
    info.biCompression = BI_RGB;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;
    
    fwrite ( &header, sizeof ( BITMAPFILEHEADER ), 1, fptr );
    fwrite ( &info, sizeof ( BITMAPINFOHEADER ), 1, fptr );
    
    int bytesPerLine = w * 3;  /* (for 24 bit images) */
    /* round up to a dword boundary */
    if ( bytesPerLine & 0x0003)
    {
        bytesPerLine |= 0x0003;
        ++bytesPerLine;
    }
    
    char *linebuf = (char *) calloc(1, bytesPerLine);
    
    for (int line = 0; line < h; line++ )
    {
        /* fill line linebuf with the image data for that line */
        /* remember that the order is BGR and if width is not a multiple
         of 4 then the last few bytes may be unused
         */
        for ( int i = 0; i < 3 * w; i++ )
        {
            linebuf [ i ] = img [ 3 * line * w + i ] * 255;
        }
        for ( int i = 0; i < 3 * w; i += 3 )
        {
            std::swap( linebuf [ i ], linebuf [ i + 2 ] );
        }
        fwrite ( linebuf, 1, bytesPerLine, fptr );
    }
    delete[] linebuf;
    
    fclose ( fptr );
}

#endif /* bitmap_h */
