#include <GL/glut.h>
#include <iostream>
#include <math.h>

// Forward declarations
void keyboardCallback(unsigned char key, int x, int y);

// Mathematical constant for trigonometric calculations
const float PI = 3.14159265f;

// ==========================================
// UTILITY FUNCTIONS - Basic Drawing Primitives
// ==========================================

/**
 * drawCircle: Draws a filled circle using GL_POLYGON
 * @param radius: Circle radius in normalized coordinates
 * @param r, g, b: RGB color values (0.0 to 1.0)
 * Uses 36 vertices (10-degree increments) for smooth circle approximation
 */
void drawCircle(float radius, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10)
    {
        float theta = i * PI / 180.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

/**
 * drawCircleAlpha: Draws a semi-transparent circle with alpha blending
 * @param radius: Circle radius in normalized coordinates
 * @param r, g, b: RGB color values (0.0 to 1.0)
 * @param a: Alpha (transparency) value (0.0 = fully transparent, 1.0 = opaque)
 */
void drawCircleAlpha(float radius, float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10)
    {
        float theta = i * PI / 180.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

/**
 * drawCar: Draws a simple 2D car with body and wheels
 * Composition: Red body (quads) + two black wheels (circles)
 * Used as a generic car sprite in village and city scenes
 */
void drawCar()
{
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, 0.0f);
    glVertex2f(0.1f, 0.0f);
    glVertex2f(0.1f, 0.05f);
    glVertex2f(-0.1f, 0.05f);
    glEnd();

    glColor3f(0.6f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, 0.05f);
    glVertex2f(0.05f, 0.05f);
    glVertex2f(0.03f, 0.08f);
    glVertex2f(-0.03f, 0.08f);
    glEnd();

    glPushMatrix();
    glTranslatef(-0.06f, 0.0f, 0.0f);
    drawCircle(0.02f, 0.1f, 0.1f, 0.1f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.06f, 0.0f, 0.0f);
    drawCircle(0.02f, 0.1f, 0.1f, 0.1f);
    glPopMatrix();
}

/**
 * drawTree: Draws a stylized tree with trunk and layered foliage
 * @param swayAngle: Wind sway rotation angle applied to entire tree
 * Composition: Brown trunk + 4 layers of green triangles (cone shape) + circular highlight
 * Used in village scene for natural environment
 */
void drawTree(float swayAngle)
{
    glPushMatrix();
    glRotatef(swayAngle, 0.0f, 0.0f, 1.0f);

    // Trunk - brown wood
    glColor3f(0.4f, 0.2f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-0.035f, 0.0f);
    glVertex2f(0.035f, 0.0f);
    glVertex2f(0.035f, 0.25f);
    glVertex2f(-0.035f, 0.25f);
    glEnd();

    // Foliage - traditional cone/triangle shape
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.0f);

    // Bottom layer - widest part
    glColor3f(0.1f, 0.45f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.25f, -0.05f);
    glVertex2f(0.25f, -0.05f);
    glVertex2f(0.0f, 0.25f);
    glEnd();

    // Middle layer - lighter green with offset
    glColor3f(0.15f, 0.55f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.18f, 0.0f);
    glVertex2f(0.18f, 0.0f);
    glVertex2f(0.0f, 0.22f);
    glEnd();
    glPopMatrix();

    // Top layer - even brighter green
    glColor3f(0.2f, 0.65f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, 0.14f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.12f, 0.0f);
    glVertex2f(0.12f, 0.0f);
    glVertex2f(0.0f, 0.18f);
    glEnd();
    glPopMatrix();

    // Highlight - brightest part at top
    glColor3f(0.25f, 0.75f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    drawCircle(0.08f, 0.25f, 0.75f, 0.2f);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}

/**
 * lerp: Linear interpolation between two values
 * @param start: Starting value
 * @param end: Ending value
 * @param t: Time/blend factor (0.0 to 1.0, clamped)
 * @return: Interpolated value between start and end
 * Used for smooth animations and transitions
 */
float lerp(float start, float end, float t)
{
    if (t > 1.0f)
        t = 1.0f;
    return start + t * (end - start);
}

// ==========================================
// GRAPHICS ALGORITHMS IMPLEMENTATION
// ==========================================

/**
 * MIDPOINT CIRCLE ALGORITHM (Bresenham Circle Algorithm)
 * Efficiently draws circles using integer arithmetic and octant symmetry
 * Reduces computation by only calculating one octant and mirroring
 * @param centerX, centerY: Center position of circle
 * @param radius: Circle radius
 * @param r, g, b: RGB color values
 */
void drawCircleBresenham(float centerX, float centerY, float radius, float r, float g, float b)
{
    glColor3f(r, g, b);
    glPointSize(2.0f);
    glBegin(GL_POINTS);

    int x = 0;
    int y = (int)radius;
    int d = 3 - 2 * (int)radius; // Decision parameter

    while (x <= y)
    {
        // Plot 8 symmetrical points (octant symmetry)
        glVertex2f(centerX + x, centerY + y);
        glVertex2f(centerX - x, centerY + y);
        glVertex2f(centerX + x, centerY - y);
        glVertex2f(centerX - x, centerY - y);
        glVertex2f(centerX + y, centerY + x);
        glVertex2f(centerX - y, centerY + x);
        glVertex2f(centerX + y, centerY - x);
        glVertex2f(centerX - y, centerY - x);

        // Update decision parameter and coordinates
        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    glEnd();
    glPointSize(1.0f);
}

/**
 * DDA LINE ALGORITHM (Digital Differential Analyzer)
 * Draws lines using floating-point arithmetic
 * Calculates incremental steps along x and y axes
 * @param x1, y1: Starting point coordinates
 * @param x2, y2: Ending point coordinates
 * @param r, g, b: RGB color values
 * @param width: Line width in pixels
 */
void drawLineDDA(float x1, float y1, float x2, float y2, float r, float g, float b, float width)
{
    glColor3f(r, g, b);
    glLineWidth(width);

    float dx = x2 - x1;
    float dy = y2 - y1;

    // Calculate number of steps (use max of |dx| and |dy|)
    float steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);

    if (steps == 0.0f)
        steps = 1.0f; // Prevent division by zero

    // Calculate increment for each step
    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; i++)
    {
        glVertex2f(x, y);
        x += xInc;
        y += yInc;
    }
    glEnd();
    glLineWidth(1.0f);
}


// ==========================================
// FORWARD DECLARATIONS (Fixes Scope Errors)
// ==========================================
void drawVillage();
void drawCity();
void drawFuture();

// ==========================================
// GLOBAL VARIABLES & SYSTEM STATES
// ==========================================

/**
 * Scene State Enumeration
 * VILLAGE: Initial peaceful rural scene
 * TRANSITION_TO_CITY: Fade between village and city
 * CITY: Urban city environment
 * TRANSITION_TO_FUTURE: Sci-fi holographic transition
 * FUTURE: 2070 futuristic space city
 */
enum SceneState
{
    VILLAGE,
    TRANSITION_TO_CITY,
    CITY,
    TRANSITION_TO_FUTURE,
    FUTURE
};

// Current active scene state
SceneState currentState = VILLAGE;

// Pause state variable - controls animation and scene transitions
bool isPaused = false;           // true = animations frozen, false = running normally

float cloudX = -1.2f;           // Horizontal position of animated cloud
float carX = -1.2f;             // Horizontal position of village car in water
float holoAngle = 0.0f;         // Rotation angle for holographic transition effects
float trafficTimer = 0.0f;      // Timer countdown for traffic light state cycling
int trafficState = 0;           // Traffic light state: 0=red, 1=yellow, 2=green
float skylineOffset = 0.0f;     // Horizontal scroll offset for parallax skyline animation
float cityCarX = -1.2f;         // Horizontal position of city street car
float fadeAlpha = 0.0f;         // Alpha transparency for scene fade transitions (0.0-1.0)
float windmillAngle = 0.0f;     // Current rotation angle of windmill blades (in degrees)
float boatX = 1.2f;             // Horizontal position of animated boat on water
float smokeProgress = 0.0f;     // Animation progress for rising smoke effect (0.0-1.0)
float windTime = 0.0f;          // Time counter for wind simulation
float windStreakX = -1.5f;      // Horizontal position of wind streak animation
float birdX = -1.0f;            // Horizontal position of primary bird flock
float rippleOffset = 0.0f;      // Animation offset for water ripple effects
float timer = 0.0f;             // General-purpose timer for animations
float transitionProgress = 0.0f; // Scene transition progress (0.0-1.0)
float pulseTime = 0.0f;         // Time counter for pulsing/glowing effects
float hoverY = 0.0f;            // Vertical hover offset for floating animation

// ======== FUTURE SCENE VARIABLES ========
float metro2070X = -3.0f;       // Horizontal position of futuristic metro train
float spaceCar1X = 1.5f;        // Horizontal position of first space hover car
float spaceCar2X = -1.5f;       // Horizontal position of second space hover car

// ======== BACKGROUND COLOR ========
float bgR = 0.5f, bgG = 0.8f, bgB = 1.0f;  // Background RGB color (updated per scene)

