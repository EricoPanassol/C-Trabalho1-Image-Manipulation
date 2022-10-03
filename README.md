# Trabalho Prático I - Ver para (não) crer
Profs. Marcelo Cohen e Edson Moreno

09/2022

# 1 Introdução

Atualmente tem sido cada vez mais difícil confirmar em imagens: há uma grande quantidade de técnicas de manipulação de imagens, desde edição manual até técnicas envolvendo algoritmos de aprendizagem de máquina. A utilização maliciosa dessas técnicas, especialmente para a produção de fake news, tem sido cada vez mais frequente.

A ideia deste trabalho é explorar um algoritmo capaz de realizar uma análise forense simplificada de imagens, porém bastante efetiva em algumas situações. Esse algoritmo consiste em amplificar o ruído já presente na imagem, de forma que seja possível enxergar distorções na qualidade desse ruído e identificar possíveis manipulações.

Por exemplo, considerando as imagens abaixo, é fácil ver na imagem com amplificação de ruído que algo está destoando do restante: a árvore mais à frente foi "copiada e colada" a partir da outra.

## (a) Original
![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/trees.jpg?)

## (b) Amplificação de ruído
![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/trees_noise.jpg)

# 2 Funcionamento

Uma imagem é representada por uma matriz de pontos (pixels), onde cada cor é definida por 3 componentes: vermelho (R), verde (G) e azul (B). Cada uma dessas componentes é codificada em **um byte**, o que produz **3 bytes por pixel** (24 bits) - ou seja, 16 milhões de possíveis cores. Em outras palavras, as intensidades (R, G, B) variam de 0 a 255, onde 0 é escuro e 255 é claro.

Veja abaixo como diversas cores são representadas nesse formato - cada cor está expressa pelas componentes RGB em hexadecimal.

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/hex-colors.jpg)

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/rgbcolors.jpg)

O algoritmo de amplificação de ruído é baseado em uma técnica denominada **filtro da mediana**. Um filtro é uma "janela" que percorre a imagem, realizando alguma operação com o valor dos pixels e armazenando o resultado dessa operação no **pixel central** da região. Esse resultado normalmente é armazenado em uma imagem de saída, preservando a original.

Para simplificar o entendimento, vamos considerar que cada pixel possui um valor numérico único, de 0 a 255. Como desejamos a mediana, precisamos **ordenar** os pixels de cada região e então substituir o pixel central na imagem de saída pelo valor presente na posição media. Veja o funcionamento abaixo para um filtro 3x3:

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/median.jpg)

Para as bordas da imagem, repete-se os pixels da borda oposta da própria região, como no exemplo abaixo para o canto superior esquerdo:

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/median1.png)

Repetindo-se o processo para toda a imagem, tem-se algo como segue:

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/median.gif)

Porém se simplesmente aplicarmos a mediana, estaremos na verdade utilizando um algoritmo de suavização, que costuma **remover** o ruído já presente em uma imagem. Portanto, o que faremos é **subtrair** o valor da mediana do valor do pixel original (central), e então armazenar esse resultado na imagem de saída. Esse resultado será amplificado por um **fator**, que poderá ser ajustado pelo teclado.

O detalhe final é que os pixels são armazenados em RGB, portanto é necessário encontrar uma forma de ordená-los considerando a sua cor. Uma forma de fazer isso é calculando a **luminância** associada à cor, isto é, o quão "clara" ou "escura" ela é. Esse cálculo pode ser feito da seguinte forma:

_lum = 0,59 . G + 0,3 . R + 0,11 . B_

Ou seja, é uma media ponderada para R, G e B. Isso deve ser feito dessa forma, pois o olho humano tem mais facilidade de distinguir tons de verde, depois tons de vermelho e por último, tons de azul.

Eis um resumo do algoritmo a ser implementado:

1. Passar por toda a imagem de entrada:
    1. Calcular a mediana de cada pixel, considerando a luminância dos pixels na região definida em torno do pixel central (ex: 3x3)
    2. Subtrair o valor da mediana (RGB) do valor do pixel central (RGB), armazenando na imagem de saída. Obs: cada componente desse resultado deve estar limitado à          faixa 0-255.
2. Exibir a imagem de saída

# 3 Detalhes Técnicos

