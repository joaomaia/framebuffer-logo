#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define LOGO_W 320
#define LOGO_H 240
#define BYTESPERPIXEL 3

extern char _binary_image_raw_start;
extern char _binary_image_raw_end;

int main()
{
  int fbfd = 0;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;
  unsigned int x = 0, y = 0;
  long int location = 0;
  long int pixel_pos = 0;

  // Open the file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (fbfd == -1) {
      perror("Error: cannot open framebuffer device");
      exit(1);
  }
  printf("The framebuffer device was opened successfully.\n");

  // Get fixed screen information
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
      perror("Error reading fixed information");
      exit(2);
  }

  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
      perror("Error reading variable information");
      exit(3);
  }

  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

  // Figure out the size of the screen in bytes
  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

  // Map the device to memory
  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if ((int)fbp[0] == -1) {
      perror("Error: failed to map framebuffer device to memory");
      exit(4);
  }
  printf("The framebuffer device was mapped to memory successfully.\n");

  char* pixel = &_binary_image_raw_start;

  //Draw in top right corner
  for (y = 0; y < LOGO_H; y++) {
    for (x = vinfo.xres-LOGO_W; x < vinfo.xres; x++) {

      //Location on the framebuffer to write the pixel info
      location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                 (y+vinfo.yoffset) * finfo.line_length;

      //Get the corresponding pixel position
      pixel_pos = ((y*LOGO_W) + (x-(vinfo.xres-LOGO_W))) * BYTESPERPIXEL;

      //If display is in 32bits mode:
      if (vinfo.bits_per_pixel == 32) {
          *(fbp + location + 0) = pixel[pixel_pos + 2]; // Some red
          *(fbp + location + 1) = pixel[pixel_pos + 1]; // Some blue
          *(fbp + location + 2) = pixel[pixel_pos + 0]; // Some green
          *(fbp + location + 3) = 0x00;                 // No alpha data.
      }

      //If display is in 16 bits mode:
      if (vinfo.bits_per_pixel == 16) {
          //NOT tested!
          //Let's assume color info is in RGB565 :).. leap of faith!
          unsigned short pix = 0;
          pix |= (((pixel[pixel_pos + 2]) >> 3) << 11);
          pix |= (((pixel[pixel_pos + 1]) >> 2) << 5);
          pix |= (((pixel[pixel_pos + 0]) >> 3) << 0);
          *(fbp + location + 0) = (char)(pix >> 8);
          *(fbp + location + 1) = (char)(pix & 0xFF);
      }
    }
  }

  munmap(fbp, screensize);
  close(fbfd);

  return 0;
}
