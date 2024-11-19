// Importación de librerías necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaración de constantes
#define MAX_CHANNELS 256
#define THRESHOLD 127 // Umbral para blanco y negro

// Prototipos de funciones
void mostrarAyuda();
void separarImagenesPorCanal(const char *archivo);
void desplegarCabeceraBMP(const char *archivoNombre);
void desplegarCabeceraPNM(const char *archivoNombre);
void generarImagenGrises(const char *archivo);
void generarImagenBN(const char *archivo, int umbral);
void calcularHistograma(const char *archivo);
void mezclarImagenes(const char *archivo1, const char *archivo2, int alpha);
void realizarTodosProcesos(int umbral, int alpha, const char *archivo1, const char *archivo2);
void separarMatrizRoja(const char *archivo);
void separarMatrizVerde(const char *archivo);
void separarMatrizAzul(const char *archivo);

// Programa principal
int main(int argc, char *argv[]) {
    // Verificar si se ha proporcionado al menos un argumento en caso contrario mostrar el menú de ayuda
    if (argc < 2) {
        mostrarAyuda();
        return 1;
    }

    // Mostrar el menú de ayuda cuando se ingresa por consola
    if (strcmp(argv[1], "-help") == 0) {
        mostrarAyuda();
        return 0;
    }

    // Procesar las imágenes y las opciones del comando
    if (argc == 3) {
        // Separar en matrices R, G, B
        if (strcmp(argv[1], "1b") == 0 || strcmp(argv[1], "1p") == 0) {
            separarImagenesPorCanal(argv[2]);
        }
        // Separar matriz R
        else if (strcmp(argv[1], "2b") == 0 || strcmp(argv[1], "2p") == 0) {
            separarMatrizRoja(argv[2]);
        }
        // Separar matriz G
        else if (strcmp(argv[1], "3b") == 0 || strcmp(argv[1], "3p") == 0) {
            separarMatrizVerde(argv[2]);
        }
        // Separar matriz B
        else if (strcmp(argv[1], "4b") == 0 || strcmp(argv[1], "4p") == 0) {
            separarMatrizAzul(argv[2]);
        }
        // Generar la imagen en escala de grises (BMP o PNM)
        else if (strcmp(argv[1], "5b") == 0 || strcmp(argv[1], "5p") == 0) {
            generarImagenGrises(argv[2]);
        }
        // Calcular el histograma de la imagen (PNM o BMP)
        else if (strcmp(argv[1], "7b") == 0 || strcmp(argv[1], "7p") == 0) {
            calcularHistograma(argv[2]);
        }
    }
    else if (argc == 4) {
        // Caso de imagen en blanco y negro con umbral (PNM o BMP)
        if (strcmp(argv[1], "6p") == 0 || strcmp(argv[1], "6b") == 0) {
            int umbral = atoi(argv[2]);
            // Verificación del rango del umbral
            if (umbral < 0 || umbral > 255) {
                printf("El umbral debe estar entre 0 y 255.\n");
                return 1;
            }
            generarImagenBN(argv[3], umbral);
        }
    }
    else if (argc == 5) {
        // Caso de mezcla de imágenes (PNM o BMP)
        if (strcmp(argv[1], "8p") == 0 || strcmp(argv[1], "8b") == 0) {
            int alpha = atoi(argv[2]);
            mezclarImagenes(argv[3], argv[4], alpha);
        }
    }
    else if (argc == 6) {
        // Caso de realizar todos los procesos (PNM o BMP)
        if (strcmp(argv[1], "9p") == 0 || strcmp(argv[1], "9b") == 0) {
            int umbral = atoi(argv[2]);
            int alpha = atoi(argv[3]);
            realizarTodosProcesos(umbral, alpha, argv[4], argv[5]);
        }
    }
    else {
        // Si el número de parámetros es incorrecto
        printf("Número de parámetros incorrecto. Usa -help para ver la ayuda.\n");
        return 1;
    }

    return 0;
}

