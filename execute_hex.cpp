// Oo
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <cstring>
#include <cstdlib>

// Allocate executable memory
void* allocate_executable_memory(size_t size) {
    void* mem = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        std::cerr << "Failed to allocate memory" << std::endl;
        exit(1);
    }
    return mem;
}

// Convert hex string to bytes
void hex_to_bytes(const std::string& hex, unsigned char* buffer) {
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), nullptr, 16));
        buffer[i / 2] = byte;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <hexcode>" << std::endl;
        return 1;
    }

    std::string hexcode = argv[1];
    size_t code_size = hexcode.length() / 2;

    // Allocate enough memory for both code and string
    size_t total_size = code_size + 14;  // 14 bytes for "Hello, world!\n"
    unsigned char* executable_memory = (unsigned char*) allocate_executable_memory(total_size);

    // Convert hex string to machine code and copy into executable memory
    hex_to_bytes(hexcode, executable_memory);

    // Append "Hello, world!\n" string to the end of the machine code
    const char* message = "Hello, world!\n";
    std::memcpy(executable_memory + code_size, message, 14);

    // Cast the executable memory to a function pointer
    typedef void (*func_ptr_t)();
    func_ptr_t func = (func_ptr_t) executable_memory;

    // Call the function (which is the machine code)
    func();

    // Free the allocated memory
    munmap(executable_memory, total_size);

    return 0;
}
