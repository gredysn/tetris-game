#pragma once

#include "ofMain.h"
#include <vector>

enum class PieceType { I, O, T, S, Z, J, L, N}; // Standard 7 tetromino types

struct PieceCell { int x, y; }; // Local offsets relative to piece origin

class Piece {
public:
    Piece(PieceType type);

    void move(int dx, int dy);
    void rotate();
    std::vector<ofVec2f> getCells(int baseX, int baseY) const;
    ofColor getColor() const;
    PieceType getType() const;

    int posX, posY; // Piece origin on board (top-left reference point)
    int texturedPiece;
    //bonus
    int getcurrentTexture() const {return texturedPiece;}

private:
    int rotationState;              // 0..3 rotation index
    PieceType type;                 // Which tetromino
    std::vector<PieceCell> cells;   // Shape definition after current rotation

    void setShape();                // Rebuild 'cells' based on type & rotation
};