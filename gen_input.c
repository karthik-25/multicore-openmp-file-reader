#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define FILE_SIZE_GB 1
#define BUFFER_SIZE 1024

void create_new_random_file(){
    FILE *file;
    char buffer[BUFFER_SIZE];
    srand(time(NULL));

    file = fopen("random_file.txt", "wb");

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (ftell(file) < FILE_SIZE_GB * 1024L* 1024L* 1024L ) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            buffer[i] = 'A' + rand() % 26;  // Random characters (0 to 255)
        }

        fwrite(buffer, sizeof(char), BUFFER_SIZE, file);
    }

    fclose(file);

    printf("File created successfully for %dGB.\n",FILE_SIZE_GB);

}

int main(){
    srand((unsigned int)10);
    create_new_random_file();
    return 0;
}