// ======== VILLAGE SECONDARY ANIMATION VARIABLES ========
float birdX2  =  0.30f;         // Horizontal position of secondary bird flock
float cloudX2 =  0.50f;         // Horizontal position of secondary cloud
float cloudX3 = -0.40f;         // Horizontal position of tertiary cloud

// ==========================================
// PAUSE/INPUT SYSTEM
// ==========================================

/**
 * displayPauseOverlay: Called when paused (no visual overlay)
 * Currently just a placeholder for potential future pause UI
 */
void displayPauseOverlay()
{
    // No overlay - just pause the animations
}

/**
 * keyboardCallback: Handles keyboard input for pause/resume
 * @param key: ASCII key pressed
 * @param x, y: Mouse position (unused)
 * 
 * SPACE: Toggle pause state
 * ESC: Exit application
 */
void keyboardCallback(unsigned char key, int x, int y)
{
    std::cout << "Key pressed: " << (int)key << std::endl;  // Debug output
    if (key == ' ')  // SPACE key to pause/resume
    {
        isPaused = !isPaused;
        std::cout << "Pause toggled: " << (isPaused ? "PAUSED" : "RUNNING") << std::endl;
        glutPostRedisplay();
    }
    else if (key == 27)  // ESC key to exit
    {
        exit(0);
    }
}

// DRAWING MODULES 

// Village

/**
 * drawFence: Renders a wooden fence segment
 * @param x, y: Position of fence base
 * 
 * Composition: Vertical wooden slats connected by horizontal rails
 * Transformation: Translation to position
 */
void drawFence(float x, float y)
{
    glColor3f(0.5f, 0.3f, 0.1f);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    for (float i = 0; i < 0.3f; i += 0.08f)
    {
        glBegin(GL_QUADS);
        glVertex2f(i, 0.0f);
        glVertex2f(i + 0.02f, 0.0f);
        glVertex2f(i + 0.02f, 0.15f);
        glVertex2f(i, 0.15f);
        glEnd();
    }
    glBegin(GL_QUADS);
    glVertex2f(-0.02f, 0.08f);
    glVertex2f(0.3f, 0.08f);
    glVertex2f(0.3f, 0.1f);
    glVertex2f(-0.02f, 0.1f);
    glEnd();
    glPopMatrix();
}

/**
 * drawBoat: Renders animated boat on water
 * Uses boatX variable for horizontal position
 * 
 * Composition: Wooden hull (brown polygon), mast (line), sail (white lines)
 * Transformation: Translation (boatX for animation)
 */
void drawBoat()
{
    glPushMatrix();
    glTranslatef(boatX, -0.65f, 0.0f);
    glColor3f(0.5f, 0.25f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.15f, 0.0f);
    glVertex2f(0.15f, 0.0f);
    glVertex2f(0.1f, -0.08f);
    glVertex2f(-0.1f, -0.08f);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.25f);
    glEnd();
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.05f);
    glVertex2f(0.0f, 0.25f);
    glVertex2f(0.15f, 0.1f);
    glEnd();
    glPopMatrix();
}

/**
 * drawWindmill: Renders rotating windmill with animated blades
 * Uses windmillAngle variable for blade rotation
 * 
 * Composition: Wooden tower (beige), rotating blades (4 triangles),
 * center hub. Blades rotate 90° between each iteration.
 * Transformation: Translation (position), Rotation (windmillAngle),
 * Scaling (0.8x for size), nested Rotation (blade angles)
 */