// Función para mostrar la ayuda del programa
void mostrarAyuda() {
    printf("Uso del programa:\n");
    printf("-help -> Muestra esta ayuda.\n");
    printf("1p <imagen.pnm> -> Separar en matrices R, G, B.\n");
    printf("2p <imagen.pnm> -> Separar matriz R.\n");
    printf("3p <imagen.pnm> -> Separar matriz G.\n");
    printf("4p <imagen.pnm> -> Separar matriz B.\n");
    printf("5b <imagen.bmp> -> Generar imagen en escala de grises a partir de una BMP.\n");
    printf("6b <umbral> <imagen.bmp> -> Generar imagen en blanco y negro a partir de un BMP, el umbral NO debe ser mayor 255 NI menor a 0.\n");
    printf("7p <imagen.pnm> -> Calcular histograma.\n");
    printf("8b <alpha> <imagen1.bmp> <imagen2.bmp> -> Mezcla de imágenes BMP, deben ser del mismo tamaño.\n");
    printf("9p <umbral> <alpha> <imagen1.pnm> <imagen2.pnm> -> Realizar todos los procesos.\n");
}

// Función para desplegar información de cabecera BMP
void desplegarCabeceraBMP(const char *archivoNombre) {
    FILE *file = fopen(archivoNombre, "rb");
    if (!file) {
        printf("Error al abrir el archivo BMP: %s\n", archivoNombre);
        return;
    }

    // Obtención de las cabeceras
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file);

    int ancho = *(int*)&header[18];
    int alto = *(int*)&header[22];
    int bitDepth = *(short*)&header[28];

    printf("Información BMP:\n");
    printf("Ancho: %d px\n", ancho);
    printf("Alto: %d px\n", alto);
    printf("Profundidad de color: %d bits\n", bitDepth);

    fclose(file);
}

// Función para desplegar información de cabecera PNM
void desplegarCabeceraPNM(const char *archivoNombre) {
    FILE *file = fopen(archivoNombre, "r");
    if (!file) {
        printf("Error al abrir el archivo PNM: %s\n", archivoNombre);
        return;
    }

    // Obtención de las cabeceras
    char type[3];
    int ancho, alto, valorMax;
    fscanf(file, "%s\n%d %d\n%d\n", type, &ancho, &alto, &valorMax);

    printf("Información PNM:\n");
    printf("Tipo: %s\n", type);
    printf("Ancho: %d px\n", ancho);
    printf("Alto: %d px\n", alto);
    printf("Valor máximo: %d\n", valorMax);

    fclose(file);
}

// Función para separar canales RGB
void separarImagenesPorCanal(const char *archivo) {
    FILE *imagen = fopen(archivo, "rb");
    if (imagen == NULL) {
        printf("No se pudo abrir el archivo %s\n", archivo);
        return;
    }

    char tipo[3];
    int ancho, alto, valorMax;

    // Leer cabecera PNM (solo soportamos P6)
    fscanf(imagen, "%2s", tipo);
    if (strcmp(tipo, "P6") != 0) {
        printf("Formato no soportado: %s\n", tipo);
        fclose(imagen);
        return;
    }

    // Ignorar comentarios en la cabecera
    fgetc(imagen); // Leer el salto de línea
    while (fgetc(imagen) == '#') {
        while (fgetc(imagen) != '\n');
    }
    fseek(imagen, -1, SEEK_CUR);

    // Leer dimensiones y valor máximo
    fscanf(imagen, "%d %d %d", &ancho, &alto, &valorMax);
    fgetc(imagen); // Leer el salto de línea

    if (valorMax != 255) {
        printf("Solo se soportan imágenes con un valor máximo de 255\n");
        fclose(imagen);
        return;
    }

    int numPixels = ancho * alto;
    unsigned char *datos = (unsigned char *)malloc(numPixels * 3);
    if (datos == NULL) {
        printf("Error al asignar memoria.\n");
        fclose(imagen);
        return;
    }

    fread(datos, sizeof(unsigned char), numPixels * 3, imagen);
    fclose(imagen);

    // Crear los archivos para los canales
    FILE *rojo = fopen("canal_rojo.pnm", "wb");
    FILE *verde = fopen("canal_verde.pnm", "wb");
    FILE *azul = fopen("canal_azul.pnm", "wb");

    if (rojo == NULL || verde == NULL || azul == NULL) {
        printf("Error al crear archivos de salida.\n");
        free(datos);
        if (rojo) fclose(rojo);
        if (verde) fclose(verde);
        if (azul) fclose(azul);
        return;
    }

    // Escribir cabeceras para los archivos
    fprintf(rojo, "P6\n%d %d\n255\n", ancho, alto);
    fprintf(verde, "P6\n%d %d\n255\n", ancho, alto);
    fprintf(azul, "P6\n%d %d\n255\n", ancho, alto);

    // Separar los canales
    for (int i = 0; i < numPixels; i++) {
        unsigned char r = datos[i * 3];
        unsigned char g = datos[i * 3 + 1];
        unsigned char b = datos[i * 3 + 2];

        fwrite(&r, sizeof(unsigned char), 1, rojo);
        fwrite("\0\0", sizeof(unsigned char), 2, rojo);

        fwrite("\0", sizeof(unsigned char), 1, verde);
        fwrite(&g, sizeof(unsigned char), 1, verde);
        fwrite("\0", sizeof(unsigned char), 1, verde);

        fwrite("\0\0", sizeof(unsigned char), 2, azul);
        fwrite(&b, sizeof(unsigned char), 1, azul);
    }

    // Liberar recursos
    fclose(rojo);
    fclose(verde);
    fclose(azul);
    free(datos);

    printf("Se han creado los archivos canal_rojo.pnm, canal_verde.pnm y canal_azul.pnm\n");
}

