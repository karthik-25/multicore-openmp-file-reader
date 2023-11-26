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
    double start, end;
    int global_counter = 0;

    start = omp_get_wtime();

    // open file and get file size
    int fd = open(filepath, O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);

    // calculate number of chunks
    int num_chunks = sb.st_size / CHUNK_SIZE + (sb.st_size % CHUNK_SIZE == 0 ? 0 : 1);

    // memory map file
    char* file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    // read file chunks in parallel
    #pragma omp parallel for num_threads(t) reduction(+:global_counter)
    for (int i = 0; i < num_chunks; i++) {
        char *chunk_start = file_data + i * CHUNK_SIZE;
        int cur_chunk_size = (i == num_chunks - 1) ? sb.st_size - i * CHUNK_SIZE : CHUNK_SIZE;
        
        for (int j = 0; j < cur_chunk_size; j++) {
            if (*(chunk_start + j) == 'A') {
                global_counter++;
            }
        }
        // fprintf(stdout, "Thread: %d | first char: %c | chunk size: %d | last char: %c\n", omp_get_thread_num(), *chunk_start, (int) cur_chunk_size, *(chunk_start + cur_chunk_size - 1));
    }
    end = omp_get_wtime();

    printf("There are %d 'a's in the file.\n", global_counter);
    printf("Execution time: %f\n", end-start);

    close(fd);
    return 0;
}