void drawWindmill()
{
    glPushMatrix();
    glTranslatef(-0.8f, 0.1f, 0.0f);
    glScalef(0.8f, 0.8f, 1.0f);
    glColor3f(0.6f, 0.5f, 0.4f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.08f, -0.4f);
    glVertex2f(0.08f, -0.4f);
    glVertex2f(0.04f, 0.2f);
    glVertex2f(-0.04f, 0.2f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    glRotatef(windmillAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(0.8f, 0.8f, 0.8f);
    for (int i = 0; i < 4; i++)
    {
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.02f, 0.0f);
        glVertex2f(0.02f, 0.0f);
        glVertex2f(0.0f, 0.3f);
        glEnd();
    }
    glPopMatrix();
    glPopMatrix();
}

/**
 * drawSmoke: Renders rising smoke effect from chimney
 * Uses smokeProgress variable (0.0-1.0) for animation cycle
 * 
 * Composition: Translucent grey circles that rise and expand
 * Effect: Y offset increases, size grows, alpha decreases with progress
 * Transformation: Translation (rising Y motion, X drift)
 */
void drawSmoke()
{
    float yOffset = smokeProgress * 0.3f;
    float size = 0.03f + (smokeProgress * 0.05f);
    float alpha = 1.0f - smokeProgress;
    if (alpha < 0.0f)
        alpha = 0.0f;
    glPushMatrix();
    glTranslatef(-0.35f, 0.1f + yOffset, 0.0f);
    glTranslatef(smokeProgress * 0.15f, 0.0f, 0.0f);
    drawCircleAlpha(size, 0.6f, 0.6f, 0.6f, alpha);
    glPopMatrix();
}

/**
 * drawWindStreaks: Renders animated wind effect lines
 * Uses windStreakX variable for horizontal scrolling
 * 
 * Composition: White semi-transparent lines that simulate wind
 * Transformation: Translation (windStreakX for continuous scroll)
 */
void drawWindStreaks()
{
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(windStreakX, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.3f);
    glVertex2f(0.4f, 0.3f);
    glVertex2f(-0.2f, 0.15f);
    glVertex2f(0.3f, 0.15f);
    glVertex2f(0.1f, -0.1f);
    glVertex2f(0.6f, -0.1f);
    glEnd();
    glPopMatrix();
}

/**
 * drawBirdFlock: Renders a flock of birds in formation
 * @param x, y: Center position of flock
 * @param scale: Size scaling of entire flock
 * 
 * Composition: 5 birds in V-formation using line segments (V-shapes)
 * Each bird is progressively scaled smaller for depth effect
 * Transformation: Translation (x,y position), nested Translations (bird offsets),
 * Scaling (individual bird sizes 0.75x, 0.65x, 0.58x, 0.52x)
 */
void drawBirdFlock(float x, float y, float scale);

void drawBirds()
{
    drawBirdFlock(birdX, 0.6f, 1.0f);
}

/**
 * drawSunRays: Renders sun with radiating ray effects
 * 
 * Composition: Central sun circle (yellow) with two rings of rays
 * Inner rays (opaque yellow), outer rays (semi-transparent yellow glow)
 * Uses 12 rays at 30-degree intervals
 * Transformation: Translation to top-right position (0.6f, 0.6f)
 */
void drawSunRays()
{
    glPushMatrix();
    glTranslatef(0.6f, 0.6f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw radiating sun rays
    glLineWidth(2.0f);
    for (int i = 0; i < 12; i++)
    {
        float angle = (i / 12.0f) * 2.0f * PI;
        float rayStart = 0.16f;
        float rayEnd = 0.38f;

        float x1 = sin(angle) * rayStart;
        float y1 = cos(angle) * rayStart;
        float x2 = sin(angle) * rayEnd;
        float y2 = cos(angle) * rayEnd;

        glColor4f(1.0f, 0.85f, 0.1f, 0.6f);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    // Outer glow for sun rays
    glLineWidth(3.0f);
    glColor4f(1.0f, 0.85f, 0.1f, 0.2f);
    for (int i = 0; i < 12; i++)
    {
        float angle = (i / 12.0f) * 2.0f * PI;
        float rayStart = 0.16f;
        float rayEnd = 0.38f;

        float x1 = sin(angle) * rayStart;
        float y1 = cos(angle) * rayStart;
        float x2 = sin(angle) * rayEnd;
        float y2 = cos(angle) * rayEnd;

        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    glDisable(GL_BLEND);
    glPopMatrix();
}

/**
 * drawSkyGradient: Renders sky background with color gradient
 * 
 * Creates vertical gradient: Deep azure (top) to pale peachy-blue (horizon)
 * Uses GL_QUADS with different colors per vertex for smooth interpolation
 * Effect: Warm sunset/morning sky aesthetic
 */
void drawSkyGradient()
{
    // Warm sky gradient: deeper blue at top, soft peachy horizon
    glBegin(GL_QUADS);
    glColor3f(0.32f, 0.58f, 0.92f);  // deep azure top
    glVertex2f(-1.0f, 1.0f);
    glVertex2f( 1.0f, 1.0f);
    glColor3f(0.60f, 0.82f, 1.0f);   // pale sky at horizon
    glVertex2f( 1.0f, -0.10f);
    glVertex2f(-1.0f, -0.10f);
    glEnd();
}

/**
 * drawMountains: Renders layered mountain landscape
 * 
 * Three mountain ranges:
 * 1. Far range: Misty blue-purple peaks with snow caps
 * 2. Mid range: Darker green-grey peaks with shadow sides
 * 3. Atmospheric haze: Semi-transparent overlay at mountain base
 * Uses shading (darker faces, lighter highlights) for 3D effect
 */
void drawMountains()
{
    // === FAR RANGE: misty blue-purple peaks ===
    glColor3f(0.52f, 0.60f, 0.76f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.00f,-0.08f); glVertex2f(-0.48f,-0.08f); glVertex2f(-0.70f, 0.74f);
    glVertex2f(-0.62f,-0.08f); glVertex2f(-0.02f,-0.08f); glVertex2f(-0.30f, 0.84f);
    glVertex2f(-0.08f,-0.08f); glVertex2f( 0.44f,-0.08f); glVertex2f( 0.18f, 0.70f);
    glVertex2f( 0.32f,-0.08f); glVertex2f( 0.88f,-0.08f); glVertex2f( 0.60f, 0.80f);
    glVertex2f( 0.72f,-0.08f); glVertex2f( 1.00f,-0.08f); glVertex2f( 0.88f, 0.52f);
    glEnd();

    // Shadow faces (right side darker)
    glColor3f(0.40f, 0.48f, 0.65f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.70f, 0.74f); glVertex2f(-0.48f,-0.08f); glVertex2f(-0.42f, 0.24f);
    glVertex2f(-0.30f, 0.84f); glVertex2f(-0.02f,-0.08f); glVertex2f( 0.02f, 0.28f);
    glVertex2f( 0.18f, 0.70f); glVertex2f( 0.44f,-0.08f); glVertex2f( 0.46f, 0.22f);
    glVertex2f( 0.60f, 0.80f); glVertex2f( 0.88f,-0.08f); glVertex2f( 0.86f, 0.26f);
    glEnd();

    // Snow caps on tallest far peaks
    glColor3f(0.93f, 0.96f, 1.00f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.36f, 0.66f); glVertex2f(-0.24f, 0.66f); glVertex2f(-0.30f, 0.84f);
    glVertex2f(-0.76f, 0.57f); glVertex2f(-0.64f, 0.57f); glVertex2f(-0.70f, 0.74f);
    glVertex2f( 0.54f, 0.62f); glVertex2f( 0.66f, 0.62f); glVertex2f( 0.60f, 0.80f);
    glVertex2f( 0.12f, 0.54f); glVertex2f( 0.24f, 0.54f); glVertex2f( 0.18f, 0.70f);
    glEnd();

    // === MID RANGE: darker green-grey peaks ===
    glColor3f(0.26f, 0.38f, 0.28f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.00f,-0.14f); glVertex2f(-0.52f,-0.14f); glVertex2f(-0.76f, 0.46f);
    glVertex2f(-0.58f,-0.14f); glVertex2f(-0.06f,-0.14f); glVertex2f(-0.36f, 0.58f);
    glVertex2f( 0.02f,-0.14f); glVertex2f( 0.52f,-0.14f); glVertex2f( 0.24f, 0.44f);
    glVertex2f( 0.40f,-0.14f); glVertex2f( 0.90f,-0.14f); glVertex2f( 0.64f, 0.54f);
    glVertex2f( 0.80f,-0.14f); glVertex2f( 1.00f,-0.14f); glVertex2f( 0.92f, 0.36f);
    glEnd();

    // Mid-range shadow sides
    glColor3f(0.16f, 0.26f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.76f, 0.46f); glVertex2f(-0.52f,-0.14f); glVertex2f(-0.46f, 0.12f);
    glVertex2f(-0.36f, 0.58f); glVertex2f(-0.06f,-0.14f); glVertex2f(-0.02f, 0.18f);
    glVertex2f( 0.64f, 0.54f); glVertex2f( 0.90f,-0.14f); glVertex2f( 0.88f, 0.18f);
    glEnd();

    // Atmospheric haze/mist at mountain base
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.62f, 0.76f, 0.92f, 0.38f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,-0.20f); glVertex2f( 1.0f,-0.20f);
    glVertex2f( 1.0f, 0.10f); glVertex2f(-1.0f, 0.10f);
    glEnd();
    glDisable(GL_BLEND);
}

/**
 * drawCloud: Renders fluffy cloud sprite with soft edges
 * @param x, y: Center position of cloud
 * @param sc: Size scaling factor
 * 
 * Composition: 7 overlapping circles (puffs) with alpha blending
 * Shadow layer below main cloud body
 * Transformation: Translation (x,y), nested Translations (puff offsets),
 * Scaling (sc parameter)
 */
void drawCloud(float x, float y, float sc)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(sc, sc, 1.0f);

    // Soft grey-blue shadow below cloud
    glPushMatrix(); glTranslatef(0.0f, -0.018f, 0.0f);
    drawCircleAlpha(0.092f, 0.74f, 0.80f, 0.90f, 0.28f);
    glPopMatrix();

    // Core puffs — bright white
    drawCircleAlpha(0.078f, 1.00f, 1.00f, 1.00f, 0.94f);

    glPushMatrix(); glTranslatef( 0.092f,  0.006f, 0.0f);
    drawCircleAlpha(0.062f, 0.98f, 0.98f, 1.00f, 0.94f); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.092f,  0.000f, 0.0f);
    drawCircleAlpha(0.058f, 0.97f, 0.97f, 1.00f, 0.94f); glPopMatrix();

    glPushMatrix(); glTranslatef( 0.042f,  0.066f, 0.0f);
    drawCircleAlpha(0.068f, 1.00f, 1.00f, 1.00f, 0.94f); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.030f,  0.058f, 0.0f);
    drawCircleAlpha(0.056f, 0.98f, 0.98f, 1.00f, 0.94f); glPopMatrix();

    glPushMatrix(); glTranslatef( 0.140f, -0.012f, 0.0f);
    drawCircleAlpha(0.048f, 0.97f, 0.97f, 1.00f, 0.90f); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.145f, -0.010f, 0.0f);
    drawCircleAlpha(0.044f, 0.97f, 0.97f, 1.00f, 0.88f); glPopMatrix();

    glPopMatrix();
    glDisable(GL_BLEND);
}

/**
 * drawSingleFlower: Renders a single flower sprite
 * @param x, y: Position of flower base
 * @param r, g, b: Petal color
 * 
 * Composition: Green stem (line), 4 petals (quads), yellow center (circle)
 * Transformation: Translation to position, nested Translation for center
 */
void drawSingleFlower(float x, float y, float r, float g, float b)
{
    float cy = y + 0.044f;
    float sz = 0.013f;
    float pl = 0.026f;

    // Stem
    glColor3f(0.22f, 0.52f, 0.12f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y); glVertex2f(x, y + 0.04f);
    glEnd();
    glLineWidth(1.0f);

    // Four petals
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x-sz, cy);     glVertex2f(x+sz, cy);
    glVertex2f(x+sz, cy+pl);  glVertex2f(x-sz, cy+pl);   // top
    glVertex2f(x-sz, cy-pl);  glVertex2f(x+sz, cy-pl);
    glVertex2f(x+sz, cy);     glVertex2f(x-sz, cy);       // bottom
    glVertex2f(x-pl, cy-sz);  glVertex2f(x,    cy-sz);
    glVertex2f(x,    cy+sz);  glVertex2f(x-pl, cy+sz);    // left
    glVertex2f(x,    cy-sz);  glVertex2f(x+pl, cy-sz);
    glVertex2f(x+pl, cy+sz);  glVertex2f(x,    cy+sz);    // right
    glEnd();

    // Yellow centre
    glPushMatrix();
    glTranslatef(x, cy, 0.0f);
    drawCircle(0.012f, 1.0f, 0.88f, 0.0f);
    glPopMatrix();
}

/**
 * drawFlowers: Renders garden of wildflowers with varied colors
 * 
 * Creates 12 individual flowers placed across landscape
 * Colors: Yellow, pink, red, violet in natural distribution
 * Uses drawSingleFlower with different RGB color values
 */
void drawFlowers()
{
    drawSingleFlower(-0.88f, -0.22f, 1.00f, 0.95f, 0.00f);  // yellow
    drawSingleFlower(-0.78f, -0.24f, 1.00f, 0.35f, 0.55f);  // pink
    drawSingleFlower(-0.68f, -0.21f, 0.85f, 0.10f, 0.10f);  // red
    drawSingleFlower(-0.52f, -0.23f, 0.75f, 0.20f, 0.85f);  // violet
    drawSingleFlower(-0.22f, -0.24f, 1.00f, 0.95f, 0.00f);  // yellow
    drawSingleFlower(-0.12f, -0.22f, 0.90f, 0.20f, 0.70f);  // purple
    drawSingleFlower( 0.08f, -0.23f, 1.00f, 0.95f, 0.00f);  // yellow
    drawSingleFlower( 0.22f, -0.25f, 1.00f, 0.40f, 0.60f);  // pink
    drawSingleFlower( 0.58f, -0.22f, 0.85f, 0.10f, 0.10f);  // red
    drawSingleFlower( 0.70f, -0.24f, 1.00f, 0.95f, 0.00f);  // yellow
    drawSingleFlower( 0.82f, -0.21f, 0.70f, 0.20f, 0.85f);  // violet
    drawSingleFlower( 0.92f, -0.23f, 1.00f, 0.40f, 0.55f);  // pink
}

