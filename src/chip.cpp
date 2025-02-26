#include <cstdint>
#include <fstream>

const unsigned int START_ADDRESS = 0x200;

class Chip8 {
    public:
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode;
};

void Chip8::LoadROM(char const* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i=0; i<size, ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }
        delete[] buffer;
    }
}

Chip8::Chip8() {
    pc = START_ADDRESS;
}
