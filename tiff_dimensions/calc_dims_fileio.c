#include <stdio.h>
#include <stdint.h>

#define WORD   uint16_t
#define DWORD  uint32_t

#define LITTLE_ENDIAN_ORDER 0x4949
#define FORMAT_VERSION      0x2A
#define IMG_WIDTH_TAG_ID    256
#define IMG_HEIGHT_TAG_ID   257

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

    FILE *img = fopen(image_name, "rb");
    if (!img)
    {
        printf("Could not open file\n");
        return 1;
    }
    
    WORD byte_order;
    fread(&byte_order, 1, sizeof(WORD), img);
    // Only support little-endian to avoid complexity
    if (byte_order != LITTLE_ENDIAN_ORDER)
    {
        fprintf(stderr, "Wrong Byte-order Identifier: 0x%X\n", byte_order);
        return 1;
    }

    WORD version;
    fread(&version, 1, sizeof(WORD), img);
    if (version != FORMAT_VERSION)
    {
        fprintf(stderr, "Wrong version word: 0x%X\n", version);
        return 1;
    }

    DWORD first_ifd_offset;
    fread(&first_ifd_offset, 1 ,sizeof(DWORD), img);
    if (first_ifd_offset == 0)
    {
        fprintf(stderr, "No images in file\n");
        return 1;
    }

    DWORD next_ifd_offset = first_ifd_offset;
    WORD page_count = 0;
    while (next_ifd_offset != 0)
    {
        page_count++;
        WORD num_tags;
        fseek(img, next_ifd_offset, SEEK_SET);
        fread(&num_tags, 1, sizeof(WORD), img);
        fseek(img, num_tags * (sizeof(WORD) * 2 + sizeof(DWORD) * 2), SEEK_CUR);
        fread(&next_ifd_offset, 1, sizeof(DWORD), img);
    }
    
    printf("No of Pages = %d\n", page_count);

    next_ifd_offset = first_ifd_offset;
    for (WORD i = 0; i < page_count; i++)
    {
        fseek(img, next_ifd_offset, SEEK_SET);
        
        WORD num_tags;
        fread(&num_tags, 1, sizeof(WORD), img);
        DWORD img_width = 0, img_height = 0;
        for (WORD i = 0; i < num_tags; i++)
        {
            WORD tag_id;
            fread(&tag_id, 1, sizeof(WORD), img);
            if (tag_id == IMG_WIDTH_TAG_ID || tag_id == IMG_HEIGHT_TAG_ID)
            {
                fseek(img, sizeof(WORD) + sizeof(DWORD), SEEK_CUR);
            }
            else
            {
                fseek(img, sizeof(WORD) + 2 * sizeof(DWORD), SEEK_CUR);
            }

            if (tag_id == IMG_WIDTH_TAG_ID)
            {
                fread(&img_width, 1, sizeof(DWORD), img);
            }

            if (tag_id == IMG_HEIGHT_TAG_ID)
            {
                fread(&img_height, 1, sizeof(DWORD), img);
            }
        }

        if (img_width == 0 || img_height == 0)
        {
            fprintf(stderr, "Failed to find width and/or height of Page %d\n", i);
            return 1;
        }

        printf("Dimensions of Page %d: ", i);
        printf("Width = %d, Height = %d\n", img_width, img_height);

        fread(&next_ifd_offset, 1, sizeof(DWORD), img);
    }

    fclose(img);
}