/**
 * drawDirtPath: Renders village dirt road with details
 * 
 * Composition: Brown dirt path (quads), wheel rut grooves (lines),
 * scattered pebbles (point sprites)
 * Texture: Simulated with line patterns and point placement
 */
void drawDirtPath()
{
    // Village lane running through the scene
    glColor3f(0.60f, 0.46f, 0.26f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.44f); glVertex2f( 1.0f, -0.44f);
    glVertex2f( 1.0f, -0.30f); glVertex2f(-1.0f, -0.30f);
    glEnd();

    // Wheel-rut grooves
    glColor3f(0.50f, 0.37f, 0.18f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f,-0.34f); glVertex2f(1.0f,-0.34f);
    glVertex2f(-1.0f,-0.40f); glVertex2f(1.0f,-0.40f);
    glEnd();
    glLineWidth(1.0f);

    // Scattered pebbles
    glColor3f(0.52f, 0.52f, 0.50f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(-0.72f,-0.37f); glVertex2f(-0.55f,-0.33f);
    glVertex2f(-0.30f,-0.41f); glVertex2f(-0.08f,-0.36f);
    glVertex2f( 0.15f,-0.38f); glVertex2f( 0.38f,-0.32f);
    glVertex2f( 0.62f,-0.42f); glVertex2f( 0.80f,-0.35f);
    glEnd();
    glPointSize(1.0f);
}

/**
 * drawWell: Renders stone well structure
 * 
 * Composition: Stone base (quads), wooden support posts and crossbeam,
 * red-tiled roof (triangle), hanging rope
 * Transformation: Translation to position (center of scene)
 */
void drawWell()
{
    // Stone well to the right of the farmhouse
    glPushMatrix();
    glTranslatef(-0.18f, -0.38f, 0.0f);

    // Stone base cylinder
    glColor3f(0.55f, 0.50f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(-0.04f, 0.0f); glVertex2f( 0.04f, 0.0f);
    glVertex2f( 0.04f, 0.10f); glVertex2f(-0.04f, 0.10f);
    glEnd();

    // Stone texture lines
    glColor3f(0.42f, 0.38f, 0.34f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.04f, 0.03f); glVertex2f(0.04f, 0.03f);
    glVertex2f(-0.04f, 0.06f); glVertex2f(0.04f, 0.06f);
    glVertex2f(-0.04f, 0.09f); glVertex2f(0.04f, 0.09f);
    glEnd();

    // Wooden support posts
    glColor3f(0.40f, 0.22f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(-0.045f, 0.08f); glVertex2f(-0.025f, 0.08f);
    glVertex2f(-0.025f, 0.20f); glVertex2f(-0.045f, 0.20f);
    glVertex2f( 0.025f, 0.08f); glVertex2f( 0.045f, 0.08f);
    glVertex2f( 0.045f, 0.20f); glVertex2f( 0.025f, 0.20f);
    glEnd();

    // Cross-beam
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, 0.18f); glVertex2f(0.05f, 0.18f);
    glVertex2f( 0.05f, 0.20f); glVertex2f(-0.05f, 0.20f);
    glEnd();

    // Tiled roof
    glColor3f(0.62f, 0.20f, 0.10f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, 0.20f); glVertex2f(0.06f, 0.20f); glVertex2f(0.0f, 0.27f);
    glEnd();

    // Rope
    glColor3f(0.38f, 0.28f, 0.12f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.19f); glVertex2f(0.0f, 0.10f);
    glEnd();
    glLineWidth(1.0f);

    glPopMatrix();
}

void drawBirdFlock(float x, float y, float bscale)
{
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(1.8f);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(bscale, bscale, 1.0f);

    glBegin(GL_LINE_STRIP);  // lead bird
    glVertex2f(-0.040f, 0.040f); glVertex2f(0.0f, 0.0f); glVertex2f(0.040f, 0.040f);
    glEnd();

    glPushMatrix(); glTranslatef(0.085f, 0.030f, 0.0f); glScalef(0.75f, 0.75f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.040f, 0.040f); glVertex2f(0.0f, 0.0f); glVertex2f(0.040f, 0.040f);
    glEnd(); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.075f, 0.018f, 0.0f); glScalef(0.65f, 0.65f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.040f, 0.040f); glVertex2f(0.0f, 0.0f); glVertex2f(0.040f, 0.040f);
    glEnd(); glPopMatrix();

    glPushMatrix(); glTranslatef(0.150f, 0.055f, 0.0f); glScalef(0.58f, 0.58f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.040f, 0.040f); glVertex2f(0.0f, 0.0f); glVertex2f(0.040f, 0.040f);
    glEnd(); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.140f, 0.048f, 0.0f); glScalef(0.52f, 0.52f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.040f, 0.040f); glVertex2f(0.0f, 0.0f); glVertex2f(0.040f, 0.040f);
    glEnd(); glPopMatrix();

    glPopMatrix();
    glLineWidth(1.0f);
}

// City

/**
 * drawSkyline: Renders animated skyline using DDA algorithm
 * Uses Digital Differential Analyzer to draw building outline edges
 */
void drawSkyline()
{
    // Draw skyline using DDA algorithm for building profiles
    glPushMatrix();
    glTranslatef(skylineOffset, 0.0f, 0.0f);
    
    float prevX = -2.0f;
    float prevH = 0.4f + (sin(prevX * 15.0f) * 0.3f);
    
    // Draw skyline outline using DDA lines
    for (float x = -2.0f + 0.25f; x < 2.0f; x += 0.25f)
    {
        float h = 0.4f + (sin(x * 15.0f) * 0.3f);
        
        // Vertical line (going up or down)
        drawLineDDA(prevX + 0.2f, prevH, prevX + 0.2f, h, 0.3f, 0.3f, 0.4f, 2.0f);
        
        // Horizontal line (roof)
        drawLineDDA(prevX + 0.2f, h, x, h, 0.3f, 0.3f, 0.4f, 2.0f);
        
        prevX = x;
        prevH = h;
    }
    
    // Fill the skyline with color
    glColor3f(0.1f, 0.1f, 0.15f);
    glAlphaFunc(GL_GREATER, 0.0f);
    for (float x = -2.0f; x < 2.0f; x += 0.25f)
    {
        float h = 0.4f + (sin(x * 15.0f) * 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(x, -0.3f);
        glVertex2f(x + 0.2f, -0.3f);
        glVertex2f(x + 0.2f, h);
        glVertex2f(x, h);
        glEnd();
    }
    
    glPopMatrix();
}

/**
 * draw3DBuilding: Renders 3D-style building with depth and windows
 * @param x: Horizontal position
 * @param height: Building height
 * @param r, g, b: Base color of building
 * 
 * Composition: Main face, side face (darker), roof (lighter), window grid
 * Transformation: Translation to position, quads for 3D isometric effect
 */
void draw3DBuilding(float x, float height, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(x, -0.4f, 0.0f);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.2f, height);
    glVertex2f(0.0f, height);
    glEnd();
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.3f, 0.08f);
    glVertex2f(0.3f, height + 0.08f);
    glVertex2f(0.2f, height);
    glEnd();
    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, height);
    glVertex2f(0.2f, height);
    glVertex2f(0.3f, height + 0.08f);
    glVertex2f(0.1f, height + 0.08f);
    glEnd();

    for (float wy = 0.1f; wy < height - 0.1f; wy += 0.15f)
    {
        for (float wx = 0.04f; wx < 0.16f; wx += 0.08f)
        {
            if ((int)(wx * 100 + wy * 100 + x * 10) % 3 != 0)
                glColor3f(0.9f, 0.9f, 0.5f);
            else
                glColor3f(0.15f, 0.15f, 0.2f);
            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + 0.04f, wy);
            glVertex2f(wx + 0.04f, wy + 0.06f);
            glVertex2f(wx, wy + 0.06f);
            glEnd();
        }
    }
    glPopMatrix();
}

/**
 * drawTrafficLight: Renders traffic light pole with LED indicators
 * @param x, y: Position of traffic light base
 * 
 * Composition: Pole (quads), lights box (quads), three LED circles (red/yellow/green)
 * Uses trafficState variable to determine which light is active
 */
void drawTrafficLight(float x, float y) 
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.015f, 0.0f);
    glVertex2f(0.015f, 0.0f);
    glVertex2f(0.015f, 0.4f);
    glVertex2f(-0.015f, 0.4f);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, 0.4f);
    glVertex2f(0.05f, 0.4f);
    glVertex2f(0.05f, 0.7f);
    glVertex2f(-0.05f, 0.7f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, 0.62f, 0.0f);
    if (trafficState == 2)
        drawCircle(0.035f, 1.0f, 0.1f, 0.1f);
    else
        drawCircle(0.035f, 0.3f, 0.0f, 0.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.52f, 0.0f);
    if (trafficState == 1) 
        drawCircle(0.035f, 1.0f, 1.0f, 0.0f);
    else
        drawCircle(0.035f, 0.3f, 0.3f, 0.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    if (trafficState == 0)
        drawCircle(0.035f, 0.0f, 1.0f, 0.0f);
    else
        drawCircle(0.035f, 0.0f, 0.3f, 0.0f);
    glPopMatrix();
    glPopMatrix();
}

