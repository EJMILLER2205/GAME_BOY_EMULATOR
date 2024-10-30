#include "gpu.h"
#include <algorithm> // For fill function
#include <stdexcept> // For exceptions

// Constructor
GPU::GPU() : cycleCounter(0), mode(GPUMode::OAM), currentScanline(0) {
    reset();
}

// Reset function
void GPU::reset() {
    mode = GPUMode::OAM;
    cycleCounter = 0;
    currentScanline = 0;
    std::fill(std::begin(frameBuffer), std::end(frameBuffer), 0);
    clearSprites(); // Clear any existing sprites on reset
}

// GPU step function
void GPU::step(uint8_t cycles) {
    cycleCounter += cycles;

    switch (mode) {
    case GPUMode::OAM:
        if (cycleCounter >= 80) {
            cycleCounter -= 80;
            mode = GPUMode::VRAM;
        }
        break;
    case GPUMode::VRAM:
        if (cycleCounter >= 172) {
            cycleCounter -= 172;
            mode = GPUMode::HBlank;
            renderScanLine();
        }
        break;
    case GPUMode::HBlank:
        if (cycleCounter >= 204) {
            cycleCounter -= 204;
            currentScanline++;
            if (currentScanline == SCREEN_HEIGHT) {
                mode = GPUMode::VBlank;
                renderFrame();
            }
            else {
                mode = GPUMode::OAM;
            }
        }
        break;
    case GPUMode::VBlank:
        if (cycleCounter >= 456) {
            cycleCounter -= 456;
            currentScanline++;
            if (currentScanline > 153) { // 10 lines in VBlank
                currentScanline = 0;
                mode = GPUMode::OAM;
            }
        }
        break;
    }
}

// Draw a single tile
void GPU::drawTile(int x, int y, uint8_t tileId, bool isBackground) {
    uint8_t* tileData = getTileData(tileId);

    for (int row = 0; row < TITLE_SIZE; ++row) {
        for (int col = 0; col < TITLE_SIZE; ++col) {
            int pixelIndex = row * TITLE_SIZE + col;
            uint8_t colorIndex = tileData[pixelIndex];
            uint32_t color = getColorFromIndex(colorIndex);

            int framebufferIndex = ((y + row) * SCREEN_WIDTH + (x + col)) * 4;

            if (x + col < SCREEN_WIDTH && y + row < SCREEN_HEIGHT) {
                frameBuffer[framebufferIndex] = (color >> 24) & 0xFF;
                frameBuffer[framebufferIndex + 1] = (color >> 16) & 0xFF;
                frameBuffer[framebufferIndex + 2] = (color >> 8) & 0xFF;
                frameBuffer[framebufferIndex + 3] = color & 0xFF;
            }
        }
    }
}

// Draw a single sprite
void GPU::drawSprite(int x, int y, uint8_t tileId, bool xFlip, bool yFlip) {
    uint8_t* spriteData = getTileData(tileId);

    for (int row = 0; row < TITLE_SIZE; row++) {
        for (int col = 0; col < TITLE_SIZE; ++col) {
            int spriteRow = yFlip ? (TITLE_SIZE - 1 - row) : row;
            int spriteCol = xFlip ? (TITLE_SIZE - 1 - col) : col;

            int pixelIndex = spriteRow * TITLE_SIZE + spriteCol;
            uint8_t colorIndex = spriteData[pixelIndex];
            if (colorIndex == 0) continue; // Skip transparent pixels

            uint32_t color = getColorFromIndex(colorIndex);
            int framebufferIndex = ((y + row) * SCREEN_WIDTH + (x + col)) * 4;

            if (x + col < SCREEN_WIDTH && y + row < SCREEN_HEIGHT) {
                frameBuffer[framebufferIndex] = (color >> 24) & 0xFF;
                frameBuffer[framebufferIndex + 1] = (color >> 16) & 0xFF;
                frameBuffer[framebufferIndex + 2] = (color >> 8) & 0xFF;
                frameBuffer[framebufferIndex + 3] = color & 0xFF;
            }
        }
    }
}

// Render a full frame
void GPU::renderFrame() {
    for (int scanline = 0; scanline < SCREEN_HEIGHT; ++scanline) {
        renderScanLine();
    }
}

// Render a single scanline
void GPU::renderScanLine() {
    for (int tileX = 0; tileX < SCREEN_WIDTH / TITLE_SIZE; ++tileX) {
        int mapX = (tileX * TITLE_SIZE + scrollX) % (TITLE_MAP_SIZE * TITLE_SIZE);
        int mapY = (currentScanline + scrollY) % (TITLE_MAP_SIZE * TITLE_SIZE);

        uint8_t tileId = backgroundTileMap[mapY / TITLE_SIZE][mapX / TITLE_SIZE];
        drawTile(tileX * TITLE_SIZE, currentScanline, tileId, true);
    }

    for (int spriteIndex = 0; spriteIndex < MAX_SPRITES; ++spriteIndex) {
        int spriteY = oam[spriteIndex * 4] - 16; // Adjust for sprite offset
        int spriteX = oam[spriteIndex * 4 + 1] - 8; // Adjust for sprite offset
        uint8_t tileId = oam[spriteIndex * 4 + 2];
        uint8_t attributes = oam[spriteIndex * 4 + 3];

        if (currentScanline >= spriteY && currentScanline < spriteY + TITLE_SIZE) {
            bool xFlip = attributes & 0x20;
            bool yFlip = attributes & 0x40;
            drawSprite(spriteX, spriteY, tileId, xFlip, yFlip);
        }
    }
}

// Framebuffer pointer access
uint8_t* GPU::getFrameBuffer() {
    return frameBuffer;
}

// Access tile data
uint8_t* GPU::getTileData(uint8_t tileId) {
    int tileAddress = tileId * 16; // Adjust according to your tile data layout
    return &vram[tileAddress];
}

// Get color from palette
uint32_t GPU::getColorFromIndex(uint8_t colorIndex) {
    switch (colorIndex) {
    case 0: return 0xFF000000; // Black
    case 1: return 0xFF555555; // Dark Gray
    case 2: return 0xFFAAAAAA; // Light Gray
    case 3: return 0xFFFFFFFF; // White
    default: return 0xFF000000; // Default to black
    }
}

// Add a sprite to the GPU
void GPU::addSprite(uint8_t tileId, int x, int y, bool xFlip, bool yFlip) {
    for (int i = 0; i < MAX_SPRITES; ++i) {
        if (oam[i * 4] == 0) { // Assuming 0 means no sprite
            oam[i * 4] = y + 16; // Sprite Y
            oam[i * 4 + 1] = x + 8; // Sprite X
            oam[i * 4 + 2] = tileId; // Tile ID
            oam[i * 4 + 3] = (xFlip ? 0x20 : 0) | (yFlip ? 0x40 : 0); // Attributes
            return;
        }
    }
}

// Remove a sprite by index
void GPU::removeSprite(int index) {
    if (index >= 0 && index < MAX_SPRITES) {
        oam[index * 4] = 0; // Clear sprite
    }
}

// Clear all sprites
void GPU::clearSprites() {
    std::fill(oam.begin(), oam.end(), 0);
}