#include <GL/glut.h>
#include <iostream>
#include <math.h>

const float PI = 3.14159265f;


// Circle ba surjo draw korar jonno expert function
void drawCircle(float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float theta = i * PI / 180.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

void drawCar() {
    // Car Body (Chassis)
    glColor3f(0.8f, 0.1f, 0.1f); // Red Car
    glBegin(GL_QUADS);
        glVertex2f(-0.1f, 0.0f); glVertex2f(0.1f, 0.0f);
        glVertex2f(0.1f, 0.05f); glVertex2f(-0.1f, 0.05f);
    glEnd();

    // Car Top (Cabin)
    glColor3f(0.6f, 0.0f, 0.0f); // Darker Red
    glBegin(GL_QUADS);
        glVertex2f(-0.05f, 0.05f); glVertex2f(0.05f, 0.05f);
        glVertex2f(0.03f, 0.08f);  glVertex2f(-0.03f, 0.08f);
    glEnd();

    // Wheels
    glPushMatrix(); glTranslatef(-0.06f, 0.0f, 0.0f); drawCircle(0.02f, 0.1f, 0.1f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.06f, 0.0f, 0.0f); drawCircle(0.02f, 0.1f, 0.1f, 0.1f); glPopMatrix();

    // Headlight Beam (Semi-transparent yellow)
    // NB: Enable GL_BLEND in init() for transparency to work properly
    glColor4f(1.0f, 1.0f, 0.0f, 0.4f); // Yellow with 40% opacity
    glBegin(GL_TRIANGLES);
        glVertex2f(0.1f, 0.025f);  // Headlight source
        glVertex2f(0.5f, -0.05f);  // Beam bottom right
        glVertex2f(0.5f, 0.1f);    // Beam top right
    glEnd();
}

// 3ti circle overlap kore ekta megh toiri hobe
void drawCloud() {
    glColor3f(1.0f, 1.0f, 1.0f); // Pure White
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f); drawCircle(0.1f, 1.0f, 1.0f, 1.0f); // Center
        glTranslatef(-0.08f, -0.02f, 0.0f); drawCircle(0.08f, 1.0f, 1.0f, 1.0f); // Left
        glTranslatef(0.16f, 0.0f, 0.0f); drawCircle(0.07f, 1.0f, 1.0f, 1.0f); // Right
    glPopMatrix();
}

// Chotto ekta gach draw korar reusable block
void drawTree(float swayAngle) {
    glPushMatrix();
        // Ekhane amra gach-ke tar base (0,0) theke rotate korchi
        glRotatef(swayAngle, 0.0f, 0.0f, 1.0f);

        // Tree Trunk (Kando)
        glColor3f(0.4f, 0.2f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-0.05f, 0.0f); glVertex2f(0.05f, 0.0f);
            glVertex2f(0.05f, 0.3f);  glVertex2f(-0.05f, 0.3f);
        glEnd();

        // Tree Leaves (Pata)
        glPushMatrix();
            glTranslatef(0.0f, 0.35f, 0.0f);
            drawCircle(0.15f, 0.1f, 0.5f, 0.1f); // Main leaf bunch

            // Adding more details to leaves (Realism)
            glTranslatef(-0.08f, -0.05f, 0.0f);
            drawCircle(0.1f, 0.1f, 0.6f, 0.15f); // Left bump
            glTranslatef(0.16f, 0.0f, 0.0f);
            drawCircle(0.1f, 0.1f, 0.4f, 0.05f); // Right bump
        glPopMatrix();
    glPopMatrix();
}

// ==========================================
// GLOBAL VARIABLES & SYSTEM STATES
// ==========================================
enum SceneState {
    VILLAGE,
    TRANSITION_TO_CITY,
    CITY,
    TRANSITION_TO_FUTURE,
    FUTURE,
    TRANSITION_TO_VILLAGE // Loop back
};

SceneState currentState = VILLAGE;

// Extra Micro-Details Variables
// City Details Variables
// Traffic & Street Lights Variables
// 🤖 Future Transformer Variables
// 💎 AI Nexus Variables
float nexusAngle = 0.0f;
float nexusHover = 0.0f;
float holoAngle = 0.0f; // হোলোগ্রাম ঘোরানোর জন্য
float transX = -1.2f;           // ট্রান্সফর্মারের স্ক্রিন পজিশন
float transProgress = 0.0f;     // 0.0 মানে গাড়ি, 1.0 মানে পুরোপুরি রোবট
int transState = 0;             // 0=Driving, 1=Morphing, 2=Robot Action, 3=Morph Back, 4=Drive Away
float laserTimer = 0.0f;        // লেজার মারার টাইমার
float trafficTimer = 0.0f;
int trafficState = 0; // 0 = Green, 1 = Yellow, 2 = Red
float skylineOffset = 0.0f; // পেছনের শহরের ছায়া মুভ করার জন্য
float cityCarX = -1.2f;     // শহরের গাড়ির পজিশন
float fadeAlpha = 0.0f; // স্মুথ ট্রানজিশনের পর্দার জন্য
float windmillAngle = 0.0f;  // হাওয়াকলের চাকা ঘোরানোর জন্য
float boatX = 1.2f;          // নৌকা ডানদিক থেকে বামদিকে আসার জন্য
float smokeProgress = 0.0f;  // ধোঁয়া উপরে উঠে মিলিয়ে যাওয়ার জন্য
float windTime = 0.0f;
float windStreakX = -1.5f;
float birdX = -1.0f;
float rippleOffset = 0.0f; // River animation-er jonno
float timer = 0.0f;
float transitionProgress = 0.0f;
float cloudX = -1.2f;
float carX = -1.2f;
float pulseTime = 0.0f;
float hoverY = 0.0f;

// Background Colors (R, G, B)
float bgR = 0.5f, bgG = 0.8f, bgB = 1.0f; // Default Sky Blue

// ==========================================
// UTILITY FUNCTIONS
// ==========================================
// Linear Interpolation for smooth transitions
float lerp(float start, float end, float t) {
    if (t > 1.0f) t = 1.0f;
    return start + t * (end - start);
}

// ==========================================
// DRAWING MODULES
// ==========================================
void drawFence(float x, float y) {
    glColor3f(0.5f, 0.3f, 0.1f); // Wood color
    glPushMatrix();
        glTranslatef(x, y, 0.0f);
        for(float i = 0; i < 0.3f; i += 0.08f) {
            glBegin(GL_QUADS); // Vertical posts
                glVertex2f(i, 0.0f); glVertex2f(i+0.02f, 0.0f);
                glVertex2f(i+0.02f, 0.15f); glVertex2f(i, 0.15f);
            glEnd();
        }
        glBegin(GL_QUADS); // Horizontal rail
            glVertex2f(-0.02f, 0.08f); glVertex2f(0.3f, 0.08f);
            glVertex2f(0.3f, 0.1f); glVertex2f(-0.02f, 0.1f);
        glEnd();
    glPopMatrix();
}

