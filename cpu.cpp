#include <iostream>
#include "cpu.h"

// Constructor and Initialization
CPU::CPU(): A(0), B(0), C(0), D(0), E(0), H(0), L(0), PC(0), SP(0xFF), zeroFlag(false), 
	carryFlag(false), halfCarryFlag(false), ime(false), 
	pendingIME(false), timerCounter(0), timerValue(0), timerFrequency(0){
	std::fill(std::begin(memory), std::end(memory), 0);
}

// SP sets/gets
uint8_t CPU::getSPHigh(){
	return (SP >> 8) & 0xFF; // Returns the high 8 bits of SP
}

uint8_t CPU::getSPLow(){
	return SP & 0xFF; // Returns the low 8 bits of SP
}

void CPU::setSPHigh(uint8_t high) {
	SP = (SP & 0x00FF) | (high << 8);
}

void CPU::setSPLow(uint8_t low) {
	SP = (SP & 0xFF00) | low;
}

// Load and Store Instructions
void CPU::loadRegister(uint8_t &dest, uint8_t &src) {
	dest = src;
}

void CPU::ldFrom(uint8_t &destReg, uint8_t &srcReg) {
	loadRegister(destReg, srcReg);
}

void CPU::loadFromAddress(uint8_t &destReg, uint16_t address) {
	destReg = memory[address];
}

void CPU::storeToMemory(uint16_t address, uint8_t &srcReg) {
	memory[address] = srcReg;
}

void CPU::loadFromRegisterPair(uint8_t &destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t address = (srcReg1 << 8) | srcReg2;  // Combine H and L to form 16-bit address
	destReg = memory[address];
}

void CPU::storeToRegisterPair(uint8_t srcReg, uint8_t destReg1, uint8_t destReg2) {
	uint16_t address = (destReg1 << 8) | destReg2;  // Combine H and L to form 16-bit address
	memory[address] = srcReg;
}

void CPU::loadFromMemory(uint8_t &destReg, uint16_t high, uint8_t low) {
	uint16_t address = (high << 8) | low;
	destReg = memory[address];
}

void CPU::storeToMemory(uint8_t high, uint8_t low, uint8_t value) {
	uint16_t address = (high << 8) | low;
	memory[address] = value;
}

void CPU::incrementRegisterPair(uint8_t& high, uint8_t& low) {
	uint16_t hl = (high << 8) | low;
	hl += 1;
	high = (hl >> 8) & 0xFF;
	low = hl & 0xFF;
}

void CPU::decrementRegisterPair(uint8_t& high, uint8_t& low) {
	uint16_t hl = (high << 8) | low;
	hl -= 1;
	high = (hl >> 8) & 0xFF;
	low = hl & 0xFF;
}

void CPU::storeToAddress(uint16_t address, uint8_t reg) {
	memory[address] = reg;
}

// Jump Instructions
void CPU::jp(uint16_t address) {
	PC = address;
}

void CPU::jpIfZero(uint16_t address) {
	if (zeroFlag) {
		PC = address;
	}
}

// Arithmetic Instructions
void CPU::add(uint8_t &destReg, uint8_t srcReg) {
	uint16_t result = destReg + srcReg;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) + (srcReg & 0x0F)) > 0x0F;
	destReg = result & 0xFF;
}

void CPU::addPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t HL = (srcReg1 << 8) | srcReg2;
	uint16_t result = destReg + HL;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) + (HL & 0x0F)) > 0x0F;
	destReg = result & 0xFF;
}

void CPU::addPairs(uint8_t& highDest, uint8_t& lowDest, uint8_t highSrc, uint8_t lowSrc) {
	uint16_t dest = (highDest << 8) | lowDest;
	uint16_t src = (highSrc << 8) | lowSrc;
	uint32_t result = dest + src;
	carryFlag = (result > 0xFFFF);
	halfCarryFlag = ((dest & 0x0FFF) + (src & 0x0FFF)) > 0x0FFF;
	highDest = (result >> 8) & 0xFF;
	lowDest = result & 0xFF;
	zeroFlag = false;
}

