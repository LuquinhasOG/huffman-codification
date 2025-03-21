# Codificação de Huffman

## Dependencias

* gcc - GNU C compiler

## Sobre:

A Codificação de Huffman é um algoritmo simple de compressão de texto. Ele funciona reduzindo a quantidade de bits utilizados para representar cada caracter do texto.
Na primeira etapa, o algoritmo lê o arquivo e conta a frequência em que os caracteres aparecem. Após isso é montado uma Árvore de Huffman, que é uma Árvore Bináriam, onde os seus nós representam os caracteres.
Os menos frequentes se encontram nos níveis mais profundos da árvore, enquanto os que mais se repetem, ficam no topo. Assim, para codificar o texto, basta fazer uma busca pelo caracter desejado, os símblos para representar as direções são '0' para a esquerda e '1' para a direita, agora sua nova representação em binário será o caminho percorrido.

    Algoritmo para criação da Árvore de Huffman:

    1. Abra o arquivo de texto.

    2. Conte quantas vezes cada símbolo se repete, e insira em uma lista de nós.

    3. Ordene a lista pela frequencia em ordem crescente.

    4. Remova os dois primeiros nós e os junte em um nó pai, e o adicione na lista.

    5. Se tiver mais que 1 nós na lista, volte ao passo 3.

    6. Fim. Árvore de Huffman criada.

Para acelerar a codificação, pode-se criar um dicionário com base na Árvore de Huffman, onde a chave é o caracter, e o valor o caminho percorrido na árvore. Para restaurar os dados originais, basta inverter as chaves pelos valores, criando um dicionário de decodificação.

    Algoritmo para criação do dicionário:

    1. Inicie um dicionário vazio.

    2. Percorra a árvore em profundidade.

    3. Enquanto percorre a árvore, se o nó atual é uma folha (ou seja um caracter), insira no dicionário o valor e o caminho percorrido.

## Usando a aplicação

O programa deve ser executado passando argumentos. O primeiro argumento deve ser comp (compress) os decomp (decompress). Para comprimir, é necessário passar também, em ordem, o nome do árquivo de origem (com extensão) e árquivo de destino (sem extensão). Para descomprimir é preciso apenas o nome do árquivo de origem codificado (sem extensão).

    Exemplos:
    Comando: huffman comp lorem.txt comprimido
    Saída: arquivo codificado comprimido.huff.

    Comando: huffman decomp comprimido
    Saída: arquivo decodificado comprimido.txt.

## Estrutura do arquivo .huff

Este formato de arquivo foi feito pensado para este projeto, objetivando um arquivo simples que pudesse armazenar o dicionário de codificação e o texto codificado.

O arquivo é dividido em duas partes pela sequência de 4 bytes 0xAAAAAAAA, o chamado de divisor. O que vem antes do divisor é o dicionário, e o que vem depois é o texto comprimido.

### O dicionário
O primeiro byte é o caracter, e o segundo byte é o tamanho do caminho, os bytes seguintes serão o caminho pela árvore. Caso o tamanho do caminho esteja entre 1 e 8 bits, é usado apenas um byte para armazenar o caminho, caso estiver ente 9 e 16 bits, é usado dois bytes, e assim por diante.

    Exemplo:
    01100001  - letra: a
    00000100  - tamanho: 4
    00001001  - caminho: 1, 0, 0, 1
    01100010  - letra b
    00001001  - tamanho: 9
    00000000  - este byte complementa o de baixo
    01110011  - caminho: 0, 0, 1, 1, 1, 0, 0, 1, 1

### O texto codificado
O primeiro byte dos dados após o divisor é o deslocamento. Ele é nescessário pois pode acontecer do tamanho do texto codificado para o binário não completar o último byte, então o deslocamento é usado para que no processo de decodificação o programa saiba quando o texto acabou. O deslocamento assume valores de 0 a 7.

    Exemplo:
    0000011  - deslocamento de 3 bits
    1101000  - texto
    1001110
    0001101
    0011000  - os últimos 3 bits não fazem parte do texto então serão ignorados.
