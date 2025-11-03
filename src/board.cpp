#include "board.h"
using namespace std;

Board::Board(int width, int height) : width(width), height(height) {
    grid.resize(height, vector<ofColor>(width, ofColor::black));
}

void Board::draw(int cellSize, int offsetX, int offsetY) const {
    // Background with transparency 
    ofEnableAlphaBlending();
    static ofImage bg;    
    static bool loaded = false;
    if(!loaded) {
        loaded = true;
    }
    
    ofSetColor(255,255,255,140); // ~55% opacity
    bg.draw(offsetX, offsetY, width * cellSize, height * cellSize);
    ofSetColor(255); // reset color for subsequent draws

    // grid lines
    ofSetColor(60);
    for (int x = 0; x <= width; ++x) {
        ofDrawLine(offsetX + x * cellSize, offsetY,
                   offsetX + x * cellSize, offsetY + height * cellSize);
    }
    for (int y = 0; y <= height; ++y) {
        ofDrawLine(offsetX, offsetY + y * cellSize,
                   offsetX + width * cellSize, offsetY + y * cellSize);
    }

    // settled cells
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x] != ofColor::black) {
                ofSetColor(grid[y][x]);
                ofDrawRectangle(offsetX + x * cellSize + 1,
                                offsetY + y * cellSize + 1,
                                cellSize - 2, cellSize - 2);
            }
        }
    }
}

bool Board::canPlacePiece(const Piece& piece) const {
    auto cells = piece.getCells(piece.posX, piece.posY);
    for (const auto &c : cells) {
        int x = static_cast<int>(c.x);
        int y = static_cast<int>(c.y);
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        if (isCellOccupied(x, y)) return false;
    }
    return true;
}

// Phase 2: Implement placePiece
// in-game tip: Using the CPLUSPLUS.com see how static_cast works
void Board::placePiece(const Piece& piece) {

}

int Board::clearLines() {
    int cleared = 0;
    for (int y = height - 1; y >= 0; --y) {
        if (isLineFull(y)) {
            clearLine(y);
            dropLinesAbove(y);
            ++cleared;
            ++y; // re-check same row after drop
        }

    }
    return cleared;
}

void Board::reset() {
    for (auto &row : grid) {
        fill(row.begin(), row.end(), ofColor::black);
    }
}


bool Board::isCellOccupied(int x, int y) const {
    if (!isValidPosition(x, y)) return true;
    return grid[y][x] != ofColor::black;
}

bool Board::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::isLineFull(int row) const {
    for (int x = 0; x < width; ++x) if (grid[row][x] == ofColor::black) return false;
    return true;
}

void Board::clearLine(int row) {
    for (int x = 0; x < width; ++x) grid[row][x] = ofColor::black;
}

void Board::dropLinesAbove(int row) {
    for (int y = row; y > 0; --y) {
        grid[y] = grid[y - 1];
    }
    fill(grid[0].begin(), grid[0].end(), ofColor::black);
}