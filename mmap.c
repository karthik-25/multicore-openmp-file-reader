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

int isNumberPresent(long arr[], int size, long target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;  // Number is present
        }
    }
    return -1;  // Number is not present
}

double findMax(double arr[], int size) {
    if (size <= 0) {
        // Handle empty array or invalid size
        printf("Invalid array size.\n");
        return -1;  // Returning a special value to indicate an error
    }

    double max = arr[0];  // Assume the first element is the maximum

    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];  // Update the maximum if a larger element is found
        }
    }

    return max;
}

double findMin(double arr[], int size) {
    if (size <= 0) {
        // Handle empty array or invalid size
        printf("Invalid array size.\n");
        return -1;  // Returning a special value to indicate an error
    }

    double min = arr[0];  // Assume the first element is the minimum

    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];  // Update the minimum if a smaller element is found
        }
    }

    return min;
}

double findAverage(double arr[], int size) {
    if (size <= 0) {
        // Handle empty array or invalid size
        printf("Invalid array size.\n");
        return -1.0;  // Returning a special value to indicate an error
    }

    double sum = 0;

    for (int i = 0; i < size; i++) {
        sum += arr[i];  // Sum up all the elements in the array
    }

    return (double)sum / size;  // Calculate the average
}

int main(int argc, char *argv[]) {
    const char* filepath = argv[1];
    int t = atoi(argv[2]);
    double start, end;
    int read_counter = 0;

    // get file size and calculate number of chunks
    struct stat sb;
    stat(filepath, &sb);
    int num_chunks = sb.st_size / CHUNK_SIZE + (sb.st_size % CHUNK_SIZE == 0 ? 0 : 1);
    printf("file size: %d\n", (int) sb.st_size);
    printf("num of chunks: %d\n", num_chunks);

    long rand_chunks[10];
    double rand_chunks_time[10];
    for(int x=0;x<10;x++){
        rand_chunks[x] =(long) rand() % num_chunks + 1;
    }

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
        
        // for testing correctness
        for (int j = 0; j < cur_chunk_size; j++) {
            read_counter++;
        }

        int idx = isNumberPresent(rand_chunks, sizeof(rand_chunks) / sizeof(rand_chunks[0]), i);
        if(idx!=-1){
            rand_chunks_time[idx] = omp_get_wtime() - start;
        }
        
    }
    end = omp_get_wtime();

    printf("Total read: %d\n", read_counter);
    printf("Execution time: %f\n", end-start);


    printf("Rand Chunks selected: ");
    int size = sizeof(rand_chunks_time) / sizeof(rand_chunks_time[0]);
    for (int i = 0; i < size; i++) {
        printf("%ld ", rand_chunks[i]);
    }
    printf("\n");

    printf("STATS: Average Response Time:%f\nMax Response Time:%f\nMin Response Time:%f\n", findAverage(rand_chunks_time, size), findMax(rand_chunks_time, size), findMin(rand_chunks_time, size));

    munmap(file_data, sb.st_size);
    close(fd);

    return 0;
}