/**
 * drawStreetLamp: Renders street lamp post with light fixture
 * @param x: Horizontal position of lamp
 * 
 * Composition: Metal pole, lamp head (triangular), light emission effect
 * Transformation: Translation for positioning
 */
void drawStreetLamp(float x)
{
    glPushMatrix();
    glTranslatef(x, -0.4f, 0.0f);
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.015f, 0.0f);
    glVertex2f(0.015f, 0.0f);
    glVertex2f(0.015f, 0.5f);
    glVertex2f(-0.015f, 0.5f);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, 0.48f);
    glVertex2f(0.06f, 0.48f);
    glVertex2f(0.0f, 0.53f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, 0.47f, 0.0f);
    drawCircle(0.02f, 1.0f, 1.0f, 0.8f);
    glPopMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.6f, 0.25f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, 0.47f);
    glVertex2f(-0.25f, -0.2f);
    glVertex2f(0.25f, -0.2f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

/**
 * drawCityCar: Renders animated vehicle moving on city street
 * Uses cityCarX variable for horizontal animation
 * 
 * Composition: Car body with wheels positioned and scaled
 * Transformation: Translation (cityCarX for movement), scaling for wheels
 */
void drawCityCar()
{
    glPushMatrix();
    glTranslatef(cityCarX, -0.6f, 0.0f);
    glColor3f(0.1f, 0.4f, 0.8f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.15f, 0.0f);
    glVertex2f(0.15f, 0.0f);
    glVertex2f(0.18f, 0.05f);
    glVertex2f(-0.18f, 0.05f);
    glEnd();
    glColor3f(0.05f, 0.2f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(-0.08f, 0.05f);
    glVertex2f(0.08f, 0.05f);
    glVertex2f(0.05f, 0.12f);
    glVertex2f(-0.1f, 0.12f);
    glEnd();
    glPushMatrix();
    glTranslatef(-0.1f, 0.0f, 0.0f);
    drawCircle(0.03f, 0.1f, 0.1f, 0.1f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.1f, 0.0f, 0.0f);
    drawCircle(0.03f, 0.1f, 0.1f, 0.1f);
    glPopMatrix();
    glEnable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 0.8f, 0.4f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.18f, 0.03f);
    glVertex2f(0.6f, -0.05f);
    glVertex2f(0.6f, 0.1f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

//  2070 SPACE CITY MODULES

/**
 * drawSpaceBackground: Renders starfield for space scenes
 * 
 * Renders 60 twinkling stars using point sprites
 * Stars use pseudo-random positioning and twinkling animation
 * with sine wave variation for glow effect
 */
void drawSpaceBackground()
{
    glEnable(GL_BLEND);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 60; i++)
    {
        float sx = -1.0f + fmod((i * 0.17f), 2.0f);
        float sy = 0.0f + fmod((i * 0.23f), 1.0f);
        float twinkle = 0.4f + sin(pulseTime * 4.0f + i) * 0.5f;
        glColor4f(1.0f, 1.0f, 1.0f, twinkle);
        glVertex2f(sx, sy);
    }
    glEnd();
    glDisable(GL_BLEND);
}
 
/**
 * draw2070Building: Renders futuristic 2070-style architecture
 * @param x, y: Position of building
 * @param height: Building height
 * @param r, g, b: Neon glow color
 * 
 * Composition: Dark base structure with sinusoidal width variation,
 * neon glow windows, top antenna, holographic panels
 * Color scheme: Dark blues/purples with neon RGB accents
 */
void draw2070Building(float x, float height, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(x, -0.2f, 0.0f);
    float width = 0.14f + fabs(sin(x * 6.0f)) * 0.05f;
    float depth = 0.06f;

    glColor3f(0.06f, 0.07f, 0.11f);
    glBegin(GL_QUADS);
    glVertex2f(-width, 0.0f);
    glVertex2f(width, 0.0f);
    glVertex2f(width, height);
    glVertex2f(-width, height);
    glEnd();

    glColor3f(0.03f, 0.04f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(width, 0.0f);
    glVertex2f(width + depth, depth * 0.6f);
    glVertex2f(width + depth, height + depth * 0.6f);
    glVertex2f(width, height);
    glEnd();

    glColor3f(0.09f, 0.1f, 0.14f);
    glBegin(GL_QUADS);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width + depth, height + depth * 0.6f);
    glVertex2f(-width + depth, height + depth * 0.6f);
    glEnd();

    glColor3f(r * 0.35f, g * 0.35f, b * 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-width * 0.08f, height);
    glVertex2f(width * 0.08f, height);
    glVertex2f(width * 0.05f, height + 0.14f);
    glVertex2f(-width * 0.05f, height + 0.14f);
    glEnd();

    glLineWidth(1.5f);
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-width, 0.0f);
    glVertex2f(-width, height);
    glVertex2f(width, height);
    glVertex2f(width, 0.0f);
    glEnd();

    for (float wy = 0.06f; wy < height - 0.06f; wy += 0.1f)
    {
        float pulse = 0.8f; // Static window brightness
        for (float wx = -width + 0.03f; wx < width - 0.03f; wx += 0.05f)
        {
            glColor4f(r * pulse, g * pulse, b * pulse, 0.85f);
            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + 0.025f, wy);
            glVertex2f(wx + 0.025f, wy + 0.045f);
            glVertex2f(wx, wy + 0.045f);
            glEnd();
        } 
    }

    glEnable(GL_BLEND);
    glColor4f(r, g, b, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-width, height * 0.55f);
    glVertex2f(width, height * 0.55f);
    glVertex2f(width, height * 0.62f);
    glVertex2f(-width, height * 0.62f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

/**
 * drawGlowingSpaceRoad: Renders futuristic illuminated road/highway
 * 
 * Composition: Glowing lanes with moving light patterns,
 * semi-transparent quad strips forming road markings
 * Animation: Lane lights pulse and scroll to simulate movement
 */
void drawGlowingSpaceRoad()
{
    glColor3f(0.03f, 0.04f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.72f);
    glVertex2f(-1.0f, -0.72f);
    glEnd();

    glColor3f(0.12f, 0.12f, 0.14f);
    glBegin(GL_QUADS);
    glVertex2f(-0.95f, -0.72f);
    glVertex2f(0.95f, -0.72f);
    glVertex2f(0.62f, -0.34f);
    glVertex2f(-0.62f, -0.34f);
    glEnd();

    glEnable(GL_BLEND);
    glColor4f(0.0f, 0.8f, 1.0f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(-0.95f, -0.72f);
    glVertex2f(-0.9f, -0.72f);
    glVertex2f(-0.58f, -0.34f);
    glVertex2f(-0.62f, -0.34f);
    glVertex2f(0.9f, -0.72f);
    glVertex2f(0.95f, -0.72f);
    glVertex2f(0.62f, -0.34f);
    glVertex2f(0.58f, -0.34f);
    glEnd();

    glLineWidth(3.0f);
    glColor4f(0.0f, 1.0f, 1.0f, 0.7f);
    glBegin(GL_LINES);
    glVertex2f(-0.95f, -0.72f);
    glVertex2f(-0.62f, -0.34f);
    glVertex2f(0.95f, -0.72f);
    glVertex2f(0.62f, -0.34f);
    glEnd();

    glColor4f(1.0f, 0.0f, 1.0f, 0.6f);
    for (float t = 0.0f; t < 1.0f; t += 0.12f)
    {
        float shift = fmod(pulseTime * 0.35f, 0.12f);
        float v = t - shift;
        if (v < 0.0f)
            v += 1.0f;

        float y0 = -0.72f + v * 0.38f;
        float left0 = -0.95f + v * 0.33f;
        float right0 = 0.95f - v * 0.33f;

        glBegin(GL_LINES);
        glVertex2f(left0 + 0.38f, y0);
        glVertex2f(right0 - 0.38f, y0);
        glEnd();
    }

    glColor4f(0.0f, 1.0f, 0.8f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-0.3f, -0.72f);
    glVertex2f(0.3f, -0.72f);
    glVertex2f(0.12f, -0.34f);
    glVertex2f(-0.12f, -0.34f);
    glEnd();
    glDisable(GL_BLEND);
}

/**
 * drawSpaceHoverCar: Renders futuristic hover vehicle
 * @param x, y: Position of vehicle
 * @param r, g, b: Neon glow color
 * @param facingRight: Direction flag for sprite orientation
 * 
 * Composition: Hover pods, cockpit, neon trim, holographic cabin,
 * glowing underbody. Includes hovering animation (Y-axis bob)
 * Transformation: Translation (position), Scaling with optional 
 * mirroring (facingRight controls -1.0 scaling on X)
 */
void drawSpaceHoverCar(float x, float y, float r, float g, float b, bool facingRight)
{
    glPushMatrix();
    glTranslatef(x, y + sin(pulseTime * 0.8f + x) * 0.01f, 0.0f);
    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.3f, 0.0f);
    glVertex2f(-0.1f, 0.02f);
    glVertex2f(-0.1f, 0.06f);
    glVertex2f(-0.4f, 0.03f);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.15f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.15f, 0.0f);
    glVertex2f(0.15f, 0.0f);
    glVertex2f(0.2f, 0.04f);
    glVertex2f(0.1f, 0.08f);
    glVertex2f(-0.1f, 0.08f);
    glEnd();
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.05f, 0.04f);
    glVertex2f(0.15f, 0.04f);
    glVertex2f(0.05f, 0.08f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

/**
 * drawForegroundMetro: Renders futuristic metro train in foreground
 * 
 * Composition: Long train carriages with segmented windows,
 * glowing railings, neon indicators, connected module system
 * Creates sense of depth with layered rendering
 * Uses metro2070X for horizontal animation
 */
void drawForegroundMetro()
{
    glColor3f(0.14f, 0.14f, 0.17f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.08f);
    glVertex2f(1.0f, -0.08f);
    glVertex2f(1.0f, -0.14f);
    glVertex2f(-1.0f, -0.14f);
    glEnd();

    glColor3f(0.09f, 0.09f, 0.12f);
    for (float px = -0.9f; px < 1.0f; px += 0.24f)
    {
        glBegin(GL_QUADS);
        glVertex2f(px, -0.14f);
        glVertex2f(px + 0.06f, -0.14f);
        glVertex2f(px + 0.05f, -0.7f);
        glVertex2f(px + 0.01f, -0.7f);
        glEnd();
    }

    glLineWidth(2.0f);
    glColor3f(0.0f, 0.9f, 0.9f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, -0.09f);
    glVertex2f(1.0f, -0.09f);
    glVertex2f(-1.0f, -0.13f);
    glVertex2f(1.0f, -0.13f);
    glEnd();

    glPushMatrix();
    glTranslatef(metro2070X, -0.11f, 0.0f);
    for (int car = 0; car < 3; car++)
    {
        float cx = car * 0.42f;

        glColor3f(0.75f, 0.8f, 0.86f);
        glBegin(GL_QUADS);
        glVertex2f(cx - 0.2f, -0.01f);
        glVertex2f(cx + 0.2f, -0.01f);
        glVertex2f(cx + 0.2f, 0.09f);
        glVertex2f(cx - 0.2f, 0.09f);
        glEnd();

        glColor3f(0.52f, 0.57f, 0.64f);
        glBegin(GL_QUADS);
        glVertex2f(cx - 0.2f, 0.09f);
        glVertex2f(cx + 0.2f, 0.09f);
        glVertex2f(cx + 0.17f, 0.12f);
        glVertex2f(cx - 0.17f, 0.12f);
        glEnd();

        glColor3f(0.03f, 0.75f, 0.82f);
        for (float wx = cx - 0.16f; wx < cx + 0.13f; wx += 0.07f)
        {
            glBegin(GL_QUADS);
            glVertex2f(wx, 0.03f);
            glVertex2f(wx + 0.05f, 0.03f);
            glVertex2f(wx + 0.05f, 0.08f);
            glVertex2f(wx, 0.08f);
            glEnd(); 
        }
    }

    glColor3f(0.92f, 0.96f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(1.04f, -0.01f);
    glVertex2f(1.2f, 0.03f);
    glVertex2f(1.2f, 0.08f);
    glVertex2f(1.04f, 0.12f);
    glVertex2f(0.96f, 0.09f);
    glVertex2f(0.96f, 0.02f);
    glEnd();

    glEnable(GL_BLEND);
    glColor4f(0.0f, 1.0f, 1.0f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.22f, -0.01f);
    glVertex2f(1.15f, -0.01f);
    glVertex2f(1.0f, 0.12f);
    glVertex2f(-0.08f, 0.12f);
    glEnd();
    glDisable(GL_BLEND);
    glPopMatrix();
}

// TRANSITION MODULES

// TRANSITION : "Holographic Sweep" (City -> Future)
/**
 * drawSciFiHolographicSweep: Renders futuristic scene transition effect
 * @param progress: Animation progress (0.0 to 1.0)
 * 
 * Creates dramatic sci-fi holographic sweep transition
 * Uses blue diagonal lines that scan across screen
 * Glow effect grows as progress increases  
 * Effect covers entire viewport with animated pattern
 */
void drawSciFiHolographicSweep(float progress) 
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    // Calculate pixel position of the scan line (moves from top to bottom)
    int scanLinePixelY = (int)(h * (1.0f - progress));

    glEnable(GL_SCISSOR_TEST);

    // 1. Draw City strictly BELOW the scan line
    glScissor(0, 0, w, scanLinePixelY);
    drawCity();

    // 2. Draw Future strictly ABOVE the scan line
    glScissor(0, scanLinePixelY, w, h - scanLinePixelY);
    drawFuture();

    glDisable(GL_SCISSOR_TEST);

    // 3. Draw strict geometric overlays ON TOP of the sliced scenes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Convert pixel Y back to OpenGL coordinates (-1 to 1) for the line
    float scanLineGL_Y = 1.0f - (progress * 2.0f);

    // The main bright scan line
    glLineWidth(3.0f);
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f); // Hard Cyan
    glBegin(GL_LINES);
    glVertex2f(-1.0f, scanLineGL_Y);
    glVertex2f(1.0f, scanLineGL_Y);
    glEnd();

    // Glow effect for the line (Clean geometric gradient, no soft blobs)
    glLineWidth(12.0f);
    glColor4f(0.0f, 1.0f, 1.0f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, scanLineGL_Y);
    glVertex2f(1.0f, scanLineGL_Y);
    glEnd();

    // Digital "Data Rain" / Scan beams dropping down from the line
    glLineWidth(1.0f);
    for (int i = 0; i < 20; i++)
    {
        float xPos = -1.0f + (i * 0.1f) + fmod(pulseTime * 0.5f, 0.1f);
        float beamLength = 0.1f + sin(pulseTime * 5.0f + i) * 0.05f;

        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);
        glBegin(GL_LINES);
        glVertex2f(xPos, scanLineGL_Y);
        glVertex2f(xPos, scanLineGL_Y - beamLength);
        glEnd();
    }

    // Moving horizontal grid lines to sell the "holographic" 3D space
    glColor4f(0.0f, 0.5f, 0.5f, 0.15f);
    glLineWidth(1.0f);
    for (float y = -1.0f; y < scanLineGL_Y; y += 0.08f)
    {
        glBegin(GL_LINES);
        glVertex2f(-1.0f, y);
        glVertex2f(1.0f, y);
        glEnd();
    }

    // Hard edge glitch blocks along the scan line (strict squares, not circles)
    for (int i = 0; i < 10; i++)
    {
        float xPos = -0.9f + (i * 0.2f) + sin(pulseTime * 10.0f + i * 5.0f) * 0.05f;
        float blockH = 0.02f + (rand() % 10) * 0.002f; // Slight randomness for digital corruption

        bool isMagenta = (i + (int)pulseTime) % 3 == 0;
        if (isMagenta)
            glColor4f(1.0f, 0.0f, 1.0f, 0.8f);
        else
            glColor4f(0.0f, 1.0f, 1.0f, 0.8f);

        glBegin(GL_QUADS);
        glVertex2f(xPos, scanLineGL_Y - blockH / 2);
        glVertex2f(xPos + 0.05f, scanLineGL_Y - blockH / 2);
        glVertex2f(xPos + 0.05f, scanLineGL_Y + blockH / 2);
        glVertex2f(xPos, scanLineGL_Y + blockH / 2);
        glEnd();
    }

    glDisable(GL_BLEND);
}

