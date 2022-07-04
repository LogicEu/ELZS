#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <imgtool.h>


/***********************
 *        elzs         *
 * @Eugenio Arteaga A. *
 **********************/


// logarithmic image algorithms //

#define OUTPUT_MAX      255.0
#define INPUT_MAX       255.0
#define LOG_DISABLE     0
#define LOG_ENABLE      1
#define LOG(f)          (log(f))
#define LOG_SCALE()     (OUTPUT_MAX / LOG(INPUT_MAX + 1.0))

static int log_op = LOG_DISABLE;

static inline double log_char(const double c, const uint8_t n)
{
    return c * LOG((double)(int)n + 1.0);
}

static inline void log_px(uint8_t* px, const double c)
{
    px[0] = (uint8_t)(int)log_char(c, px[0]);
    px[1] = (uint8_t)(int)log_char(c, px[1]);
    px[2] = (uint8_t)(int)log_char(c, px[2]);
}


// exposure latitude zone system color palette //

#define PALETTE_COUNT   15
#define MAX_SUM         765 // 255 * 3 color channels
#define LATITUDE_SIZE   (MAX_SUM / PALETTE_COUNT)

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

static bmp_t bmp_log(const bmp_t* restrict in)
{
    bmp_t bmp = bmp_copy(in);
    
    const double c = LOG_SCALE();

    const unsigned w = bmp.width, h = bmp.height;
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            uint8_t *px = px_at(&bmp, x, y);
            log_px(px, c);
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

            unsigned n = (unsigned)px[0] + (unsigned)px[1] + (unsigned)px[2];
            n = n * (n <= MAX_SUM - 1) + (MAX_SUM - 1) * (n > MAX_SUM - 1);
            n /= LATITUDE_SIZE;

            memcpy(px, elzs_palette + n, ch);
        }
    }

    return bmp;
}


// main elzs interface //

static const char* filextensions[] = {"-elzs", "-log"};

static inline char* strdot(const char* str)
{
    char* dot = NULL;
    for (int i = 0; str[i]; ++i) {
        if (str[i] == '.') {
            dot = (char*)(size_t)(str + i);
        }
    }
    return dot;
}

static inline void elzs_palette_print(void)
{
    bmp_t bmp = bmp_palette(32, 32);
    bmp_write("palette.png", &bmp);
    bmp_free(&bmp);
}

static inline void elzs_help(void)
{
    printf("elzs usage:\n");
    printf("-help, -h\t:Help and usage information.\n");
    printf("-version, -v\t:Software version information.\n");
    printf("-palette, -p\t:Print the Exposure Latitude Zone System color palette.\n");
    printf("-log\t\t:Uncompress image with logarithmic algorithm.\n");
    printf("<arg>\t\t:Input image file to export with the color latitude system.\n");
}

static inline void elzs_version(void)
{
    printf("elzs\tExposure Latitude Zone System\tversion 0.1.0.\n");
}

static inline int elzs_gen(const char* path)
{
    char* dot = strdot(path);
    if (!dot) {
        printf("elzs needs a .png, .jpg, .ppm, .gif or similar file extension.\n");
        printf("elzs error: '%s'\n", path);
        return EXIT_FAILURE;
    }

    if (dot == path) {
        printf("elzs needs a valid image file name.\n");
        printf("elzs error: '%s'\n", path);
        return EXIT_FAILURE;
    }

    bmp_t bmp = bmp_load(path);
    if (!bmp.pixels) {
        printf("elzs could not open image file.\n");
        printf("elzs error: '%s'\n", path);
        return EXIT_FAILURE;
    }

    char output_name[128];
    memset(output_name, 0, 128);
    memcpy(output_name, path, dot - path);

    strcat(output_name, filextensions[log_op]);
    strcat(output_name, dot);

    bmp_t out = !log_op ? bmp_elzs(&bmp) : bmp_log(&bmp);
    bmp_write(output_name, &out);
    
    printf("elzs succesfully writed file '%s'.\n", output_name);

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

    int input = 0;

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
        else if (!strcmp(argv[i], "-log")) {
            log_op = LOG_ENABLE;
        }
        else {
            ++input;
            if (elzs_gen(argv[i])) {
                printf("elzs could not open file '%s'.\n", argv[i]);
            }
        }
    }

    if (!input) {
        printf("Missing input image. Use -help for more information.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
