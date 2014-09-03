#include <SDL2/SDL_image.h>

int main()
{
    SDL_Surface* img = IMG_Load("./logo.jpg");
    FILE *f = fopen("./image.raw", "w");
    fwrite(img->pixels, img->w*img->h*img->format->BytesPerPixel, 1, f);
    fclose(f);

    return 0;
}
