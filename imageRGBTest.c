// imageRGBTest - A program that performs a comprehensive set of operations on RGB images,
//                serving as a robust test suite for the imageRGB module.
//
// Este código foi modificado para incluir contadores de testes e formatação de cor (ANSI).
//
// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
// 2025

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assumindo que estes ficheiros existem no ambiente de compilação
#include "error.h"
#include "imageRGB.h"
#include "instrumentation.h"

// --- Definições de Cores ANSI ---
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// --- Contadores Globais para o Resumo Final ---
int global_passed_count = 0;
int global_total_count = 0;

// --- Macro para Verificação de Testes, Contagem e Coloração ---
// Recebe ponteiros para os contadores locais de passados e total
#define ASSERT_CHECK(condition, test_name, passed_count_ptr, total_count_ptr) \
    do { \
        int __condition = (condition); \
        (*total_count_ptr)++; \
        global_total_count++; \
        if (!__condition) { \
            fprintf(stderr, "  " ANSI_COLOR_RED "[FAIL]" ANSI_COLOR_RESET " Teste %s falhou!\n", (test_name)); \
        } else { \
            (*passed_count_ptr)++; \
            global_passed_count++; \
            printf("  " ANSI_COLOR_GREEN "[PASS]" ANSI_COLOR_RESET " Teste %s passou.\n", (test_name)); \
        } \
    } while (0)

// Função auxiliar para verificar condições (mantida, mas o ASSERT_CHECK é preferencial para testes)
static void check(int condition, const char* failmsg) {
    if (!condition) {
        // Usar fprintf(stderr, ...) é mais robusto para mensagens de erro
        fprintf(stderr, "Check Failed: %s\n", failmsg);
    }
}

// Protótipos para organização do código de teste
static void TestImageCreationAndManagement(int section_num);
static void TestImageFileIO(int section_num);
static void TestImageInformationAndComparison(int section_num);
static void TestImageGeometricTransformations(int section_num);
static void TestImageRegionFillingAndSegmentation(int section_num);
static void TestImageIsValidPixelFunction(int section_num);

int main(int argc, char* argv[]) {
    program_name = argv[0];
    if (argc != 1) {
        error(1, 0, "Usage: imageRGBTest");
    }

    ImageInit();
    printf(ANSI_COLOR_YELLOW "--- Starting Comprehensive imageRGB Test Suite ---\n" ANSI_COLOR_RESET);

    TestImageCreationAndManagement(1);
    TestImageFileIO(2);
    TestImageInformationAndComparison(3);
    TestImageGeometricTransformations(4);
    TestImageRegionFillingAndSegmentation(5);
    TestImageIsValidPixelFunction(6);

    // --- Resumo Global ---
    printf("\n" ANSI_COLOR_YELLOW "--- Test Suite Finished ---\n" ANSI_COLOR_RESET);
    
    // O resumo final usa a contagem global
    printf(ANSI_COLOR_YELLOW "(%d/%d) %d out of %d tests passed.\n" ANSI_COLOR_RESET,
           global_passed_count, global_total_count, global_passed_count, global_total_count);
           
    printf("Instrumentation Count (Pixel Memory Accesses): %llu\n", InstrCount[0]);
    
    return 0;
}

