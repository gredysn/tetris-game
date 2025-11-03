// ofApp.h
// Main application layer controlling game state, input, rendering, timing,
// audio, progression, piece lifecycle and UI (sidebar / preview / score).
#pragma once

#include "ofMain.h"
#include "board.h"
#include "piece.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    Board board;              // Settled blocks grid (10x20 standard)
    Piece* currentPiece;      // Currently falling piece
    Piece* nextPiece;         // Next piece preview
    Piece* prevPiece;         // Previous piece for switching pieces
    
    float dropTimer;                // Accumulates elapsed time toward gravity drop
    float dropInterval;             // Current gravity interval (adjusts with level)
    int cellSize;                   // Pixel size of one board cell
    int boardOffsetX, boardOffsetY; // Top-left pixel offset for board drawing
    int score;                      // Player score
    bool gameOver;                  // Game over flag
    bool muted = false;             // Audio mute toggle

    // Progression
    int level = 1;                 // Current level (increases by lines)
    int totalLinesCleared = 0;     // Aggregate lines for level calc
    float baseDropInterval = 0.5f; // Starting gravity interval


    int pendingHardDropBonus = 0;

    // Audio
    ofSoundPlayer musicPlayer;     // Background music
    ofSoundPlayer rotateSound;     // Rotation SFX
    ofSoundPlayer lineClearSound;  // Line clear SFX
    ofSoundPlayer loseSound;       // Game over SFX
    
    void spawnNewPiece();                      // Create & position next piece
    bool canMovePiece(int dx, int dy);         // Collision test for translation
    void movePiece(int dx, int dy);            // Apply movement if valid
    void rotatePiece();                        // Attempt rotation (with rollback)
    void dropPiece();                          // Single gravity step or lock
    void hardDrop();                           // Instantly drop to floor + bonus
    int descendHardDrop();                     // Recursive descent distance
    void lockPiece();                          // Merge piece into board + line clear
    void resetGame();                          // Reinitialize state
    PieceType getRandomPieceType();            // Uniform random piece type (simple RNG)
    void playLineClear(int linesCleared);      // Play SFX if lines were cleared
    void updateMusicState();                   // Handle mute/unmute logic
    void recalcSpeed(int linesClearedJustNow); // Adjust gravity speed by level
    void allocateNextIfNeeded();               // Lazy-create next piece
    void drawGhostPiece();                     // Draw landing shadow of current piece
    void switchToPrevPiece();                  // Swap current piece with previous one
    void drawPreviousPiece();                  // Draw previous piece in sidebar
};