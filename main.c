#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // Para usar strings

#ifdef WIN32
#include <windows.h> // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>   // Funções da OpenGL
#include <GL/glu.h>  // Funções da GLU
#include <GL/glut.h> // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include "SOIL.h"

// Um pixel Pixel (24 bits)
typedef struct
{
    unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct
{
    int width, height;
    Pixel *img;
} Img;

// Protótipos
void load(char *name, Img *pic);
void processa();

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);

// Largura da janela
int width;

// Altura da janela
int height;

// Fator de multiplicação do ruído
int fator;

// Identificadores de textura
GLuint tex[2];

// As 2 imagens
Img pic[2];

// Imagem selecionada (0,1)
int sel;

//Ordena o array.
void Array_sort(double *array , int n)
{ 
    int i=0 , j=0 , temp=0;
    for(i=0 ; i<n ; i++)
    {
        for(j=0 ; j<n-1 ; j++)
        {
            if(array[j]>array[j+1])
            {
                temp        = array[j];
                array[j]    = array[j+1];
                array[j+1]  = temp;
            }
        }
    }
}

// Carrega uma imagem para a struct Img
void load(char *name, Img *pic)
{
    int chan;
    pic->img = (Pixel *)SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);
    if (!pic->img)
    {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        printf("seeing [im. entrada]\n");
        exit(1);
    }
    glutInit(&argc, argv);

    // Define do modo de operacao da GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // pic[0] -> imagem de entrada
    // pic[1] -> imagem de saida

    // Carrega a imagem
    load(argv[1], &pic[0]);

    fator = 5;
    if (argc == 3)
        fator = atoi(argv[2]);

    width = pic[0].width;
    height = pic[0].height;

    // A largura e altura da imagem de saída são iguais às da imagem de entrada (0)
    pic[1].width = pic[0].width;
    pic[1].height = pic[0].height;
    pic[1].img = calloc(pic[1].width * pic[1].height, 3); // W x H x 3 bytes (Pixel)

    // Especifica o tamanho inicial em pixels da janela GLUT
    glutInitWindowSize(width, height);

    // Cria a janela passando como argumento o titulo da mesma
    glutCreateWindow("Analise Forense de Imagens");

    // Registra a funcao callback de redesenho da janela de visualizacao
    glutDisplayFunc(draw);

    // Registra a funcao callback para tratamento das teclas ASCII
    glutKeyboardFunc(keyboard);

    // Exibe as dimensões na tela, para conferência
    printf("Entrada  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    sel = 0; // entrada

    // Define a janela de visualizacao 2D
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);

    // Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char *)pic[0].img, width, height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char *)pic[1].img, width, height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Aplica o algoritmo
    processa();

    // Entra no loop de eventos, não retorna
    glutMainLoop();
}

