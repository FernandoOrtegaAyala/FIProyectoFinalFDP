// Importación de librerías necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Declaración de constantes
#define MAX_CHANNELS 256
#define THRESHOLD 127 // Umbral para blanco y negro

// Prototipos de funciones
void mostrarAyuda();
void separarImagenesPorCanal(const char *archivo);
void desplegarCabeceraBMP(const char *archivoNombre);
void desplegarCabeceraPNM(const char *archivoNombre);
void generarImagenGrises(const char *archivo);
void generarImagenBlancoNegro(const char *archivo, int umbral);
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
        // Llamada a la función con el archivo de imagen y el umbral
        generarImagenBlancoNegro(argv[3], umbral);
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
    // Verificar que el archivo existe
    FILE *test = fopen(archivo, "rb");
    if (!test) {
        printf("Error: No se puede abrir el archivo %s\n", archivo);
        return;
    }
    fclose(test);

    // Verificar la extensión del archivo
    size_t len = strlen(archivo);
    if (len < 4) {
        printf("Error: Nombre de archivo inválido\n");
        return;
    }

    // Verificar que sea un archivo .pnm o .bmp
    const char *extension = archivo + len - 3;
    if (strcmp(extension, "pnm") != 0 && strcmp(extension, "bmp") != 0) {
        printf("Error: El archivo debe ser .pnm o .bmp\n");
        return;
    }

    // Desplegar información de la cabecera
    if (strcmp(extension, "pnm") == 0) {
        desplegarCabeceraPNM(archivo);
    }
    else if (strcmp(extension, "bmp") == 0) {
        desplegarCabeceraBMP(archivo);
    }

    // Separar la imagen en sus tres canales
    printf("Separando imagen en canales RGB...\n");
    
    printf("Procesando canal rojo...\n");
    separarMatrizRoja(archivo);
    
    printf("Procesando canal verde...\n");
    separarMatrizVerde(archivo);
    
    printf("Procesando canal azul...\n");
    separarMatrizAzul(archivo);
    
    printf("Proceso completado.\n");
    printf("Archivos generados:\n");
    printf("- rojo.%s\n", extension);
    printf("- verde.%s\n", extension);
    printf("- azul.%s\n", extension);
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
        // Variables para almacenar la información de la cabecera
        char type[3];
        int ancho, alto, valorMax;
        
        // Leer la cabecera del archivo PNM
        fscanf(file, "%s", type);  // P6 o P3
        fscanf(file, "%d %d", &ancho, &alto);  // Ancho y alto
        fscanf(file, "%d", &valorMax);  // Valor máximo de color (255 normalmente)

        // Mostar la información de la cabecera PNM
        desplegarCabeceraPNM(archivo);
        
        // Verificar el tipo de archivo PNM
        if (strcmp(type, "P6") == 0) {
            // Manejo de archivo binario P6
            FILE *output = fopen("rojo.pnm", "wb");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            unsigned char pixel[3];
            for (int i = 0; i < ancho * alto; i++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel
                fwrite(&pixel[0], sizeof(unsigned char), 1, output);  // Canal rojo
                fwrite("\0\0", sizeof(unsigned char), 2, output);  // Rellenar con 0 para G y B
            }

            fclose(output);
        }
        else if (strcmp(type, "P3") == 0) {
            // Manejo de archivo P3 (texto)
            FILE *output = fopen("rojo.pnm", "w");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            int r, g, b;
            for (int i = 0; i < ancho * alto; i++) {
                fscanf(file, "%d %d %d", &r, &g, &b);  // Leer un píxel
                fprintf(output, "%d\n", r);  // Canal rojo
                fprintf(output, "0\n");      // Canal verde a 0
                fprintf(output, "0\n");      // Canal azul a 0
            }

            fclose(output);
        }
        else {
            printf("Formato PNM no soportado: %s\n", type);
        }
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP Si el archivo es de este formato se realizan los siguientes procesos
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        int padding = (4 - (ancho * 3) % 4) % 4;  // Calcular el relleno

        FILE *output = fopen("rojo.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }


        // Desplegar la información de la cabecera BMP
        desplegarCabeceraBMP(archivo);

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel

                // El canal rojo está en la tercera posición (índice 2) para BMP (BGR)
                unsigned char rojo = pixel[2];  // Canal rojo (BMP usa BGR, no RGB)

                // Escribir solo el canal rojo y rellenar con ceros para G y B
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal azul a 0
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal verde a 0
                fwrite(&rojo, sizeof(unsigned char), 1, output);  // Canal rojo
            }
            // Saltar el relleno en el archivo de entrada
            fseek(file, padding, SEEK_CUR);
            // Añadir el relleno en el archivo de salida
            for (int p = 0; p < padding; p++) {
                fwrite("\0", sizeof(unsigned char), 1, output);
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
        char type[3];
        int ancho, alto, valorMax;
        
        // Leer la cabecera del archivo PNM
        fscanf(file, "%s", type);  // P6 o P3
        fscanf(file, "%d %d", &ancho, &alto);  // Ancho y alto
        fscanf(file, "%d", &valorMax);  // Valor máximo de color (255 normalmente)

        // Mostar la información de la cabecera PNM
        desplegarCabeceraPNM(archivo);
        
        // Verificar el tipo de archivo PNM
        if (strcmp(type, "P6") == 0) {
            // Manejo de archivo binario P6
            FILE *output = fopen("verde.pnm", "wb");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            unsigned char pixel[3];
            for (int i = 0; i < ancho * alto; i++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal rojo a 0
                fwrite(&pixel[1], sizeof(unsigned char), 1, output);  // Canal verde
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal azul a 0
            }

            fclose(output);
        }
        else if (strcmp(type, "P3") == 0) {
            // Manejo de archivo P3 (texto)
            FILE *output = fopen("verde.pnm", "w");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            int r, g, b;
            for (int i = 0; i < ancho * alto; i++) {
                fscanf(file, "%d %d %d", &r, &g, &b);  // Leer un píxel
                fprintf(output, "0\n");      // Canal rojo a 0
                fprintf(output, "%d\n", g);  // Canal verde
                fprintf(output, "0\n");      // Canal azul a 0
            }

            fclose(output);
        }
        else {
            printf("Formato PNM no soportado: %s\n", type);
        }
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP Si el archivo es de este formato se realizan los siguientes procesos
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        int padding = (4 - (ancho * 3) % 4) % 4;  // Calcular el relleno

        FILE *output = fopen("verde.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Mostar la información de la cabecera BMP
        desplegarCabeceraBMP(archivo);

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel

                // El canal verde está en la segunda posición (índice 1) para BMP (BGR)
                unsigned char verde = pixel[1];  // Canal verde (BMP usa BGR, no RGB)

                // Escribir solo el canal verde y rellenar con ceros para R y B
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal azul a 0
                fwrite(&verde, sizeof(unsigned char), 1, output);  // Canal verde
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal rojo a 0
            }
            // Saltar el relleno en el archivo de entrada
            fseek(file, padding, SEEK_CUR);
            // Añadir el relleno en el archivo de salida
            for (int p = 0; p < padding; p++) {
                fwrite("\0", sizeof(unsigned char), 1, output);
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
        char type[3];
        int ancho, alto, valorMax;
        
        // Leer la cabecera del archivo PNM
        fscanf(file, "%s", type);  // P6 o P3
        fscanf(file, "%d %d", &ancho, &alto);  // Ancho y alto
        fscanf(file, "%d", &valorMax);  // Valor máximo de color (255 normalmente)

        // Mostar la información de la cabecera PNM
        desplegarCabeceraPNM(archivo);
        
        // Verificar el tipo de archivo PNM
        if (strcmp(type, "P6") == 0) {
            // Manejo de archivo binario P6
            FILE *output = fopen("azul.pnm", "wb");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            unsigned char pixel[3];
            for (int i = 0; i < ancho * alto; i++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal rojo a 0
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal verde a 0
                fwrite(&pixel[2], sizeof(unsigned char), 1, output);  // Canal azul
            }

            fclose(output);
        }
        else if (strcmp(type, "P3") == 0) {
            // Manejo de archivo P3 (texto)
            FILE *output = fopen("azul.pnm", "w");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles
            int r, g, b;
            for (int i = 0; i < ancho * alto; i++) {
                fscanf(file, "%d %d %d", &r, &g, &b);  // Leer un píxel
                fprintf(output, "0\n");      // Canal rojo a 0
                fprintf(output, "0\n");      // Canal verde a 0
                fprintf(output, "%d\n", b);  // Canal azul
            }

            fclose(output);
        }
        else {
            printf("Formato PNM no soportado: %s\n", type);
        }
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP Si el archivo es de este formato se realizan los siguientes procesos
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        int padding = (4 - (ancho * 3) % 4) % 4;  // Calcular el relleno

        FILE *output = fopen("azul.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        // Mostar la información de la cabecera BMP
        desplegarCabeceraBMP(archivo);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel

                // El canal azul está en la primera posición (índice 0) para BMP (BGR)
                unsigned char azul = pixel[0];  // Canal azul (BMP usa BGR, no RGB)

                // Escribir solo el canal azul y rellenar con ceros para R y G
                fwrite(&azul, sizeof(unsigned char), 1, output);  // Canal azul
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal verde a 0
                fwrite("\0", sizeof(unsigned char), 1, output);  // Canal rojo a 0
            }
            // Saltar el relleno en el archivo de entrada
            fseek(file, padding, SEEK_CUR);
            // Añadir el relleno en el archivo de salida
            for (int p = 0; p < padding; p++) {
                fwrite("\0", sizeof(unsigned char), 1, output);
            }
        }

        fclose(output);
    }

    fclose(file);
}

