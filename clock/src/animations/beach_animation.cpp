#include "animation_base.h"

void Animations::initBeach(AnimationManager* manager) {
    manager->state.waveScale = 1.0f;
    manager->state.cyclePosition = 0.0f;
    manager->state.treePos.x = 120;
    manager->state.treePos.y = 30;
}

void Animations::renderBeach(AnimationManager* manager) {
    float time = manager->state.time * 0.005f;
    
    // Update wave animation
    manager->state.cyclePosition = fmod(time * 0.1f, 1.0f);
    if (manager->state.cyclePosition <= 0.35f) {
        manager->state.waveScale = 1.0f + (manager->state.cyclePosition / 0.35f) * 0.3f;
    } else if (manager->state.cyclePosition <= 0.69f) {
        manager->state.waveScale = 1.3f - ((manager->state.cyclePosition - 0.35f) / (0.69f - 0.35f)) * 0.3f;
    } else {
        manager->state.waveScale = 1.0f;
    }
    
    // Sky gradient (40% of height)
    for (int y = 0; y < 26; y++) {  // 40% of 64
        uint8_t blue1 = 3;   // #037ccb deep blue component
        uint8_t blue2 = 130; // #82ccef light blue component
        uint8_t blue = map(y, 0, 25, blue1, blue2);
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            manager->drawPixelWithBlend(x, y, blue/3, blue*2/3, blue);
        }
    }
    
    // Dry sand background
    for (int y = 42; y < DISPLAY_HEIGHT; y++) {  // 65% to bottom
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            manager->drawPixelWithBlend(x, y, 253, 241, 215);  // #fdf1d7
        }
    }
    
    // Sea with animation
    int seaTop = 26;  // 40% of 64
    int seaHeight = (int)(19.2f * manager->state.waveScale);  // 30% of 64, scaled
    
    for (int y = seaTop; y < seaTop + seaHeight && y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            // Sea gradient
            float gradient = (float)(y - seaTop) / seaHeight;
            uint8_t r, g, b;
            if (gradient < 0.25f) {
                float t = gradient / 0.25f;
                r = (uint8_t)(8 + t * (18 - 8));
                g = (uint8_t)(122 + t * (156 - 122));
                b = (uint8_t)(193 + t * (192 - 193));
            } else if (gradient < 0.5f) {
                float t = (gradient - 0.25f) / 0.25f;
                r = (uint8_t)(18 + t * (42 - 18));
                g = (uint8_t)(156 + t * (212 - 156));
                b = (uint8_t)(192 + t * (229 - 192));
            } else if (gradient < 0.75f) {
                float t = (gradient - 0.5f) / 0.25f;
                r = (uint8_t)(42 + t * (150 - 42));
                g = (uint8_t)(212 + t * (233 - 212));
                b = (uint8_t)(229 + t * (239 - 229));
            } else {
                float t = (gradient - 0.75f) / 0.25f;
                r = (uint8_t)(150 + t * (222 - 150));
                g = (uint8_t)(233 + t * (236 - 233));
                b = (uint8_t)(239 + t * (211 - 239));
            }
            manager->drawPixelWithBlend(x, y, r, g, b);
        }
    }
    
    // Wet sand animation
    float wetSandOpacity = 0.2f;
    if (manager->state.cyclePosition >= 0.34f && manager->state.cyclePosition <= 0.35f) {
        wetSandOpacity = 0.2f + ((manager->state.cyclePosition - 0.34f) / 0.01f) * 0.2f;  // 0.2 to 0.4
    } else if (manager->state.cyclePosition > 0.35f) {
        wetSandOpacity = 0.4f - ((manager->state.cyclePosition - 0.35f) / 0.65f) * 0.2f;  // 0.4 back to 0.2
    }
    
    // Draw wet sand with opacity
    for (int y = seaTop; y < seaTop + 24 && y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t alpha = (uint8_t)(wetSandOpacity * 255);
            manager->drawPixelWithBlend(x, y, 236, 192, 117, alpha);  // #ecc075
        }
    }
    
    // Seabird (render before tree) - use sin16 for optimized sine wave
    int birdX = (int)(fmod(time * 10, 150)) - 10;
    int birdY = 8 + (int)((sin16(TO_SIN16(time * 0.5f)) / 32767.0f) * 3);
    
    if (birdX >= 0 && birdX < DISPLAY_WIDTH - 4 && birdY >= 0 && birdY < DISPLAY_HEIGHT) {
        // Simple bird shape
        manager->drawPixelWithBlend(birdX, birdY - 1, 80, 80, 80);
        manager->drawPixelWithBlend(birdX - 2, birdY, 80, 80, 80);
        manager->drawPixelWithBlend(birdX + 2, birdY, 80, 80, 80);
        manager->drawPixelWithBlend(birdX + 1, birdY + 1, 80, 80, 80);
    }
    
    // Palm tree
    int treeX = (int)manager->state.treePos.x;
    int treeY = (int)manager->state.treePos.y;
    
    // Palm trunk - curved
    for (int y = 0; y < 52; y++) {  // Height of trunk
        int x = treeX - (int)((y * y) / 50.0f);  // Curve calculation
        if (x >= 0 && x < DISPLAY_WIDTH && treeY - 32 + y >= 0 && treeY - 32 + y < DISPLAY_HEIGHT) {
            // Draw trunk width
            for (int w = 0; w < 6; w++) {
                if (x + w - 3 >= 0 && x + w - 3 < DISPLAY_WIDTH) {
                    manager->drawPixelWithBlend(x + w - 3, treeY - 32 + y, 170, 131, 102);  // #aa8366
                }
            }
        }
    }
    
    // Palm leaves (simplified triangular shapes)
    int leafColors[3][3] = {{57, 77, 0}, {70, 115, 1}, {70, 90, 5}};  // RGB values
    int leafX = treeX - 8;  // Top of curved trunk
    int leafY = treeY - 32;
    
    // Draw 3 palm leaves
    for (int leaf = 0; leaf < 3; leaf++) {
        int startX = leafX + leaf * 8 - 8;
        int startY = leafY + leaf * 6 + 6;
        
        // Draw triangular leaf
        for (int i = 0; i < 15; i++) {  // Leaf length
            int width = max(1, 4 - i / 4);  // Tapering width
            for (int w = 0; w < width; w++) {
                int px = startX + i - leaf * 3;
                int py = startY + w - width / 2;
                if (px >= 0 && px < DISPLAY_WIDTH && py >= 0 && py < DISPLAY_HEIGHT) {
                    manager->drawPixelWithBlend(px, py, leafColors[leaf][0], leafColors[leaf][1], leafColors[leaf][2]);
                }
            }
        }
    }
    
    // Wave ripples - use sin16 for optimized wave calculations
    for (int i = 0; i < 32; i++) {
        int x = i * 4;
        int waveY = seaTop + (int)((sin16(TO_SIN16(time * 3 + i * 0.3f)) / 32767.0f) * 1.5f);
        float rippleOpacity = (sin16(TO_SIN16(time * 2 + i * 0.2f)) / 32767.0f) * 0.3f + 0.3f;
        uint8_t alpha = (uint8_t)(rippleOpacity * 255);
        if (x < DISPLAY_WIDTH && waveY >= 0 && waveY < DISPLAY_HEIGHT) {
            manager->drawPixelWithBlend(x, waveY, 255, 255, 255, alpha);
            if (x + 1 < DISPLAY_WIDTH) {
                manager->drawPixelWithBlend(x + 1, waveY, 255, 255, 255, alpha);
            }
        }
    }
} 