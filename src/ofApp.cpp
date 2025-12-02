#include "ofApp.h"
using namespace std;

//--------------------------------------------------------------
void ofApp::setup(){
    
    background.load("images/aot_pixelated.png");

    music.load("sounds/tetris_theme.mp3");
    music.setLoop(true);
    music.setVolume(0.6f);
    music.play();
    
    muted = false;
    
    ofSetWindowTitle("Tetris");
    ofSetBackgroundColor(ofColor::black);
    ofSetFrameRate(60);
    
    cellSize = 36;
    boardOffsetX = 50;
    boardOffsetY = 50;
    dropInterval = 0.8f;
    dropTimer = 0;
    score = 0;
    gameOver = false;
    currentPiece = nullptr;
    nextPiece = nullptr;
    level = 1;
    totalLinesCleared = 0;
    baseDropInterval = 0.8f;

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
    //gameOver:
    gameOverImage.load("images/GameOver.png");
    //gameStart:
    gameStartImage.load("images/TetrisGameStartState.png");

    isStartScreen = true;
    //textured pieces png
    textures[0].load("images/bricksaot.png");
    textures[1].load("images/rope.png");
    textures[2].load("images/mikasascarf.png");
    textures[3].load("images/map.png");

    for(int i = 0; i < 4; i++){
        textures[i].setImageType(OF_IMAGE_COLOR_ALPHA);
        textures[i].setCompression(OF_COMPRESS_NONE);
        textures[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    }
    cout << "0: " <<textures[0].isAllocated() << endl;
    cout << "1: " <<textures[1].isAllocated() << endl;
    cout << "2: " <<textures[2].isAllocated() << endl;
    cout << "3: " <<textures[3].isAllocated() << endl;


    updateMusicState();

    spawnNewPiece();
}

//--------------------------------------------------------------
void ofApp::update(){

 if (gameOver) return;

    // Tiempo (en segundos) desde el último frame
    float dt = ofGetLastFrameTime();

    // Por si dt viene raro (0 o negativo)
    if (dt <= 0) {
        dt = 1.0f / 60.0f;
    }

    // Acumulamos tiempo
    dropTimer += dt;

    // Cuando el tiempo acumulado supera el intervalo, bajamos la pieza
    if (dropTimer >= dropInterval) {
        dropPiece();
        // Restamos el intervalo en vez de poner 0, por si el frame fue pesado
        dropTimer -= dropInterval;
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    //gameStart screen
    if(isStartScreen){
        ofSetColor(255);
         gameStartImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        return;

    }
    
    
   // En vez de toda la ventana:
int boardWidthPixels  = board.getWidth()  * cellSize;
int boardHeightPixels = board.getHeight() * cellSize;

background.draw(boardOffsetX, boardOffsetY,
                boardWidthPixels, boardHeightPixels);
   
    // Draw board
    board.draw(cellSize, boardOffsetX, boardOffsetY, useTextures, textures);
    ofPushStyle();
    drawGhostPiece();
    ofPopStyle();

    // Draw current block 
    if (currentPiece) {
        ofPushStyle();
        auto cells = currentPiece->getCells(currentPiece->posX, currentPiece->posY);
        
        for (const auto& cell : cells) {
            int x = static_cast<int>(cell.x);
            int y = static_cast<int>(cell.y);
            if (x >= 0 && x < board.getWidth() && y >= 0 && y < board.getHeight()) {
               if(useTextures){
                ofSetColor(255);
                textures[currentPiece->getcurrentTexture()].draw(
                    boardOffsetX + x * cellSize + 1,
                    boardOffsetY + y * cellSize + 1,
                    cellSize - 2,
                    cellSize - 2
                );
               }else{
                ofSetColor(currentPiece->getColor());
                ofDrawRectangle(
                     boardOffsetX + x * cellSize + 1,
                    boardOffsetY + y * cellSize + 1,
                    cellSize - 2,
                    cellSize - 2
                );
               }
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
    ofDrawBitmapString("A: Left", sideX, controlsY + 40);
    ofDrawBitmapString("D: Right", sideX, controlsY + 60);
    ofDrawBitmapString("R: Rotate", sideX, controlsY + 80);
    ofDrawBitmapString("S: Soft Drop", sideX, controlsY + 100);
    ofDrawBitmapString("N: New Game", sideX, controlsY + 120);
    ofDrawBitmapString("M: Mute / Unmute", sideX, controlsY + 140);
    ofDrawBitmapString("Q: Hard Drop", sideX, controlsY + 160);
    ofDrawBitmapString("T: Textured Piece", sideX, controlsY + 180);
    ofDrawBitmapString("O: Original Piece", sideX, controlsY + 200);
    
    //phase2:
    if (gameOver) {

        ofSetColor(255);
        gameOverImage.draw(
            boardOffsetX + (board.getWidth() * cellSize - gameOverImage.getWidth() * 0.3f) / 2,
            boardOffsetY + (board.getHeight() * cellSize - gameOverImage.getHeight() * 0.3f) / 2,
         gameOverImage.getWidth() * 0.3f, gameOverImage.getHeight() * 0.3f  
        );
        
        return;
        
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //game start
    if(isStartScreen && (key == ' ')){
        isStartScreen = false;
        resetGame();
        return;
    }
     if (key == 'm' || key == 'M') {
       muted = !muted;       
    updateMusicState();    
}
//bonus logic for textured pieces
    if(key == 't' || key == 'T'){
        useTextures = true;
        return;
    }
    if(key == 'o' || key == 'O'){
        useTextures = false;
        return;
    }

   
    // Allow new game even when gameOver
    if (gameOver) {
        if (key == 'n' || key == 'N') { resetGame(); }
        return;
    }
    
    switch (key) {
        case OF_KEY_LEFT:
        case 'a':
        case 'A':
            movePiece(-1, 0);
            break;
        case OF_KEY_RIGHT:
        case 'd':
        case 'D':
            movePiece(1, 0);
            break;
        case 'w':
        case 'W':    
        case 'r':
        case 'R':
        case OF_KEY_UP:
            rotatePiece();
           
            break;
        case 'q':
        case 'Q':
        hardDrop();
       
        break;
        
        case OF_KEY_DOWN:
        case 's':
        case 'S':
            dropPiece();
            break;
        case 'n':
        case 'N':
            resetGame();
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
    //phase2 : isGameover()
    if(board.isGameOver()){
        gameOver = true;
    }


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
if (!currentPiece || gameOver) return;

    // Bajar 
    int rows = descendHardDrop();

    
    if (rows <= 0) {
        lockPiece();
        return;
    }

    // Bonus por hard drop: 10 puntos por fila * nivel
    pendingHardDropBonus = rows * 10 * level;

    // Fijamos la pieza en el board
    lockPiece();
}

int ofApp::descendHardDrop() {
    if (!currentPiece) return 0;

    int rows = 0;
    // Mientras se pueda mover hacia abajo, bajamos
    while (canMovePiece(0, 1)) {
        movePiece(0, 1);
        rows++;
    }
    return rows;
}
//--------------------------------------------------------------
// Phase 3: Implement ghost piece functionality
void ofApp::drawGhostPiece() {
 // Si no hay pieza o el juego terminó, no dibujamos nada
    if (!currentPiece || gameOver) return;

    // Guardamos la posición original de la pieza
    int origX = currentPiece->posX;
    int origY = currentPiece->posY;

    // "Dejamos caer" la pieza virtualmente hasta donde pueda
    while (true) {
        currentPiece->posY++;  // intentamos bajar una fila

        if (!board.canPlacePiece(*currentPiece)) {
            // Si ya no se puede colocar, nos pasamos una fila
            currentPiece->posY--;  // retrocedemos una
            break;
        }
    }

    // Ahora la pieza está en la posición de aterrizaje (ghost position)
    auto cells = currentPiece->getCells(currentPiece->posX, currentPiece->posY);

    // Dibujamos el ghost: contorno transparente
    ofSetColor(255, 255, 255, 80); // blanco con alpha
    ofNoFill();
    for (const auto& cell : cells) {
        int x = static_cast<int>(cell.x);
        int y = static_cast<int>(cell.y);

        if (x >= 0 && x < board.getWidth() && y >= 0 && y < board.getHeight()) {
            ofDrawRectangle(
                boardOffsetX + x * cellSize + 1,
                boardOffsetY + y * cellSize + 1,
                cellSize - 2,
                cellSize - 2
            );
        }
    }

    // Restauramos la posición original de la pieza para no afectar el juego
    currentPiece->posX = origX;
    currentPiece->posY = origY;
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
        if(!currentPiece) return;
        if(!prevPiece){
            prevPiece = currentPiece;
            currentPiece = nextPiece;
            nextPiece = new Piece(getRandomPieceType());
        }
        else{
            Piece* temp = currentPiece;
            currentPiece = prevPiece;
            prevPiece = temp;
        }
}

//--------------------------------------------------------------
PieceType ofApp::getRandomPieceType() {
    // Generate random number between 0 and 6
    int random = ofRandom(8);
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
      if (muted) {
        // Música en mute
        music.setPaused(true);
    } else {
        // Música sonando
        music.setPaused(false);
        if (!music.isPlaying() && !gameOver) {
            music.play();
        }
    }
}
//--------------------------------------------------------------

// Phase 2: Adjust gravity speed based on score progression
void ofApp::recalcSpeed(int linesClearedJustNow) {
if (linesClearedJustNow <= 0) return;

    // Acumular líneas totales
    totalLinesCleared += linesClearedJustNow;

    // Subir de nivel cada 10 líneas, por ejemplo
    int newLevel = 1 + totalLinesCleared / 8;

    if (newLevel != level) {
        level = newLevel;

        // Reducir el intervalo de caída a medida que sube el nivel
        float minInterval = 0.18f;      // límite mínimo
        float step = 0.04f;            // cuánto se reduce por nivel

        dropInterval = baseDropInterval - (level - 1) * step;
        if (dropInterval < minInterval) {
            dropInterval = minInterval;
        }
    }
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

    spawnNewPiece();

    // Muy importante: respetar si estaba muteado
    updateMusicState();
}