// Aplica o algoritmo e gera a saída em pic[1]
void processa()
{
    // Converte para interpretar como matriz
    // Pixel* in = pic[0].img;
    // Pixel* out = pic[1].img;
    Pixel(*in)[width] = (Pixel(*)[width])pic[0].img;
    Pixel(*out)[width] = (Pixel(*)[width])pic[1].img;

    // Aplica o algoritmo e gera a saida em out (pic[1].img)
    // ...
    // ...
    int regsize = 3;
    int tam = regsize * regsize;

    printf("Region size*size: %d\n", tam);
    printf("Fator: %d\n\n", fator);

    // Exemplo: inverte as cores da imagem de entrada
    // for(int y=0; y<height; y++) {
    //     for(int x=0; x<width; x++) {
    //         out[y][x].r = 255 - in[y][x].r;
    //         out[y][x].g = 255 - in[y][x].g;
    //         out[y][x].b = 255 - in[y][x].b;
    //     }
    // }

    // Exemplo: fórmula da luminância (o quão clara ou escura é a cor)
    // lum = 0.59 . G + 0.3 . R + 0.1 . B
    // Define a luminância de cada pixel
    for(int altura=0; altura<height; altura++) {
        for(int largura=0; largura<width; largura++) {
            double lumPixelCentral = 0.59 * in[altura][largura].g + 0.3 * in[altura][largura].r + 0.1 * in[altura][largura].b;
            if(altura != 0 && altura != height-1 && largura != 0 && largura != width-1){
                // Criando os pixels que faltam
                Pixel pixel1 = in[altura-1][largura-1];
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel3 = in[altura-1][largura+1];
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel7 = in[altura+1][largura-1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel9 = in[altura+1][largura+1];

                // Atribuindo uma luminância para cada pixel
                double lumPixel1 = 0.59 * pixel1.g + 0.3 * pixel1.r + 0.1 * pixel1.b;
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel3 = 0.59 * pixel3.g + 0.3 * pixel3.r + 0.1 * pixel3.b;
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel7 = 0.59 * pixel7.g + 0.3 * pixel7.r + 0.1 * pixel7.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel9 = 0.59 * pixel9.g + 0.3 * pixel9.r + 0.1 * pixel9.b;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel1){
                    subtraendo = pixel1;
                }
                else if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel3){
                    subtraendo = pixel3;
                }
                else if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixel7){
                    subtraendo = pixel7;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixel9){
                    subtraendo = pixel9;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }
            else if(altura == 0 && largura != 0 && largura != width-1){// linha de cima
                // Criando os pixels que faltam
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel7 = in[altura+1][largura-1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel9 = in[altura+1][largura+1];
                Pixel pixel1 = pixel7;
                Pixel pixel2 = pixel8;
                Pixel pixel3 = pixel9;

                // Atribuindo uma luminância para cada pixel
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel7 = 0.59 * pixel7.g + 0.3 * pixel7.r + 0.1 * pixel7.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel9 = 0.59 * pixel9.g + 0.3 * pixel9.r + 0.1 * pixel9.b;
                double lumPixel1 = lumPixel7;
                double lumPixel2 = lumPixel8;
                double lumPixel3 = lumPixel9;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixel7){
                    subtraendo = pixel7;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixel9){
                    subtraendo = pixel9;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------

            else if(altura != 0 && altura != height-1 && largura == 0){// linha da esquerda
                // Criando os pixels que faltam
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel3 = in[altura-1][largura+1];
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel9 = in[altura+1][largura+1];
                Pixel pixel1 = pixel3;
                Pixel pixel4 = pixel6;
                Pixel pixel7 = pixel9;

                // Atribuindo uma luminância para cada pixel
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel3 = 0.59 * pixel3.g + 0.3 * pixel3.r + 0.1 * pixel3.b;
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel9 = 0.59 * pixel9.g + 0.3 * pixel9.r + 0.1 * pixel9.b;
                double lumPixel1 = lumPixel3;
                double lumPixel4 = lumPixel6;
                double lumPixel7 = lumPixel9;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel3){
                    subtraendo = pixel3;
                }
                else if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixel9){
                    subtraendo = pixel9;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

             // ---------------------------------------------------------------------------------------

            else if(altura != 0 && altura != height-1 && largura == width-1){// linha da direita
                // Criando os pixels que faltam
                Pixel pixel1 = in[altura-1][largura-1];
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel7 = in[altura+1][largura-1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel3 = pixel1;
                Pixel pixel6 = pixel4;
                Pixel pixel9 = pixel7;

                // Atribuindo uma luminância para cada pixel
                double lumPixel1 = 0.59 * pixel1.g + 0.3 * pixel1.r + 0.1 * pixel1.b;
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel7 = 0.59 * pixel7.g + 0.3 * pixel7.r + 0.1 * pixel7.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel3 = lumPixel1;
                double lumPixel6 = lumPixel4;
                double lumPixel9 = lumPixel7;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel1){
                    subtraendo = pixel1;
                }
                else if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixel7){
                    subtraendo = pixel7;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------

            else if(altura == 0 && largura != 0 && largura != width-1){// linha de baixo
                // Criando os pixels que faltam
                Pixel pixel1 = in[altura-1][largura-1];
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel3 = in[altura-1][largura+1];
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel7 = pixel1;
                Pixel pixel8 = pixel2;
                Pixel pixel9 = pixel3;

                // Atribuindo uma luminância para cada pixel
                double lumPixel1 = 0.59 * pixel1.g + 0.3 * pixel1.r + 0.1 * pixel1.b;
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel3 = 0.59 * pixel3.g + 0.3 * pixel3.r + 0.1 * pixel3.b;
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel7 = lumPixel1;
                double lumPixel8 = lumPixel2;
                double lumPixel9 = lumPixel3;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel1){
                    subtraendo = pixel1;
                }
                else if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel3){
                    subtraendo = pixel3;
                }
                else if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------

            else if(altura == 0 && largura == 0){ //Canto superior esquerdo
                // Criando os pixels que faltam
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel9 = in[altura+1][largura+1];
                Pixel pixel1 = pixel9;
                Pixel pixel2 = pixel8;
                Pixel pixel3 = pixel9;
                Pixel pixel4 = pixel6;

                // Atribuindo uma luminância para cada pixel
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel9 = 0.59 * pixel9.g + 0.3 * pixel9.r + 0.1 * pixel9.b;
                double lumPixel1 = lumPixel9;
                double lumPixel2 = lumPixel8;
                double lumPixel3 = lumPixel9;
                double lumPixel4 = lumPixel6;
                double lumPixel7 = lumPixel9;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixel9){
                    subtraendo = pixel9;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------

            else if(altura == 0 && largura == width-1){ //Canto superior direito
                // Criando os pixels que faltam
                Pixel pixel7 = in[altura+1][largura-1];
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel8 = in[altura+1][largura];
                Pixel pixel9 = pixel7;
                Pixel pixel1 = pixel7;
                Pixel pixel2 = pixel8;
                Pixel pixel3 = pixel7;
                Pixel pixel6 = pixel4;
                
                // Atribuindo uma luminância para cada pixel
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel7 = 0.59 * pixel7.g + 0.3 * pixel7.r + 0.1 * pixel7.b;
                double lumPixel8 = 0.59 * pixel8.g + 0.3 * pixel8.r + 0.1 * pixel8.b;
                double lumPixel1 = lumPixel7;
                double lumPixel2 = lumPixel8;
                double lumPixel3 = lumPixel7;
                double lumPixel6 = lumPixel4;
                double lumPixel9 = lumPixel7;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel7){
                    subtraendo = pixel7;
                }
                else if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixel8){
                    subtraendo = pixel8;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------
            
            else if(altura == height-1 && largura == 0){ //Canto inferior esquerdo
                // Criando os pixels que faltam
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel3 = in[altura-1][largura+1];
                Pixel pixel6 = in[altura][largura+1];
                Pixel pixel1 = pixel3;
                Pixel pixel4 = pixel6;
                Pixel pixel7 = pixel3;
                Pixel pixel8 = pixel2;
                Pixel pixel9 = pixel3;
                
                // Atribuindo uma luminância para cada pixel
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel3 = 0.59 * pixel3.g + 0.3 * pixel3.r + 0.1 * pixel3.b;
                double lumPixel6 = 0.59 * pixel6.g + 0.3 * pixel6.r + 0.1 * pixel6.b;
                double lumPixel1 = lumPixel3;
                double lumPixel4 = lumPixel6;
                double lumPixel7 = lumPixel3;
                double lumPixel8 = lumPixel2;
                double lumPixel9 = lumPixel3;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel3){
                    subtraendo = pixel3;
                }
                else if(mediana == lumPixel6){
                    subtraendo = pixel6;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }

            // ---------------------------------------------------------------------------------------

            else if(altura == height-1 && largura == width-1){ // Canto inferior direito
                // Criando os pixels que faltam
                Pixel pixel1 = in[altura-1][largura-1];
                Pixel pixel2 = in[altura-1][largura];
                Pixel pixel4 = in[altura][largura-1];
                Pixel pixel3 = pixel1;
                Pixel pixel6 = pixel4;
                Pixel pixel7 = pixel1;
                Pixel pixel8 = pixel2;
                Pixel pixel9 = pixel1;
                
                // Atribuindo uma luminância para cada pixel
                double lumPixel1 = 0.59 * pixel1.g + 0.3 * pixel1.r + 0.1 * pixel1.b;
                double lumPixel2 = 0.59 * pixel2.g + 0.3 * pixel2.r + 0.1 * pixel2.b;
                double lumPixel4 = 0.59 * pixel4.g + 0.3 * pixel4.r + 0.1 * pixel4.b;
                double lumPixel3 = lumPixel1;
                double lumPixel6 = lumPixel4;
                double lumPixel7 = lumPixel1;
                double lumPixel8 = lumPixel2;
                double lumPixel9 = lumPixel1;
                
                // Adicionando os pixels num array para encontrar a mediana
                double pixels[9] = {lumPixel1, lumPixel2, lumPixel3, lumPixel4, lumPixelCentral, lumPixel6, lumPixel7, lumPixel8, lumPixel9};
                
                Array_sort(pixels, 9);
                
                double mediana = pixels[4];
                Pixel subtraendo;
                
                // Procura qual pixel tem a luminância igual a mediana
                if(mediana == lumPixel1){
                    subtraendo = pixel1;
                }
                else if(mediana == lumPixel2){
                    subtraendo = pixel2;
                }
                else if(mediana == lumPixel4){
                    subtraendo = pixel4;
                }
                else if(mediana == lumPixelCentral){
                    subtraendo = in[altura][largura];
                }
                
                // subtraio o R, G, B do pixel central pelo R, G, B do pixel da mediana
                out[altura][largura].r = in[altura][largura].r - subtraendo.r;
                out[altura][largura].g = in[altura][largura].g - subtraendo.g;
                out[altura][largura].b = in[altura][largura].b - subtraendo.b;
            }
        }
    }

    // Se desejar salvar a imagem de saída, descomente a linha abaixo
    //SOIL_save_image("out.bmp", SOIL_SAVE_TYPE_BMP, pic[1].width, pic[1].height, 3, (const unsigned char *)pic[1].img);

    // Faz upload da nova textura na GPU - NÃO ALTERAR
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, out);
}


// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        // ESC: libera memória e finaliza
        free(pic[0].img);
        free(pic[1].img);
        exit(1);
    }
    if (key >= '1' && key <= '2')
        // 1-2: seleciona a imagem correspondente (origem ou destino)
        sel = key - '1';

    if(key == '=') {
        fator += 5;
        processa();
    }
    if(key == '-') {
        fator -= 5;
        processa();
    }
    glutPostRedisplay();
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Preto
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/Pixel.txt

    glColor3ub(255, 255, 255); // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(1, 0);
    glVertex2f(pic[sel].width, 0);

    glTexCoord2f(1, 1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0, 1);
    glVertex2f(0, pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}
