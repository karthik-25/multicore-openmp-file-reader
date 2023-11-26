#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>

// mention assumptions in report, that no error checking is done. its assumed input is valid and I/O operations are successful

#define FILE_SIZE_GB 1
#define BUFFER_SIZE 1024
#define CHUNK_SIZE 1024

int main(int argc, char *argv[]) {
    const char* filepath = argv[1];
    int t = atoi(argv[2]);

    int fd = open(filepath, O_RDONLY);

    struct stat sb;
    fstat(fd, &sb);

    char* file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    int chunk_size = sb.st_size / t;
    printf("%d\n", chunk_size);

    int global_counter = 0;
    double start, end;

    start = omp_get_wtime();
    #pragma omp parallel for num_threads(t) reduction(+:global_counter)
    for (int i = 0; i < t; i++) {
        char *chunk_start = file_data + i * chunk_size;
        int cur_chunk_size = (i == t - 1) ? sb.st_size - i * chunk_size : chunk_size;
        
        for (int j = 0; j < cur_chunk_size; j++) {
            if (*(chunk_start + j) == 'a') {
                global_counter++;
            }
        }
        fprintf(stdout, "Thread: %d | first char: %c | chunk size: %d | last char: %c\n", omp_get_thread_num(), *chunk_start, (int) cur_chunk_size, *(chunk_start + cur_chunk_size - 1));
    }
    end = omp_get_wtime();

    printf("There are %d 'a's in the file.\n", global_counter);
    printf("Execution time: %f\n", end-start);

    close(fd);
    return 0;
}