// Función para convertir a escala de grises
void generarImagenGrises(const char *archivo) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Verificar el tipo de archivo (PNM o BMP)
    if (archivo[strlen(archivo) - 3] == 'p') {  // PNM
        char type[3];
        int ancho, alto, valorMax;
        
        // Leer la cabecera del archivo PNM
        fscanf(file, "%s", type);  // P6 o P3
        fscanf(file, "%d %d", &ancho, &alto);  // Ancho y alto
        fscanf(file, "%d", &valorMax);  // Valor máximo de color (255 normalmente)

        // Mostar la información de la cabecera PNM
        desplegarCabeceraPNM(archivo);
        
        // Verificar el tipo de archivo PNM
        if (strcmp(type, "P6") == 0) {
            // Manejo de archivo binario P6
            FILE *output = fopen("escala_de_grises.pnm", "wb");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles y convertir a escala de grises
            unsigned char pixel[3];
            for (int i = 0; i < ancho * alto; i++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel
                
                // Calcular el valor de gris
                unsigned char gris = (pixel[0] + pixel[1] + pixel[2]) / 3;

                // Escribir el mismo valor en los tres canales para la escala de grises
                fwrite(&gris, sizeof(unsigned char), 1, output);
                fwrite(&gris, sizeof(unsigned char), 1, output);
                fwrite(&gris, sizeof(unsigned char), 1, output);
            }

            fclose(output);
        }
        else if (strcmp(type, "P3") == 0) {
            // Manejo de archivo P3 (texto)
            FILE *output = fopen("escala_de_grises.pnm", "w");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y separar los píxeles y convertir a escala de grises
            int r, g, b;
            for (int i = 0; i < ancho * alto; i++) {
                fscanf(file, "%d %d %d", &r, &g, &b);  // Leer un píxel

                // Calcular el valor de gris
                int gris = (r + g + b) / 3;

                // Escribir el valor gris en los tres canales
                fprintf(output, "%d\n", gris);  // R
                fprintf(output, "%d\n", gris);  // G
                fprintf(output, "%d\n", gris);  // B
            }

            fclose(output);
        }
        else {
            printf("Formato PNM no soportado: %s\n", type);
        }
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        FILE *output = fopen("escala_de_grises.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        // Mostar la información de la cabecera BMP
        desplegarCabeceraBMP(archivo);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel

                // Calcular el valor de gris
                unsigned char gris = (pixel[0] + pixel[1] + pixel[2]) / 3;

                // Escribir el mismo valor en los tres canales para la escala de grises
                fwrite(&gris, sizeof(unsigned char), 1, output);  // Canal azul (en BMP)
                fwrite(&gris, sizeof(unsigned char), 1, output);  // Canal verde
                fwrite(&gris, sizeof(unsigned char), 1, output);  // Canal rojo
            }
        }

        fclose(output);
    }

    fclose(file);
}