// Función para separar la matriz roja
void separarMatrizRoja(const char *archivo) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Verificar el tipo de archivo (PNM o BMP)
    if (archivo[strlen(archivo) - 3] == 'p') {  // PNM
        FILE *output = fopen("rojo.pnm", "w");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }
        char type[3];
        int ancho, alto, valorMax;
        fscanf(file, "%s\n%d %d\n%d\n", type, &ancho, &alto, &valorMax);
        fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

        // Leer los píxeles y extraer el canal rojo
        unsigned char pixel[3];
        for (int i = 0; i < ancho * alto; i++) {
            fread(pixel, sizeof(unsigned char), 3, file);
            fprintf(output, "%d\n", pixel[0]);  // Canal rojo
        }
        fclose(output);
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        FILE *output = fopen("rojo.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);
                unsigned char rojo = pixel[2];  // Canal rojo
                fwrite(&rojo, sizeof(unsigned char), 1, output);
                fwrite("\0\0", sizeof(unsigned char), 2, output);  // Rellenar con 0 para G y B
            }
        }
        fclose(output);
    }
    fclose(file);
}

// Función para separar la matriz verde
void separarMatrizVerde(const char *archivo) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Verificar el tipo de archivo (PNM o BMP)
    if (archivo[strlen(archivo) - 3] == 'p') {  // PNM
        FILE *output = fopen("verde.pnm", "w");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }
        char type[3];
        int ancho, alto, valorMax;
        fscanf(file, "%s\n%d %d\n%d\n", type, &ancho, &alto, &valorMax);
        fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

        // Leer los píxeles y extraer el canal verde
        unsigned char pixel[3];
        for (int i = 0; i < ancho * alto; i++) {
            fread(pixel, sizeof(unsigned char), 3, file);
            fprintf(output, "%d\n", pixel[1]);  // Canal verde
        }
        fclose(output);
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        FILE *output = fopen("verde.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);
                unsigned char verde = pixel[1];  // Canal verde
                fwrite("\0", sizeof(unsigned char), 1, output);  // Rellenar con 0 para R
                fwrite(&verde, sizeof(unsigned char), 1, output);
                fwrite("\0", sizeof(unsigned char), 1, output);  // Rellenar con 0 para B
            }
        }
        fclose(output);
    }
    fclose(file);
}

// Función para separar la matriz azul
void separarMatrizAzul(const char *archivo) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Verificar el tipo de archivo (PNM o BMP)
    if (archivo[strlen(archivo) - 3] == 'p') {  // PNM
        FILE *output = fopen("azul.pnm", "w");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }
        char type[3];
        int ancho, alto, valorMax;
        fscanf(file, "%s\n%d %d\n%d\n", type, &ancho, &alto, &valorMax);
        fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

        // Leer los píxeles y extraer el canal azul
        unsigned char pixel[3];
        for (int i = 0; i < ancho * alto; i++) {
            fread(pixel, sizeof(unsigned char), 3, file);
            fprintf(output, "%d\n", pixel[2]);  // Canal azul
        }
        fclose(output);
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        FILE *output = fopen("azul.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);
                unsigned char azul = pixel[0];  // Canal azul
                fwrite("\0\0", sizeof(unsigned char), 2, output);  // Rellenar con 0 para R y G
                fwrite(&azul, sizeof(unsigned char), 1, output);
            }
        }
        fclose(output);
    }
    fclose(file);
}

