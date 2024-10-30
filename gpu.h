#include <array>
#include <cstdint>
#include <vector>
#include <string>

#ifndef gpu_H
#define gpu_H

// Constants for screen size
constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 144;
constexpr int TITLE_SIZE = 8;
constexpr int TITLE_MAP_SIZE = 32;
constexpr int VRAM_SIZE = 8192;
constexpr int MAX_SPRITES = 40; // Game Boy hardware limit for max sprites per frame

// VRAM and OAM memory
std::array<uint8_t, VRAM_SIZE> vram;
std::array<uint8_t, MAX_SPRITES * 4> oam; // 4 bytes per sprite

// Scroll registers
uint8_t scrollX = 0;
uint8_t scrollY = 0;

// Background tile map (assuming a 32x32 grid of tiles)
std::array<std::array<uint8_t, TITLE_MAP_SIZE>, TITLE_MAP_SIZE> backgroundTileMap;

// Enum to manage GPU
enum class GPUMode {
    HBlank, VBlank, OAM, VRAM
};

class GPU {
private:
    // RGBA frame buffer
    uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT * 4];
    // GPU mode
    GPUMode mode;
    // Counts cycles per line/frame
    uint16_t cycleCounter;
    int currentScanline;

    // Additional private functions for rendering
    void drawTile(int x, int y, uint8_t tileId, bool isBackground);
    void drawSprite(int x, int y, uint8_t tileId, bool xFlip, bool yFlip);

    // Helper functions for loading textures
    void loadTexture(const std::string& filePath);
    void addSprite(int x, int y, uint8_t tileId, bool xFlip, bool yFlip);

public:
    // Constructor
    GPU();

    // Reset GPU
    void reset();
    // GPU cycle operations
    void step(uint8_t cycles);
    // Render a single scanline
    void renderScanLine();
    // Render the full frame
    void renderFrame();

    // Access frame buffer pointer
    uint8_t* getFrameBuffer();

    // Helper functions
    uint8_t* getTileData(uint8_t tileId);
    uint32_t getColorFromIndex(uint8_t colorIndex);

    // Functions to manage sprites
    void addSprite(uint8_t tileId, int x, int y, bool xFlip = false, bool yFlip = false);
    void removeSprite(int index); // to remove a sprite by index
    void clearSprites(); // to clear all sprites
};

#endif
