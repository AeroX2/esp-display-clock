#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>
#include <FastLED.h>
#include <FastTrig.h>
#include "display.h"

// Animation types
enum AnimationType {
    ANIM_PLASMA = 0,
    ANIM_PARTICLES,
    ANIM_FIRE,
    ANIM_SKYLINE,
    ANIM_GALAXY,
    ANIM_LIGHTNING,
    ANIM_PIPES,
    ANIM_ORBITAL,
    ANIM_STARS,
    ANIM_BEACH,
    ANIM_COUNT  // Total number of animations
};

// Animation timing constants
#define ANIMATION_CYCLE_DURATION (3 * 60 * 60 * 1000UL)  // 30 seconds for testing (change to 3 * 60 * 60 * 1000UL for production)
#define FADE_DURATION 2000  // 2 seconds fade duration
#define MAX_PARTICLES 15
#define MAX_LIGHTNING 3
#define MAX_PIPES 5
#define MAX_ORBITAL_PARTICLES 20
#define MAX_STARS 50

// Structures for animation data
struct Particle {
    float x, y;
    float velocityX, velocityY;
    uint8_t red, green, blue;
    uint8_t trail[200];  // Trail positions
    uint8_t trailLength;
    bool active;
};

struct Lightning {
    struct Point {
        float x, y;
    };
    Point branches[3][64];  // 3 branches, max 64 points each
    uint8_t branchLengths[3];
    float progress;
    uint32_t frame;
    bool reachedGround;
    bool active;
};

struct Building {
    float x, y;
    uint8_t width, height;
    bool slantedTop;
    float slantedTopHeight;
    bool slantedTopDirection;
    bool spireTop;
    float spireTopWidth, spireTopHeight;
    bool antennaTop;
    float antennaTopWidth, antennaTopHeight;
    uint16_t color;
};

struct Star {
    float x, y;
    float size;
    float brightness;
    float twinkleSpeed;
    float phase;
};

// Animation state structure
struct AnimationState {
    uint32_t frameCount;
    float time;
    
    // Animation-specific data
    Particle particles[MAX_PARTICLES];
    Lightning lightnings[MAX_LIGHTNING];
    Building buildings[50];  // For skyline
    Star stars[MAX_STARS];
    
    // Pipes and orbital data
    struct {
        float x, y;
        uint8_t direction;
        uint8_t red, green, blue;
        float trail[200][2];  // x,y pairs
        uint8_t trailLength;
        uint8_t moveCounter;
        uint8_t lastDirection;
    } pipes[MAX_PIPES];
    
    struct {
        float x, y;
        float velocityX, velocityY;
        uint8_t red, green, blue;
        float trail[200][2];
        uint8_t trailLength;
    } orbitalParticles[MAX_ORBITAL_PARTICLES];
    
    struct {
        float x, y;
        float velocityX, velocityY;
    } gravityPoint;
    
    // Beach animation data
    float waveScale;
    float cyclePosition;
    struct {
        float x, y;
    } treePos;
    
    bool initialized;
};

// Animation manager
class AnimationManager {
private:
    AnimationType currentAnimation;
    AnimationType nextAnimation;
    uint32_t lastCycleTime;
    uint32_t fadeStartTime;
    bool inFade;
    uint8_t fadeProgress;  // 0-255
    bool autoMode;  // True for auto-cycling, false for manual control
    
    // Helper functions
    float hue2rgb(float p, float q, float t);
    
public:
    AnimationState state;  // Make state public so individual animations can access it
    
    // Make helper functions public so individual animations can use them
    void hslToRgb(float h, float s, float l, uint8_t* r, uint8_t* g, uint8_t* b);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void drawPixelWithBlend(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);
    void applyFade(uint8_t fadeAmount);
    AnimationManager();
    void begin();
    void update();
    void initializeAnimation(AnimationType type);
    
    // Manual control methods
    void setAnimation(AnimationType type);
    void setAutoMode(bool enabled);
    bool isAutoMode() const { return autoMode; }
    
    // Individual animation functions (now implemented in separate files)
    void renderPlasma();
    void renderParticles();
    void renderFire();
    void renderSkyline();
    void renderGalaxy();
    void renderLightning();
    void renderPipes();
    void renderOrbital();
    void renderStars();
    void renderBeach();
    
    // Getters
    AnimationType getCurrentAnimation() const { return currentAnimation; }
    bool isInFade() const { return inFade; }
    uint8_t getFadeProgress() const { return fadeProgress; }
};

// Global animation manager instance
extern AnimationManager animationManager;

// Math helper macros
#define TO_SIN16(x) ((x) * 10430)  // Convert float to FastLED 16-bit angle
#define PLASMA_SCALE 0.08f
#define TIME_SCALE 0.03f

#endif // ANIMATIONS_H 