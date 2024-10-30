#include <iostream>
#include "cpu.h"

// Function to run and test CPU operations
void runCPUTests(CPU& cpu) {
    std::cout << "=== Running CPU Tests ===" << std::endl;

    // Testing Load and Store Instructions
    cpu.memory[0] = 0x3E;  // Load A with immediate value
    cpu.memory[1] = 0x42;  // Value to load into A
    cpu.executeNextInstruction();
    std::cout << "Load Immediate to A: A = " << +cpu.A << " (Expected: 66)" << std::endl;

    cpu.memory[2] = 0x06;  // Load B with immediate value
    cpu.memory[3] = 0x77;  // Value to load into B
    cpu.executeNextInstruction();
    std::cout << "Load Immediate to B: B = " << +cpu.B << " (Expected: 119)" << std::endl;

    cpu.memory[4] = 0xEA;     // Store A at a specific address
    cpu.memory[5] = 0x10;     // Low byte of address
    cpu.memory[6] = 0x80;     // High byte of address
    cpu.executeNextInstruction();
    std::cout << "Store A to memory[0x8010]: memory[0x8010] = " << +cpu.memory[0x8010] << " (Expected: 66)" << std::endl;

    cpu.memory[0x8010] = 0x55;  // Modify memory for load test
    cpu.memory[7] = 0xFA;       // Load from address 0x8010 to A
    cpu.memory[8] = 0x10;
    cpu.memory[9] = 0x80;
    cpu.executeNextInstruction();
    std::cout << "Load A from memory[0x8010]: A = " << +cpu.A << " (Expected: 85)" << std::endl;

    // Testing Arithmetic Instructions
    cpu.memory[10] = 0x80;  // ADD B to A
    cpu.executeNextInstruction();
    std::cout << "Add B to A: A = " << +cpu.A << " (Expected: 204), Zero Flag = " << cpu.zeroFlag << std::endl;

    // Testing Logical Instructions
    cpu.memory[11] = 0xA0;  // AND B with A
    cpu.executeNextInstruction();
    std::cout << "AND B with A: A = " << +cpu.A << ", Zero Flag = " << cpu.zeroFlag << std::endl;

    cpu.memory[12] = 0xAF;  // XOR A with A (should clear A)
    cpu.executeNextInstruction();
    std::cout << "XOR A with A: A = " << +cpu.A << " (Expected: 0), Zero Flag = " << cpu.zeroFlag << std::endl;

    // Testing Jump Instructions
    cpu.memory[13] = 0x3E;  // Load immediate value into A (to set up jump test)
    cpu.memory[14] = 0x01;  // Value to load
    cpu.executeNextInstruction();
    cpu.memory[15] = 0xC3;  // Unconditional Jump
    cpu.memory[16] = 0x18;  // Low byte of jump address
    cpu.memory[17] = 0x00;  // High byte of jump address
    cpu.executeNextInstruction();
    std::cout << "Jump to Address 0x0018: PC = " << +cpu.PC << " (Expected: 24)" << std::endl;

    // Testing Rotate Instructions
    cpu.memory[24] = 0x17;  // Rotate A left through carry
    cpu.executeNextInstruction();
    std::cout << "Rotate Left A: A = " << +cpu.A << ", Carry Flag = " << cpu.carryFlag << std::endl;

    std::cout << "=== CPU Tests Completed ===" << std::endl;
}

int main() {
    CPU cpu;
    runCPUTests(cpu);
    return 0;
}