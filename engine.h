#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

const int WIDTH = 300;
const int HEIGHT = 225;
const int CELL_SIZE = 3;

enum Element{
    AIR    = 0,        
    SAND   = 1,  
    WATER  = 2,   
    STONE  = 3,   
    DIRT   = 4, 
    FIRE   = 5,  
    ACID_L = 6,  
    ACID_R = 7,  
    SMOKE  = 8,   
};


class SandBoxEngine{
private:
    std::vector<unsigned short> grid;
    std::vector<unsigned short> colors;
    std::vector<bool> activeGrid;
    sf::VertexArray pixels;

    int getIndex(int x, int y){
        return y*WIDTH+x;
    }

    bool isVaild(int x,int y) const{
        return (x>=0 && x <WIDTH && y >= 0 && y < HEIGHT);
    }

    typedef void (SandBoxEngine::*ElementUpdateFunc)(int x, int y, int index, int color);
    ElementUpdateFunc updateFunctions[9];

    sf::Color getCellColor(int type, int colorBit = 0){
        switch (type){
            case AIR: return sf::Color(20, 20, 20); break;
            case SAND: return (colorBit == 0) ? sf::Color(235,190,85) : sf::Color(185, 135, 45); break;
            case WATER: return (colorBit == 0) ? sf::Color(40, 160, 175) : sf::Color(127, 255, 212); break;
            case STONE: return (colorBit == 0) ? sf::Color(70, 75, 80) : sf::Color(110, 115, 120); break;
            case ACID_L: case ACID_R: return (colorBit == 0) ? sf::Color(143, 254, 9) : sf::Color(45, 140, 5); break; 
            case FIRE: return (colorBit == 0) ? sf::Color(255, 69, 0) : sf::Color(255, 140, 0); break;
            case SMOKE: return (colorBit == 0) ? sf::Color(80, 80, 80) : sf::Color(120, 120, 120); break;
            case DIRT: return (colorBit == 0) ? sf::Color(120, 75, 45) : sf::Color(85, 50, 30); break;
        }
        return sf::Color::Black;
    }

    void activeNeighbors(int x, int y){
        int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

        for(int i = 0; i < 8; ++i){
            int tx = x+dx[i], ty = y+dy[i];

            if(isVaild(tx, ty)){
                activeGrid[getIndex(tx, ty)] = true;
            }
        }
    }

    void updatePixelVertices(int index, sf::Color cellColor) {
        int vIndex = index * 4;
        pixels[vIndex + 0].color = cellColor;
        pixels[vIndex + 1].color = cellColor;
        pixels[vIndex + 2].color = cellColor;
        pixels[vIndex + 3].color = cellColor;
    }

    inline void moveElement(int x, int y, int to, int type, int oldColor, int newColor){
        int from = getIndex(x, y);
        activeNeighbors(x, y);

        grid[from] = AIR;
        grid[to] = type;
        colors[to] = oldColor;
        colors[from] = newColor;

        updatePixelVertices(from, getCellColor(AIR, newColor));
        updatePixelVertices(to, getCellColor(type, oldColor));
    }

    bool AcidAtTheBottom(int currentIndex, int x, int y, int color){
        if(!isVaild(x,y+1)) return false;

        int downIdx = getIndex(x,y+1);

        if(isVaild(x,y) && (grid[downIdx] == ACID_L || grid[downIdx] == ACID_R)){
            grid[currentIndex] = AIR;
            grid[downIdx] = AIR;

            colors[currentIndex] = color;
            colors[downIdx] = 0;

            return true;
        }
        return false;
    }

    bool WaterAtTheBottom(int currentIndex, int x, int y){
        if(!isVaild(x, y+1)) return false;

        int downIdx = getIndex(x, y+1);

        if(grid[downIdx] == WATER){
            std::swap(grid[currentIndex], grid[downIdx]);
            std::swap(colors[currentIndex], colors[downIdx]);

            return true;
        }
        return false;
    }