// --- Seção 1: Testes de Criação e Gestão de Imagem ---
static void TestImageCreationAndManagement(int section_num) {
    printf("\n## %d. Image Creation and Management Tests\n", section_num);

    int local_passed_count = 0;
    int local_total_count = 0;

    Image white_image = NULL;
    Image image_chess_black = NULL;
    Image image_chess_red = NULL;
    Image copy_chess_black = NULL;
    Image copy_chess_red = NULL;
    Image image_palete = NULL;

    // 1.1 - ImageCreate (White image)
    printf("1.1: ImageCreate (100x100)\n");
    white_image = ImageCreate(100, 100);
    int check1_1 = (white_image != NULL && ImageWidth(white_image) == 100 && ImageHeight(white_image) == 100 && ImageColors(white_image) == 2);
    ASSERT_CHECK(check1_1, "ImageCreate", &local_passed_count, &local_total_count);

    // 1.2 - ImageCreateChess (Black)
    printf("1.2: ImageCreateChess (150x120, edge 10, Black)\n");
    image_chess_black = ImageCreateChess(150, 120, 10, 0x000000);
    int check1_2 = (image_chess_black != NULL && ImageWidth(image_chess_black) == 150 && ImageHeight(image_chess_black) == 120 && ImageColors(image_chess_black) == 2);
    ASSERT_CHECK(check1_2, "ImageCreateChess_Black", &local_passed_count, &local_total_count);
    
    // 1.3 - ImageCreateChess (Red)
    printf("1.3: ImageCreateChess (20x20, edge 8, Red)\n");
    image_chess_red = ImageCreateChess(20, 20, 8, 0xff0000); // Red
    int check1_3 = (image_chess_red != NULL && ImageWidth(image_chess_red) == 20 && ImageHeight(image_chess_red) == 20 && ImageColors(image_chess_red) == 3);
    ASSERT_CHECK(check1_3, "ImageCreateChess_Red", &local_passed_count, &local_total_count);

    // 1.4 - ImageCopy (Deep copy)
    printf("1.4: ImageCopy (chess_black)\n");
    copy_chess_black = ImageCopy(image_chess_black);
    int check1_4 = (copy_chess_black != NULL && ImageIsEqual(image_chess_black, copy_chess_black));
    ASSERT_CHECK(check1_4, "ImageCopy_Black", &local_passed_count, &local_total_count);
    
    printf("1.5: ImageCopy (chess_red)\n");
    copy_chess_red = ImageCopy(image_chess_red);
    int check1_5 = (copy_chess_red != NULL && ImageIsEqual(image_chess_red, copy_chess_red));
    ASSERT_CHECK(check1_5, "ImageCopy_Red", &local_passed_count, &local_total_count);

    // 1.6 - ImageCreatePalete
    printf("1.6: ImageCreatePalete (32x32, edge 8)\n");
    image_palete = ImageCreatePalete(32, 32, 8);
    // 32/8 = 4 tiles wide, 4 tiles high. 16 cores (ou FIXED_LUT_SIZE se for menor)
    int check1_6 = (image_palete != NULL && ImageWidth(image_palete) == 32 && ImageHeight(image_palete) == 32 && ImageColors(image_palete) == 1000);
    ASSERT_CHECK(check1_6, "ImageCreatePalete_Small", &local_passed_count, &local_total_count);
    
    // 1.7 - ImageDestroy
    printf("1.7: ImageDestroy\n");
    ImageDestroy(&white_image);
    ImageDestroy(&image_chess_black);
    ImageDestroy(&image_chess_red);
    ImageDestroy(&copy_chess_black);
    ImageDestroy(&copy_chess_red);
    ImageDestroy(&image_palete);
    // Verifica implicitamente se os ponteiros foram definidos para NULL
    int check1_7 = (white_image == NULL && image_chess_black == NULL && image_chess_red == NULL && copy_chess_black == NULL && copy_chess_red == NULL && image_palete == NULL);
    ASSERT_CHECK(check1_7, "ImageDestroy_Cleanup", &local_passed_count, &local_total_count);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}