// Scenario

/**
 * drawVillage: Main rural village scene renderer
 * Renders: Sky, mountains, sun, animated clouds, hills, windmill,
 * ground, river, boat, farmhouse, forest, and animal flocks
 */
void drawVillage()
{
    // === 1. SKY GRADIENT ===
    drawSkyGradient();

    // === 2. FAR BACKGROUND MOUNTAINS ===
    drawMountains();

    // === 3. SUN RAYS + SUN DISC ===
    drawSunRays();
    glPushMatrix();
    glTranslatef(0.6f, 0.6f, 0.0f);
    drawCircle(0.13f, 1.0f, 0.9f, 0.0f);
    drawCircle(0.10f, 1.0f, 0.5f, 0.0f);
    glPopMatrix();

    // === 4. ANIMATED CLOUDS ===
    drawCloud(cloudX,         0.72f, 1.00f);
    drawCloud(cloudX2,        0.56f, 0.82f);
    drawCloud(cloudX3,        0.65f, 1.25f);
    drawCloud(cloudX - 0.65f, 0.80f, 0.65f);  // small trailing cloud

    // === 5. NEAR HILLS (foreground terrain) ===
    glBegin(GL_TRIANGLES);
    glColor3f(0.50f, 0.70f, 0.40f); glVertex2f(-0.6f,  0.5f);
    glColor3f(0.30f, 0.50f, 0.20f); glVertex2f(-1.0f, -0.2f);
    glColor3f(0.40f, 0.60f, 0.30f); glVertex2f(-0.6f, -0.2f);

    glColor3f(0.50f, 0.70f, 0.40f); glVertex2f(-0.6f,  0.5f);
    glColor3f(0.40f, 0.60f, 0.30f); glVertex2f(-0.6f, -0.2f);
    glColor3f(0.20f, 0.40f, 0.10f); glVertex2f(-0.2f, -0.2f);

    glColor3f(0.40f, 0.60f, 0.30f); glVertex2f( 0.2f,  0.7f);
    glColor3f(0.20f, 0.40f, 0.10f); glVertex2f(-0.3f, -0.2f);
    glColor3f(0.30f, 0.50f, 0.20f); glVertex2f( 0.2f, -0.2f);

    glColor3f(0.40f, 0.60f, 0.30f); glVertex2f( 0.2f,  0.7f);
    glColor3f(0.30f, 0.50f, 0.20f); glVertex2f( 0.2f, -0.2f);
    glColor3f(0.10f, 0.30f, 0.10f); glVertex2f( 0.7f, -0.2f);
    glEnd();

    // === 6. WINDMILL ===
    drawWindmill();

    // === 7. GREEN GROUND ===
    glColor3f(0.30f, 0.68f, 0.20f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,-1.0f); glVertex2f(1.0f,-1.0f);
    glVertex2f( 1.0f,-0.2f); glVertex2f(-1.0f,-0.2f);
    glEnd();
    // Lighter grass strip at ground line
    glColor3f(0.38f, 0.78f, 0.28f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,-0.20f); glVertex2f(1.0f,-0.20f);
    glVertex2f( 1.0f,-0.24f); glVertex2f(-1.0f,-0.24f);
    glEnd();

    // === 8. DIRT LANE ===
    drawDirtPath();

    // === 9. RIVER / WATER ===
    glColor3f(0.12f, 0.52f, 0.88f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,-0.9f); glVertex2f(1.0f,-0.9f);
    glVertex2f( 1.0f,-0.5f); glVertex2f(-1.0f,-0.5f);
    glEnd();
    // Water shimmer highlight strip
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.55f, 0.82f, 1.0f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f,-0.54f); glVertex2f(1.0f,-0.54f);
    glVertex2f( 1.0f,-0.50f); glVertex2f(-1.0f,-0.50f);
    glEnd();
    glDisable(GL_BLEND);

    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glLineWidth(2.0f);
    for (float ry = -0.8f; ry < -0.55f; ry += 0.1f)
    {
        glBegin(GL_LINES);
        for (float rx = -1.0f; rx < 1.0f; rx += 0.3f)
        {
            glVertex2f(rx + rippleOffset, ry);
            glVertex2f(rx + rippleOffset + 0.1f, ry);
        }
        glEnd();
    }
    drawBoat();
    glLineWidth(1.0f);

    // === 10. CHIMNEY SMOKE ===
    drawSmoke();

    // === 11. EXTENDED FENCES ===
    drawFence(-0.92f, -0.30f);
    drawFence(-0.62f, -0.30f);
    drawFence(-0.32f, -0.30f);
    drawFence( 0.08f, -0.30f);
    drawFence( 0.38f, -0.30f);

    // === 12. WILDFLOWERS ===
    drawFlowers();

    // === 13. STONE WELL ===
    drawWell();

    // === 14. FARMHOUSE ===
    glPushMatrix();
    glTranslatef(-0.6f, -0.4f, 0.0f);

    glColor3f(0.8f, 0.6f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(-0.2f, 0.0f); glVertex2f(0.1f, 0.0f);
    glVertex2f( 0.1f, 0.3f); glVertex2f(-0.2f, 0.3f);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0.1f, 0.0f); glVertex2f(0.35f, 0.10f);
    glVertex2f(0.35f, 0.4f); glVertex2f(0.1f, 0.3f);
    glEnd();

    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.25f, 0.3f); glVertex2f(0.15f, 0.3f); glVertex2f(-0.05f, 0.5f);
    glEnd();

    glColor3f(0.6f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0.15f, 0.3f); glVertex2f(-0.05f, 0.5f);
    glVertex2f(0.2f,  0.6f); glVertex2f( 0.4f,  0.4f);
    glEnd();

    glColor3f(0.3f, 0.1f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, 0.0f); glVertex2f(0.0f, 0.0f);
    glVertex2f( 0.0f, 0.15f); glVertex2f(-0.1f, 0.15f);
    glEnd();

    glColor3f(0.1f, 0.6f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.15f, 0.15f); glVertex2f(0.25f, 0.19f);
    glVertex2f(0.25f, 0.29f); glVertex2f(0.15f, 0.25f);
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (float vy = 0.0f; vy < 0.28f; vy += 0.015f)
    {
        float vx = -0.18f + sin(vy * 40.0f) * 0.015f;
        glColor3f(0.1f, 0.4f, 0.1f); glVertex2f(vx, vy);
        glColor3f(0.3f, 0.6f, 0.1f); glVertex2f(vx + 0.01f, vy - 0.01f);
    }
    for (float vy = 0.0f; vy < 0.25f; vy += 0.02f)
    {
        float vx = 0.32f + cos(vy * 50.0f) * 0.01f;
        glColor3f(0.1f, 0.3f, 0.05f); glVertex2f(vx, vy);
    }
    glEnd();
    glPopMatrix();

    // === 15. TREES & FOREST ===
    float sway = sin(windTime) * 1.5f;

    // Two feature trees (original positions)
    glPushMatrix(); glTranslatef(0.40f, -0.30f, 0.0f);
    drawTree(sway); glPopMatrix();

    glPushMatrix(); glTranslatef(0.72f, -0.40f, 0.0f); glScalef(0.90f, 0.90f, 1.0f);
    drawTree(sway * 0.80f); glPopMatrix();

    // Right-side forest — foreground trees
    glPushMatrix(); glTranslatef(0.88f, -0.36f, 0.0f); glScalef(0.70f, 0.70f, 1.0f);
    drawTree(sway * 1.15f); glPopMatrix();

    glPushMatrix(); glTranslatef(0.97f, -0.32f, 0.0f); glScalef(0.80f, 0.80f, 1.0f);
    drawTree(sway * 0.92f); glPopMatrix();

    // Right-side forest — depth (smaller, sit on hill line)
    glPushMatrix(); glTranslatef(0.82f, -0.20f, 0.0f); glScalef(0.55f, 0.55f, 1.0f);
    drawTree(sway * 1.05f); glPopMatrix();

    glPushMatrix(); glTranslatef(0.93f, -0.18f, 0.0f); glScalef(0.50f, 0.50f, 1.0f);
    drawTree(sway * 0.80f); glPopMatrix();

    // Left-side trees near windmill
    glPushMatrix(); glTranslatef(-0.52f, -0.36f, 0.0f); glScalef(0.78f, 0.78f, 1.0f);
    drawTree(sway * 1.10f); glPopMatrix();

    glPushMatrix(); glTranslatef(-0.42f, -0.40f, 0.0f); glScalef(0.68f, 0.68f, 1.0f);
    drawTree(sway * 0.88f); glPopMatrix();

    // === 16. WIND STREAKS ===
    drawWindStreaks();

    // === 17. BIRD FLOCKS ===
    drawBirdFlock(birdX,  0.60f, 1.00f);  // main flock, high up
    drawBirdFlock(birdX2, 0.44f, 0.72f);  // second flock, lower & smaller
}

