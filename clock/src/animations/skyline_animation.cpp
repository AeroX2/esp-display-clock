#include "animation_base.h"

void Animations::initSkyline(AnimationManager* manager) {
    // Initialize buildings
    for (int layer = 0; layer < 3; layer++) {
        int buildingCount = 8 + layer * 2;
        float x = 0;
        for (int i = 0; i < buildingCount; i++) {
            int idx = layer * 15 + i;
            if (idx >= 50) break;
            
            manager->state.buildings[idx].width = random(8, 20);
            manager->state.buildings[idx].height = random(10, 45);
            manager->state.buildings[idx].x = x;
            manager->state.buildings[idx].y = DISPLAY_HEIGHT - manager->state.buildings[idx].height;
            manager->state.buildings[idx].slantedTop = random(0, 5) == 0;
            manager->state.buildings[idx].spireTop = random(0, 14) == 0;
            manager->state.buildings[idx].antennaTop = random(0, 10) == 0;
            
            // Color based on layer
            uint8_t gray = 60 + layer * 20;
            manager->state.buildings[idx].color = manager->color565(gray, gray, gray);
            
            x += manager->state.buildings[idx].width + random(2, 8);
        }
    }
}

void Animations::renderSkyline(AnimationManager* manager) {
    // Draw sky gradient
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        uint8_t blue = map(y, 0, DISPLAY_HEIGHT, 51, 102);  // #000033 to #000066
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            manager->drawPixelWithBlend(x, y, 0, 0, blue);
        }
    }
    
    // Draw stars - use sin16 for optimized brightness calculation
    for (int i = 0; i < 50; i++) {
        int x = (i * 13) % DISPLAY_WIDTH;
        int y = (i * 7) % 40;
        float brightness = (sin16(TO_SIN16(manager->state.time * 0.1f + i)) / 32767.0f) * 0.5f + 0.5f;
        uint8_t starBright = (uint8_t)(brightness * 255);
        manager->drawPixelWithBlend(x, y, starBright, starBright, starBright);
    }
    
    // Draw buildings (simplified for memory constraints)
    for (int layer = 0; layer < 3; layer++) {
        float speed = 0.02f + layer * 0.04f;
        uint8_t gray = 60 + layer * 20;
        
        for (int i = 0; i < 10; i++) {
            float x = fmod(i * 15 - manager->state.time * speed * 100, DISPLAY_WIDTH + 20) - 10;
            int height = 10 + (i + layer) * 3;
            int width = 6 + layer * 2;
            
            // Draw simple rectangle building
            for (int bx = 0; bx < width && x + bx < DISPLAY_WIDTH; bx++) {
                for (int by = 0; by < height && DISPLAY_HEIGHT - by >= 0; by++) {
                    if (x + bx >= 0) {
                        manager->drawPixelWithBlend((int)(x + bx), DISPLAY_HEIGHT - by - 1, gray, gray, gray);
                    }
                }
            }
        }
    }
} 