Ao ser iniciado, o programa irá carregar um arquivo de imagem. Para tanto, utilizaremos uma biblioteca simples (integrada no projeto) denominada SOIL.

Após a leitura da imagem, o programa deverá realizar o algoritmo de amplificação do ruído dos pixels da imagem de entrada, gerando o resultado na imagem de saída, criada com as mesmas dimensões.

Veja no trecho de código abaixo as definições das estruturas e imagens:

```c
// Um pixel RGB (24 bits)
typedef struct {
    unsigned char r, g, b;
} Pixel;

// Uma imagem RGB
typedef struct {
    int width, height;
    Pixel* img;            // vetor de pixels
} Img;

// A função processa() define:

// As 2 imagens em forma de matriz
RGB in[][], out[][];
...

// in e out armazenarão as imagens:
// in => origem
// out => destino (inicializada com zeros)

// Cada pixel está armazenado em, por ex, in[y][x], onde x,y são as coordendas
// do pixel. Exemplo: acessando os dois primeiros pixels
// da imagem de entrada
RGB aux1 = in[0][0];
RGB aux2 = in[1][0];
...
```

Você deverá implementar o seu algoritmo dentro da função **processa()**, já presente no código-fonte. **Não altere nenhuma outra parte do código**.

## 4 Código base e imagens de teste

O arquivo [seeing.zip](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/seeing.zip) contém o projeto completo para a implementação do trabalho. Esse código já implementa a exibição das 2 imagens na tela gráfica, usando a biblioteca OpenGL (use as teclas 1 e 2 para selecionar a imagem, '=' para aumentar o fator de amplificação e '-' para reduzi-lo). O projeto pode ser compilado no Linux, Windows ou macOS, seguindo as instruções abaixo. Para a compilação no Linux, é necessário ter instalado os pacotes de desenvolvimento da biblioteca OpenGL. Para Ubuntu, Mint, Debian e derivados, instale com:

```bash
sudo apt-get install freeglut3-dev
```

Para a compilação no Windows ou no macOS, não é necessário instalar mais nada - o compilador já vem com as bibliotecas necessárias.

## 4.1 Visual Studio Code

Se você estiver utilizando o Visual Studio Code, basta descompactar o zip e abrir a pasta.
    
    - Para **compilar**: use Ctrl+Shift+B (⌘+Shift+B no macOS).
    
    - Para **executar**: use F5 para usar o debugger ou Ctrl+F5 para executar sem o debugger.
    
## 4.2 Outros ambientes ou terminal

Caso esteja usando outro ambiente de desenvolvimento, fornecemos um Makefile para Linux e macOS, e outro para Windows (Makefile.mk).
    Dessa forma, para compilar no Linux ou macOS, basta digitar:
```bash
make
```

Se estiver utilizando o Windows, o comando é similar:

```bash
mingw32-make -f Makefile.mk
```

Alternativamente, você também pode utilizar o CMake (Cross Platform Make) para compilar pelo terminal. Para tanto, crie um diretório build embaixo do diretório do projeto e faça:

```c
cd build
cmake ..
make -j  # ou mingw32-make -j no Windows
```

Se for utilizado o Linux ou macOS, o projeto também pode ser compilado manualmente pelo terminal usando o comando make (e o Makefile fornecido no projeto):

## 5 Avaliação

Leia com atenção os critérios de avaliação:

- Pontuação:
    - Conversão das cores para luminância: 3 pontos
    - Cálculo da mediana: 4 pontos
    - Subtração das imagens: 3 pontos
- Os trabalhos são **em duplas ou individuais**. A pasta do projeto deve ser compactada em um arquivo .zip e este deve ser submetido pelo Moodle até a data e hora especificadas.
- Não envie .rar, .7z, .tar.gz - apenas .zip.
- O código deve estar identado corretamente (qualquer editor moderno **faz isso automaticamente**).
- **A cópia parcial ou completa do trabalho terá como conseqüência a atribuição de nota ZERO ao trabalho dos alunos envolvidos. A verificação de cópias é feita inclusive entre turmas.**
- **A cópia de código ou algoritmos existentes da Internet também não é permitida**. Se alguma idéia encontrada na rede for utilizada na implementação, sua descrição e referência deve ser mencionada claramente no código.
