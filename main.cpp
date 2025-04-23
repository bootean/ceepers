#include "raylib.h"
#include <vector>
#include <iostream>
#include <cmath> // For lerp function

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 30;  // Size of each grid cell in pixels
const int GRID_COLS = 100;  // Number of columns in the grid
const int GRID_ROWS = 100;  // Number of rows in the grid
const float CAMERA_SPEED = 0.08f; // Camera lerp speed (0.0 to 1.0)
const int CAMERA_DELAY_FRAMES = 7; // Frames to wait before camera starts moving

// Entity struct to represent game objects
struct Entity {
    int x;  // Grid position X
    int y;  // Grid position Y
    Color color;
    
    Entity(int _x, int _y, Color _color) : x(_x), y(_y), color(_color) {}
    
    // Draw the entity on the grid with camera offset
    void draw() const {
        // Draw a rectangle for the entity
        DrawRectangle(
            x * GRID_SIZE + 1,  // +1 for a small gap
            y * GRID_SIZE + 1,
            GRID_SIZE - 2,      // -2 for a small gap
            GRID_SIZE - 2,
            color
        );
    }
    
    // Move in a direction (dx, dy)
    bool move(int dx, int dy) {
        int new_x = x + dx;
        int new_y = y + dy;
        
        // Check bounds
        if (new_x >= 0 && new_x < GRID_COLS && new_y >= 0 && new_y < GRID_ROWS) {
            x = new_x;
            y = new_y;
            return true; // Movement successful
        }
        return false; // Movement failed
    }
};

// Linear interpolation function
float lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

// Draw the grid
void drawGrid() {
    // Draw vertical lines
    for (int x = 0; x <= GRID_COLS; x++) {
        DrawLine(x * GRID_SIZE, 0, x * GRID_SIZE, GRID_ROWS * GRID_SIZE, LIGHTGRAY);
    }
    
    // Draw horizontal lines
    for (int y = 0; y <= GRID_ROWS; y++) {
        DrawLine(0, y * GRID_SIZE, GRID_COLS * GRID_SIZE, y * GRID_SIZE, LIGHTGRAY);
    }
}

int main() {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Eepers C++");
    SetTargetFPS(60);
    
    // Create player at the center of the grid
    Entity player(GRID_COLS / 2, GRID_ROWS / 2, BLUE);
    
    // Initialize camera
    Camera2D camera = { 0 };
    camera.target = { (float)(player.x * GRID_SIZE + GRID_SIZE/2), (float)(player.y * GRID_SIZE + GRID_SIZE/2) };
    camera.offset = { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    // Target position for camera (for smooth movement)
    Vector2 targetPosition = { camera.target.x, camera.target.y };
    
    // Camera delay counter
    int cameraDelayCounter = 0;
    bool cameraMoving = false;
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Check for window close button or Q key press
        if (IsKeyPressed(KEY_Q)) {
            break;  // Exit the game loop
        }
        
        // Update
        // Handle input for player movement
        bool playerMoved = false;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            playerMoved = player.move(0, -1);
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            playerMoved = player.move(0, 1);
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            playerMoved = player.move(-1, 0);
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            playerMoved = player.move(1, 0);
        }
        
        // Update camera behavior based on player movement
        if (playerMoved) {
            targetPosition = { (float)(player.x * GRID_SIZE + GRID_SIZE/2), (float)(player.y * GRID_SIZE + GRID_SIZE/2) };
            cameraDelayCounter = 0;
            cameraMoving = false;
        }
        
        // Update camera delay counter
        if (!cameraMoving && cameraDelayCounter < CAMERA_DELAY_FRAMES) {
            cameraDelayCounter++;
            if (cameraDelayCounter >= CAMERA_DELAY_FRAMES) {
                cameraMoving = true;
            }
        }
        
        // Smoothly move camera towards target position after delay
        if (cameraMoving) {
            camera.target.x = lerp(camera.target.x, targetPosition.x, CAMERA_SPEED);
            camera.target.y = lerp(camera.target.y, targetPosition.y, CAMERA_SPEED);
            
            // Check if camera is close enough to target to stop moving
            float dx = fabs(camera.target.x - targetPosition.x);
            float dy = fabs(camera.target.y - targetPosition.y);
            if (dx < 0.5f && dy < 0.5f) {
                cameraMoving = false;
            }
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Begin camera mode
        BeginMode2D(camera);
        
        // Draw grid
        drawGrid();
        
        // Draw player
        player.draw();
        
        // End camera mode
        EndMode2D();
        
        // Draw UI elements (not affected by camera)
        DrawFPS(10, 10);
        
        EndDrawing();
    }
    
    // De-initialization
    CloseWindow();
    
    return 0;
}