// --- পালতোলা নৌকা (Sailboat) ---
void drawBoat() {
    glPushMatrix();
        glTranslatef(boatX, -0.65f, 0.0f); // নদীর ঠিক ওপরে

        // নৌকার বডি (কাঠের রঙ)
        glColor3f(0.5f, 0.25f, 0.1f);
        glBegin(GL_POLYGON);
            glVertex2f(-0.15f, 0.0f);  glVertex2f(0.15f, 0.0f);
            glVertex2f(0.1f, -0.08f);  glVertex2f(-0.1f, -0.08f);
        glEnd();

        // নৌকার মাস্তুল (Mast - Pole)
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_LINES);
            glVertex2f(0.0f, 0.0f); glVertex2f(0.0f, 0.25f);
        glEnd();

        // নৌকার পাল (Sail - সাদা কাপড়)
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.0f, 0.05f);  // Bottom
            glVertex2f(0.0f, 0.25f);  // Top
            // হাওয়া যেদিকে বইছে, পাল সেদিকে একটু ফুলে থাকবে
            glVertex2f(0.15f, 0.1f);
        glEnd();
    glPopMatrix();
}

// --- হাওয়াকল (Windmill) ---
void drawWindmill() {
    glPushMatrix();
        glTranslatef(-0.8f, 0.1f, 0.0f); // পাহাড়ের কাছে ব্যাকগ্রাউন্ডে
        glScalef(0.8f, 0.8f, 1.0f); // একটু ছোট করে দিলাম

        // Tower (টাওয়ার)
        glColor3f(0.6f, 0.5f, 0.4f);
        glBegin(GL_POLYGON);
            glVertex2f(-0.08f, -0.3f); glVertex2f(0.08f, -0.3f);
            glVertex2f(0.04f, 0.2f);   glVertex2f(-0.04f, 0.2f);
        glEnd();

        // Rotating Blades (ঘূর্ণায়মান পাখা)
        glPushMatrix();
            glTranslatef(0.0f, 0.2f, 0.0f); // পাখার সেন্টার
            glRotatef(windmillAngle, 0.0f, 0.0f, 1.0f); // চাকা ঘুরবে!

            glColor3f(0.8f, 0.8f, 0.8f); // পাখার রঙ
            for(int i = 0; i < 4; i++) {
                glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // ৯০ ডিগ্রি পর পর ৪টি পাখা
                glBegin(GL_TRIANGLES);
                    glVertex2f(-0.02f, 0.0f); glVertex2f(0.02f, 0.0f);
                    glVertex2f(0.0f, 0.3f);
                glEnd();
            }
        glPopMatrix();
    glPopMatrix();
}

// --- চিমনির ধোঁয়া (Animated Smoke) ---
void drawSmoke() {
    // ধোঁয়া ধীরে ধীরে উপরে উঠবে এবং বড় হবে
    float yOffset = smokeProgress * 0.3f;
    float size = 0.03f + (smokeProgress * 0.05f);

    // ধোঁয়া যত উপরে উঠবে, তত মিলিয়ে যাবে (Alpha transparency 1.0 থেকে 0.0 হবে)
    float alpha = 1.0f - smokeProgress;
    if (alpha < 0.0f) alpha = 0.0f;

    glPushMatrix();
        // কুঁড়েঘরের চিমনির ঠিক ওপরে পজিশন
        glTranslatef(-0.35f, 0.1f + yOffset, 0.0f);
        // হাওয়া ধোঁয়াকে ডানদিকে ঠেলে দিচ্ছে
        glTranslatef(smokeProgress * 0.15f, 0.0f, 0.0f);

        // GL_BLEND অন করা থাকতে হবে
        glColor4f(0.6f, 0.6f, 0.6f, alpha);
        drawCircle(size, 0.6f, 0.6f, 0.6f); // drawCircle ফাংশনটি আগে থেকেই তোমার কোডে আছে
    glPopMatrix();
}

void drawWindStreaks() {
    // Enable blending is required for this transparency
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f); // 40% opaque white
    glLineWidth(2.0f);

    glPushMatrix();
        glTranslatef(windStreakX, 0.0f, 0.0f);

        glBegin(GL_LINES);
            // Streak 1 (Top)
            glVertex2f(0.0f, 0.3f); glVertex2f(0.4f, 0.3f);
            // Streak 2 (Middle, slightly offset)
            glVertex2f(-0.2f, 0.15f); glVertex2f(0.3f, 0.15f);
            // Streak 3 (Bottom)
            glVertex2f(0.1f, -0.1f); glVertex2f(0.6f, -0.1f);
        glEnd();
    glPopMatrix();
}

void drawBirds() {
    glColor3f(0.1f, 0.1f, 0.1f); // Dark silhouette
    glLineWidth(2.0f);

    glPushMatrix();
        glTranslatef(birdX, 0.6f, 0.0f);

        // Bird 1 (Classic 'V' shape)
        glBegin(GL_LINE_STRIP);
            glVertex2f(-0.04f, 0.04f); glVertex2f(0.0f, 0.0f); glVertex2f(0.04f, 0.04f);
        glEnd();

        // Bird 2 (Smaller, trailing behind)
        glPushMatrix();
            glTranslatef(0.08f, 0.03f, 0.0f);
            glScalef(0.7f, 0.7f, 1.0f);
            glBegin(GL_LINE_STRIP);
                glVertex2f(-0.04f, 0.04f); glVertex2f(0.0f, 0.0f); glVertex2f(0.04f, 0.04f);
            glEnd();
        glPopMatrix();

    glPopMatrix();
}

// --- শহরের পেছনের ছায়া (Parallax Skyline) ---
void drawSkyline() {
    glColor3f(0.1f, 0.1f, 0.15f); // খুব গাঢ় নীল/কালো (দূরের ছায়া)
    glPushMatrix();
        glTranslatef(skylineOffset, 0.0f, 0.0f); // প্যারালাক্স মুভমেন্ট

        // লুপ চালিয়ে অনেকগুলো পেছনের বিল্ডিং ড্র করা
        for(float x = -2.0f; x < 2.0f; x += 0.25f) {
            // sin() ব্যবহার করে রেন্ডম হাইট তৈরি করা
            float h = 0.4f + (sin(x * 15.0f) * 0.3f);
            glBegin(GL_QUADS);
                glVertex2f(x, -0.3f);        glVertex2f(x + 0.2f, -0.3f);
                glVertex2f(x + 0.2f, h);     glVertex2f(x, h);
            glEnd();
        }
    glPopMatrix();
}