void CPU::adc(uint8_t& destReg, uint8_t srcReg) {
	uint8_t carryValue = carryFlag ? 1 : 0;
	uint16_t result = destReg + srcReg + carryValue;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) + (srcReg & 0x0F) + carryValue) > 0x0F;
	destReg = result & 0xFF;
}

void CPU::adcPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint8_t carryValue = carryFlag ? 1 : 0;
	uint16_t result = destReg + srcReg1 + srcReg2 + carryValue;
	zeroFlag = (result & 0xFF) == 0;
	halfCarryFlag = ((destReg & 0x0F) + (srcReg1 & 0x0F) + (srcReg2 & 0x0F) + carryValue) > 0x0F;
	destReg = result & 0xFF;
}

void CPU::sub(uint8_t &destReg, uint8_t srcReg) {
	uint16_t result = destReg - srcReg;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result < 0;
	halfCarryFlag = (destReg & 0x0F) < (srcReg & 0x0F);
	destReg = result & 0xFF;
}

void CPU::subPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t HL = (srcReg1 << 8) | srcReg2;
	uint16_t result = destReg - HL;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) < (HL & 0x0F));
	destReg = result & 0xFF;
}

void CPU::sbc(uint8_t& destReg, uint8_t srcReg) {
	uint8_t carryValue = carryFlag ? 1 : 0;
	uint16_t result = destReg - srcReg - carryValue;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) < ((srcReg & 0x0F) + carryValue));
	destReg = result & 0xFF;
}

void CPU::sbcPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint8_t carryValue = carryFlag ? 1 : 0;
	uint16_t result = destReg - srcReg1 - srcReg2 - carryValue;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = ((destReg & 0x0F) < ((srcReg1 & 0x0F) + (srcReg2 & 0x0F) + carryValue));
	destReg = result & 0xFF;
}

void CPU::inc(uint8_t destReg) {
	uint16_t result = destReg + 1;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = result > 0xFF;
	halfCarryFlag = (destReg & 0xFF) == 0x0F;
	destReg = result & 0xFF;
}

void CPU::dec(uint8_t destReg) {
	uint16_t result = destReg - 1;
	zeroFlag = (result & 0xFF) == 0;
	carryFlag = destReg == 0;
	halfCarryFlag = (destReg & 0xFF) == 0;
	destReg = result & 0xFF;
}

void CPU::incPair(uint8_t &destReg1, uint8_t &destReg2) {
	if (++destReg2 == 0) {
		++destReg1;
	}
	zeroFlag = (destReg1 == 0 && destReg2 == 0);
	halfCarryFlag = (destReg2 == 0);
}

void CPU::decPair(uint8_t& destReg1, uint8_t& destReg2) {
	if (--destReg2 == 0xFF) {
		--destReg1;
	}
	zeroFlag = (destReg1 == 0 && destReg2 == 0);
	halfCarryFlag = (destReg2 == 0x0F);
}

// Logical Instructions
void CPU::andOp(uint8_t &destReg, uint8_t srcReg) {
	destReg = destReg & srcReg;
	zeroFlag = (destReg == 0);
	halfCarryFlag = true;
	carryFlag = false;
}

void CPU::andOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t HL = (srcReg1 >> 8) | srcReg2;
	destReg &= HL;
	zeroFlag = (destReg == 0);
	carryFlag = false;
	halfCarryFlag = false;
}

void CPU::orOp(uint8_t &destReg, uint8_t srcReg) {
	destReg = destReg | srcReg;
	zeroFlag = (destReg == 0);
	halfCarryFlag = false;
	carryFlag = false;
}

void CPU::orOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t HL = (srcReg1 >> 8) | srcReg2;
	destReg |= HL;
	zeroFlag = (destReg == 0);
	carryFlag = false;
	halfCarryFlag = false;
}

void CPU::xorOp(uint8_t &destReg, uint8_t srcReg) {
	destReg = destReg ^ srcReg;
	zeroFlag  = (destReg == 0);
	halfCarryFlag = false;
	carryFlag = false;
}

void CPU::xorOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2) {
	uint16_t HL = (srcReg1 >> 8) | srcReg2;
	destReg ^= HL;
	zeroFlag = (destReg == 0);
	carryFlag = false;
	halfCarryFlag = false;
}

