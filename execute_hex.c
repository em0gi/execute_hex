#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define CODE_SIZE 1024 // Size of the executable memory block

// Function to initialize memory with NOP instructions
void initializeMemory(unsigned char *mem, size_t size) {
    memset(mem, 0x90, size); // 0x90 is the NOP instruction in x86_64
}

// Function to convert a hex string to a binary array
void hexToBinary(const char *hexStr, unsigned char *binArray) {
    size_t len = strlen(hexStr);
    for (size_t i = 0; i < len; i += 2) {
        sscanf(hexStr + i, "%2hhx", &binArray[i / 2]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hexadecimal_code>\n", argv[0]);
        return 1;
    }

    const char *hexStr = argv[1];
    size_t hexLen = strlen(hexStr);

    if (hexLen % 2 != 0) {
        fprintf(stderr, "Hexadecimal code must have an even number of characters.\n");
        return 1;
    }

    // Allocate memory for the executable code block
    unsigned char *execMemory = (unsigned char *)mmap(NULL, CODE_SIZE,
                                                      PROT_READ | PROT_WRITE | PROT_EXEC,
                                                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (execMemory == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Initialize the memory with NOP instructions
    initializeMemory(execMemory, CODE_SIZE);

    // Convert hex to binary
    size_t binSize = hexLen / 2;
    if (binSize > CODE_SIZE) {
        fprintf(stderr, "Hexadecimal code is too large for the allocated memory.\n");
        munmap(execMemory, CODE_SIZE);
        return 1;
    }

    unsigned char *binaryData = (unsigned char *)malloc(binSize);
    if (binaryData == NULL) {
        perror("Failed to allocate memory for binary data");
        munmap(execMemory, CODE_SIZE);
        return 1;
    }
    hexToBinary(hexStr, binaryData);

    // Copy binary data to executable memory
    memcpy(execMemory, binaryData, binSize);

    // Cast to function pointer and execute
    void (*func)() = (void (*)())execMemory;

    // Print the address we are jumping to
    printf("Jumping to address: %p\n", (void *)func);

    // Execute the code
    func();

    // Clean up
    free(binaryData);
    munmap(execMemory, CODE_SIZE);

    return 0;
}