// --- সলিড থ্রিডি বিল্ডিং (Reusable 3D Building Generator) ---
// x = পজিশন, height = উচ্চতা, r,g,b = বিল্ডিংয়ের রঙ
void draw3DBuilding(float x, float height, float r, float g, float b) {
    glPushMatrix();
        glTranslatef(x, -0.4f, 0.0f); // মাটি থেকে শুরু

        // 1. সামনের দেয়াল (Front Wall)
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);    glVertex2f(0.2f, 0.0f);
            glVertex2f(0.2f, height);  glVertex2f(0.0f, height);
        glEnd();

        // 2. পাশের দেয়াল (Side Wall - ছায়ার জন্য একটু ডার্ক রঙ)
        glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
        glBegin(GL_QUADS);
            glVertex2f(0.2f, 0.0f);           glVertex2f(0.3f, 0.08f);
            glVertex2f(0.3f, height + 0.08f); glVertex2f(0.2f, height);
        glEnd();

        // 3. ছাদ (Roof - সবচেয়ে উজ্জ্বল রঙ)
        glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
        glBegin(GL_QUADS);
            glVertex2f(0.0f, height);         glVertex2f(0.2f, height);
            glVertex2f(0.3f, height + 0.08f); glVertex2f(0.1f, height + 0.08f);
        glEnd();

        // 4. ডাইনামিক জানালা (Glowing Windows)
        for(float wy = 0.1f; wy < height - 0.1f; wy += 0.15f) {
            for(float wx = 0.04f; wx < 0.16f; wx += 0.08f) {
                // ম্যাথ দিয়ে ডিসাইড হবে কোন আলো জ্বলবে আর কোনটা নিভবে
                if ((int)(wx * 100 + wy * 100 + x * 10) % 3 != 0) {
                    glColor3f(0.9f, 0.9f, 0.5f); // Light ON (হলুদ)
                } else {
                    glColor3f(0.15f, 0.15f, 0.2f); // Light OFF (অন্ধকার)
                }
                glBegin(GL_QUADS);
                    glVertex2f(wx, wy);               glVertex2f(wx + 0.04f, wy);
                    glVertex2f(wx + 0.04f, wy + 0.06f); glVertex2f(wx, wy + 0.06f);
                glEnd();
            }
        }
    glPopMatrix();
}

// --- 🚦 স্মার্ট ট্রাফিক সিগন্যাল (Traffic Light) ---
void drawTrafficLight(float x, float y) {
    glPushMatrix();
        glTranslatef(x, y, 0.0f);

        // সিগন্যালের খুঁটি (Pole)
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(-0.015f, 0.0f); glVertex2f(0.015f, 0.0f);
            glVertex2f(0.015f, 0.4f);  glVertex2f(-0.015f, 0.4f);
        glEnd();

        // সিগন্যাল বক্স (Black Box)
        glColor3f(0.1f, 0.1f, 0.15f);
        glBegin(GL_QUADS);
            glVertex2f(-0.05f, 0.4f);  glVertex2f(0.05f, 0.4f);
            glVertex2f(0.05f, 0.7f);   glVertex2f(-0.05f, 0.7f);
        glEnd();

        // লাল বাতি (Red)
        glPushMatrix(); glTranslatef(0.0f, 0.62f, 0.0f);
        if (trafficState == 2) drawCircle(0.035f, 1.0f, 0.1f, 0.1f); // On
        else drawCircle(0.035f, 0.3f, 0.0f, 0.0f); // Off
        glPopMatrix();

        // হলুদ বাতি (Yellow)
        glPushMatrix(); glTranslatef(0.0f, 0.52f, 0.0f);
        if (trafficState == 1) drawCircle(0.035f, 1.0f, 1.0f, 0.0f); // On
        else drawCircle(0.035f, 0.3f, 0.3f, 0.0f); // Off
        glPopMatrix();

        // সবুজ বাতি (Green)
        glPushMatrix(); glTranslatef(0.0f, 0.42f, 0.0f);
        if (trafficState == 0) drawCircle(0.035f, 0.0f, 1.0f, 0.0f); // On
        else drawCircle(0.035f, 0.0f, 0.3f, 0.0f); // Off
        glPopMatrix();
    glPopMatrix();
}

// --- 💡 স্ট্রিট ল্যাম্প (আপগ্রেডেড - আলোর বিচ্ছুরণ সহ) ---
void drawStreetLamp(float x) {
    glPushMatrix();
        glTranslatef(x, -0.4f, 0.0f); // ফুটপাতের ওপর

        // খুঁটি (Pole)
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(-0.015f, 0.0f); glVertex2f(0.015f, 0.0f);
            glVertex2f(0.015f, 0.5f);  glVertex2f(-0.015f, 0.5f);
        glEnd();

        // বাতির শেড (Lamp Head)
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.06f, 0.48f); glVertex2f(0.06f, 0.48f);
            glVertex2f(0.0f, 0.53f);
        glEnd();

        // আলোর বাল্ব (Glowing Bulb)
        glPushMatrix(); glTranslatef(0.0f, 0.47f, 0.0f); drawCircle(0.02f, 1.0f, 1.0f, 0.8f); glPopMatrix();

        // 🌟 ম্যাজিক: আলোর বিচ্ছুরণ (Light Cone)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 0.6f, 0.25f); // 25% স্বচ্ছ হলুদ আলো
        glBegin(GL_TRIANGLES);
            glVertex2f(0.0f, 0.47f);   // টপ পয়েন্ট (বাল্বের কাছে)
            glVertex2f(-0.25f, -0.2f); // রাস্তার বামে
            glVertex2f(0.25f, -0.2f);  // রাস্তার ডানে
        glEnd();
        glDisable(GL_BLEND);
    glPopMatrix();
}

// --- 🌌 দ্য কোয়ান্টাম নেক্সাস (Pro-Level Future Core) ---
void drawMegaNexus() {
    glPushMatrix();
        glTranslatef(0.0f, 0.4f + (sin(pulseTime) * 0.05f), 0.0f); // স্মুথ ভাসমান মোশন

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 1. Vertical Energy Beam (আকাশে ওঠা শক্তির রশ্মি)
        glBegin(GL_QUADS);
            glColor4f(0.0f, 1.0f, 1.0f, 0.0f); glVertex2f(-0.08f, 1.0f); // Top Fade
            glColor4f(0.0f, 1.0f, 1.0f, 0.0f); glVertex2f(0.08f, 1.0f);
            glColor4f(0.0f, 1.0f, 1.0f, 0.6f); glVertex2f(0.05f, 0.0f);  // Bottom Bright
            glColor4f(0.0f, 1.0f, 1.0f, 0.6f); glVertex2f(-0.05f, 0.0f);
        glEnd();

        // 2. Glowing Core (মাঝখানের জ্বলন্ত সূর্য)
        glColor4f(0.0f, 1.0f, 1.0f, 0.8f); drawCircle(0.12f, 0.0f, 1.0f, 1.0f); // Solid Cyan
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); drawCircle(0.06f, 1.0f, 1.0f, 1.0f); // Pure White Center

        // 3. Fast Inner Hologram Ring (ভেতরের দ্রুত ঘূর্ণায়মান রিং)
        glPushMatrix();
            glRotatef(nexusAngle * 3.0f, 0.0f, 0.0f, 1.0f);
            glColor4f(1.0f, 0.0f, 1.0f, 0.9f); // Neon Magenta
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(0.0f, 0.2f);  glVertex2f(0.18f, 0.08f);
                glVertex2f(0.18f, -0.08f); glVertex2f(0.0f, -0.2f);
                glVertex2f(-0.18f, -0.08f); glVertex2f(-0.18f, 0.08f);
            glEnd();
        glPopMatrix();

        // 4. Slow Outer Cyber-Ring (বাইরের ধীরে ঘোরা জটিল রিং)
        glPushMatrix();
            glRotatef(-nexusAngle, 0.0f, 0.0f, 1.0f); // উল্টো ঘুরবে
            glColor4f(0.0f, 0.8f, 1.0f, 0.5f);
            glLineWidth(1.5f);
            // একাধিক ত্রিভুজ দিয়ে থ্রিডি শেপ বোঝানো
            for(int i = 0; i < 360; i += 45) {
                glPushMatrix();
                glRotatef(i, 0.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                    glVertex2f(0.0f, 0.3f); glVertex2f(0.05f, 0.25f); glVertex2f(-0.05f, 0.25f);
                glEnd();
                glPopMatrix();
            }
            drawCircle(0.3f, 0.0f, 1.0f, 1.0f); // Outer boundary
        glPopMatrix();

        glDisable(GL_BLEND);
    glPopMatrix();
}