/**
 * drawCity: Modern urban city scene renderer
 * Renders: Sun (Midpoint Circle), animated skyline (DDA algorithm),
 * street, 3D buildings, traffic lights, street lamps, and city vehicles
 */
void drawCity()
{
    // Draw sun using Midpoint Circle Algorithm (Bresenham)
    drawCircleBresenham(0.7f, 0.75f, 0.12f, 1.0f, 0.9f, 0.2f);
    
    drawSkyline();

    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.45f);
    glVertex2f(-1.0f, -0.45f);
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.45f);
    glVertex2f(1.0f, -0.45f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    glColor3f(1.0f, 0.8f, 0.0f);
    for (float i = -0.9f; i < 1.0f; i += 0.4f)
    {
        glBegin(GL_QUADS);
        glVertex2f(i, -0.75f);
        glVertex2f(i + 0.2f, -0.75f);
        glVertex2f(i + 0.2f, -0.72f);
        glVertex2f(i, -0.72f);
        glEnd();
    }

    draw3DBuilding(-0.9f, 0.6f, 0.3f, 0.3f, 0.4f);
    draw3DBuilding(-0.5f, 0.9f, 0.4f, 0.3f, 0.3f);
    draw3DBuilding(-0.1f, 0.5f, 0.3f, 0.4f, 0.3f);
    draw3DBuilding(0.3f, 0.8f, 0.4f, 0.4f, 0.4f);
    draw3DBuilding(0.7f, 0.7f, 0.3f, 0.3f, 0.5f);

    drawStreetLamp(-0.7f);
    drawStreetLamp(0.7f);
    drawTrafficLight(0.3f, -0.4f);
    drawCityCar();
}

/**
 * drawFuture: Futuristic sci-fi space city scene (year 2070)
 * Renders: Dark space background with stars, neon sun (dual Bresenham circles),
 * space buildings (2070-style), glowing road, hover cars, and metro train
 * 
 * Visual style: Cyberpunk/neon aesthetic with dark colors and glow effects
 * Uses algorithms: Bresenham circles for neon sun effect
 */