// Shift instructions
void CPU::shiftLeft(uint8_t &destReg) {
	carryFlag = (destReg & 0x80) != 0;
	destReg = destReg << 1;
	zeroFlag = (destReg == 0);
	halfCarryFlag = false;
}

void CPU::shiftRight(uint8_t &destReg) {
	carryFlag = (destReg & 0x01) != 0;
	destReg = destReg >> 1;
	zeroFlag = (destReg == 0);
	halfCarryFlag = false;
}

// Rotate Instructions
void CPU::rotateLeft(uint8_t &destReg) {
	carryFlag = (destReg & 0x80) != 0;
	destReg = (destReg << 1) | (destReg >> 7);
	zeroFlag = (destReg == 0);
	halfCarryFlag = false;
}

void CPU::rotateRight(uint8_t &destReg) {
	carryFlag = (destReg & 0x01) != 0;
	destReg = (destReg >> 1) | (destReg << 7);
	zeroFlag = (destReg == 0);
	halfCarryFlag = false;
}

// Comparison instructions
void CPU::comp(uint8_t srcReg, uint8_t srcReg2) {
	uint16_t result = srcReg - srcReg2;
	zeroFlag = (result == 0);
	carryFlag = (srcReg < srcReg2);
	halfCarryFlag = (srcReg * 0x0F) < (srcReg2 & 0x0F);
}

void CPU::compPair(uint8_t srcReg1, uint8_t srcReg2, uint8_t srcReg3) {
	uint16_t HL = (srcReg2 << 8) | srcReg3;
	uint16_t result = HL - A;
	zeroFlag = ((result & 0xFF) == 0);
	carryFlag = (HL < A);
	halfCarryFlag = ((HL & 0x0F) < (A & 0x0F));
}

// Interrupt Handling
void CPU::serviceInterrupt(uint16_t address) {
	// Clear the interrupt master enable flag (IME) to disable further interrupts temporarily
	ime = false;

	// Push the current PC onto the stack to save the return address
	push(PC);

	// Jump to the interrupt handler address
	PC = address;

	// Additional handling may be added if needed
}

void CPU::executeEI() {
	pendingIME = true;  // Mark IME to be enabled after the next instruction
}

void CPU::executeDI() {
	ime = false;        // Disable interrupts immediately
	pendingIME = false; // Reset pending flag
}

void CPU::handleInterrupts() {
	uint8_t interruptEnable = memory[0xFFFF]; // IE register
	uint8_t interruptFlag = memory[0xFF0F]; // IF register

	// Check each interrupt in priority order
	if ((interruptEnable & 0x01) && (interruptFlag & 0x01)) {
		// V-Blank Interrupt
		serviceInterrupt(0x40);  // Address 0x40 for V-Blank
		memory[0xFF0F] &= ~0x01; // Clear the IF flag
	}
	else if ((interruptEnable & 0x02) && (interruptFlag & 0x02)) {
		// LCD STAT Interrupt
		serviceInterrupt(0x48);
		memory[0xFF0F] &= ~0x02;
	}
	else if ((interruptEnable & 0x04) && (interruptFlag & 0x04)) {
		// Timer Interrupt
		serviceInterrupt(0x50);
		memory[0xFF0F] &= ~0x04;
	}
	else if ((interruptEnable & 0x08) && (interruptFlag & 0x08)) {
		// Serial Interrupt
		serviceInterrupt(0x58);
		memory[0xFF0F] &= ~0x08;
	}
	else if ((interruptEnable & 0x10) && (interruptFlag & 0x10)) {
		// Joypad Interrupt
		serviceInterrupt(0x60);
		memory[0xFF0F] &= ~0x10;
	}
}

void CPU::updateIME() {
	if (pendingIME) {
		ime = true;
		pendingIME = false;
	}
}

// Stack Instructions
void CPU::push(uint16_t value) {
	memory[--SP] = (value >> 8) & 0xFF;
	memory[--SP] = value & 0xFF;
}