// Función para convertir a escala de grises
void generarImagenGrises(const char *archivoNombre) {
    FILE *img = fopen(archivoNombre, "rb");
    if (!img) {
        printf("Error al abrir el archivo de imagen.\n");
        return;
    }

    // Leer la cabecera de la imagen
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, img);

    int ancho = *(int*)&header[18], alto = *(int*)&header[22];
    int rowPadded = (ancho * 3 + 3) & (~3); // Alineación de filas para imágenes BMP

    unsigned char *data = (unsigned char *)malloc(rowPadded);
    if (!data) {
        printf("Error al asignar memoria.\n");
        fclose(img);
        return;
    }

    // Crear archivo de salida para la imagen en escala de grises
    FILE *output = fopen("grises_image.bmp", "wb");
    if (!output) {
        printf("Error al crear el archivo de salida.\n");
        fclose(img);
        free(data);
        return;
    }

    // Modificar la cabecera para que la imagen sea de 8 bits por píxel (escala de grises)
    header[28] = 8;  // Cambiar a 8 bits por píxel (1 byte por píxel)
    *(int*)&header[34] = 256 * 256;  // Tamaño de la tabla de colores (256 colores)

    // Escribir la nueva cabecera en el archivo de salida
    fwrite(header, sizeof(unsigned char), 54, output);

    // Escribir la tabla de colores (256 tonos de grises)
    for (int i = 0; i < 256; i++) {
        fputc(i, output);  // Rojo
        fputc(i, output);  // Verde
        fputc(i, output);  // Azul
        fputc(0, output);  // Reservado
    }

    // Leer los píxeles de la imagen, convertir a escala de grises y escribir en el archivo de salida
    for (int i = 0; i < alto; i++) {
        fread(data, sizeof(unsigned char), rowPadded, img);

        for (int j = 0; j < ancho; j++) {
            unsigned char r = data[j * 3 + 2]; // Canal rojo
            unsigned char g = data[j * 3 + 1]; // Canal verde
            unsigned char b = data[j * 3];     // Canal azul

            // Promedio de los valores RGB para obtener el valor en escala de grises
            unsigned char gray = (r + g + b) / 3;

            // Escribir el valor de gris en el archivo de salida (8 bits por píxel)
            fputc(gray, output);
        }
    }

    printf("La imagen ha sido convertida a escala de grises.\n");

    fclose(img);
    fclose(output);
    free(data);
}


// Función para convertir a blanco y negro
void generarImagenBN(const char *archivoNombre, int threshold) {
    FILE *img = fopen(archivoNombre, "rb");
    if (!img) {
        printf("Error al abrir el archivo de imagen.\n");
        return;
    }

    // Leer la cabecera de la imagen
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, img);

    int ancho = *(int*)&header[18], alto = *(int*)&header[22];
    int rowPadded = (ancho * 3 + 3) & (~3); // Alineación de filas para imágenes BMP

    unsigned char *data = (unsigned char *)malloc(rowPadded);
    if (!data) {
        printf("Error al asignar memoria.\n");
        fclose(img);
        return;
    }

    // Crear archivo de salida para la imagen en blanco y negro
    FILE *output = fopen("black_and_white_image.bmp", "wb");
    if (!output) {
        printf("Error al crear el archivo de salida.\n");
        fclose(img);
        free(data);
        return;
    }

    // Escribir la cabecera de la imagen de salida (misma cabecera que la original)
    fwrite(header, sizeof(unsigned char), 54, output);

    // Leer los píxeles de la imagen, convertir a escala de grises y aplicar el umbral
    for (int i = 0; i < alto; i++) {
        fread(data, sizeof(unsigned char), rowPadded, img);

        for (int j = 0; j < ancho; j++) {
            unsigned char r = data[j * 3 + 2]; // Canal rojo
            unsigned char g = data[j * 3 + 1]; // Canal verde
            unsigned char b = data[j * 3];     // Canal azul

            // Convertir a gris (promedio de los tres canales)
            unsigned char gray = (r + g + b) / 3;

            // Aplicar umbral: blanco o negro
            unsigned char bw = (gray >= threshold) ? 255 : 0;

            // Escribir el píxel en el archivo de salida (blanco o negro)
            fputc(bw, output);
            fputc(bw, output);
            fputc(bw, output);
        }
    }

    printf("La imagen ha sido convertida a blanco y negro con umbral %d.\n", threshold);

    fclose(img);
    fclose(output);
    free(data);
}


