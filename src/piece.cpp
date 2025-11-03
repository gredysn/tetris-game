#include "piece.h"
using namespace std;

Piece::Piece(PieceType type)
: posX(0), posY(0), rotationState(0), type(type) { // order matches declaration
    setShape();
}

void Piece::move(int dx, int dy) {
    posX += dx;
    posY += dy;
}

// Phase 1: Implement rotate
// in-game tip: Read about modulo operator on CPLUSPLUS.com, and how the pieces are defined in setShape() with rotationState.
void Piece::rotate() {

}

vector<ofVec2f> Piece::getCells(int baseX, int baseY) const {
    vector<ofVec2f> result;
    result.reserve(cells.size());
    for (const auto& c : cells) {
        result.emplace_back(baseX + c.x, baseY + c.y);
    }
    return result;
}

ofColor Piece::getColor() const {
    switch (type) {
        // Phase 1: Define colors for each PieceType

    }
    return ofColor::white; // Fallback
}

PieceType Piece::getType() const {
    return type;
}

// Define the shape of each piece type in its various rotation states

void Piece::setShape() {
    cells.clear();
    switch (type) {
        case PieceType::I:
            if (rotationState % 2 == 0)
                cells = {{0,0},{1,0},{2,0},{3,0}};
            else
                cells = {{0,0},{0,1},{0,2},{0,3}};
            break;
        case PieceType::O:
            cells = {{0,0},{1,0},{0,1},{1,1}};
            break;
        case PieceType::T:
            if (rotationState == 0)      cells = {{1,0},{0,1},{1,1},{2,1}};
            else if (rotationState == 1) cells = {{1,0},{1,1},{2,1},{1,2}};
            else if (rotationState == 2) cells = {{0,1},{1,1},{2,1},{1,2}};
            else                         cells = {{1,0},{0,1},{1,1},{1,2}};
            break;
        case PieceType::S:
            if (rotationState % 2 == 0)  cells = {{1,0},{2,0},{0,1},{1,1}};
            else                         cells = {{0,0},{0,1},{1,1},{1,2}};
            break;
        case PieceType::Z:
            if (rotationState % 2 == 0)  cells = {{0,0},{1,0},{1,1},{2,1}};
            else                         cells = {{1,0},{0,1},{1,1},{0,2}};
            break;
        case PieceType::J:
            if (rotationState == 0)      cells = {{0,0},{0,1},{1,1},{2,1}};
            else if (rotationState == 1) cells = {{1,0},{2,0},{1,1},{1,2}};
            else if (rotationState == 2) cells = {{0,1},{1,1},{2,1},{2,2}};
            else                         cells = {{1,0},{1,1},{0,2},{1,2}};
            break;
        case PieceType::L:
            if (rotationState == 0)      cells = {{2,0},{0,1},{1,1},{2,1}};
            else if (rotationState == 1) cells = {{1,0},{1,1},{1,2},{2,2}};
            else if (rotationState == 2) cells = {{0,1},{1,1},{2,1},{0,2}};
            else                         cells = {{0,0},{1,0},{1,1},{1,2}};
            break;

    }
}