// --- Seção 2: Testes de Leitura/Escrita de Ficheiros ---
static void TestImageFileIO(int section_num) {
    printf("\n## %d. Image File I/O Tests\n", section_num);
    
    int local_passed_count = 0;
    int local_total_count = 0;

    Image image_chess_black = ImageCreateChess(150, 120, 10, 0x000000);
    Image image_chess_red = ImageCreateChess(20, 20, 8, 0xff0000);
    Image image_palete = ImageCreatePalete(32, 32, 8);
    
    Image loaded_pbm = NULL;
    Image loaded_ppm_chess = NULL;
    Image loaded_ppm_palete = NULL;
    Image image_pbm_load = NULL;
    Image image_ppm_load = NULL;

    // 2.1 - ImageSavePBM
    printf("2.1: ImageSavePBM (chess_black -> test_chess_black.pbm)\n");
    ASSERT_CHECK(ImageSavePBM(image_chess_black, "test_chess_black.pbm") == 1, "ImageSavePBM_Success", &local_passed_count, &local_total_count);

    // 2.2 - ImageLoadPBM
    printf("2.2: ImageLoadPBM (test_chess_black.pbm)\n");
    loaded_pbm = ImageLoadPBM("test_chess_black.pbm");
    int check2_2 = (loaded_pbm != NULL && ImageIsEqual(image_chess_black, loaded_pbm));
    ASSERT_CHECK(check2_2, "ImageLoadPBM_Verify", &local_passed_count, &local_total_count);

    // 2.3 - ImageSavePPM
    printf("2.3: ImageSavePPM (chess_red -> test_chess_red.ppm)\n");
    ASSERT_CHECK(ImageSavePPM(image_chess_red, "test_chess_red.ppm") == 1, "ImageSavePPM_Chess_Success", &local_passed_count, &local_total_count);
    
    printf("2.4: ImageSavePPM (palete -> test_palete.ppm)\n");
    ASSERT_CHECK(ImageSavePPM(image_palete, "test_palete.ppm") == 1, "ImageSavePPM_Palete_Success", &local_passed_count, &local_total_count);

    // 2.5 - ImageLoadPPM
    printf("2.5: ImageLoadPPM (test_chess_red.ppm)\n");
    loaded_ppm_chess = ImageLoadPPM("test_chess_red.ppm");
    int check2_5 = (loaded_ppm_chess != NULL && ImageIsEqual(image_chess_red, loaded_ppm_chess));
    ASSERT_CHECK(check2_5, "ImageLoadPPM_Chess_Verify", &local_passed_count, &local_total_count);
    
    printf("2.6: ImageLoadPPM (test_palete.ppm)\n");
    loaded_ppm_palete = ImageLoadPPM("test_palete.ppm");
    int check2_6 = (loaded_ppm_palete != NULL && ImageIsEqual(image_palete, loaded_ppm_palete));
    ASSERT_CHECK(check2_6, "ImageLoadPPM_Palete_Verify", &local_passed_count, &local_total_count);

    // 2.7 - Teste com ficheiros externos (assumindo img/feep.pbm e img/feep.ppm existem)
    printf("2.7: ImageLoadPBM (img/feep.pbm)\n");
    image_pbm_load = ImageLoadPBM("img/feep.pbm");
    ASSERT_CHECK(image_pbm_load != NULL, "ImageLoadPBM_External", &local_passed_count, &local_total_count);

    printf("2.8: ImageLoadPPM (img/feep.ppm)\n");
    image_ppm_load = ImageLoadPPM("img/feep.ppm");
    ASSERT_CHECK(image_ppm_load != NULL, "ImageLoadPPM_External", &local_passed_count, &local_total_count);

    // Cleanup
    ImageDestroy(&image_chess_black);
    ImageDestroy(&image_chess_red);
    ImageDestroy(&image_palete);
    ImageDestroy(&loaded_pbm);
    ImageDestroy(&loaded_ppm_chess);
    ImageDestroy(&loaded_ppm_palete);
    ImageDestroy(&image_pbm_load);
    ImageDestroy(&image_ppm_load);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}

// --- Seção 3: Testes de Informação e Comparação ---
static void TestImageInformationAndComparison(int section_num) {
    printf("\n## %d. Image Information and Comparison Tests\n", section_num);

    int local_passed_count = 0;
    int local_total_count = 0;

    Image img_A = ImageCreateChess(50, 40, 5, 0x123456); // 50x40, 3 cores
    Image img_B = ImageCopy(img_A);                      
    Image img_C = ImageCreateChess(50, 40, 5, 0x123456); 
    Image img_D = ImageCreateChess(50, 41, 5, 0x123456); // diferente altura
    Image img_E = ImageCreateChess(50, 40, 5, 0xFF0000); // cor diferente

    // 3.1 - ImageWidth, ImageHeight, ImageColors
    printf("3.1: ImageWidth, ImageHeight, ImageColors\n");
    ASSERT_CHECK(ImageWidth(img_A) == 50, "ImageWidth_A", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageHeight(img_A) == 40, "ImageHeight_A", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageColors(img_A) == 3, "ImageColors_A", &local_passed_count, &local_total_count);

    // 3.2 - ImageIsEqual
    printf("3.2: ImageIsEqual\n");
    ASSERT_CHECK(ImageIsEqual(img_A, img_B), "ImageIsEqual_A_B (Copy)", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageIsEqual(img_A, img_C), "ImageIsEqual_A_C (Same Pixels, Different Object)", &local_passed_count, &local_total_count);
    ASSERT_CHECK(!ImageIsEqual(img_A, img_D), "ImageIsEqual_A_D (Different Height)", &local_passed_count, &local_total_count);
    ASSERT_CHECK(!ImageIsEqual(img_A, img_E), "ImageIsEqual_A_E (Different Colors)", &local_passed_count, &local_total_count);

    // 3.3 - ImageIsDifferent
    printf("3.3: ImageIsDifferent\n");
    ASSERT_CHECK(ImageIsDifferent(img_A, img_D), "ImageIsDifferent_A_D", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageIsDifferent(img_A, img_E), "ImageIsDifferent_A_E", &local_passed_count, &local_total_count);
    ASSERT_CHECK(!ImageIsDifferent(img_A, img_B), "ImageIsDifferent_A_B (Should be Equal)", &local_passed_count, &local_total_count);

    // Cleanup
    ImageDestroy(&img_A);
    ImageDestroy(&img_B);
    ImageDestroy(&img_C);
    ImageDestroy(&img_D);
    ImageDestroy(&img_E);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}