// Cálculo del histograma
void calcularHistograma(const char *archivo) {
    FILE *fp = fopen(archivo, "rb");
    if (!fp) {
        perror("Error al abrir el archivo");
        return;
    }

    // Identificar el formato del archivo
    char formato[3];
    fscanf(fp, "%2s", formato);

    // Variables comunes
    int ancho, alto, valorMaxor;
    fscanf(fp, "%d %d %d", &ancho, &alto, &valorMaxor);

    // Validar el formato de la imagen
    if (strcmp(formato, "P2") == 0 || strcmp(formato, "P5") == 0) {
        // Imagen en escala de grises
        int histograma[256] = {0}; // Inicializar el histograma

        // Leer los valores de la imagen y actualizar el histograma
        int valor;
        while (fscanf(fp, "%d", &valor) == 1) {
            histograma[valor]++;
        }

        // Escribir el histograma en un archivo
        FILE *out = fopen("histGris.txt", "w");
        if (!out) {
            perror("Error al crear el archivo de salida");
            fclose(fp);
            return;
        }

        fprintf(out, "Tono\tValor\tHistograma\n");
        for (int i = 0; i < 256; i++) {
            fprintf(out, "%3d\t%5d\t", i, histograma[i]);
            for (int j = 0; j < histograma[i]; j++) {
                fprintf(out, "*");
            }
            fprintf(out, "\n");
        }

        fclose(out);
        printf("Histograma generado en 'histGris.txt'.\n");
    } else if (strcmp(formato, "P3") == 0 || strcmp(formato, "P6") == 0) {
        // Imagen a color
        int histR[256] = {0};
        int histG[256] = {0};
        int histB[256] = {0};

        // Leer los valores de los canales y actualizar los histogramas
        int r, g, b;
        while (fscanf(fp, "%d %d %d", &r, &g, &b) == 3) {
            histR[r]++;
            histG[g]++;
            histB[b]++;
        }

        // Escribir los histogramas en archivos separados
        FILE *outR = fopen("histR.txt", "w");
        FILE *outG = fopen("histG.txt", "w");
        FILE *outB = fopen("histB.txt", "w");

        if (!outR || !outG || !outB) {
            perror("Error al crear los archivos de salida");
            fclose(fp);
            return;
        }

        // Escribir histograma R
        fprintf(outR, "Tono\tValor\tHistograma\n");
        for (int i = 0; i < 256; i++) {
            fprintf(outR, "%3d\t%5d\t", i, histR[i]);
            for (int j = 0; j < histR[i]; j++) {
                fprintf(outR, "*");
            }
            fprintf(outR, "\n");
        }
        fclose(outR);

        // Escribir histograma G
        fprintf(outG, "Tono\tValor\tHistograma\n");
        for (int i = 0; i < 256; i++) {
            fprintf(outG, "%3d\t%5d\t", i, histG[i]);
            for (int j = 0; j < histG[i]; j++) {
                fprintf(outG, "*");
            }
            fprintf(outG, "\n");
        }
        fclose(outG);

        // Escribir histograma B
        fprintf(outB, "Tono\tValor\tHistograma\n");
        for (int i = 0; i < 256; i++) {
            fprintf(outB, "%3d\t%5d\t", i, histB[i]);
            for (int j = 0; j < histB[i]; j++) {
                fprintf(outB, "*");
            }
            fprintf(outB, "\n");
        }
        fclose(outB);

        printf("Histogramas generados en 'histR.txt', 'histG.txt' y 'histB.txt'.\n");
    } else {
        printf("Formato de archivo no soportado.\n");
    }

    fclose(fp);
}


