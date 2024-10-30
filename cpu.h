	#include <iostream>
	#ifndef cpu_H
	#define cpu_H

	class CPU {
	public:
		// General Purpose Registers
		uint8_t A, B, C, D, E, H, L;

		// Program Coutner and Stack Pointer
		uint16_t PC, SP;

		// Flags
		bool zeroFlag, carryFlag, halfCarryFlag, ime, pendingIME;

		// Memory(64KB)
		uint8_t memory[65536];

		// Timer Variables
		uint32_t timerCounter;
		uint8_t timerValue;
		uint8_t timerFrequency;

		// Constructor
		CPU();

		// SP sets/gets
		uint8_t getSPHigh();
		uint8_t getSPLow();
		void setSPHigh(uint8_t high);
		void setSPLow(uint8_t low);

		// Load and Store Instrucions
		void loadRegister(uint8_t& dest, uint8_t& src);
		void ldFrom(uint8_t &destReg, uint8_t &srcReg);
		void loadFromAddress(uint8_t &destReg, uint16_t address);
		void storeToMemory(uint16_t address, uint8_t &srcReg);
		void loadFromRegisterPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void storeToRegisterPair(uint8_t srcReg, uint8_t destReg1, uint8_t destReg2);
		void loadFromMemory(uint8_t& destReg, uint16_t high, uint8_t low);
		void storeToMemory(uint8_t high, uint8_t low, uint8_t value);
		void incrementRegisterPair(uint8_t &high, uint8_t &low);
		void decrementRegisterPair(uint8_t &high, uint8_t &low);
		void storeToAddress(uint16_t address, uint8_t reg);

		// Jump Instructions
		void jp(uint16_t address);
		void jpIfZero(uint16_t address);

		// Arithmetic Instructions
		void add(uint8_t &destReg, uint8_t srcReg);
		void addPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void addPairs(uint8_t& highDest, uint8_t& lowDest, uint8_t highSrc, uint8_t lowSrc);
		void adc(uint8_t& destReg, uint8_t srcReg);
		void adcPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void sub(uint8_t &destReg, uint8_t srcReg);
		void subPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void sbc(uint8_t& destReg, uint8_t srcReg);
		void sbcPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void inc(uint8_t destReg);
		void dec(uint8_t destReg);
		void incPair(uint8_t& destReg1, uint8_t& destReg2);
		void decPair(uint8_t& destReg1, uint8_t& destReg2);

		// Logical instructions
		void andOp(uint8_t &destReg, uint8_t srcReg);
		void andOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void orOp(uint8_t &destReg, uint8_t srcReg);
		void orOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);
		void xorOp(uint8_t &destReg, uint8_t srcReg);
		void xorOpPair(uint8_t& destReg, uint8_t srcReg1, uint8_t srcReg2);

		// Shift instructions
		void shiftLeft(uint8_t &destReg);
		void shiftRight(uint8_t &destReg);

		// Rotate Instructions
		void rotateLeft(uint8_t &destReg);
		void rotateRight(uint8_t &destReg);

		// Comparison instructions
		void comp(uint8_t srcReg, uint8_t srcReg2);
		void compPair(uint8_t srcReg1, uint8_t srcReg2, uint8_t srcReg3);

		// Interrupt handling
		void serviceInterrupt(uint16_t interrupt);
		void handleInterrupts();
		void executeEI();
		void executeDI();
		void updateIME();
	
		// Stack Instructions
		void push(uint16_t value);
		uint16_t pop();

		// Timers and clocks
		void updateTimers(int cycles);
		void updateClock();

		// Graphics Processing Unit
		void updateGraphics();

		// Misc.
		void reset();

		// Fetch-Decode-Execute Cycle
		uint8_t fetch();
		uint16_t fetch16BitImmediate();
		void executeNextInstruction();
	};

	#endif