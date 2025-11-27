#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageRGB.h"
#include "instrumentation.h"

extern unsigned long InstrCount[]; // counters from instrumentation.c

static void print_header(void) {
  printf("test,type,imgA,imgB,width,height,pixels,result,time_sec,time_ctu,pixreads,pixwrites,lutreads,lutwrites,pixvalidations,stackops,queueops,peakstack,peakqueue,peakrecdepth\n");
}
static void print_line(const char *test, const char *type, const char *a, const char *b, const Image img, int result) {
  double elapsed_sec = cpu_time() - InstrTime;
  double elapsed_ctu = elapsed_sec / InstrCTU;
  unsigned long pixels = (unsigned long)ImageWidth(img) * (unsigned long)ImageHeight(img);
  printf("%s,%s,%s,%s,%u,%u,%lu,%d,%.6f,%.6f,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
         test, type, a, b,
         (unsigned)ImageWidth(img), (unsigned)ImageHeight(img), pixels, result,
         elapsed_sec, elapsed_ctu,
         InstrCount[0], InstrCount[1], InstrCount[2], InstrCount[3], InstrCount[4], InstrCount[5], InstrCount[6], InstrCount[7], InstrCount[8], InstrCount[9]);
}

static void run_equal_tests(Image base, const char *name) {
  // Self equality (pointer check fast path)
  InstrReset();
  int r = ImageIsEqual(base, base);
  print_line("ImageIsEqual", "self", name, name, base, r);

  // Deep copy equality (forces full scan)
  Image copy = ImageCopy(base);
  InstrReset();
  r = ImageIsEqual(base, copy);
  print_line("ImageIsEqual", "deep_equal", name, "copy", base, r);

  // Rotated version (likely early mismatch)
  Image rot = ImageRotate180CW(base);
  InstrReset();
  r = ImageIsEqual(base, rot);
  print_line("ImageIsEqual", "rotated", name, "rot180", base, r);

  // Different size (immediate mismatch)
  Image diffSize = ImageCreate(ImageWidth(base)+1, ImageHeight(base)+1);
  InstrReset();
  r = ImageIsEqual(base, diffSize);
  print_line("ImageIsEqual", "size_diff", name, "bigger", base, r);

  ImageDestroy(&copy);
  ImageDestroy(&rot);
  ImageDestroy(&diffSize);
}

static void run_fill_tests(Image white, const char *name) {
  uint32 w = ImageWidth(white);
  uint32 h = ImageHeight(white);
  int seed_u = (int)w/2;
  int seed_v = (int)h/2;

  // Recursive fill
  Image img1 = ImageCopy(white);
  InstrReset();
  int painted = ImageRegionFillingRecursive(img1, seed_u, seed_v, BLACK);
  print_line("fill", "recursive", name, "", img1, painted);
  ImageDestroy(&img1);

  // Stack fill
  Image img2 = ImageCopy(white);
  InstrReset();
  painted = ImageRegionFillingWithSTACK(img2, seed_u, seed_v, BLACK);
  print_line("fill", "stack", name, "", img2, painted);
  ImageDestroy(&img2);

  // Queue fill
  Image img3 = ImageCopy(white);
  InstrReset();
  painted = ImageRegionFillingWithQUEUE(img3, seed_u, seed_v, BLACK);
  print_line("fill", "queue", name, "", img3, painted);
  ImageDestroy(&img3);
}

static void run_segmentation_tests(Image img, const char *name) {
  // Using stack and queue variants for segmentation
  Image s1 = ImageCopy(img);
  InstrReset();
  int regions = ImageSegmentation(s1, ImageRegionFillingWithSTACK);
  print_line("segment", "stack", name, "", s1, regions);
  ImageDestroy(&s1);

  Image s2 = ImageCopy(img);
  InstrReset();
  regions = ImageSegmentation(s2, ImageRegionFillingWithQUEUE);
  print_line("segment", "queue", name, "", s2, regions);
  ImageDestroy(&s2);

  // CAUSES SEGMENTATION FAULT ON LARGE IMAGES DUE TO DEEP RECURSION
  // Image s3 = ImageCopy(img);
  // InstrReset();
  // regions = ImageSegmentation(s3, ImageRegionFillingRecursive);
  // print_line("segment", "recursive", name, "", s3, regions);
  // ImageDestroy(&s3);
}

