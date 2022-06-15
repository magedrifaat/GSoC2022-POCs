#include <stdio.h>
#include <stdint.h>

#include <tiffio.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./write_pages page_count\n");
        return 1;
    }

    int page_count;
    sscanf(argv[1], "%d", &page_count);

    TIFF *tif = TIFFOpen("test_image.tif", "w");
    if (tif)
    {
        for (int i = 0; i < page_count; i++)
        {
            TIFFCreateDirectory(tif);
            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, 1);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH, 1);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, 1);
            TIFFSetField(tif, TIFFTAG_COMPRESSION, 1);

            uint8_t pixel = 255;
            TIFFWriteRawStrip(tif, 0, &pixel, 1);
            TIFFFlush(tif);
        }
        TIFFClose(tif);
    }

    return 0;
}