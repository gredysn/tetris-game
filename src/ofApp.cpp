#include "ofApp.h"
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("Tetris");
    ofSetBackgroundColor(ofColor::black);
    ofSetFrameRate(60);
    
    cellSize = 30;
    boardOffsetX = 50;
    boardOffsetY = 50;
    dropInterval = 0.5f;
    dropTimer = 0;
    score = 0;
    gameOver = false;
    currentPiece = nullptr;
    nextPiece = nullptr;
    level = 1;
    totalLinesCleared = 0;
    baseDropInterval = 0.5f;

    // Audio setup 
    // # Phase 1: Load music file from "data/sounds/" directory
    rotateSound.load("sounds/rotateBlock.mp3");
    rotateSound.setMultiPlay(true);
    rotateSound.setVolume(0.8f);

    lineClearSound.load("sounds/lineClear.mp3");
    lineClearSound.setMultiPlay(true);
    lineClearSound.setVolume(0.9f);

    loseSound.load("sounds/lose.mp3");
    loseSound.setMultiPlay(false);
    loseSound.setLoop(false);
    loseSound.setVolume(1.0f);

    spawnNewPiece();
}

//--------------------------------------------------------------
void ofApp::update(){

    dropTimer += ofGetLastFrameTime(); // Accumulate elapsed frame time

    // Move piece down when interval reached
    if (dropTimer >= dropInterval) {
        dropPiece();
        dropTimer = 0;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw board
    board.draw(cellSize, boardOffsetX, boardOffsetY);
    ofPushStyle();
    drawGhostPiece();
    ofPopStyle();

    // Draw current block 
    if (currentPiece) {
        ofPushStyle();
        auto cells = currentPiece->getCells(currentPiece->posX, currentPiece->posY);
        ofSetColor(currentPiece->getColor());
        for (const auto& cell : cells) {
            int x = static_cast<int>(cell.x);
            int y = static_cast<int>(cell.y);
            if (x >= 0 && x < board.getWidth() && y >= 0 && y < board.getHeight()) {
                ofDrawRectangle(boardOffsetX + x * cellSize + 1,
                                boardOffsetY + y * cellSize + 1,
                                cellSize - 2, cellSize - 2);
            }
        }
        ofPopStyle();
    }
    
    // Draw UI / Sidebar
    ofSetColor(255);
    int sideX = boardOffsetX + board.getWidth() * cellSize + 20;
    ofPushMatrix();
    ofTranslate(sideX, boardOffsetY + 20);
    ofScale(1.6f,1.6f);
    ofDrawBitmapString("SCORE", 0, 0);
    ofDrawBitmapString(ofToString(score), 0, 20);
    ofPopMatrix();


    // Next piece preview box
    int previewY = boardOffsetY + 130;
    ofDrawBitmapString("Next:", sideX, previewY);
    ofNoFill();
    ofSetColor(200); 
    int previewCells = 5; // 5x5 grid
    int previewBox = cellSize * previewCells;
    ofDrawRectangle(sideX, previewY + 10, previewBox, previewBox);
    ofFill();

    if (nextPiece) {
        auto cells = nextPiece->getCells(0,0);
        ofSetColor(nextPiece->getColor());
        int minX=999, maxX=-999, minY=999, maxY=-999;
        for (auto &c: cells){
            if(c.x<minX)minX=c.x; if(c.x>maxX)maxX=c.x;
            if(c.y<minY)minY=c.y; if(c.y>maxY)maxY=c.y;
        }
        int pieceW = (maxX - minX + 1);
        int pieceH = (maxY - minY + 1);
        // Float centering inside previewCells grid
        float offsetXCells = (previewCells - pieceW) * 0.5f - minX;
        float offsetYCells = (previewCells - pieceH) * 0.5f - minY;
        for(auto &c: cells){
            float rx = c.x + offsetXCells;
            float ry = c.y + offsetYCells;
            ofDrawRectangle(sideX + rx*cellSize + 1, previewY + 10 + ry*cellSize + 1, cellSize-2, cellSize-2);
        }
    }

    // Previous piece preview box
    drawPreviousPiece();

    // Controls
    int controlsY = previewY + previewBox + 30;
    ofSetColor(255);
    ofDrawBitmapString("Controls:", sideX, controlsY);
    ofDrawBitmapString("< >: Move", sideX, controlsY + 20);
    ofDrawBitmapString("R: Rotate", sideX, controlsY + 40);
    ofDrawBitmapString("V: Soft Drop", sideX, controlsY + 60);
    ofDrawBitmapString("N: New Game", sideX, controlsY + 80);
    
    if (gameOver) {

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // Allow new game even when gameOver
    if (gameOver) {
        if (key == 'n' || key == 'N') { resetGame(); }
        return;
    }
    
    switch (key) {
        case OF_KEY_LEFT:
            movePiece(-1, 0);
            break;
        case OF_KEY_RIGHT:
            movePiece(1, 0);
            break;
        case 'r':
        case 'R':
        case OF_KEY_UP:
            rotatePiece();
            break;
        case OF_KEY_DOWN:
            dropPiece();
            break;
        case 'n':
        case 'N':
            break;
        case 'p':
        case 'P':
            switchToPrevPiece();
            break;

        // Debug key to add score
        case 'l':
        case 'L':
            score+=1000; 
            break;
    }
}

//--------------------------------------------------------------
void ofApp::spawnNewPiece() {
    // Ensure there is a queued next piece
    allocateNextIfNeeded();
    delete currentPiece;
    currentPiece = nextPiece;
    nextPiece = new Piece(getRandomPieceType());
    currentPiece->posX = board.getWidth() / 2 - 1;
    currentPiece->posY = 0;

}

//--------------------------------------------------------------
bool ofApp::canMovePiece(int dx, int dy) {
    if (!currentPiece) return false;

    currentPiece->posX += dx;
    currentPiece->posY += dy;

    bool canMove = board.canPlacePiece(*currentPiece);

    currentPiece->posX -= dx;
    currentPiece->posY -= dy;

    return canMove;
}

//--------------------------------------------------------------
void ofApp::movePiece(int dx, int dy) {
    if (canMovePiece(dx, dy)) {
        currentPiece->posX += dx;
        currentPiece->posY += dy;
    }
}

//--------------------------------------------------------------
void ofApp::rotatePiece() {
    if (!currentPiece) return;

    currentPiece->rotate();
    if (!board.canPlacePiece(*currentPiece)) {
        // Invalid rotation -> rotate back 3 times (reverse 1 step)
        currentPiece->rotate();
        currentPiece->rotate();
        currentPiece->rotate();
    }
    else {
        if (!muted) rotateSound.play();
    }
}

//--------------------------------------------------------------
void ofApp::dropPiece() {
    if (canMovePiece(0, 1)) {
        movePiece(0, 1);
    } else {
        lockPiece(); // Cannot descend further: settle piece
    }
}

// # Phase 2: Implement hard drop functionality
void ofApp::hardDrop() {
    
}

int ofApp::descendHardDrop() {
    return 0;
}
//--------------------------------------------------------------
// Phase 3: Implement ghost piece functionality
void ofApp::drawGhostPiece() {

}

//--------------------------------------------------------------
void ofApp::drawPreviousPiece() {
    if (!prevPiece) return;
    ofPushStyle();

    // =========================================================================
    // This will only draw if prevPiece is implemented correctly
    //
    // YOU DONT NEED TO DO ANYTHING HERE
    // =========================================================================

    // Position and Color
    int sideX = boardOffsetX + board.getWidth() * cellSize + 200;
    int previewY = boardOffsetY + 130; // Position below the next piece preview
    ofSetColor(255);
    ofDrawBitmapString("Previous:", sideX, previewY);
    ofNoFill();
    ofSetColor(200);

    int previewCells = 5;
    int previewBox = cellSize * previewCells;
    ofDrawRectangle(sideX, previewY + 10, previewBox, previewBox);
    ofFill();
    auto cells = prevPiece->getCells(0,0);
    ofSetColor(prevPiece->getColor());
    int minX=999, maxX=-999, minY=999, maxY=-999;
    for (auto &c: cells){
        if(c.x<minX)minX=c.x; if(c.x>maxX)maxX=c.x;
        if(c.y<minY)minY=c.y; if(c.y>maxY)maxY=c.y;
    }
    int pieceW = (maxX - minX + 1);
    int pieceH = (maxY - minY + 1);
    float offsetXCells = (previewCells - pieceW) * 0.5f - minX;
    float offsetYCells = (previewCells - pieceH) * 0.5f - minY;
    for(auto &c: cells){
        float rx = c.x + offsetXCells;
        float ry = c.y + offsetYCells;
        ofDrawRectangle(sideX + rx*cellSize + 1, previewY + 10 + ry*cellSize + 1, cellSize-2, cellSize-2);
    }
    ofPopStyle();
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::lockPiece() {
    if (!currentPiece) return;

    board.placePiece(*currentPiece);

    //Clear any full lines
    int linesCleared = board.clearLines();
    if (linesCleared > 0) {
        // Basic scoring: 100 * lines * level
        score += 100 * linesCleared * level;
        playLineClear(linesCleared);
        recalcSpeed(linesCleared);
    }

    if (pendingHardDropBonus > 0 && linesCleared > 0) {
        score += pendingHardDropBonus;
    }

    // Clear pending bonus regardless so it doesn't carry over.
    pendingHardDropBonus = 0;

    dropTimer = 0.0f;

    spawnNewPiece();
}

//--------------------------------------------------------------

// Phase 3: Implement previous piece switching
void ofApp::switchToPrevPiece() {

}

//--------------------------------------------------------------
PieceType ofApp::getRandomPieceType() {
    // Generate random number between 0 and 6
    int random = ofRandom(7);
    // Map random number to a PieceType
    return static_cast<PieceType>(random);
}

void ofApp::playLineClear(int linesCleared) {
    if (muted) return;
    if (linesCleared > 0) {
        lineClearSound.play();
    }
}

void ofApp::updateMusicState() {
    float vol = muted ? 0.0f : 0.6f;
    musicPlayer.setVolume(vol);

    // muted ? 0.0f : 0.8f => if muted, silence sound; else normal volume
    rotateSound.setVolume(muted ? 0.0f : 0.8f);
    lineClearSound.setVolume(muted ? 0.0f : 0.9f);
    if (!muted && !musicPlayer.isPlaying() && !gameOver) musicPlayer.play();
}
//--------------------------------------------------------------

// Phase 2: Adjust gravity speed based on score progression
void ofApp::recalcSpeed(int linesClearedJustNow) {

}

//--------------------------------------------------------------
void ofApp::allocateNextIfNeeded() {
    if (!nextPiece) {
        nextPiece = new Piece(getRandomPieceType());
    }
}

//--------------------------------------------------------------
void ofApp::resetGame() {
    // Clear board & stats
    board.reset();
    score = 0;
    level = 1;
    totalLinesCleared = 0;
    dropInterval = baseDropInterval;
    dropTimer = 0.0f;
    gameOver = false;

    // Clean up existing pieces
    delete currentPiece; currentPiece = nullptr;
    delete nextPiece;    nextPiece = nullptr;
    delete prevPiece;    prevPiece = nullptr;

    if (!muted) {
        if (musicPlayer.isPlaying()) musicPlayer.stop();
        musicPlayer.play();
        musicPlayer.setVolume(0.6f);
    }

    spawnNewPiece();
}