// Función para convertir a blanco y negro
void generarImagenBlancoNegro(const char *archivo, int umbral) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Verificar el tipo de archivo (PNM o BMP)
    if (archivo[strlen(archivo) - 3] == 'p') {  // PNM
        char type[3];
        int ancho, alto, valorMax;
        
        // Leer la cabecera del archivo PNM
        fscanf(file, "%s", type);  // P6 o P3
        fscanf(file, "%d %d", &ancho, &alto);  // Ancho y alto
        fscanf(file, "%d", &valorMax);  // Valor máximo de color (255 normalmente)

        // Mostrar la cabecera PNM
        desplegarCabeceraPNM(archivo);
        
        // Verificar el tipo de archivo PNM
        if (strcmp(type, "P6") == 0) {
            // Manejo de archivo binario P6
            FILE *output = fopen("blanco_y_negro.pnm", "wb");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y convertir los píxeles a blanco y negro según el umbral
            unsigned char pixel[3];
            for (int i = 0; i < ancho * alto; i++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel
                
                // Calcular el valor de gris (promedio de R, G y B)
                unsigned char gris = (pixel[0] + pixel[1] + pixel[2]) / 3;

                // Convertir a blanco o negro según el umbral
                unsigned char blancoYNegro = (gris >= umbral) ? 255 : 0;

                // Escribir el valor blanco o negro en los tres canales
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal rojo
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal verde
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal azul
            }

            fclose(output);
        }
        else if (strcmp(type, "P3") == 0) {
            // Manejo de archivo P3 (texto)
            FILE *output = fopen("blanco_y_negro.pnm", "w");
            if (!output) {
                printf("Error al abrir el archivo de salida\n");
                fclose(file);
                return;
            }

            // Escribir la cabecera en el archivo de salida
            fprintf(output, "%s\n%d %d\n%d\n", type, ancho, alto, valorMax);

            // Leer y convertir los píxeles a blanco y negro según el umbral
            int r, g, b;
            for (int i = 0; i < ancho * alto; i++) {
                fscanf(file, "%d %d %d", &r, &g, &b);  // Leer un píxel

                // Calcular el valor de gris
                int gris = (r + g + b) / 3;

                // Convertir a blanco o negro según el umbral
                int blancoYNegro = (gris >= umbral) ? 255 : 0;

                // Escribir el valor blanco o negro en los tres canales
                fprintf(output, "%d\n", blancoYNegro);  // R
                fprintf(output, "%d\n", blancoYNegro);  // G
                fprintf(output, "%d\n", blancoYNegro);  // B
            }

            fclose(output);
        }
        else {
            printf("Formato PNM no soportado: %s\n", type);
        }
    }
    else if (archivo[strlen(archivo) - 3] == 'b') {  // BMP
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, file);

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];
        FILE *output = fopen("blanco_y_negro.bmp", "wb");
        if (!output) {
            printf("Error al abrir el archivo de salida\n");
            fclose(file);
            return;
        }

        // Mostrar la cabecera BMP
        desplegarCabeceraBMP(archivo);

        // Copiar el encabezado al archivo de salida
        fwrite(header, sizeof(unsigned char), 54, output);

        unsigned char pixel[3];
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                fread(pixel, sizeof(unsigned char), 3, file);  // Leer píxel

                // Calcular el valor de gris (promedio de B, G y R en BMP)
                unsigned char gris = (pixel[0] + pixel[1] + pixel[2]) / 3;

                // Convertir a blanco o negro según el umbral
                unsigned char blancoYNegro = (gris >= umbral) ? 255 : 0;

                // Escribir el valor blanco o negro en los tres canales
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal azul (en BMP)
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal verde
                fwrite(&blancoYNegro, sizeof(unsigned char), 1, output);  // Canal rojo
            }
        }

        fclose(output);
    }

    fclose(file);
}