// --- Seção 4: Testes de Transformações Geométricas ---
static void TestImageGeometricTransformations(int section_num) {
    printf("\n## %d. Image Geometric Transformations Tests\n", section_num);

    int local_passed_count = 0;
    int local_total_count = 0;

    Image img_original = ImageCreatePalete(100, 50, 10); // 100x50
    Image img_90CW = NULL;
    Image img_180CW = NULL;
    Image img_90CW_twice = NULL;

    // 4.1 - ImageRotate90CW
    printf("4.1: ImageRotate90CW\n");
    img_90CW = ImageRotate90CW(img_original); // Deve ser 50x100
    ImageSavePPM(img_90CW, "test_rotated_90CW.ppm");
    int check4_1 = (img_90CW != NULL && ImageWidth(img_90CW) == 50 && ImageHeight(img_90CW) == 100);
    ASSERT_CHECK(check4_1, "ImageRotate90CW_Dimensions", &local_passed_count, &local_total_count);

    // 4.2 - ImageRotate180CW
    printf("4.2: ImageRotate180CW\n");
    img_180CW = ImageRotate180CW(img_original); // Deve ser 100x50
    ImageSavePPM(img_180CW, "test_rotated_180CW.ppm");
    int check4_2 = (img_180CW != NULL && ImageWidth(img_180CW) == 100 && ImageHeight(img_180CW) == 50);
    ASSERT_CHECK(check4_2, "ImageRotate180CW_Dimensions", &local_passed_count, &local_total_count);

    // 4.3 - ImageRotate90CW twice vs ImageRotate180CW
    printf("4.3: ImageRotate90CW twice == ImageRotate180CW\n");
    img_90CW_twice = ImageRotate90CW(img_90CW); // Deve ser 100x50
    ImageSavePPM(img_90CW_twice, "test_rotated_90CW_twice.ppm");
    int check4_3 = (ImageIsEqual(img_180CW, img_90CW_twice));
    ASSERT_CHECK(check4_3, "ImageRotate_Equivalence", &local_passed_count, &local_total_count);

    // Cleanup
    ImageDestroy(&img_original);
    ImageDestroy(&img_90CW);
    ImageDestroy(&img_180CW);
    ImageDestroy(&img_90CW_twice);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}