// --- 🤖 আপগ্রেডেড অ্যাডভান্সড ট্রান্সফর্মার ---
void drawTransformer() {
    glPushMatrix();
        glTranslatef(transX, -0.4f, 0.0f);

        float p = transProgress;

        // ⚡ মর্ফিং এনার্জি শিল্ড (যখন ট্রান্সফর্ম হবে তখন নীল বিদ্যুৎ দেখা যাবে)
        if (p > 0.1f && p < 0.9f) {
            glEnable(GL_BLEND);
            glColor4f(0.0f, 0.5f, 1.0f, 0.3f);
            drawCircle(0.2f + (sin(pulseTime * 10.0f) * 0.02f), 0.0f, 0.5f, 1.0f);
            glDisable(GL_BLEND);
        }

        // 1. মেকানিক্যাল পা / পেছনের চাকা (Detailed Legs)
        glPushMatrix();
            glTranslatef(lerp(-0.15f, -0.05f, p), lerp(0.0f, -0.15f, p), 0.0f);
            glColor3f(0.2f, 0.2f, 0.2f); // মেটাল জয়েন্ট
            if(p > 0.5f) {
                glBegin(GL_QUADS); // পায়ের মেকানিকস
                    glVertex2f(-0.04f, 0.0f); glVertex2f(0.04f, 0.0f);
                    glVertex2f(0.06f, 0.1f);  glVertex2f(-0.06f, 0.1f);
                glEnd();
            }
            drawCircle(0.04f, 0.0f, 0.8f, 1.0f); // গ্লোয়িং হুইল/পায়ের পাতা
        glPopMatrix();

        // দ্বিতীয় পা (আলাদা করে খুলবে)
        glPushMatrix();
            glTranslatef(lerp(-0.15f, 0.08f, p), lerp(0.0f, -0.15f, p), 0.0f);
            if(p > 0.5f) {
                glColor3f(0.15f, 0.15f, 0.15f);
                glBegin(GL_QUADS);
                    glVertex2f(-0.04f, 0.0f); glVertex2f(0.04f, 0.0f);
                    glVertex2f(0.06f, 0.1f);  glVertex2f(-0.06f, 0.1f);
                glEnd();
            }
            drawCircle(0.04f, 0.0f, 0.5f, 0.8f);
        glPopMatrix();

        // 2. মেইন বডি প্যানেল (Splitting Armor)
        glPushMatrix();
            glTranslatef(0.0f, lerp(0.0f, 0.05f, p), 0.0f);
            glRotatef(lerp(0.0f, 90.0f, p), 0.0f, 0.0f, 1.0f);

            // আউটার আর্মার (Outer Armor)
            glColor3f(0.1f, 0.15f, 0.25f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.2f, -0.05f); glVertex2f(0.2f, -0.05f);
                glVertex2f(0.18f, 0.05f);  glVertex2f(-0.18f, 0.05f);
            glEnd();

            // ইনার কোর (Inner Glowing Core - শুধু রোবট মোডে স্পষ্ট হবে)
            if (p > 0.3f) {
                glColor3f(1.0f, 0.5f, 0.0f); // কমলা কোর
                glBegin(GL_QUADS);
                    glVertex2f(-0.05f, -0.02f); glVertex2f(0.05f, -0.02f);
                    glVertex2f(0.05f, 0.02f);  glVertex2f(-0.05f, 0.02f);
                glEnd();
            }
        glPopMatrix();

        // 3. হাত এবং অস্ত্র (Arms & Weapons)
        glPushMatrix();
            glTranslatef(lerp(0.15f, 0.12f, p), lerp(0.0f, 0.1f, p), 0.0f);
            drawCircle(0.03f, 0.2f, 0.2f, 0.2f); // কাঁধের জয়েন্ট

            if (p > 0.8f) {
                // হাতের কামান (Arm Cannon)
                glColor3f(0.4f, 0.4f, 0.4f);
                glBegin(GL_QUADS);
                    glVertex2f(0.0f, -0.02f); glVertex2f(0.15f, -0.02f);
                    glVertex2f(0.15f, 0.02f);  glVertex2f(0.0f, 0.02f);
                glEnd();
                // কামানের মাথায় নীল আলো
                glColor3f(0.0f, 1.0f, 1.0f);
                glBegin(GL_QUADS);
                    glVertex2f(0.13f, -0.03f); glVertex2f(0.16f, -0.03f);
                    glVertex2f(0.16f, 0.03f);  glVertex2f(0.13f, 0.03f);
                glEnd();
            }
        glPopMatrix();

        // 4. রোবটের মাথা (Detailed Head)
        glPushMatrix();
            glTranslatef(lerp(0.0f, 0.0f, p), lerp(0.05f, 0.22f, p), 0.0f);
            // মাথার হেলমেট
            glColor3f(0.8f, 0.1f, 0.2f);
            glBegin(GL_POLYGON);
                glVertex2f(-0.06f, 0.0f); glVertex2f(0.06f, 0.0f);
                glVertex2f(0.04f, 0.08f); glVertex2f(0.0f, 0.1f); glVertex2f(-0.04f, 0.08f);
            glEnd();

            if (p > 0.8f) {
                // সাইবারনেটিক চোখ (Glowing Eyes)
                glColor3f(1.0f, 1.0f, 0.0f);
                glBegin(GL_QUADS);
                    glVertex2f(0.01f, 0.04f); glVertex2f(0.05f, 0.04f);
                    glVertex2f(0.04f, 0.06f); glVertex2f(0.01f, 0.06f);
                glEnd();
            }
        glPopMatrix();

        // 5. 💥 হেভি লেজার ব্লাস্ট (Heavy Plasma Beam)
        if (transState == 2) {
            glEnable(GL_BLEND);

            // আউটার বিম (সায়ান গ্লো)
            glColor4f(0.0f, 1.0f, 1.0f, 0.6f);
            glLineWidth(12.0f);
            glBegin(GL_LINES);
                glVertex2f(0.27f, 0.1f); glVertex2f(1.5f, 0.1f);
            glEnd();

            // ইনার বিম (সাদা/উজ্জ্বল)
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            glLineWidth(4.0f);
            glBegin(GL_LINES);
                glVertex2f(0.27f, 0.1f); glVertex2f(1.5f, 0.1f);
            glEnd();

            glDisable(GL_BLEND);
        }

    glPopMatrix();
}


