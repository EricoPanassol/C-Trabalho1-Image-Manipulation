# Trabalho Prático I - Ver para (não) crer
Profs. Marcelo Cohen e Edson Moreno

09/2022

# Introdução

Atualmente tem sido cada vez mais difícil confirmar em imagens: há uma grande quantidade de técnicas de manipulação de imagens, desde edição manual até técnicas envolvendo algoritmos de aprendizagem de máquina. A utilização maliciosa dessas técnicas, especialmente para a produção de fake news, tem sido cada vez mais frequente.

A ideia deste trabalho é explorar um algoritmo capaz de realizar uma análise forense simplificada de imagens, porém bastante efetiva em algumas situações. Esse algoritmo consiste em amplificar o ruído já presente na imagem, de forma que seja possível enxergar distorções na qualidade desse ruído e identificar possíveis manipulações.

Por exemplo, considerando as imagens abaixo, é fácil ver na imagem com amplificação de ruído que algo está destoando do restante: a árvore mais à frente foi "copiada e colada" a partir da outra.

## (a) Original
![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/trees.jpg?)

## (b) Amplificação de ruído
![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/trees_noise.jpg)

# Funcionamento

Uma imagem é representada por uma matriz de pontos (pixels), onde cada cor é definida por 3 componentes: vermelho (R), verde (G) e azul (B). Cada uma dessas componentes é codificada em **um byte**, o que produz **3 bytes por pixel** (24 bits) - ou seja, 16 milhões de possíveis cores. Em outras palavras, as intensidades (R, G, B) variam de 0 a 255, onde 0 é escuro e 255 é claro.

Veja abaixo como diversas cores são representadas nesse formato - cada cor está expressa pelas componentes RGB em hexadecimal.

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/hex-colors.jpg)

![alt text](https://mflash.github.io/progswb2/trab/t1-222-hjsdv5sftysc734r/rgbcolors.jpg)