// --- Seção 5: Testes de Region Filling e Segmentation ---
static void TestImageRegionFillingAndSegmentation(int section_num) {
    printf("\n## %d. Region Filling and Segmentation Tests\n", section_num);
    
    int local_passed_count = 0;
    int local_total_count = 0;

    // Imagem para testes de preenchimento de região: 20x20, edge 8.
    Image img_base = ImageCreateChess(20, 20, 8, 0x000000); 

    // 5.1 - ImageRegionFillingRecursive
    printf("5.1: ImageRegionFillingRecursive\n");
    Image img_rec = ImageCopy(img_base);
    int count_rec = ImageRegionFillingRecursive(img_rec, 10, 10, WHITE); // Região 8x8 = 64 pixels
    ImageSavePPM(img_rec, "test_region_fill_recursive.ppm");
    ASSERT_CHECK(count_rec == 64, "ImageRegionFillingRecursive_Count", &local_passed_count, &local_total_count);
    ImageDestroy(&img_rec);

    // 5.2 - ImageRegionFillingWithSTACK
    printf("5.2: ImageRegionFillingWithSTACK\n");
    Image img_stack = ImageCopy(img_base);
    int count_stack = ImageRegionFillingWithSTACK(img_stack, 10, 10, WHITE);
    ImageSavePPM(img_stack, "test_region_fill_stack.ppm");
    ASSERT_CHECK(count_stack == 64, "ImageRegionFillingWithSTACK_Count", &local_passed_count, &local_total_count);
    ImageDestroy(&img_stack);

    // 5.3 - ImageRegionFillingWithQUEUE
    printf("5.3: ImageRegionFillingWithQUEUE\n");
    Image img_queue = ImageCopy(img_base);
    int count_queue = ImageRegionFillingWithQUEUE(img_queue, 10, 10, WHITE);
    ImageSavePPM(img_queue, "test_region_fill_queue.ppm");
    ASSERT_CHECK(count_queue == 64, "ImageRegionFillingWithQUEUE_Count", &local_passed_count, &local_total_count);
    ImageDestroy(&img_queue);

    // 5.4 - ImageSegmentation (usando STACK)
    printf("5.4: ImageSegmentation (with STACK filling)\n");
    Image img_seg_stack = ImageCopy(img_base);
    int regions_stack = ImageSegmentation(img_seg_stack, &ImageRegionFillingWithSTACK);
    // 3x3 = 9 regiões, 4 brancas segmentadas.
    int check5_4 = (regions_stack == 4);
    ASSERT_CHECK(check5_4, "ImageSegmentation_Stack_RegionsCount", &local_passed_count, &local_total_count);
    ImageDestroy(&img_seg_stack);
    
    // 5.5 - ImageSegmentation (usando QUEUE)
    printf("5.5: ImageSegmentation (with QUEUE filling)\n");
    Image img_seg_queue = ImageCopy(img_base);
    int regions_queue = ImageSegmentation(img_seg_queue, &ImageRegionFillingWithQUEUE);
    int check5_5 = (regions_queue == 4);
    ASSERT_CHECK(check5_5, "ImageSegmentation_Queue_RegionsCount", &local_passed_count, &local_total_count);
    ImageDestroy(&img_seg_queue);

    // 5.6 - ImageSegmentation (usando RECURSIVE)
    printf("5.6: ImageSegmentation (with RECURSIVE filling)\n");
    Image img_seg_rec = ImageCopy(img_base);
    int regions_rec = ImageSegmentation(img_seg_rec, &ImageRegionFillingRecursive);
    int check5_6 = (regions_rec == 4);
    ASSERT_CHECK(check5_6, "ImageSegmentation_Recursive_RegionsCount", &local_passed_count, &local_total_count);
    ImageDestroy(&img_seg_rec);
    
    // Cleanup
    ImageDestroy(&img_base);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}

// --- Seção 6: Testes de Validação de Pixel ---
static void TestImageIsValidPixelFunction(int section_num) {
    printf("\n## %d. ImageIsValidPixel Test\n", section_num);

    int local_passed_count = 0;
    int local_total_count = 0;

    Image img = ImageCreate(10, 5); // W=10, H=5. Coordenadas (u=coluna, v=linha). u: 0-9, v: 0-5
    
    // 6.1 - Casos Válidos (u: 0-4, v: 0-9)
    printf("6.1: ImageIsValidPixel (Valid)\n");
    ASSERT_CHECK(ImageIsValidPixel(img, 0, 0), "ImageIsValidPixel_TopLeft", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageIsValidPixel(img, 9, 4), "ImageIsValidPixel_BottomRight", &local_passed_count, &local_total_count);
    ASSERT_CHECK(ImageIsValidPixel(img, 5, 2), "ImageIsValidPixel_Middle", &local_passed_count, &local_total_count);
    
    // 6.2 - Casos Inválidos
    printf("6.2: ImageIsValidPixel (Invalid)\n");
    ASSERT_CHECK(!ImageIsValidPixel(img, -1, 5), "ImageIsValidPixel_Negative_U", &local_passed_count, &local_total_count);
    ASSERT_CHECK(!ImageIsValidPixel(img, 5, 5), "ImageIsValidPixel_Out_of_Bounds_V (5 >= H=5)", &local_passed_count, &local_total_count);
    ASSERT_CHECK(!ImageIsValidPixel(img, 10, 2), "ImageIsValidPixel_Out_of_Bounds_U (10 >= W=10)", &local_passed_count, &local_total_count);

    // Cleanup
    ImageDestroy(&img);

    // --- Resumo da Seção ---
    printf("--- Section %d Summary: (" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET "/" ANSI_COLOR_GREEN "%d" ANSI_COLOR_RESET ") %d out of %d tests passed. ---\n",
           section_num, local_passed_count, local_total_count, local_passed_count, local_total_count);
}