    void updateSand(int x, int y, int currentIndex, int color){
        int downIdx = getIndex(x,y+1);

        if(AcidAtTheBottom(currentIndex, x, y, color)) return;
        if(WaterAtTheBottom(currentIndex, x, y)) return;

        if(isVaild(x,y+1) && grid[getIndex(x,y+1)] == AIR){
            moveElement(x, y, downIdx, SAND, colors[currentIndex], color);
            return;
        }

        int sideDir = (std::rand() % 2 == 0) ? -1 : 1;
        int diagL = getIndex(x - sideDir, y+1);
        int diagR = getIndex(x + sideDir, y+1);

        if(isVaild(x - sideDir, y+1) && grid[diagL] == AIR) {
            moveElement(x, y, diagL, SAND, colors[currentIndex], color);
            return;
        }
        if(isVaild(x + sideDir, y+1) && grid[diagR] == AIR){
            moveElement(x, y, diagR, SAND, colors[currentIndex], color);
            return;
        }

        //Появление земли
        int dx[] = {0,0,1,-1};
        int dy[] = {1,-1,0,0};

        for(int i =0; i < 4; ++i){
            int tx = x+dx[i];
            int ty = y+dy[i];

            if(isVaild(tx, ty)){
                int targeIdx = getIndex(tx, ty);

                if(grid[targeIdx] == WATER){
                    grid[currentIndex] = AIR;
                    grid[targeIdx] = DIRT;
                    return;
                }
            }
        }

        activeGrid[currentIndex] = false;
    }

    void updateWater(int x, int y, int currentIndex, int color){
        if(AcidAtTheBottom(currentIndex, x, y, color)) return;

        int downIdx = getIndex(x, y+1);
        if(isVaild(x, y+1) && grid[downIdx] == AIR){
            moveElement(x, y, downIdx, WATER, colors[currentIndex], color);
            return;
        }

        int sideDir = (std::rand() % 2 == 0) ? -1 : 1;

        int diagL = x+sideDir;
        int diagR = x - sideDir;

        if(isVaild(diagL, y+1) && grid[getIndex(diagL, y+1)] == AIR){
            moveElement(x, y, getIndex(diagL, y+1), WATER, colors[currentIndex], color);
            return;
        }
        if(isVaild(diagR, y+1) && grid[getIndex(diagR, y+1)] == AIR){
            moveElement(x, y, getIndex(diagR, y+1), WATER, colors[currentIndex], color);
            return;
        }

        int maxSteps = 15;
        int currentX = x;

        for(int i = 0; i < maxSteps; ++i){
            int nextX = currentX + sideDir;
            if(isVaild(nextX, y) && grid[getIndex(nextX, y)] == AIR){
                currentX = nextX;
            } else {
                break;
            }
        }

        if(currentX != x){
            moveElement(x, y, getIndex(currentX, y), WATER, colors[currentIndex], color);
            return;
        }

        activeGrid[currentIndex] = false;
    }

    void updateDirt(int x, int y, int currentIndex, int color){
        if(AcidAtTheBottom(currentIndex, x, y, color)) return;
        if(WaterAtTheBottom(currentIndex, x, y)) return;

        int downIdx = getIndex(x, y+1);
        if(isVaild(x, y+1) && grid[downIdx] == AIR){
            moveElement(x, y, downIdx, DIRT, colors[currentIndex], color);
            return;
        }

        int sideDir = (std::rand() % 2 == 0) ? -1 : 1;
        int diagL = x+sideDir;
        int diagR = x-sideDir;

        if(isVaild(diagL, y+1) && grid[getIndex(diagL, y+1)] == AIR){
            moveElement(x, y, getIndex(diagL, y+1), DIRT, colors[currentIndex], color);
            return;
        }
        if(isVaild(diagR, y+1) && grid[getIndex(diagR, y+1)] == AIR){
            moveElement(x, y, getIndex(diagR, y+1), DIRT, colors[currentIndex], color);
            return;
        }

        activeGrid[currentIndex] = false;
    }