// --- 🏢 মেগাস্ট্রাকচার (Cyberpunk Neon Building) ---
void drawMegaBuilding(float x, float height, float scale, float r, float g, float b) {
    glPushMatrix();
        glTranslatef(x, -0.4f, 0.0f);
        glScalef(scale, 1.0f, 1.0f);

        // 1. মেইন মেটাল বডি (Dark Sloped Armor)
        glColor3f(0.05f, 0.05f, 0.1f); // একদম ডার্ক ব্লু/কালো
        glBegin(GL_POLYGON);
            glVertex2f(-0.2f, 0.0f); glVertex2f(0.2f, 0.0f);
            glVertex2f(0.15f, height); glVertex2f(-0.15f, height);
        glEnd();

        // 2. গ্লাস রিফ্লেকশন প্যানেল (Glass overlay)
        glEnable(GL_BLEND);
        glColor4f(r, g, b, 0.3f); // ট্রান্সপারেন্ট কালার
        glBegin(GL_POLYGON);
            glVertex2f(-0.15f, 0.0f); glVertex2f(0.15f, 0.0f);
            glVertex2f(0.1f, height * 0.9f); glVertex2f(-0.1f, height * 0.9f);
        glEnd();

        // 3. পালসিং নিয়ন স্ট্রিপ (Pulsing Neon Lights on edges)
        float neonGlow = 0.5f + (sin(pulseTime * 2.0f + x) * 0.5f); // পালস করবে
        glColor4f(r * neonGlow, g * neonGlow, b * neonGlow, 1.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-0.2f, 0.0f); glVertex2f(-0.15f, height);
            glVertex2f(0.15f, height); glVertex2f(0.2f, 0.0f);
        glEnd();

        // 4. ফ্লোটিং ডেটা রিং (বিল্ডিংয়ের চারপাশে ঘুরবে)
        glPushMatrix();
            glTranslatef(0.0f, height * 0.6f, 0.0f);
            glScalef(1.0f, 0.2f, 1.0f); // চ্যাপ্টা করে থ্রিডি রিংয়ের ফিল
            glColor4f(1.0f, 1.0f, 0.0f, 0.6f); // Cyber Yellow
            drawCircle(0.25f, 1.0f, 1.0f, 0.0f);
        glPopMatrix();

        glDisable(GL_BLEND);
    glPopMatrix();
}

// --- 🚗 শহরের গাড়ি (যেটা সিগন্যাল মেনে চলবে) ---
void drawCityCar() {
    glPushMatrix();
        glTranslatef(cityCarX, -0.6f, 0.0f); // রাস্তায়

        // গাড়ির বডি (আধুনিক শেপ)
        glColor3f(0.1f, 0.4f, 0.8f); // নীল গাড়ি
        glBegin(GL_POLYGON);
            glVertex2f(-0.15f, 0.0f); glVertex2f(0.15f, 0.0f);
            glVertex2f(0.18f, 0.05f); glVertex2f(-0.18f, 0.05f);
        glEnd();

        // গাড়ির ছাদ (Cabin)
        glColor3f(0.05f, 0.2f, 0.4f);
        glBegin(GL_QUADS);
            glVertex2f(-0.08f, 0.05f); glVertex2f(0.08f, 0.05f);
            glVertex2f(0.05f, 0.12f);  glVertex2f(-0.1f, 0.12f);
        glEnd();

        // চাকা (Wheels)
        glPushMatrix(); glTranslatef(-0.1f, 0.0f, 0.0f); drawCircle(0.03f, 0.1f, 0.1f, 0.1f); glPopMatrix();
        glPushMatrix(); glTranslatef(0.1f, 0.0f, 0.0f);  drawCircle(0.03f, 0.1f, 0.1f, 0.1f); glPopMatrix();

        // হেডলাইট (Headlight Beam)
        glEnable(GL_BLEND);
        glColor4f(1.0f, 1.0f, 0.8f, 0.4f);
        glBegin(GL_TRIANGLES);
            glVertex2f(0.18f, 0.03f); // লাইটের সোর্স
            glVertex2f(0.6f, -0.05f); // রাস্তায় আলো পড়ছে
            glVertex2f(0.6f, 0.1f);   // উপরের আলো
        glEnd();
        glDisable(GL_BLEND);
    glPopMatrix();
}

void drawVillage() {
    // 1. Pahar (Gradient Shading: Dark at bottom, Light at top)
// 1. 3D Pahar (Left and Right sides with light and shadow)
    glBegin(GL_TRIANGLES);
        // --- Left Mountain ---
        // Left Side (Bright / Sunlit)
        glColor3f(0.5f, 0.7f, 0.4f); glVertex2f(-0.6f, 0.5f);  // Peak
        glColor3f(0.3f, 0.5f, 0.2f); glVertex2f(-1.0f, -0.2f); // Bottom Left
        glColor3f(0.4f, 0.6f, 0.3f); glVertex2f(-0.6f, -0.2f); // Bottom Center

        // Right Side (Shadow)
        glColor3f(0.5f, 0.7f, 0.4f); glVertex2f(-0.6f, 0.5f);  // Peak
        glColor3f(0.4f, 0.6f, 0.3f); glVertex2f(-0.6f, -0.2f); // Bottom Center
        glColor3f(0.2f, 0.4f, 0.1f); glVertex2f(-0.2f, -0.2f); // Bottom Right

        // --- Right Mountain (Taller) ---
        // Left Side (Bright)
        glColor3f(0.4f, 0.6f, 0.3f); glVertex2f(0.2f, 0.7f);   // Peak
        glColor3f(0.2f, 0.4f, 0.1f); glVertex2f(-0.3f, -0.2f); // Bottom Left
        glColor3f(0.3f, 0.5f, 0.2f); glVertex2f(0.2f, -0.2f);  // Bottom Center

        // Right Side (Shadow)
        glColor3f(0.4f, 0.6f, 0.3f); glVertex2f(0.2f, 0.7f);   // Peak
        glColor3f(0.3f, 0.5f, 0.2f); glVertex2f(0.2f, -0.2f);  // Bottom Center
        glColor3f(0.1f, 0.3f, 0.1f); glVertex2f(0.7f, -0.2f);  // Bottom Right
    glEnd();

    // NEW: Windmill in the background
    drawWindmill();

    // 2. Sun (With Glowing Halo Effect)
    glPushMatrix();
        glTranslatef(0.6f, 0.6f, 0.0f);
        drawCircle(0.18f, 1.0f, 0.9f, 0.0f); // Outer Glow (Yellow)
        drawCircle(0.14f, 1.0f, 0.5f, 0.0f); // Inner Core (Orange)
    glPopMatrix();

    // 3. Ground
    glColor3f(0.3f, 0.7f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.2f);  glVertex2f(-1.0f, -0.2f);
    glEnd();

    // 4. River with Animated Ripples
    glColor3f(0.1f, 0.5f, 0.9f); // Deep Blue River
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.9f); glVertex2f(1.0f, -0.9f);
        glVertex2f(1.0f, -0.5f);  glVertex2f(-1.0f, -0.5f);
    glEnd();



    // Water Ripples (White dashed lines that move)
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // Semi-transparent white
    glLineWidth(2.0f);
    for(float y = -0.8f; y < -0.55f; y += 0.1f) {
        glBegin(GL_LINES);
        for(float x = -1.0f; x < 1.0f; x += 0.3f) {
            glVertex2f(x + rippleOffset, y);
            glVertex2f(x + rippleOffset + 0.1f, y);
        }
        glEnd();

            // NEW: Boat on the river
    drawBoat();
    }
