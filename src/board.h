#pragma once

#include "ofMain.h"
#include "piece.h"
#include <vector>

class Board {
public:
    Board(int width = 10, int height = 20);

    void draw(int cellSize, int offsetX, int offsetY) const; // Render grid + settled cells
    bool canPlacePiece(const Piece& piece) const;            // True if piece fits
    void placePiece(const Piece& piece);                     // Merge piece cells into grid
    int clearLines();                                        // Clear full rows; returns count
    void reset();                                            // Wipe board
    bool isGameOver() const;                                 // True if top row occupied

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isCellOccupied(int x, int y) const;

private:
    int width, height;                 // Dimensions of board
    vector<vector<ofColor>> grid;      // Color grid; black means empty

    bool isValidPosition(int x, int y) const; // Inside bounds
    bool isLineFull(int row) const;           // Row fully occupied
    void clearLine(int row);                  // Set row cells to empty
    void dropLinesAbove(int row);             // Shift everything above down one
};