#include "chip.hpp"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <chrono>
#include <random>
#include <iostream>
#include <algorithm>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void Chip8::LoadROM(char const* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        std::cerr << "ERROR: Failed to open ROM file\n";
        return;
    }

    std::streampos size = file.tellg();

    if (size <= 0) {
        std::cerr << "ERROR: ROM file is empty or invalid\n";
        file.close();
        return;
    }

    if (size > (4096 - START_ADDRESS)) {
        std::cerr << "WARNING: ROM size (" << size << " bytes) exceeds available memory (" 
                  << (4096 - START_ADDRESS) << " bytes)\n";
    }

    char* buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    for (long i = 0; i < std::min<uint16_t>(size, 16L); ++i) {
        memory[START_ADDRESS + i] = buffer[i];
    }

    delete[] buffer;
}

void Chip8::Cycle() {
    pc = std::clamp(pc, static_cast<uint16_t>(START_ADDRESS), static_cast<uint16_t>(MEMORY_SIZE - 2));
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;
    
    uint8_t op_high = (opcode & 0xF000) >> 12;
    if (op_high > 0xF || !table[op_high]) {
        HandleInvalidOpcode();
        return;
    }

    if (op_high == 0xF && (opcode & 0x00FF) > 0x65) {
        HandleInvalidOpcode();
        return;
    }

    if (sp >= 16) {
        std::cerr << "STACK OVERFLOW! Resetting...\n";
        Reset();
        return;
    }

    if ((opcode & 0xFF00) == 0xF600 && (opcode & 0x00FF) > 0x65) {
        std::cerr << "Detected problematic opcode pattern: " << std::hex << opcode << "\n";
        HandleInvalidOpcode();
        return;
    }

    (this->*table[op_high])();
    delayTimer = std::max(0, delayTimer - 1);
    soundTimer = std::max(0, soundTimer - 1);
}

void Chip8::HandleInvalidOpcode() {
    std::cerr << "INVALID OPCODE: " << std::hex << opcode 
              << " at PC=" << (pc-2) << "\n";
    pc = START_ADDRESS;
}

void Chip8::Reset() {
    pc = START_ADDRESS;
    sp = 0;
    opcode = 0;
    index = 0;
    memset(registers, 0, sizeof(registers));
    memset(stack, 0, sizeof(stack));
}

void Chip8::Table0() {
    uint8_t op_low = opcode & 0x000Fu;
    if (op_low > 0xF || !table0[op_low]) {
        HandleInvalidOpcode();
        return;
    }
    ((*this).*(table0[op_low]))();
}

void Chip8::Table8() {
    uint8_t op_low = opcode & 0x000Fu;
    if (op_low > 0xF || !table8[op_low]) {
        HandleInvalidOpcode();
        return;
    }
    ((*this).*(table8[op_low]))();
}

void Chip8::TableE() {
    uint8_t op_low = opcode & 0x000Fu;
    if (op_low > 0xF || !tableE[op_low]) {
        HandleInvalidOpcode();
        return;
    }
    ((*this).*(tableE[op_low]))();
}

void Chip8::TableF() {
    uint8_t op_low = opcode & 0x00FFu;
    if (op_low > 0x65 || !tableF[op_low]) {
        HandleInvalidOpcode();
        return;
    }
    ((*this).*(tableF[op_low]))();
}

void Chip8::OP_NULL() {
    static int invalidCount = 0;
    if (++invalidCount > 10) {
        std::cerr << "TOO MANY INVALID OPS! Resetting...\n";
        Reset();
        invalidCount = 0;
    }
}

Chip8::Chip8() 
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;
    opcode = 0;
    index = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = false;
    
    memset(registers, 0, sizeof(registers));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));
    memset(video, 0, sizeof(video));
    memset(keypad, 0, sizeof(keypad));

    for (unsigned int i=0; i<FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    for (int i = 0; i <= 0xF; i++) {
        table[i] = &Chip8::OP_NULL;
    }
    
    for (int i = 0; i <= 0xF; i++) {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL; 
        tableE[i] = &Chip8::OP_NULL;
    }
    
    for (int i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }

    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {
    if (sp == 0) {
        std::cerr << "Stack underflow at 00EE!\n";
        Reset();
        return;
    }
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    if (address >= 0x200 && address < 0xFFF) {
        pc = address;
    } else {
        pc += 2;
    }
}

void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte) {
        pc += 2;
    }
}

void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte) {
        pc += 2;
    }
}

void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255U) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x1u);

    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFFu;
    index = address;
}

void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = address + registers[0];
}

void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    if (index + height >= MEMORY_SIZE) {
        std::cerr << "DRAW: Invalid sprite memory access\n";
        return;
    }

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;
    bool pixelChanged = false;

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) {
            if (xPos + col >= VIDEO_WIDTH || yPos + row >= VIDEO_HEIGHT)
                continue;

            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint8_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel) {
                if (*screenPixel == 1)
                    registers[0xF] = 1;

                *screenPixel ^= 1;
                pixelChanged = true;
            }
        }
    }
    drawFlag = pixelChanged;
}

void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (keypad[key]) {
        pc += 2;
    }
}

void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (!keypad[key]) {
        pc += 2;
    }
}

void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[0]) {
        registers[Vx] = 0;
    } else if (keypad[1]) {
        registers[Vx] = 1;
    } else if (keypad[2]) {
        registers[Vx] = 2;
    } else if (keypad[3]) {
        registers[Vx] = 3;
    } else if (keypad[4]) {
        registers[Vx] = 4;
    } else if (keypad[5]) {
        registers[Vx] = 5;
    } else if (keypad[6]) {
        registers[Vx] = 6;
    } else if (keypad[7]) {
        registers[Vx] = 7;
    } else if (keypad[8]) {
        registers[Vx] = 8;
    } else if (keypad[9]) {
        registers[Vx] = 9;
    } else if (keypad[10]) {
        registers[Vx] = 10;
    } else if (keypad[11]) {
        registers[Vx] = 11;
    } else if (keypad[12]) {
        registers[Vx] = 12;
    } else if (keypad[13]) {
        registers[Vx] = 13;
    } else if (keypad[14]) {
        registers[Vx] = 14;
    } else if (keypad[15]) {
        registers[Vx] = 15;
    } else {
        pc -= 2;
    }
}

void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint16_t oldIndex = index;
    index += registers[Vx];
    
    if (index < oldIndex || index >= MEMORY_SIZE) {
        std::cerr << "Index register overflow: 0x" << std::hex << index << "\n";
        index %= MEMORY_SIZE;
    }
    
    if (index < FONTSET_START_ADDRESS) {
        index = FONTSET_START_ADDRESS;
    }
}

void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx] & 0x0F;  
    index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    if ((index + Vx) >= MEMORY_SIZE) {
        std::cerr << "ERROR: Memory store out of bounds\n";
        return;
    }
    
    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if ((index + Vx) >= MEMORY_SIZE) {
        std::cerr << "ERROR: Memory store out of bounds\n";
        return;
    }

    for (uint8_t i=0; i <= Vx; ++i) {
        registers[i] = memory[index + i];
    }
}
