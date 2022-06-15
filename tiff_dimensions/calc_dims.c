#include <stdio.h>
#include <stdint.h>
#include <tiffio.h>

int main(int argc, char **argv)
{
    char *image_name;
    if (argc == 2)
    {
        image_name = argv[1];
        printf("Checking dimensions of %s\n", image_name);
    }
    else
    {
        printf("Usage: ./calc_dims IMAGE_NAME\n");
        return 1;
    }

    TIFF* tif = TIFFOpen(image_name, "r");
    if (tif)
    {
        int page_count = 0;
        do {
            page_count++;
        } while (TIFFReadDirectory(tif));

        printf("No of Pages = %d\n", page_count);

        for (int i = 0; i < page_count; i++)
        {
            TIFFSetDirectory(tif, i);
            uint32_t width, height;
            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
            
            printf("Dimensions of Page %d: Width = %d, Height = %d\n",
                     i, width, height);
        }

        TIFFClose(tif);
    }

    return 0;
}