#include"CImg.h"
/*
TO COMPILE:
g++ -o example.o example.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
*/

/*
To load JPEG images, make sure the ImageMagick software is installed 
*/

using namespace cimg_library;
int main(){
  CImgDisplay disp;
    for(int i=1;i<=500;i++){
      char name[1000];
     
      sprintf(name,"vid/image%03d.jpg",i);
      CImg<unsigned char>img(name);
      disp=img;
      usleep(30000);
    }
}
