#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imgtool.h>

#define MAX_SUM 765
#define PALETTE_COUNT 15
#define LATITUDE_SIZE (MAX_SUM / PALETTE_COUNT)

static const uint8_t elzs_palette[][4] = {
    {0, 0,  0,  255},       // Black
    {105, 55, 105, 255},    // Purple
    {25, 25, 255, 255},     // Dark Blue
    {105, 105, 255, 255},   // Light Blue
    {0, 155, 0, 255},       // Dark Green
    {0, 255, 0, 255},       // Green
    {105, 255, 105, 255},   // Light Green
    {130, 130, 130, 255},   // Grey
    {155, 175, 105, 255},   // Brown
    {255, 255, 55, 255},    // Yellow
    {255, 205, 55, 255},    // Light Orange
    {255, 165, 0, 255},     // Orange
    {255, 55, 55, 255},     // Light Red
    {255, 0, 0, 255},       // Red
    {255, 255, 255, 255}    // White
};
    
static bmp_t bmp_palette(const unsigned w, const unsigned h)
{
    bmp_t bmp = bmp_new(w * PALETTE_COUNT, h, 4);
    for (unsigned i = 0; i < PALETTE_COUNT; ++i) {
        for (unsigned y = 0; y < h; ++y) {
            for (unsigned x = 0; x < w; ++x) {
                uint8_t* px = px_at(&bmp, x + i * w, y);
                memcpy(px, elzs_palette + i, 4);
            }
        }
    }
    return bmp;
}

static bmp_t bmp_elzs(const bmp_t* restrict in)
{
    bmp_t bmp = bmp_copy(in);

    const unsigned w = bmp.width, h = bmp.height, ch = bmp.channels;
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            uint8_t *px = px_at(&bmp, x, y);

            unsigned c = (unsigned)px[0] + (unsigned)px[1] + (unsigned)px[2];
            c = c * (c <= MAX_SUM - 1) + (MAX_SUM - 1) * (c > MAX_SUM - 1);
            c /= LATITUDE_SIZE;
            
            memcpy(px, elzs_palette + c, ch);
        }
    }

    return bmp;
}

static void elzs_palette_print(void)
{
    bmp_t bmp = bmp_palette(32, 32);
    bmp_write("palette.png", &bmp);
    bmp_free(&bmp);
}

static void elzs_help(void)
{
    printf("elzs usage:\n");
    printf("-help, -h\t:Help and usage information.\n");
    printf("-version, -v\t:Software version information.\n");
    printf("-palette, -p\t:Print the Exposure Latitude Zone System color palette.\n");
    printf("<arg>\t\t:Input image file to export with the color latitude system.\n");
}

static void elzs_version(void)
{
    printf("elzs\tExposure Latitude Zone System\tversion 0.1.0.\n");
}

static int elzs_gen(const char* path)
{
    bmp_t bmp = bmp_load(path);
    if (!bmp.pixels) {
        return EXIT_FAILURE;
    }

    bmp_t out = bmp_elzs(&bmp);
    bmp_write("elzs.png", &out);

    bmp_free(&bmp);
    bmp_free(&out);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Missing input image. Use -help for more information.\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-help") || !strcmp(argv[i], "-h")) {
            elzs_help();
            return EXIT_SUCCESS;
        }
        else if (!strcmp(argv[i], "-version") || !strcmp(argv[i], "-v")) {
            elzs_version();
            return EXIT_SUCCESS;
        }
        else if (!strcmp(argv[i], "-palette") || !strcmp(argv[i], "-p")) {
            elzs_palette_print();
        }
        else if (elzs_gen(argv[i])) {
            printf("Could not open file '%s'.\n", argv[i]);
        }
    }

    return EXIT_SUCCESS;
}