drawSmoke();

    // 6. Add Fences
    drawFence(-0.9f, -0.3f);
    drawFence(-0.2f, -0.3f);

// 5. 3D Isometric Hut
    glPushMatrix();
        glTranslatef(-0.6f, -0.4f, 0.0f); // Position

        // Front Wall (Light Brown)
        glColor3f(0.8f, 0.6f, 0.4f);
        glBegin(GL_QUADS);
            glVertex2f(-0.2f, 0.0f); glVertex2f(0.1f, 0.0f);
            glVertex2f(0.1f, 0.3f);  glVertex2f(-0.2f, 0.3f);
        glEnd();

        // Side Wall (Darker Brown for Shadow)
        glColor3f(0.5f, 0.3f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(0.1f, 0.0f);  glVertex2f(0.35f, 0.1f);
            glVertex2f(0.35f, 0.4f); glVertex2f(0.1f, 0.3f);
        glEnd();

        // Front Roof (Bright Red)
        glColor3f(0.8f, 0.2f, 0.2f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.25f, 0.3f); glVertex2f(0.15f, 0.3f);
            glVertex2f(-0.05f, 0.5f);
        glEnd();

        // Side Roof (Dark Red)
        glColor3f(0.6f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(0.15f, 0.3f);  glVertex2f(-0.05f, 0.5f);
            glVertex2f(0.2f, 0.6f);   glVertex2f(0.4f, 0.4f);
        glEnd();

        // Door on Front Wall
        glColor3f(0.3f, 0.1f, 0.05f);
        glBegin(GL_QUADS);
            glVertex2f(-0.1f, 0.0f);  glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.15f);  glVertex2f(-0.1f, 0.15f);
        glEnd();

        // Window on Side Wall
        glColor3f(0.1f, 0.6f, 0.8f); // Glass blue
        glBegin(GL_QUADS);
            glVertex2f(0.15f, 0.15f); glVertex2f(0.25f, 0.19f);
            glVertex2f(0.25f, 0.29f); glVertex2f(0.15f, 0.25f);
        glEnd();

        // 🌿 6. Lotapata (Creeping Vines on the Walls)
        glPointSize(3.0f); // পাতার সাইজ
        glBegin(GL_POINTS);
            // সামনের দেয়াল বেয়ে ওঠা লতা
            for(float vy = 0.0f; vy < 0.28f; vy += 0.015f) {
                // sin() দিয়ে লতাটাকে একটু আঁকাবাঁকা (organic) করা হয়েছে
                float vx = -0.18f + sin(vy * 40.0f) * 0.015f;

                glColor3f(0.1f, 0.4f, 0.1f); // গাঢ় সবুজ পাতা
                glVertex2f(vx, vy);

                glColor3f(0.3f, 0.6f, 0.1f); // হালকা সবুজ পাতা (ডিটেইলস)
                glVertex2f(vx + 0.01f, vy - 0.01f);
            }

            // পাশের দেয়ালে জানালার কাছে ওঠা লতা
            for(float vy = 0.0f; vy < 0.25f; vy += 0.02f) {
                float vx = 0.32f + cos(vy * 50.0f) * 0.01f;
                glColor3f(0.1f, 0.3f, 0.05f); // ছায়াতে থাকা গাঢ় পাতা
                glVertex2f(vx, vy);
            }
        glEnd();
    glPopMatrix();


    // --- (Keep your existing mountains, sun, ground, river, hut, fences here) ---

    // Calculate wind sway angle using sine wave (sways between -5 and +5 degrees)
    float sway = sin(windTime) * 5.0f;

    // 5. Gach (Trees with realistic sway!)
    glPushMatrix();
        glTranslatef(0.4f, -0.3f, 0.0f);
        drawTree(sway); // Pass the sway angle
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.7f, -0.4f, 0.0f);
        glScalef(0.8f, 0.8f, 1.0f);
        drawTree(sway * 1.2f); // Choto gach ektu beshi dolbe (1.2x)
    glPopMatrix();

    // 6. Draw the Cartoon Wind & Birds
    drawWindStreaks();
    drawBirds();
}


void drawCity() {
    // 1. পেছনের ছায়া (Parallax Skyline)
    drawSkyline();

    // 2. Road & Footpath (তোমার পুরোনো কোড)
    glColor3f(0.15f, 0.15f, 0.15f); // Dark Road
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.45f); glVertex2f(-1.0f, -0.45f);
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f); // Concrete Footpath
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.45f); glVertex2f(1.0f, -0.45f);
        glVertex2f(1.0f, -0.4f);   glVertex2f(-1.0f, -0.4f);
    glEnd();

    // Divider Lines
    glColor3f(1.0f, 0.8f, 0.0f);
    for (float i = -0.9f; i < 1.0f; i += 0.4f) {
        glBegin(GL_QUADS);
            glVertex2f(i, -0.75f); glVertex2f(i + 0.2f, -0.75f);
            glVertex2f(i + 0.2f, -0.72f); glVertex2f(i, -0.72f);
        glEnd();
    }

    // 3. 3D Buildings (এখানে ম্যাজিক!)
    // পজিশন, উচ্চতা, এবং RGB কালার দিয়ে দিয়েছি
    draw3DBuilding(-0.9f, 0.6f,  0.3f, 0.3f, 0.4f); // নীলচে বিল্ডিং
    draw3DBuilding(-0.5f, 0.9f,  0.4f, 0.3f, 0.3f); // লালচে উঁচু বিল্ডিং
    draw3DBuilding(-0.1f, 0.5f,  0.3f, 0.4f, 0.3f); // সবুজ বিল্ডিং
    draw3DBuilding(0.3f,  0.8f,  0.4f, 0.4f, 0.4f); // গ্রে বিল্ডিং
    draw3DBuilding(0.7f,  0.7f,  0.3f, 0.3f, 0.5f); // ডিপ ব্লু বিল্ডিং

// 4. Street Lamps (আপগ্রেডেড)
    drawStreetLamp(-0.7f);
    drawStreetLamp(0.7f);

    // 5. Traffic Signal (রাস্তার ডানদিকে)
    drawTrafficLight(0.3f, -0.4f);

    // 6. City Car (গাড়ি)
    drawCityCar();
}


