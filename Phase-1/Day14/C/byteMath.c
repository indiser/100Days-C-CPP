#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
#include<string.h>
#include<stdint.h>
#include<assert.h>

#pragma pack(1)
typedef struct //54-bytes
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffbits;

    uint32_t biSize;
    int32_t biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;

} BMPHeader;


// Opening the BMP file
void openBMP(char *filePath, BMPHeader *header, unsigned char **pixels)
{
    FILE *file = fopen(filePath, "rb");

    if(!file)
    {
        perror("Error opening File");
        return;
    }

    fread(header, sizeof(BMPHeader), 1, file);

    *pixels = malloc(header->biWidth * abs(header->biHeight) * 3);

    if(!pixels) return -1;


    fseek(file, header->bfOffbits, SEEK_SET);

    int padding = (4 - (header->biWidth * 3) % 4) % 4;
    for (size_t i = 0; i < abs(header->biHeight); i++)
    {
        fread(*pixels + (i * header->biWidth * 3), 3, header->biWidth, file);
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
}


// Verifying the BMP file
bool verifyBMP(BMPHeader header)
{
    if(header.bfType == 0x4D42) return true;
    return false;
}

void isBMP(bool a)
{
    if(a)
        printf("Is BMP\n");
    else
        printf("Not BMP\n");
}


// Manupulate BMP
void invertBMP(BMPHeader header, unsigned char *pixels)
{
    size_t totalBytes = header.biWidth * abs(header.biHeight) * 3;
    for (size_t i = 0; i < totalBytes; i++)
    {
        pixels[i] = 255 - pixels[i];
    }
}

void writeBMP(char *filePath, BMPHeader *header, unsigned char *pixels)
{
    FILE *file = fopen(filePath, "wb");

    if(!file)
    {
        perror("Error creating File");
        return;
    }

    fwrite(header, sizeof(BMPHeader), 1, file);

    int padding = (4 - (header->biWidth * 3) % 4) % 4;
    unsigned char padBytes[3] = {0, 0, 0};

    for (size_t i = 0; i < abs(header->biHeight); i++)
    {
        fwrite(pixels + (i * header->biWidth * 3), 3, header->biWidth, file);
        if (padding > 0)
        {
            fwrite(padBytes, 1, padding, file);
        }
    }

    fclose(file);
}

int main()
{
    BMPHeader header;
    unsigned char *pixels = NULL;

    openBMP("cute_cat.bmp", &header, &pixels);

    isBMP(verifyBMP(header));

    printf("Width: %u\n", header.biWidth);
    printf("Height: %u\n", header.biHeight);
    printf("Bits per pixel: %u\n", header.biBitCount);

    assert(header.biBitCount == 24);

    printf("Row padding: %d\n", (4 - (header.biWidth * 3) % 4) % 4);

    invertBMP(header, pixels);
    writeBMP("inverted_cute_cat.bmp", &header, pixels);

    free(pixels);
    

    return 0;
}