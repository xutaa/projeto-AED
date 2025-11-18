// imageRGBTest - A program that performs some operations on RGB images.
//
// This program is an example use of the imageRGB module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
// 2025

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "imageRGB.h"
#include "instrumentation.h"

static void check(int condition, const char* failmsg) {
  if (!condition) {
    printf("%s\n", failmsg);
  }
}

int main(int argc, char* argv[]) {
  program_name = argv[0];
  if (argc != 1) {
    error(1, 0, "Usage: imageRGBTest");
  }

  ImageInit();

  // Image* image;

  // Creating and displaying some images

  printf("1) ImageCreate\n");
  Image white_image = ImageCreate(100, 100);
  check(white_image != NULL, "1) ERROR");
  // ImageRAWPrint(white_image);

  printf("2) ImageCreateChess(black) + ImageSavePBM\n");
  Image image_chess_1 = ImageCreateChess(150, 120, 30, 0x000000);  // black
  check(image_chess_1 != NULL, "2) ERROR");
  // ImageRAWPrint(image_chess_1);
  ImageSavePBM(image_chess_1, "chess_image_1.pbm");

  printf("3) ImageCreateChess(red) + ImageSavePPM\n");
  Image image_chess_2 = ImageCreateChess(20, 20, 8, 0xff0000);  // red
  check(image_chess_2 != NULL, "3) ERROR");
  // ImageRAWPrint(image_chess_2);
  ImageSavePPM(image_chess_2, "chess_image_2.ppm");

  printf("4) ImageCreateChess(all black)\n");
  Image black_image = ImageCreateChess(100, 100, 100, 0x000000);  // all black
  check(black_image != NULL, "4) ERROR");
  // ImageRAWPrint(black_image);
  ImageSavePBM(black_image, "black_image.pbm");

  printf("5) ImageCopy\n");
  Image copy_image_1 = ImageCopy(image_chess_1);
  check(copy_image_1 != NULL, "5.1) ERROR");
  if (copy_image_1 != NULL) {
    ImageSavePBM(copy_image_1, "copy_image_1.pbm");
  }
  Image copy_image_2 = ImageCopy(image_chess_2);
  check(copy_image_2 != NULL, "5.2) ERROR");
  if (copy_image_2 != NULL) {
    ImageSavePPM(copy_image_2, "copy_image_2.ppm");
  }

  printf("6) ImageLoadPBM\n");
  Image image_1 = ImageLoadPBM("img/feep.pbm");
  check(image_1 != NULL, "6) ERROR");
  // ImageRAWPrint(image_1);

  printf("7) ImageLoadPPM\n");
  Image image_2 = ImageLoadPPM("img/feep.ppm");
  check(image_2 != NULL, "7) ERROR");
  // ImageRAWPrint(image_2);

  printf("8) ImageCreatePalete\n");
  Image image_3 = ImageCreatePalete(4 * 32, 4 * 32, 4);
  check(image_3 != NULL, "8) ERROR");
  ImageSavePPM(image_3, "palete.ppm");

  printf("8.5) ImageLoadPPM\n");
  Image load_palete = ImageLoadPPM("palete.ppm");
  check(load_palete != NULL, "8.50) ERROR");
  ImageRAWPrint(load_palete);

  printf("9) ImageRotate90CW\n");
  Image image_3_90CW = ImageRotate90CW(image_3);
  check(image_3_90CW != NULL, "9) ERROR");
  ImageSavePPM(image_3_90CW, "palete_rotated90CW.ppm");

  printf("10) ImageRotate180CW\n");
  Image image_3_180CW = ImageRotate180CW(image_3);
  check(image_3_180CW != NULL, "10) ERROR");
  ImageSavePPM(image_3_180CW, "palete_rotated180CW.ppm");

  printf("11) ImageRotate90CW twice\n");
  Image image_3_90CW_2 = ImageRotate90CW(image_3_90CW);
  check(image_3_90CW_2 != NULL, "11) ERROR");
  ImageSavePPM(image_3_90CW_2, "palete_rotated90CW_2.ppm");

  printf("101) Test ImageIsEqual\n");
  check(ImageIsEqual(image_chess_1, copy_image_1), "101.1) image_chess_1, copy_image_1 are not equal");
  check(ImageIsEqual(image_chess_2, copy_image_2), "101.2) image_chess_2, copy_image_2 are not equal");
  check(ImageIsEqual(image_chess_1, image_chess_1), "101.3) image_chess_1, image_chess_1 are not equal");
  check(ImageIsEqual(image_3_180CW, image_3_90CW_2), "101.4) image_3_180CW, image_3_90CW_2 are not equal");
  
  printf("102) Test ImageIsDifferent\n");
  check(ImageIsDifferent(image_chess_1, black_image), "101.3) image_chess_1, black_image are not different");

  // ImageDestroyAll()

  ImageDestroy(&white_image);
  ImageDestroy(&black_image);
  if (copy_image_1 != NULL) {
    ImageDestroy(&copy_image_1);
  }
  if (copy_image_2 != NULL) {
    ImageDestroy(&copy_image_2);
  }
  ImageDestroy(&image_chess_1);
  ImageDestroy(&image_chess_2);
  ImageDestroy(&image_1);
  ImageDestroy(&image_2);
  ImageDestroy(&image_3);
  ImageDestroy(&image_3_90CW);
  ImageDestroy(&image_3_180CW);
  ImageDestroy(&image_3_90CW_2);


  return 0;
}