void drawFuture()
{
    glColor3f(0.01f, 0.01f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Draw glowing future sun using Bresenham circle (neon effect)
    drawCircleBresenham(-0.8f, 0.8f, 0.1f, 0.0f, 0.8f, 1.0f);
    drawCircleBresenham(-0.8f, 0.8f, 0.08f, 0.2f, 0.9f, 1.0f);
    
    drawSpaceBackground();

    glColor3f(0.03f, 0.04f, 0.08f);
    for (float bx = -1.0f; bx < 1.0f; bx += 0.16f)
    {
        float h = 0.12f + fabs(sin(bx * 12.0f)) * 0.2f;
        glBegin(GL_QUADS);
        glVertex2f(bx, -0.2f);
        glVertex2f(bx + 0.13f, -0.2f);
        glVertex2f(bx + 0.13f, -0.2f + h);
        glVertex2f(bx, -0.2f + h);
        glEnd();
    }

    draw2070Building(-0.78f, 0.58f, 0.2f, 0.9f, 1.0f);
    draw2070Building(-0.48f, 0.82f, 0.0f, 1.0f, 0.9f);
    draw2070Building(-0.16f, 0.66f, 0.7f, 0.8f, 1.0f);
    draw2070Building(0.16f, 0.75f, 1.0f, 0.45f, 0.9f);
    draw2070Building(0.44f, 0.6f, 0.3f, 1.0f, 0.8f);
    draw2070Building(0.76f, 0.87f, 1.0f, 0.65f, 0.3f);

    drawGlowingSpaceRoad();
    drawSpaceHoverCar(spaceCar1X, -0.43f, 1.0f, 0.2f, 0.6f, false);
    drawSpaceHoverCar(spaceCar2X, -0.49f, 0.0f, 0.9f, 0.4f, true);
    drawForegroundMetro();
}

// ==========================================
// CORE RENDERING ENGINE
// ==========================================

/**
 * display: Main OpenGL render callback function
 * 
 * Called every frame to render the active scene
 * Workflow:
 * 1. Clear screen with current background color
 * 2. Load identity matrix for fresh transformations
 * 3. Render appropriate scene based on currentState (VILLAGE, CITY, FUTURE)
 * 4. Handle scene transitions with fade and holographic effects
 * 5. Swap buffers for double-buffering (smooth animation)
 */
void display()
{
    static int frameCount = 0;
    if (frameCount++ % 30 == 0)  // Print every 30 frames
        std::cout << "Display: isPaused=" << isPaused << std::endl;
    
    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (currentState == VILLAGE ||
        (currentState == TRANSITION_TO_CITY && transitionProgress < 0.5f))
    {
        drawVillage();
    }
    else if (currentState == CITY ||
             (currentState == TRANSITION_TO_CITY && transitionProgress >= 0.5f))
    {
        drawCity();
    }
    else if (currentState == FUTURE)
    {
        drawFuture();
    }

    // Apply new structural transitions completely isolated from normal scene drawing
    if (currentState == TRANSITION_TO_FUTURE)
    {
        drawSciFiHolographicSweep(transitionProgress);
    }

    // Keep simple fade ONLY for the basic Village <-> City transitions
    if (fadeAlpha > 0.0f && currentState == TRANSITION_TO_CITY)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(bgR, bgG, bgB, fadeAlpha);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
        glDisable(GL_BLEND);
    } 

    // Draw pause overlay if paused (BEFORE buffer swap so it displays)
    if (isPaused)
    {
        displayPauseOverlay();
    }
    
    glutSwapBuffers();
}

// TIME & STATE MANAGER

/**
 * update: Timer callback for updating animation state
 * @param value: Timer ID (not used)
 * 
 * Called at regular intervals (16ms ≈ 60 FPS) to update:
 * - All animation variables (positions, angles, progress counters)
 * - Scene transitions and state changes
 * - Timers and effect durations
 * - Vehicle and object movements
 * 
 * Then redraws scene and reschedules next update call
 */
void update(int value)
{
    // Skip all animation updates when paused
    if (!isPaused)
    {
        if (currentState == FUTURE || currentState == TRANSITION_TO_FUTURE)
        {
            metro2070X += 0.04f; // Slower metro movement
            if (metro2070X > 3.0f)
                metro2070X = -3.0f;
            spaceCar1X -= 0.03f;
            if (spaceCar1X < -1.5f)
                spaceCar1X = 1.5f;
            spaceCar2X += 0.04f;
            if (spaceCar2X > 1.5f)
                spaceCar2X = -1.5f;
        }

        trafficTimer += 0.05f;
        if (trafficTimer < 3.0f)
            trafficState = 0;
        else if (trafficTimer < 4.5f)
            trafficState = 1;
        else if (trafficTimer < 8.0f)
            trafficState = 2;
        else
            trafficTimer = 0.0f;

        if (trafficState == 0)
            cityCarX += 0.015f;
        else if (trafficState == 1)
        {
            if (cityCarX > -0.1f && cityCarX < 0.2f)
                cityCarX += 0.005f;
            else
                cityCarX += 0.015f;
        }
        else if (trafficState == 2)
        {
            if (cityCarX > -0.1f && cityCarX < 0.2f)
            {
            }
            else
                cityCarX += 0.015f;
        }
        if (cityCarX > 1.5f)
            cityCarX = -1.5f;

        skylineOffset -= 0.005f;
        if (skylineOffset < -1.0f)
            skylineOffset = 0.0f;
        cloudX += 0.005f;
        if (cloudX > 1.2f)
            cloudX = -1.2f;

        rippleOffset += 0.01f;
        if (rippleOffset > 0.3f)
            rippleOffset = 0.0f;

        carX += 0.02f;
        if (carX > 1.2f)
            carX = -1.2f;
        pulseTime += 0.2f;

        windTime += 0.1f;
        windStreakX += 0.05f;
        if (windStreakX > 1.5f)
            windStreakX = -1.5f;

        birdX += 0.01f;
        if (birdX > 1.2f)
            birdX = -1.2f;
        birdX2 += 0.008f;
        if (birdX2 > 1.3f)
            birdX2 = -1.3f;

        cloudX2 += 0.003f;
        if (cloudX2 > 1.5f)
            cloudX2 = -1.5f;
        cloudX3 += 0.006f;
        if (cloudX3 > 1.5f)
            cloudX3 = -1.5f;

        windmillAngle -= 1.5f;
        if (windmillAngle <= -360.0f)
            windmillAngle += 360.0f;

        boatX -= 0.005f;
        if (boatX < -1.5f)
            boatX = 1.5f;

        smokeProgress += 0.02f;
        if (smokeProgress >= 1.0f)
            smokeProgress = 0.0f;

        switch (currentState)
        {
        case VILLAGE:
            bgR = 0.5f;
            bgG = 0.8f;
            bgB = 1.0f;
            if (timer >= 5.0f)
            {
                currentState = TRANSITION_TO_CITY;
                timer = 0.0f;
            }
            break;

        case TRANSITION_TO_CITY:
            transitionProgress += 0.02f;
            bgR = lerp(0.5f, 0.2f, transitionProgress);
            bgG = lerp(0.8f, 0.2f, transitionProgress);
            bgB = lerp(1.0f, 0.4f, transitionProgress);
            
            if (transitionProgress < 0.5f)
                fadeAlpha = transitionProgress * 2.0f;
            else
                fadeAlpha = 1.0f - ((transitionProgress - 0.5f) * 2.0f);
            if (transitionProgress >= 1.0f)
            {
                currentState = CITY;
                transitionProgress = 0.0f;
                fadeAlpha = 0.0f;
                timer = 0.0f;
            }
            break;

        case CITY:
            bgR = 0.2f;
            bgG = 0.2f;
            bgB = 0.4f; // Set strict city background color
            if (timer >= 5.0f)
            {
                currentState = TRANSITION_TO_FUTURE;
                timer = 0.0f;
            }
            break;

        case TRANSITION_TO_FUTURE:
            transitionProgress += 0.015f; // Slow, deliberate structural sweep
            if (transitionProgress >= 1.0f)
            {
                currentState = FUTURE;
                transitionProgress = 0.0f;
                timer = 0.0f;
                bgR = 0.0f;
                bgG = 0.0f;
                bgB = 0.05f; // Snap to future color
            }
            break;

        case FUTURE:
            bgR = 0.0f;
            bgG = 0.0f;
            bgB = 0.05f;
            if (timer >= 8.0f)
            {
                currentState = VILLAGE;
                timer = 0.0f;
                transitionProgress = 0.0f;
                bgR = 0.5f;
                bgG = 0.8f;
                bgB = 1.0f;
            }
            break;
        }
        timer += 0.1f;
    }
    
    // Always redisplay (even when paused, to show pause overlay)
    glutPostRedisplay();
    glutTimerFunc(80, update, 0);
}



void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Computer Graphics Project: The Evolution - Cinematic Transitions");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardCallback);  // Register keyboard callback for pause
    glutTimerFunc(30, update, 0);
    glutMainLoop();
    return 0;
}