void drawFuture() {
    // 1. Circuit Grid Ground (সাইবারপাংক মাটি)
    glColor3f(0.02f, 0.02f, 0.08f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.4f);  glVertex2f(-1.0f, -0.4f);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(0.0f, 0.8f, 0.8f); // নিয়নের নীল গ্রিড
    glBegin(GL_LINES);
    // পেছনের দিকে চলে যাওয়া লাইন (Perspective)
    for(float i = -1.5f; i <= 1.5f; i += 0.3f) {
        glVertex2f(i, -1.0f); glVertex2f(i * 0.3f, -0.4f);
    }
    // আনুভূমিক লাইন
    for(float y = -0.9f; y < -0.4f; y += 0.15f) {
        glVertex2f(-1.0f, y); glVertex2f(1.0f, y);
    }
    glEnd();

// 2. NEW: Cyberpunk Megastructures
    // পেছনে বিশাল সাইজের বিল্ডিং, সামনে মাঝারি
    drawMegaBuilding(-0.7f, 0.9f, 1.2f, 0.0f, 1.0f, 1.0f); // Cyan Megatower
    drawMegaBuilding(0.7f, 0.8f, 1.0f, 1.0f, 0.0f, 1.0f);  // Magenta Megatower
    drawMegaBuilding(-0.3f, 0.6f, 0.8f, 1.0f, 1.0f, 0.0f); // Yellow Tower
    drawMegaBuilding(0.3f, 0.5f, 0.7f, 0.0f, 1.0f, 0.0f);  // Green Data Center

    // 3. NEW: The Quantum Nexus (মাঝখানে আকাশে)
    drawMegaNexus();

    // 4. 🤖 দ্য মেকানিক্যাল ট্রান্সফর্মার (আগের কোডটাই থাকবে)
    drawTransformer();
}

// ==========================================
// CORE RENDERING ENGINE
// ==========================================
void display() {
    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 🎬 Fade Timing Logic (পর্দা গাঢ় থাকা অবস্থায় সিন চেঞ্জ হবে)
    if (currentState == VILLAGE ||
       (currentState == TRANSITION_TO_CITY && transitionProgress < 0.5f) ||
       (currentState == TRANSITION_TO_VILLAGE && transitionProgress >= 0.5f)) {
        drawVillage();
    }
    else if (currentState == CITY ||
            (currentState == TRANSITION_TO_FUTURE && transitionProgress < 0.5f) ||
            (currentState == TRANSITION_TO_CITY && transitionProgress >= 0.5f)) {
        drawCity();
    }
    else if (currentState == FUTURE ||
            (currentState == TRANSITION_TO_VILLAGE && transitionProgress < 0.5f) ||
            (currentState == TRANSITION_TO_FUTURE && transitionProgress >= 0.5f)) {
        drawFuture();
    }

    // 🌫️ The Magic Fade Overlay (কুয়াশা বা আকাশের রঙের সাথে মিশে যাওয়া)
    if (fadeAlpha > 0.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // পর্দাটা আকাশের রঙেরই হবে, শুধু Alpha (স্বচ্ছতা) চেঞ্জ হবে
        glColor4f(bgR, bgG, bgB, fadeAlpha);
        glBegin(GL_QUADS);
            glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
            glVertex2f(1.0f, 1.0f);   glVertex2f(-1.0f, 1.0f);
        glEnd();
        glDisable(GL_BLEND);
    }

    glutSwapBuffers();
}