// Función auxiliar para convertir string a minúsculas
void strToLower(char *str) {
    for(int i = 0; str[i]; i++) {
        if(str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32;
        }
    }
}

void calcularHistograma(const char *archivo) {
    FILE *file = fopen(archivo, "rb");
    if (!file) {
        printf("Error al abrir el archivo %s\n", archivo);
        return;
    }

    // Determinar el tipo de archivo por la extensión
    const char *extension = strrchr(archivo, '.');
    if (!extension) {
        printf("Error: archivo sin extensión\n");
        fclose(file);
        return;
    }

    // Crear una copia de la extensión para convertirla a minúsculas
    char ext[5];
    strncpy(ext, extension, 4);
    ext[4] = '\0';
    strToLower(ext);

    if (strcmp(ext, ".pnm") == 0) {
        char tipo[4];  // Espacio para P3/P6 y el terminador nulo
        if (fscanf(file, "%3s", tipo) != 1) {
            printf("Error al leer el tipo de archivo PNM\n");
            fclose(file);
            return;
        }

        // Mostrar la cabecera PNM
        desplegarCabeceraPNM(archivo);

        // Saltar comentarios
        int c;
        while ((c = fgetc(file)) == '#') {
            while ((c = fgetc(file)) != '\n' && c != EOF);
        }
        ungetc(c, file);

        // Leer dimensiones y valor máximo
        int ancho, alto, valorMax;
        if (fscanf(file, "%d %d %d", &ancho, &alto, &valorMax) != 3) {
            printf("Error al leer las dimensiones o valor máximo\n");
            fclose(file);
            return;
        }
        fgetc(file); // Consumir el último salto de línea

        // Verificar dimensiones válidas
        if (ancho <= 0 || alto <= 0 || valorMax <= 0 || valorMax > 255) {
            printf("Dimensiones o valor máximo inválidos\n");
            fclose(file);
            return;
        }

        // Verificar posible overflow en la asignación de memoria
        if ((long long)ancho * alto > INT_MAX / 3) {
            printf("Imagen demasiado grande para procesar\n");
            fclose(file);
            return;
        }

        if (strcmp(tipo, "P6") == 0 || strcmp(tipo, "P3") == 0) {
            // Imagen a color
            unsigned char *pixels = (unsigned char *)malloc(ancho * alto * 3);
            if (!pixels) {
                printf("Error al asignar memoria para los píxeles\n");
                fclose(file);
                return;
            }

            if (strcmp(tipo, "P6") == 0) {
                // Formato binario
                if (fread(pixels, 1, ancho * alto * 3, file) != (size_t)(ancho * alto * 3)) {
                    printf("Error al leer los píxeles\n");
                    free(pixels);
                    fclose(file);
                    return;
                }
            } else {
                // Formato texto (P3)
                for (int i = 0; i < ancho * alto * 3; i++) {
                    int valor;
                    if (fscanf(file, "%d", &valor) != 1 || valor < 0 || valor > valorMax) {
                        printf("Error al leer valor de píxel\n");
                        free(pixels);
                        fclose(file);
                        return;
                    }
                    pixels[i] = (unsigned char)((valor * 255) / valorMax);
                }
            }

            // Calcular histogramas RGB
            int histR[256] = {0}, histG[256] = {0}, histB[256] = {0};
            for (int i = 0; i < ancho * alto; i++) {
                histR[pixels[i * 3]]++;
                histG[pixels[i * 3 + 1]]++;
                histB[pixels[i * 3 + 2]]++;
            }

            // Guardar histogramas
            FILE *histFiles[3];
            const char *fileNames[3] = {"histR.txt", "histG.txt", "histB.txt"};
            int *hists[3] = {histR, histG, histB};

            for (int i = 0; i < 3; i++) {
                histFiles[i] = fopen(fileNames[i], "w");
                if (!histFiles[i]) {
                    printf("Error al crear archivo %s\n", fileNames[i]);
                    for (int j = 0; j < i; j++) fclose(histFiles[j]);
                    free(pixels);
                    fclose(file);
                    return;
                }

                for (int j = 0; j < 256; j++) {
                    fprintf(histFiles[i], "%d %d ", j, hists[i][j]);
                    for (int k = 0; k < hists[i][j] / 100; k++) { // Escalado para mejor visualización
                        fprintf(histFiles[i], "*");
                    }
                    fprintf(histFiles[i], "\n");
                }
                fclose(histFiles[i]);
            }

            free(pixels);
            printf("Histogramas RGB generados exitosamente\n");

        } else if (strcmp(tipo, "P5") == 0) {
            // Imagen en escala de grises
            unsigned char *pixels = (unsigned char *)malloc(ancho * alto);
            if (!pixels) {
                printf("Error al asignar memoria para los píxeles\n");
                fclose(file);
                return;
            }

            if (fread(pixels, 1, ancho * alto, file) != (size_t)(ancho * alto)) {
                printf("Error al leer los píxeles\n");
                free(pixels);
                fclose(file);
                return;
            }

            // Calcular histograma
            int histGris[256] = {0};
            for (int i = 0; i < ancho * alto; i++) {
                histGris[pixels[i]]++;
            }

            // Guardar histograma
            FILE *histFile = fopen("histGris.txt", "w");
            if (!histFile) {
                printf("Error al crear archivo de histograma\n");
                free(pixels);
                fclose(file);
                return;
            }

            for (int i = 0; i < 256; i++) {
                fprintf(histFile, "%d %d ", i, histGris[i]);
                for (int j = 0; j < histGris[i] / 100; j++) { // Escalado para mejor visualización
                    fprintf(histFile, "*");
                }
                fprintf(histFile, "\n");
            }

            fclose(histFile);
            free(pixels);
            printf("Histograma en escala de grises generado exitosamente\n");

        } else {
            printf("Formato PNM no soportado: %s\n", tipo);
            fclose(file);
            return;
        }

    } else if (strcmp(ext, ".bmp") == 0) {
        unsigned char header[54];
        if (fread(header, 1, 54, file) != 54) {
            printf("Error al leer la cabecera BMP\n");
            fclose(file);
            return;
        }

        // Verificar firma BMP
        if (header[0] != 'B' || header[1] != 'M') {
            printf("No es un archivo BMP válido\n");
            fclose(file);
            return;
        }

        // Mostrar la cabecera BMP
        desplegarCabeceraBMP(archivo);

        // Verificar profundidad de color (24 bits)
        if (*(unsigned short*)&header[28] != 24) {
            printf("Solo se soportan imágenes BMP de 24 bits\n");
            fclose(file);
            return;
        }

        int ancho = *(int*)&header[18];
        int alto = *(int*)&header[22];

        // Verificar dimensiones válidas
        if (ancho <= 0 || alto <= 0) {
            printf("Dimensiones inválidas en archivo BMP\n");
            fclose(file);
            return;
        }

        // Calcular padding (cada fila debe ser múltiplo de 4 bytes)
        int padding = (4 - (ancho * 3) % 4) % 4;

        // Verificar posible overflow
        if ((long long)ancho * alto > INT_MAX / 3) {
            printf("Imagen demasiado grande para procesar\n");
            fclose(file);
            return;
        }

        unsigned char *pixels = (unsigned char *)malloc(ancho * alto * 3);
        if (!pixels) {
            printf("Error al asignar memoria para los píxeles\n");
            fclose(file);
            return;
        }

        // Leer píxeles teniendo en cuenta el padding
        for (int y = 0; y < alto; y++) {
            if (fread(&pixels[y * ancho * 3], 1, ancho * 3, file) != (size_t)(ancho * 3)) {
                printf("Error al leer los píxeles\n");
                free(pixels);
                fclose(file);
                return;
            }
            // Saltar el padding
            fseek(file, padding, SEEK_CUR);
        }

        // Calcular histogramas RGB (BMP usa BGR)
        int histR[256] = {0}, histG[256] = {0}, histB[256] = {0};
        for (int i = 0; i < ancho * alto; i++) {
            histB[pixels[i * 3]]++;     // Blue
            histG[pixels[i * 3 + 1]]++; // Green
            histR[pixels[i * 3 + 2]]++; // Red
        }

        // Guardar histogramas
        FILE *histFiles[3];
        const char *fileNames[3] = {"histR.txt", "histG.txt", "histB.txt"};
        int *hists[3] = {histR, histG, histB};

        for (int i = 0; i < 3; i++) {
            histFiles[i] = fopen(fileNames[i], "w");
            if (!histFiles[i]) {
                printf("Error al crear archivo %s\n", fileNames[i]);
                for (int j = 0; j < i; j++) fclose(histFiles[j]);
                free(pixels);
                fclose(file);
                return;
            }

            for (int j = 0; j < 256; j++) {
                fprintf(histFiles[i], "%d %d ", j, hists[i][j]);
                for (int k = 0; k < hists[i][j] / 100; k++) { // Escalado para mejor visualización
                    fprintf(histFiles[i], "*");
                }
                fprintf(histFiles[i], "\n");
            }
            fclose(histFiles[i]);
        }

        free(pixels);
        printf("Histogramas RGB generados exitosamente\n");

    } else {
        printf("Formato de archivo no soportado\n");
        fclose(file);
        return;
    }

    fclose(file);
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
    FILE *output = fopen("imagenMezclada.pnm", "wb");
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

    // Mezclar las imágenes píxel por píxel (invertir filas de img1 para BMP)
    for (int i = alto1 - 1; i >= 0; i--) { // Invertir el recorrido de filas
        fseek(img1, 54 + i * rowPadded1, SEEK_SET);
        fseek(img2, 54 + i * rowPadded2, SEEK_SET);

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

    printf("Las imágenes se han mezclado y guardado en imagenMezclada.pnm.\n");

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
    generarImagenBlancoNegro(archivo2, umbral);  // Asumiendo que archivo2 es BMP y tiene un umbral
    
    // Paso 5: Mezclar las imágenes usando el valor de alpha
    printf("Mezclando imágenes...\n");
    mezclarImagenes(archivo1, archivo2, alpha);  // Mezcla archivo1 con archivo2 utilizando el valor de alpha
    
    // Mostrando la finalización de los procesos
    printf("Todos los procesos se han completado.\n");
}