static void run_maze_tests(void) {
  const char *maze_path = "img/maze41x41.pbm";
  Image maze = ImageLoadPBM(maze_path);
  if (maze == NULL) return;
  // Name
  const char *name = "maze41x41";
  // Equality tests
  run_equal_tests(maze, name);
  // Fill tests: seed near entrance (0,1) and center as a second case
  int seed_u1 = 0, seed_v1 = 1;
  int seed_u2 = (int)ImageWidth(maze)/2-1, seed_v2 = (int)ImageHeight(maze)/2;

  // For fills, operate on copies to keep image intact per run
  Image m1 = ImageCopy(maze);
  InstrReset();
  int painted = ImageRegionFillingWithSTACK(m1, seed_u1, seed_v1, BLACK);
  print_line("fill", "stack", name, "seed01", m1, painted);
  ImageDestroy(&m1);

  Image m2 = ImageCopy(maze);
  InstrReset();
  painted = ImageRegionFillingWithSTACK(m2, seed_u2, seed_v2, BLACK);
  print_line("fill", "stack", name, "seedCenter", m2, painted);
  ImageDestroy(&m2);

  Image m3 = ImageCopy(maze);
  InstrReset();
  painted = ImageRegionFillingWithQUEUE(m3, seed_u1, seed_v1, BLACK);
  print_line("fill", "queue", name, "seed01", m3, painted);
  ImageDestroy(&m3);

  Image m4 = ImageCopy(maze);
  InstrReset();
  painted = ImageRegionFillingWithQUEUE(m4, seed_u2, seed_v2, BLACK);
  print_line("fill", "queue", name, "seedCenter", m4, painted);
  ImageDestroy(&m4);

  Image m5 = ImageCopy(maze);
  InstrReset();
  painted = ImageRegionFillingRecursive(m5, seed_u1, seed_v1, BLACK);
  print_line("fill", "recursive", name, "seed01", m5, painted);
  ImageDestroy(&m5);

  Image m6 = ImageCopy(maze);
  InstrReset();
  painted = ImageRegionFillingRecursive(m6, seed_u2, seed_v2, BLACK);
  print_line("fill", "recursive", name, "seedCenter", m6, painted);
  ImageDestroy(&m6);

  // Segmentation using stack and queue variants
  Image s1 = ImageCopy(maze);
  InstrReset();
  int regions = ImageSegmentation(s1, ImageRegionFillingWithSTACK);
  print_line("segment", "stack", name, "", s1, regions);
  ImageDestroy(&s1);

  Image s2 = ImageCopy(maze);
  InstrReset();
  regions = ImageSegmentation(s2, ImageRegionFillingWithQUEUE);
  print_line("segment", "queue", name, "", s2, regions);
  ImageDestroy(&s2);

  Image s3 = ImageCopy(maze);
  InstrReset();
  regions = ImageSegmentation(s3, ImageRegionFillingRecursive);
  print_line("segment", "recursive", name, "", s3, regions);
  ImageDestroy(&s3);
  
  ImageDestroy(&maze);
}
static void run_suite_for_dims(int w, int h) {
  int base = (w < h ? w : h);
  int chess_edge = base/10; if (chess_edge < 1) chess_edge = 1;
  int palete_edge = base/8; if (palete_edge < 1) palete_edge = 1;
  Image chess = ImageCreateChess((uint32)w, (uint32)h, (uint32)chess_edge, 0xff0000);
  Image palete = ImageCreatePalete((uint32)w, (uint32)h, (uint32)palete_edge);
  Image white = ImageCreate((uint32)w, (uint32)h);
  char name_chess[40];
  char name_palete[40];
  char name_white[40];
  snprintf(name_chess, sizeof(name_chess), "chess%dx%d", w, h);
  snprintf(name_palete, sizeof(name_palete), "palete%dx%d", w, h);
  snprintf(name_white, sizeof(name_white), "white%dx%d", w, h);
  run_equal_tests(chess, name_chess);
  run_equal_tests(palete, name_palete);
  run_fill_tests(white, name_white);
  run_segmentation_tests(chess, name_chess);
  run_segmentation_tests(white, name_white);
  ImageDestroy(&chess);
  ImageDestroy(&palete);
  ImageDestroy(&white);
}

int main(int argc, char **argv) {
  ImageInit();
  print_header();
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      char *arg = argv[i];
      int w=0,h=0;
      if (strchr(arg,'x') || strchr(arg,'X')) {
        char *x = strchr(arg,'x'); if (!x) x = strchr(arg,'X');
        w = atoi(arg);
        h = atoi(x+1);
      } else {
        w = h = atoi(arg);
      }
      if (w <= 0 || h <= 0) continue;
      run_suite_for_dims(w,h);
    }
  } else {
    int squares[] = {32,64,100,128,150,256};
    int rects[][2] = {{64,128},{80,160},{96,192},{128,256},{192,256},{256,512}};
    int nsq = sizeof(squares)/sizeof(squares[0]);
    int nrect = sizeof(rects)/sizeof(rects[0]);
    for (int i=0;i<nsq;++i) run_suite_for_dims(squares[i], squares[i]);
    for (int i=0;i<nrect;++i) run_suite_for_dims(rects[i][0], rects[i][1]);
  }
  // Always attempt maze tests once per run
  run_maze_tests();
  return 0;
}