// Mezclar dos imágenes
void mezclarImagenes(const char *file1, const char *file2, int alpha) {
    FILE *img1 = fopen(file1, "rb");
    FILE *img2 = fopen(file2, "rb");

    if (!img1 || !img2) {
        printf("Error al abrir uno de los archivos de imagen.\n");
        return;
    }

    // Leer las cabeceras de ambas imágenes
    unsigned char header1[54], header2[54];
    fread(header1, sizeof(unsigned char), 54, img1);
    fread(header2, sizeof(unsigned char), 54, img2);

    // Verificar que ambas imágenes tengan el mismo tamaño
    int ancho1 = *(int*)&header1[18], alto1 = *(int*)&header1[22];
    int ancho2 = *(int*)&header2[18], alto2 = *(int*)&header2[22];

    if (ancho1 != ancho2 || alto1 != alto2) {
        printf("Las imágenes no tienen el mismo tamaño.\n");
        fclose(img1);
        fclose(img2);
        return;
    }

    // Leer datos de las imágenes
    int rowPadded1 = (ancho1 * 3 + 3) & (~3);
    int rowPadded2 = (ancho2 * 3 + 3) & (~3);
    unsigned char *data1 = (unsigned char *)malloc(rowPadded1);
    unsigned char *data2 = (unsigned char *)malloc(rowPadded2);

    // Crear archivo de salida para la imagen mezclada
    FILE *output = fopen("mixed_image_with_alpha.pnm", "wb");
    if (!output) {
        printf("Error al crear el archivo de salida.\n");
        fclose(img1);
        fclose(img2);
        free(data1);
        free(data2);
        return;
    }

    // Escribir la cabecera de la imagen PNM
    fprintf(output, "P6\n%d %d\n255\n", ancho1, alto1);

    // Mezclar las imágenes píxel por píxel
    for (int i = 0; i < alto1; i++) {
        fread(data1, sizeof(unsigned char), rowPadded1, img1);
        fread(data2, sizeof(unsigned char), rowPadded2, img2);

        for (int j = 0; j < ancho1 * 3; j += 3) {
            unsigned char r_f = data1[j + 2], g_f = data1[j + 1], b_f = data1[j]; // FRENTE
            unsigned char r_b = data2[j + 2], g_b = data2[j + 1], b_b = data2[j]; // FONDO

            // Mezcla utilizando la fórmula
            unsigned char r_m = (r_f * alpha) / 256 + (r_b * (255 - alpha)) / 256;
            unsigned char g_m = (g_f * alpha) / 256 + (g_b * (255 - alpha)) / 256;
            unsigned char b_m = (b_f * alpha) / 256 + (b_b * (255 - alpha)) / 256;

            // Escribir los valores mezclados en el archivo de salida
            fputc(r_m, output);
            fputc(g_m, output);
            fputc(b_m, output);
        }
    }

    printf("Las imágenes se han mezclado y guardado en mixed_image_with_alpha.pnm.\n");

    fclose(img1);
    fclose(img2);
    fclose(output);
    free(data1);
    free(data2);
}


void realizarTodosProcesos(int umbral, int alpha, const char *archivo1, const char *archivo2) {
    // Paso 1: Separar la imagen en canales R, G y B
    printf("Separando la imagen en canales R, G, B...\n");
    separarImagenesPorCanal(archivo1);  // Asumiendo que archivo1 es una imagen PNM
    
    // Paso 2: Generar la imagen en escala de grises (para archivo1, que podría ser BMP)
    printf("Generando la imagen en escala de grises...\n");
    generarImagenGrises(archivo1);  // Asumiendo que archivo1 es una imagen BMP
    
    // Paso 3: Calcular el histograma para la imagen
    printf("Calculando el histograma...\n");
    calcularHistograma(archivo1);  // Se calcula el histograma para archivo1 (puede ser PNM o BMP)
    
    // Paso 4: Generar la imagen en blanco y negro con un umbral (para archivo2, que podría ser BMP)
    printf("Generando imagen en blanco y negro con umbral...\n");
    generarImagenBN(archivo2, umbral);  // Asumiendo que archivo2 es BMP y tiene un umbral
    
    // Paso 5: Mezclar las imágenes usando el valor de alpha
    printf("Mezclando imágenes...\n");
    mezclarImagenes(archivo1, archivo2, alpha);  // Mezcla archivo1 con archivo2 utilizando el valor de alpha
    
    // Mostrando la finalización de los procesos
    printf("Todos los procesos se han completado.\n");
}
