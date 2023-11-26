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
    int read_counter = 0;

    // get file size and calculate number of chunks
    struct stat sb;
    stat(filepath, &sb);
    int num_chunks = sb.st_size / CHUNK_SIZE + (sb.st_size % CHUNK_SIZE == 0 ? 0 : 1);
    printf("file size: %d\n", sb.st_size);
    printf("num of chunks: %d\n", num_chunks);

    // start timer
    start = omp_get_wtime();

    // open file and memory map it
    int fd = open(filepath, O_RDONLY);
    char* file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    // read file chunks in parallel
    #pragma omp parallel for num_threads(t) reduction(+:read_counter)
    for (int i = 0; i < num_chunks; i++) {
        char *chunk_start = file_data + i * CHUNK_SIZE;
        int cur_chunk_size = (i == num_chunks - 1) ? sb.st_size - i * CHUNK_SIZE : CHUNK_SIZE;
        
        for (int j = 0; j < cur_chunk_size; j++) {
            read_counter++;
        }
    }
    end = omp_get_wtime();

    printf("Total read: %d\n", read_counter);
    printf("Execution time: %f\n", end-start);

    close(fd);
    return 0;
}