// Timers and Clocks
void CPU::updateTimers(int cycles) {
	timerCounter += cycles;
	if (timerCounter & 0x04) {
		while (timerCounter >= timerFrequency) {
			timerCounter -= timerFrequency;
			timerValue++;
			if (timerValue == 0) {
				memory[0xFF0F] |= 0x04;
			}
		}
	}
}

void CPU::updateClock() {
	updateTimers(1);
}

uint16_t CPU::pop() {
	uint16_t value = (memory[SP] << 8) | memory[SP + 1];
	SP += 2;
	return value;
}

// Misc.
void CPU::reset() {
	A = B = C = D = E = H = L = 0;
	PC = 0x0100;
	SP = 0xFFFE;
	zeroFlag = carryFlag = halfCarryFlag = ime = pendingIME = false;
	}

// Fetch-Decode-Execute Cycle
uint8_t CPU::fetch() {
	return memory[PC++];
}

uint16_t CPU::fetch16BitImmediate() {
	uint16_t lowByte = memory[PC++];
	uint16_t highByte = memory[PC++];
	return (highByte << 8) | lowByte;
}

void CPU::executeNextInstruction() {
	if (ime) {
		handleInterrupts();
	}

	uint8_t opcode = fetch();

	switch (opcode) {
	// Load OpCodes
	case 0x40: // B
		ldFrom(B, B);
		break;
	case 0x41:
		ldFrom(B, C);
		break;
	case 0x42:
		ldFrom(B, D);
		break;
	case 0x43:
		ldFrom(B, E);
		break;
	case 0x44:
		ldFrom(B, H);
		break;
	case 0x45:
		ldFrom(B, L);
		break;
	case 0x46:
		loadFromRegisterPair(B, H, L);
		break;
	case 0x47:
		ldFrom(B, A);
		break;

	case 0x48: // C
		ldFrom(C, B);
		break;
	case 0x49:
		ldFrom(C, C);
		break;
	case 0x4A:
		ldFrom(C, D);
		break;
	case 0x4B:
		ldFrom(C, E);
		break;
	case 0x4C:
		ldFrom(C, H);
		break;
	case 0x4D:
		ldFrom(C, L);
		break;
	case 0x4E:
		loadFromRegisterPair(C, H, L);
		break;
	case 0x4F:
		ldFrom(C, A);
		break;

	case 0x50: // D
		ldFrom(D, B);
		break;
	case 0x51:
		ldFrom(D, C);
		break;
	case 0x52:
		ldFrom(D, D);
		break;
	case 0x53:
		ldFrom(D, E);
		break;
	case 0x54:
		ldFrom(D, H);
		break;
	case 0x55:
		ldFrom(D, L);
		break;
	case 0x56:
		loadFromRegisterPair(D, H, L);
		break;
	case 0x57:
		ldFrom(D, A);
		break;

	case 0x58: // E
		ldFrom(E, B);
		break;
	case 0x59:
		ldFrom(E, C);
		break;
	case 0x5A:
		ldFrom(E, D);
		break;
	case 0x5B:
		ldFrom(E, E);
		break;
	case 0x5C:
		ldFrom(E, H);
		break;
	case 0x5D:
		ldFrom(E, L);
		break;
	case 0x5E:
		loadFromRegisterPair(E, H, L);
		break;
	case 0x5F:
		ldFrom(E, A);
		break;

	case 0x60: // H
		ldFrom(H, B);
		break;
	case 0x61:
		ldFrom(H, C);
		break;
	case 0x62:
		ldFrom(H, D);
		break;
	case 0x63:
		ldFrom(H, E);
		break;
	case 0x64:
		ldFrom(H, H);
		break;
	case 0x65:
		ldFrom(H, L);
		break;
	case 0x66:
		loadFromRegisterPair(H, H, L);
		break;
	case 0x67:
		ldFrom(H, A);
		break;

	case 0x68: // L
		ldFrom(L, B);
		break;
	case 0x69:
		ldFrom(L, C);
		break;
	case 0x6A:
		ldFrom(L, D);
		break;
	case 0x6B:
		ldFrom(L, E);
		break;
	case 0x6C:
		ldFrom(L, H);
		break;
	case 0x6D:
		ldFrom(L, L);
		break;
	case 0x6E:
		loadFromRegisterPair(L, H, L);
		break;
	case 0x6F:
		ldFrom(L, A);
		break;

	case 0x78: // A
		ldFrom(A, B);
		break;
	case 0x79:
		ldFrom(A, C);
		break;
	case 0x7A:
		ldFrom(A, D);
		break;
	case 0x7B:
		ldFrom(A, E);
		break;
	case 0x7C:
		ldFrom(A, H);
		break;
	case 0x7D:
		ldFrom(A, L);
		break;
	case 0x7E:
		loadFromRegisterPair(A, H, L);
		break;
	case 0x7F:
		ldFrom(A, A);
		break;

	case 0x0A: // Memory Load/Store Instructions
		loadFromRegisterPair(A, B, C);
		break;
	case 0x1A:
		loadFromRegisterPair(A, D, E);
		break;
	case 0x02:
		storeToRegisterPair(A, B, C);
		break;
	case 0x12:
		storeToRegisterPair(A, D, E);
		break;
	case 0xFA: {
		uint16_t address = fetch16BitImmediate();
		loadFromAddress(A, address);
		break;
	}
	case 0xEA: {
		uint16_t address = fetch16BitImmediate();
		storeToAddress(address, A);
		break;
	}
	case 0x22:
		storeToMemory(H, L, A);
		incrementRegisterPair(H, L);
		break;
	case 0x32: {
		storeToMemory(H, L, A);
		decrementRegisterPair(H, L);
		break;
	}
	case 0x2A: {
		loadFromMemory(A, H, L);
		incrementRegisterPair(H, L);
		break;
	}
	case 0x3A: {
		loadFromMemory(A, H, L);
		decrementRegisterPair(H, L);
		break;
	}

	case 0x06: // Load 8 bit Immediate to Register
		B = fetch();
		break;
	case 0x0E:
		C = fetch();
		break;
	case 0x16:
		D = fetch();
		break;
	case 0x1E:
		E = fetch();
		break;
	case 0x26:
		H = fetch();
		break;
	case 0x2E:
		L = fetch();
		break;
	case 0x3E:
		A = fetch();
		break;

	case 0xE0: // Load into memory-mapped registers
		storeToAddress(0xFF00 + fetch(), A);
		break;
	case 0xF0:
		loadFromAddress(A, 0xFF00 + fetch());
		break;
	case 0xE2:
		storeToAddress(0xFF00 + C, A);
		break;
	case 0xF2:
		loadFromAddress(A, 0xFF00 + C);
		break;

	// 8 Bit Arithmetic Instructions
	case 0x80:
		add(A, B);
		break;
	case 0x81:
		add(A, C);
		break;
	case 0x82:
		add(A, D);
		break;
	case 0x83:
		add(A, E);
		break;
	case 0x84:
		add(A, H);
		break;
	case 0x85:
		add(A, L);
		break;
	case 0x86:
		addPair(A, H, L);
		break;
	case 0x87:
		add(A, A);
		break;
	case 0xC6:
		add(A, fetch());
		break;

	case 0x88:
		adc(A, B);
		break;
	case 0x89:
		adc(A, C);
		break;
	case 0x8A:
		adc(A, D);
		break;
	case 0x8B:
		adc(A, E);
		break;
	case 0x8C:
		adc(A, H);
		break;
	case 0x8D:
		adc(A, L);
		break;
	case 0x8E:
		adcPair(A, H, L);
		break;
	case 0x8F:
		adc(A, A);
		break;
	case 0xCE:
		adc(A, fetch());
		break;

	case 0x90:
		sub(A, B);
		break;
	case 0x91:
		sub(A, C);
		break;
	case 0x92:
		sub(A, D);
		break;
	case 0x93:
		sub(A, E);
		break;
	case 0x94:
		sub(A, H);
		break;
	case 0x95:
		sub(A, L);
		break;
	case 0x96:
		subPair(A, H, L);
		break;
	case 0x97:
		sub(A, A);
		break;
	case 0xD6:
		sub(A, fetch());
		break;

	case 0x98:
		sbc(A, B);
		break;
	case 0x99:
		sbc(A, C);
		break;
	case 0x9A:
		sbc(A, D);
		break;
	case 0x9B:
		sbc(A, E);
		break;
	case 0x9C:
		sbc(A, H);
		break;
	case 0x9D:
		sbc(A, L);
		break;
	case 0x9E:
		sbcPair(A, H, L);
		break;
	case 0x9F:
		sbc(A, A);
		break;
	case 0xDE:
		sbc(A, fetch());
		break;

	case 0x3C:
		inc(A);
		break;
	case 0x04:
		inc(B);
		break;
	case 0x0C:
		inc(C);
		break;
	case 0x14:
		inc(D);
		break;
	case 0x1C:
		inc(E);
		break;
	case 0x24:
		inc(H);
		break;
	case 0x2C:
		inc(L);
		break;
	case 0x34:
		incPair(H, L);

	case 0x3D:
		dec(A);
		break;
	case 0x05:
		dec(B);
		break;
	case 0x0D:
		dec(C);
		break;
	case 0x15:
		dec(D);
		break;
	case 0x1D:
		dec(E);
		break;
	case 0x25:
		dec(H);
		break;
	case 0x2D:
		dec(L);
		break;
	case 0x35:
		decPair(H, L);

	// 16 Bit Arithmetic Instructions
	case 0x09:
		addPairs(H, L, B, C);
		break;
	case 0x19:
		addPairs(H, L, D, E);
		break;
	case 0x29:
		addPairs(H, L, H, L);
		break;
	case 0x39:
		addPairs(H, L, getSPHigh(), getSPLow());
		break;
	case 0xE8:{
		int8_t immediateValue = static_cast<int8_t>(fetch());
		SP += immediateValue;
		break;
	}

	// Logical Instructions
	case 0xA0: // and
		andOp(A, B);
		break;
	case 0xA1:
		andOp(A, C);
		break;
	case 0xA2:
		andOp(A, D);
		break;
	case 0xA3:
		andOp(A, E);
		break;
	case 0xA4:
		andOp(A, H);
		break;
	case 0xA5:
		andOp(A, L);
		break;
	case 0xA6:
		andOpPair(A, H, L); // Check
		break;
	case 0xA7:
		andOp(A, A);
		break;
	case 0xE6:
		andOp(A, fetch());
		break;

	case 0xB0: // or
		orOp(A, B);
		break;
	case 0xB1:
		orOp(A, C);
		break;
	case 0xB2:
		orOp(A, D);
		break;
	case 0xB3:
		orOp(A, E);
		break;
	case 0xB4:
		orOp(A, H);
		break;
	case 0xB5:
		orOp(A, L);
		break;
	case 0xB6:
		orOpPair(A, H, L); // Check
		break;
	case 0xB7:
		orOp(A, A);
		break;
	case 0xF6:
		orOp(A, fetch());
		break;

	case 0xA8: // xor
		xorOp(A, B);
		break;
	case 0xA9:
		xorOp(A, C);
		break;
	case 0xAA:
		xorOp(A, D);
		break;
	case 0xAB:
		xorOp(A, E);
		break;
	case 0xAC:
		xorOp(A, H);
		break;
	case 0xAD:
		xorOp(A, L);
		break;
	case 0xAE:
		xorOpPair(A, H, L);
		break;
	case 0xAF:
		xorOp(A, A);
		break;
	case 0xEE:
		xorOp(A, fetch());
		break;

	case 0xB8: // comp
		comp(A, B);
		break;
	case 0xB9:
		comp(A, C);
		break;
	case 0xBA:
		comp(A, D);
		break;
	case 0xBB:
		comp(A, E);
		break;
	case 0xBC:
		comp(A, H);
	case 0xBD:
		comp(A, L);
	case 0xBE:
		compPair(A, H, L);
		break;
	case 0xBF:
		comp(A, A);
		break;
	case 0xFE:
		comp(A, fetch());
		break;

	// Other Opcodes
	case 0xC3: {
		uint16_t address = fetch() | (fetch() << 8);
		jp(address);
		break;
	}
	default:
		std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << std::dec << std::endl;
		break;
	}
	updateIME();
}