// ==========================================
// TIME & STATE MANAGER
// ==========================================
void update(int value) {
    timer += 0.1f; // Increment timer (each 100ms)

    // ==========================================
    // ANIMATION LOGIC (সব মুভিং অবজেক্টের ম্যাথ)
    // ==========================================

    // 🤖 Transformer Logic State Machine
    // শুধুমাত্র ফিউচার মোডেই এই অ্যানিমেশন কাজ করবে
    if (currentState == FUTURE) {
        switch(transState) {
            case 0: // 🚗 Driving In (গাড়ি হয়ে স্ক্রিনের মাঝখানে আসবে)
                transX += 0.02f;
                if (transX >= 0.0f) transState = 1; // মাঝখানে আসলে থামবে
                break;

            case 1: // 🔄 Morphing to Robot (ট্রান্সফর্ম শুরু)
                transProgress += 0.02f;
                if (transProgress >= 1.0f) {
                    transProgress = 1.0f;
                    transState = 2; // ট্রান্সফর্ম শেষ, এখন অ্যাকশন!
                }
                break;

            case 2: // 💥 Robot Action (লেজার মারবে ২ সেকেন্ড ধরে)
                laserTimer += 0.1f;
                pulseTime += 0.5f; // লেজার মারার সময় রোবট কাঁপবে (ঐচ্ছিক)
                if (laserTimer >= 2.0f) {
                    transState = 3; // মারা শেষ, এবার ব্যাক করবে
                    laserTimer = 0.0f;
                }
                break;

            case 3: // 🔄 Morphing to Car (আবার গাড়ি হয়ে যাবে)
                transProgress -= 0.02f;
                if (transProgress <= 0.0f) {
                    transProgress = 0.0f;
                    transState = 4;
                }
                break;

            case 4: // 🚗 Drive Away (স্ক্রিন থেকে চলে যাবে)
                transX += 0.03f; // একটু জোরে পালাবে
                break;
        }
    }
// 🚦 1. Traffic Signal Timer Logic
    trafficTimer += 0.05f;
    if (trafficTimer < 3.0f) {
        trafficState = 0; // প্রথম ৩ সেকেন্ড: Green
    } else if (trafficTimer < 4.5f) {
        trafficState = 1; // পরের ১.৫ সেকেন্ড: Yellow
    } else if (trafficTimer < 8.0f) {
        trafficState = 2; // শেষের ৩.৫ সেকেন্ড: Red
    } else {
        trafficTimer = 0.0f; // আবার সবুজ হয়ে লুপ শুরু হবে
    }

    // 🚗 2. Smart Car Movement (গাড়ি সিগন্যাল চেক করবে)
    // গাড়িটা সিগন্যালের একটু আগে (X = 0.0 থেকে 0.2 এর মধ্যে) থাকলে ব্রেক করবে।
    if (trafficState == 0) { // Green (সবুজ)
        cityCarX += 0.015f;  // ফুল স্পিডে চলবে
    }
    else if (trafficState == 1) { // Yellow (হলুদ)
        if (cityCarX > -0.1f && cityCarX < 0.2f) {
            cityCarX += 0.005f; // সিগন্যালের কাছাকাছি থাকলে স্লো হয়ে যাবে
        } else {
            cityCarX += 0.015f; // দূরে থাকলে বা পার হয়ে গেলে চলতে থাকবে
        }
    }
    else if (trafficState == 2) { // Red (লাল)
        if (cityCarX > -0.1f && cityCarX < 0.2f) {
            // ব্রেক! গাড়ি এখানে থেমে থাকবে, X এর ভ্যালু বাড়বে না।
        } else {
            cityCarX += 0.015f; // সিগন্যাল পার হয়ে গেলে আর থামবে না
        }
    }

    // গাড়ি স্ক্রিন পার হয়ে গেলে আবার বামদিক থেকে আসবে
    if (cityCarX > 1.5f) {
        cityCarX = -1.5f;
    }
    // City Background Animation (Parallax)
    skylineOffset -= 0.005f; // খুব ধীরে মুভ করবে, যাতে বিশালত্ব বোঝায়
    if (skylineOffset < -1.0f) skylineOffset = 0.0f; // লুপ
    // 1. Cloud
    cloudX += 0.005f;
    if (cloudX > 1.2f) cloudX = -1.2f;

    // 2. River Ripples
    rippleOffset += 0.01f;
    if (rippleOffset > 0.3f) rippleOffset = 0.0f;

    // 3. Hover Car (Future)
    hoverY = sin(pulseTime) * 0.1f;

    // 4. City Car
    carX += 0.02f;
    if (carX > 1.2f) carX = -1.2f;

    // 🔮 Hologram Spin Animation
    holoAngle += 0.05f;
    // 5. Neon Pulse
    pulseTime += 0.2f;

    // 💎 AI Nexus Animation
    nexusAngle += 1.0f; // ক্রিস্টাল রিং স্পিন করবে
    nexusHover = sin(pulseTime) * 0.05f; // ক্রিস্টালটা শূন্যে ভাসবে (Hover)

    // 6. NEW: Wind & Tree Sway (গাছ দোলানো)
    windTime += 0.1f;

    // 7. NEW: Wind Streaks (হাওয়ার রেখা)
    windStreakX += 0.05f;
    if (windStreakX > 1.5f) windStreakX = -1.5f;

    // 8. NEW: Flying Birds (পাখি ওড়া)
    birdX += 0.01f;
    if (birdX > 1.2f) birdX = -1.2f;

    // 9. NEW: Windmill Rotation (হাওয়াকল ঘুরবে)
    windmillAngle -= 1.5f; // মাইনাস দিলে ডানদিক থেকে বামে ঘুরবে (হাওয়ার দিকে)
    if (windmillAngle <= -360.0f) windmillAngle += 360.0f; // Reset angle to avoid overflow

    // 10. NEW: Boat Movement (নৌকা ভাসবে)
    boatX -= 0.005f; // ডান থেকে বামে যাবে
    if (boatX < -1.5f) boatX = 1.5f; // স্ক্রিন পার হলে আবার ডানদিকে চলে আসবে

    // 11. NEW: Chimney Smoke (ধোঁয়া উপরে উঠবে)
    smokeProgress += 0.02f;
    if (smokeProgress >= 1.0f) smokeProgress = 0.0f; // ধোঁয়া লুপ হবে

    // ==========================================
    // STATE MACHINE LOGIC (সিন পরিবর্তনের লজিক)
    // ==========================================
switch(currentState) {
        case VILLAGE:
            if (timer >= 5.0f) { // Stay in village for 5 seconds
                currentState = TRANSITION_TO_CITY;
                timer = 0.0f;
            }
            break;

        case TRANSITION_TO_CITY:
            transitionProgress += 0.02f;
            bgR = lerp(0.5f, 0.2f, transitionProgress);
            bgG = lerp(0.8f, 0.2f, transitionProgress);
            bgB = lerp(1.0f, 0.4f, transitionProgress);

            // Fade Math (0 থেকে 0.5 পর্যন্ত গাঢ় হবে, 0.5 এর পর ক্লিয়ার হবে)
            if (transitionProgress < 0.5f) {
                fadeAlpha = transitionProgress * 2.0f; // 0.0 -> 1.0
            } else {
                fadeAlpha = 1.0f - ((transitionProgress - 0.5f) * 2.0f); // 1.0 -> 0.0
            }

            if (transitionProgress >= 1.0f) {
                currentState = CITY;
                transitionProgress = 0.0f;
                fadeAlpha = 0.0f; // রিসেট
                timer = 0.0f;
            }
            break;

        case CITY:
            if (timer >= 5.0f) { // Stay in city for 5 seconds
                currentState = TRANSITION_TO_FUTURE;
                timer = 0.0f;
            }
            break;

        case TRANSITION_TO_FUTURE:
            transitionProgress += 0.02f;
            bgR = lerp(0.2f, 0.0f, transitionProgress); // Dark to Deep Purple/Black
            bgG = lerp(0.2f, 0.0f, transitionProgress);
            bgB = lerp(0.4f, 0.1f, transitionProgress);

            // Fade Math (শহর থেকে ফিউচারে যাওয়ার সময়)
            if (transitionProgress < 0.5f) {
                fadeAlpha = transitionProgress * 2.0f;
            } else {
                fadeAlpha = 1.0f - ((transitionProgress - 0.5f) * 2.0f);
            }

            if (transitionProgress >= 1.0f) {
                currentState = FUTURE;
                transitionProgress = 0.0f;
                fadeAlpha = 0.0f; // রিসেট
                timer = 0.0f;
            }
            break;

       case FUTURE:
            // আগে ছিল 5 সেকেন্ডের টাইমার, এখন সেটা ডিলিট করে দিয়েছি।
            // এর বদলে আমরা চেক করছি ট্রান্সফর্মার তার শেষ স্টেজে (transState == 4)
            // আছে কি না এবং সে স্ক্রিন পার হয়ে যাচ্ছে কি না।

            if (transState == 4 && transX >= 1.4f) {
                currentState = TRANSITION_TO_VILLAGE;

                // নেক্সট লুপের জন্য ট্রান্সফর্মারকে আবার রিসেট করে দিলাম
                transX = -1.2f;
                transState = 0;
                timer = 0.0f;
            }
            break;

        case TRANSITION_TO_VILLAGE:
            transitionProgress += 0.02f;
            bgR = lerp(0.0f, 0.5f, transitionProgress); // Back to Sky Blue
            bgG = lerp(0.0f, 0.8f, transitionProgress);
            bgB = lerp(0.1f, 1.0f, transitionProgress);

            // Fade Math (ফিউচার থেকে আবার গ্রামে ফেরার সময়)
            if (transitionProgress < 0.5f) {
                fadeAlpha = transitionProgress * 2.0f;
            } else {
                fadeAlpha = 1.0f - ((transitionProgress - 0.5f) * 2.0f);
            }

            if (transitionProgress >= 1.0f) {
                currentState = VILLAGE;
                transitionProgress = 0.0f;
                fadeAlpha = 0.0f; // রিসেট
                timer = 0.0f;
            }
            break;
    }

    glutPostRedisplay();

    // 100ms এর জায়গায় 30ms দিলে অ্যানিমেশন একদম স্মুথ (Smooth) মাখনের মতো হবে!
    glutTimerFunc(100, update, 0);
}

// ==========================================
// INITIALIZATION & MAIN
// ==========================================
void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Simple 2D coordinate system
    glMatrixMode(GL_MODELVIEW);


    // Enable blending for potential future alpha transitions
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffering is CRITICAL for smooth animation
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Computer Graphics Project: The Evolution");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(100, update, 0); // Start the timer loop

    glutMainLoop();
    return 0;
}