    void updateAcid(int x, int y, int currentIndex, int color) {
        int currentType = grid[currentIndex]; 
        int downIndex = getIndex(x, y + 1);

        if (isVaild(x, y + 1)) {
            if (grid[downIndex] == AIR) {
                grid[currentIndex] = AIR; 
                grid[downIndex] = currentType;
                colors[downIndex] = colors[currentIndex]; 
                colors[currentIndex] = color;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(downIndex, getCellColor(currentType, color));
                return; 
            } else if (grid[downIndex] != ACID_L && grid[downIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[downIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(downIndex, getCellColor(AIR));
                return;
            }
        }

        int dir = (std::rand() % 2 == 0) ? -1 : 1;
        int sideX = x + dir;

        if (isVaild(sideX, y)) {
            int sideIndex = getIndex(sideX, y);
            if (grid[sideIndex] == AIR) {
                grid[currentIndex] = AIR;
                grid[sideIndex] = (dir == -1) ? ACID_L : ACID_R; 
                colors[sideIndex] = colors[currentIndex]; 
                colors[currentIndex] = color;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(sideIndex, getCellColor(currentType, color));
                return;
            } else if (grid[sideIndex] != ACID_L && grid[sideIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[sideIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(sideIndex, getCellColor(AIR));
                return;
            }
        }

        int altX = x - dir; 

        if (isVaild(altX, y)) {
            int altIndex = getIndex(altX, y);
            if (grid[altIndex] == AIR) {
                grid[currentIndex] = AIR;
                grid[altIndex] = (dir == 1) ? ACID_L : ACID_R; 
                colors[altIndex] = colors[currentIndex]; 
                colors[currentIndex] = color;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(altIndex, getCellColor(currentType, color));
                return;
            } else if (grid[altIndex] != ACID_L && grid[altIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[altIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(altIndex, getCellColor(AIR));
                return;
            }
        }

        activeGrid[currentIndex] = false;
    }

    void updateFire(int x, int y, int currentIndex, int color){
        if(std::rand()%100 < 5){
            if (std::rand() % 2 == 0) {
                grid[currentIndex] = SMOKE;
                activeNeighbors(x, y);
                updatePixelVertices(currentIndex, getCellColor(SMOKE, colors[currentIndex]));
            } else {
                grid[currentIndex] = AIR;
                activeNeighbors(x, y);
                updatePixelVertices(currentIndex, getCellColor(AIR));
            }
            return;
        }

        int dx[] = {-1,  0,  1, -1, 1, -1, 0, 1};
        int dy[] = {-1, -1, -1,  0, 0,  1, 1, 1};

        //Тушение огня
        bool isExtinguished = false;
        for (int i = 0; i < 8; ++i) {
            int targetX = x + dx[i];
            int targetY = y + dy[i];

            if (isVaild(targetX, targetY)) {
                int targetIndex = getIndex(targetX, targetY);
            
                if (grid[targetIndex] == WATER) { 
                    grid[currentIndex] = SMOKE;
                    grid[targetIndex] = AIR;
                    colors[currentIndex] = color;
                    colors[targetIndex] = 0;

                    isExtinguished = true;
                    updatePixelVertices(currentIndex, getCellColor(SMOKE, colors[currentIndex]));
                    updatePixelVertices(targetIndex, getCellColor(AIR));
                    activeNeighbors(x, y);
                    activeNeighbors(targetX, targetY);
                    return;
                }
            }
        }

        //Распростронение огня
        if(std::rand() % 5 == 0){
            int dir = std::rand() % 8;

            for (int i = 0; i < 8; ++i) {
                int currentDir = (dir + i) % 8;
                int targetX = x + dx[currentDir];
                int targetY = y + dy[currentDir];

                if (isVaild(targetX, targetY)) {
                    int targetIndex = getIndex(targetX, targetY);

                    if (grid[targetIndex] == DIRT) {
                        grid[targetIndex] = FIRE;
                        colors[targetIndex] = std::rand() % 2;
                        updatePixelVertices(targetIndex, getCellColor(FIRE, colors[targetIndex]));
                        activeNeighbors(x, y);
                        break;
                    }
                }
            }
        }
    }

    void updateSmoke(int x, int y, int currentIndex, int color){
        if (std::rand() % 100 < 1) { 
            grid[currentIndex] = AIR;
            colors[currentIndex] = 0;
            updatePixelVertices(currentIndex, getCellColor(AIR));
            return;
        }

        if (y == 0) {
            if (std::rand() % 100 < 15) {
                grid[currentIndex] = AIR;
                colors[currentIndex] = 0;
                updatePixelVertices(currentIndex, getCellColor(AIR));
            }    
            return;
        }

        if (std::rand() % 100 < 30) {
        
            int dirX = (std::rand() % 3) - 1;
            int tx = x + dirX;
            int ty = y - 1; 

            if (isVaild(tx, ty) && grid[getIndex(tx, ty)] == AIR) {
                int targetIndex = getIndex(tx, ty);
                moveElement(x, y, targetIndex, SMOKE, colors[currentIndex], color);
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(targetIndex, getCellColor(SMOKE, color));
                return;
            }
            else {
                int sideX = x + ((std::rand() % 2 == 0) ? -1 : 1);
                if (isVaild(sideX, y) && grid[getIndex(sideX, y)] == AIR) {
                    int targetIndex = getIndex(sideX, y);
                    moveElement(x, y, targetIndex, SMOKE, colors[currentIndex], color);
                    updatePixelVertices(currentIndex, getCellColor(AIR));
                    updatePixelVertices(targetIndex, getCellColor(SMOKE, color));
                    return;
                }
            }
        }
    }

public:
    SandBoxEngine(){
        grid.assign(WIDTH*HEIGHT, AIR);
        colors.assign(WIDTH*HEIGHT, 0);
        activeGrid.assign(WIDTH*HEIGHT, true);
        pixels.setPrimitiveType(sf::PrimitiveType::Quads);
        pixels.resize(WIDTH*HEIGHT*4);

        updateFunctions[AIR] = nullptr;
        updateFunctions[SAND] = &SandBoxEngine::updateSand;
        updateFunctions[WATER] = &SandBoxEngine::updateWater;
        updateFunctions[DIRT] = &SandBoxEngine::updateDirt;
        updateFunctions[STONE] = nullptr;
        updateFunctions[FIRE] = &SandBoxEngine::updateFire;
        updateFunctions[ACID_L] = &SandBoxEngine::updateAcid;
        updateFunctions[ACID_R] = &SandBoxEngine::updateAcid;
        updateFunctions[SMOKE] = &SandBoxEngine::updateSmoke;

        for(int y = 0; y < HEIGHT; ++y){
            for(int x = 0; x < WIDTH; ++x){
                int index = getIndex(x,y);
                int vIndex = 4*index;

                float left = x*CELL_SIZE;
                float top = y *CELL_SIZE;
                float rigth = left + CELL_SIZE;
                float bottom = top + CELL_SIZE;

                pixels[vIndex + 0].position = sf::Vector2f(left,top);
                pixels[vIndex + 1].position = sf::Vector2f(rigth, top);
                pixels[vIndex + 2].position = sf::Vector2f(rigth, bottom);
                pixels[vIndex + 3].position = sf::Vector2f(left, bottom);

                colors[index] = std::rand() % 2;

                updatePixelVertices(index, getCellColor(AIR, colors[index]));
            }
        }
    }

    void addBlock(int mouseX, int mouseY, int brushSize, unsigned int BlockId) {
        for (int dy = -brushSize; dy <= brushSize; ++dy) {
            for (int dx = -brushSize; dx <= brushSize; ++dx) {
                int nx = mouseX + dx;
                int ny = mouseY + dy;
            
                if (!isVaild(nx, ny)) continue;
                int index = getIndex(nx, ny);
                bool blockChanged = false;

                if (BlockId == FIRE) {
                    if (grid[index] == DIRT) {
                        grid[index] = FIRE;
                        blockChanged = true;
                    }
                }
                else if (BlockId == AIR || grid[index] == AIR) {
                    if (BlockId == ACID_L) {
                        grid[index] = (std::rand() % 2 == 0) ? ACID_L : ACID_R;
                        blockChanged = true;
                    } else {
                        grid[index] = BlockId;
                        colors[index] = std::rand() % 2;
                        blockChanged = true;

                        updatePixelVertices(index, getCellColor(BlockId, colors[index]));
                    }
                }
                if(blockChanged){
                    activeNeighbors(nx, ny);
                }
            }
        }
    }


    void FillGrid(int type){
        std::fill(grid.begin(), grid.end(), type);
        for(int i = 0; i < HEIGHT*WIDTH; ++i){
            updatePixelVertices(i, getCellColor(type, colors[i]));
        }
    }

    // Логика обновления блоков
    void update(){
        for(int y = HEIGHT - 1; y >= 0; --y){
            bool leftToRight = (std::rand() % 2 == 0); 

            for(int step = 0; step < WIDTH; ++step){

                int x = leftToRight ? step : (WIDTH - 1 - step);
                int currentIndex = getIndex(x,y);
                int color = std::rand() % 2;
                int type = grid[currentIndex];

                if(!activeGrid[getIndex(x,y)]) continue;
                if (updateFunctions[type] != nullptr) {
                    (this->*updateFunctions[type])(x, y, currentIndex, color);
                }
            }
        }
    }


    void draw(sf::RenderWindow& win){
        win.draw(pixels);
    }
};
