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
    
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::Uint8> pixelBuffer;


    int getIndex(int x, int y){
        return y*WIDTH+x;
    }

    inline bool isVaild(int x, int y) const {
        return (static_cast<unsigned int>(x) < static_cast<unsigned int>(WIDTH) && 
            static_cast<unsigned int>(y) < static_cast<unsigned int>(HEIGHT));
    }

    typedef void (SandBoxEngine::*ElementUpdateFunc)(int x, int y, int index, int color);
    ElementUpdateFunc updateFunctions[9];

    // Более оптимизированый рандом
    uint32_t fastRandState = 123456789;
    inline uint32_t fastRand(){
        fastRandState ^= (fastRandState << 13);
        fastRandState ^= (fastRandState >> 17);
        fastRandState ^= (fastRandState << 5);
        return fastRandState;
    }

    uint32_t getCellColor(int type, int colorBit = 0) {
        switch (type) {
            case AIR:    return 0xff141414;
            case SAND:   return (colorBit == 0) ? 0xff55C6EB : 0xff2D87B9; 
            case WATER:  return (colorBit == 0) ? 0xffAFA028 : 0xffFFFF7F; 
            case STONE:  return (colorBit == 0) ? 0xff504B46 : 0xff78736E; 
            case ACID_L: 
            case ACID_R: return (colorBit == 0) ? 0xff09FE8F : 0xff058C2D; 
            case FIRE:   return (colorBit == 0) ? 0xff0045FF : 0xff008CFF;
            case SMOKE:  return (colorBit == 0) ? 0xff505050 : 0xff787878; 
            case DIRT:   return (colorBit == 0) ? 0xff2D4B78 : 0xff1E3255; 
        }
        return 0xff000000; 
    }


    void activeNeighbors(int x, int y){
        int dx[] = { 0, 1, 1, -1, 0, 1};
        int dy[] = { -1, 0, 0, 1, 1, 1};

        for(int i = 0; i < 8; ++i){
            int tx = x+dx[i], ty = y+dy[i];

            if(isVaild(tx, ty)){
                activeGrid[getIndex(tx, ty)] = true;
            }
        }
    }

    void updatePixelVertices(int index, uint32_t cellColor) {
        uint32_t* pixelPtr = reinterpret_cast<uint32_t*>(&pixelBuffer[index * 4]);
        *pixelPtr = cellColor;
    }

    inline void moveElement(int x, int y, int to, int type, int oldColor, int newColor){
        int from = getIndex(x, y);

        grid[from] = AIR;
        grid[to] = type;
        colors[to] = oldColor;
        colors[from] = newColor;

        updatePixelVertices(from, getCellColor(AIR, newColor));
        updatePixelVertices(to, getCellColor(type, oldColor));

        activeNeighbors(x, y);

        activeGrid[to] = true; 
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
            updatePixelVertices(currentIndex, getCellColor(grid[currentIndex], fastRand() & 1));
            updatePixelVertices(downIdx, getCellColor(grid[downIdx], fastRand() & 1));
            activeNeighbors(x, y);
            activeNeighbors(x, y+1);

            return true;
        }
        return false;
    }

    void updateSand(int x, int y, int currentIndex, int color) {
        bool waterNearby = false;
        int targetWaterIdx = -1;

        for (int dy = -5; dy <= 5; ++dy) {
            int targetY = y + dy;
            if (targetY < 0 || targetY >= HEIGHT) continue;

            int rowOffset = targetY * WIDTH;

            for (int dx = -5; dx <= 5; ++dx) {
                int targetX = x + dx;
                if (targetX < 0 || targetX >= WIDTH) continue;

                int checkIdx = rowOffset + targetX;

                if (grid[checkIdx] == WATER) {
                    waterNearby = true;
                    targetWaterIdx = checkIdx;
                    break; 
                }
            }
            if (waterNearby) break;
        }

        if (waterNearby) {
            if((fastRand() % 100) < 1){
                grid[currentIndex] = DIRT;
                updatePixelVertices(currentIndex, getCellColor(DIRT, colors[currentIndex]));

                activeNeighbors(x, y);
                activeGrid[currentIndex] = false; 
                return; 
            }
        }
        int downIdx = getIndex(x, y + 1);

        if (AcidAtTheBottom(currentIndex, x, y, color)) return;
        if (WaterAtTheBottom(currentIndex, x, y)) return;

        if (isVaild(x, y + 1) && grid[downIdx] == AIR) {
            moveElement(x, y, downIdx, SAND, colors[currentIndex], color);
            return;
        }

        if (isVaild(x, y + 1)) {
            activeGrid[downIdx] = true;
        }

        int sideDir = (fastRand() & 1) ? -1 : 1;
        int diagL = getIndex(x + sideDir, y + 1);
        int diagR = getIndex(x - sideDir, y + 1);

        if (isVaild(x + sideDir, y + 1) && grid[diagL] == AIR) {
            moveElement(x, y, diagL, SAND, colors[currentIndex], color);
            return;
        }
        if (isVaild(x - sideDir, y + 1) && grid[diagR] == AIR) {
            moveElement(x, y, diagR, SAND, colors[currentIndex], color);
            return;
        }

        activeGrid[currentIndex] = false;
    }


    void updateWater(int x, int y, int currentIndex, int color) {
        if (AcidAtTheBottom(currentIndex, x, y, color)) return;

        int downIdx = getIndex(x, y + 1);
        if (isVaild(x, y + 1) && grid[downIdx] == AIR) {
            moveElement(x, y, downIdx, WATER, colors[currentIndex], color);
            return;
        }

        int sideDir = (fastRand() & 1) ? -1 : 1;
        int diagL = x + sideDir;
        int diagR = x - sideDir;

        if (isVaild(diagL, y + 1) && grid[getIndex(diagL, y + 1)] == AIR) {
            moveElement(x, y, getIndex(diagL, y + 1), WATER, colors[currentIndex], color);
            return;
        }
        if (isVaild(diagR, y + 1) && grid[getIndex(diagR, y + 1)] == AIR) {
            moveElement(x, y, getIndex(diagR, y + 1), WATER, colors[currentIndex], color);
            return;
        }

        int targetX = x;
        int maxSpread = 25;

        for (int i = 1; i <= maxSpread; ++i) {
            int nx = x + (i * sideDir);
            if (!isVaild(nx, y)) break;

            int nextIdx = getIndex(nx, y);
        
            if (grid[nextIdx] == AIR) {
                targetX = nx;
            
                if (isVaild(nx, y + 1) && grid[getIndex(nx, y + 1)] == AIR) {
                    break;
                }
            } else {
                break; 
            }
        }

        if (targetX != x) {
            moveElement(x, y, getIndex(targetX, y), WATER, colors[currentIndex], color);
            return;
        }

        int altX = x - sideDir;
        if (isVaild(altX, y) && grid[getIndex(altX, y)] == AIR) {
            moveElement(x, y, getIndex(altX, y), WATER, colors[currentIndex], color);
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

        if (isVaild(x, y + 1)) {
            activeGrid[downIdx] = true;
        }

        int sideDir = (fastRand() & 1) ? -1 : 1;
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
                activeNeighbors(x, y+1);
                return; 
            } else if (grid[downIndex] != ACID_L && grid[downIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[downIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(downIndex, getCellColor(AIR));
                activeNeighbors(x, y+1);
                activeNeighbors(x, y);
                return;
            }
        }

        int dir = (fastRand() & 1) ? -1 : 1;
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
                activeNeighbors(sideX, y);
                return;
            } else if (grid[sideIndex] != ACID_L && grid[sideIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[sideIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(sideIndex, getCellColor(AIR));
                activeNeighbors(sideX, y);
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
                activeNeighbors(altX, y);
                return;
            } else if (grid[altIndex] != ACID_L && grid[altIndex] != ACID_R) {
                grid[currentIndex] = AIR; 
                grid[altIndex] = AIR;
                updatePixelVertices(currentIndex, getCellColor(AIR));
                updatePixelVertices(altIndex, getCellColor(AIR));
                activeNeighbors(altX, y);
                return;
            }
        }
    }

    void updateFire(int x, int y, int currentIndex, int color){
        if(fastRand()%100 < 5){
            if (fastRand() % 2 == 0) {
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
        if(fastRand() % 5 == 0){
            int dir = fastRand() % 8;

            for (int i = 0; i < 8; ++i) {
                int currentDir = (dir + i) % 8;
                int targetX = x + dx[currentDir];
                int targetY = y + dy[currentDir];

                if (isVaild(targetX, targetY)) {
                    int targetIndex = getIndex(targetX, targetY);

                    if (grid[targetIndex] == DIRT) {
                        grid[targetIndex] = FIRE;
                        colors[targetIndex] = fastRand() % 2;
                        updatePixelVertices(targetIndex, getCellColor(FIRE, colors[targetIndex]));
                        activeNeighbors(x, y);
                        break;
                    }
                }
            }
        }
    }

    void updateSmoke(int x, int y, int currentIndex, int color){
        if (fastRand() % 100 < 1) { 
            grid[currentIndex] = AIR;
            colors[currentIndex] = 0;
            updatePixelVertices(currentIndex, getCellColor(AIR));
            return;
        }

        if (y == 0) {
            if (fastRand() % 100 < 15) {
                grid[currentIndex] = AIR;
                colors[currentIndex] = 0;
                updatePixelVertices(currentIndex, getCellColor(AIR));
            }    
            return;
        }

        if (std::rand() % 100 < 30) {
        
            int dirX = (fastRand() % 3) - 1;
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
                int sideX = x + ((fastRand() & 1) ? -1 : 1);
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

        texture.create(WIDTH,HEIGHT);
        sprite.setTexture(texture);
        sprite.setScale(CELL_SIZE, CELL_SIZE);

        pixelBuffer.assign(WIDTH*HEIGHT*4, 255);

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

                colors[index] = fastRand() & 1;
                updatePixelVertices(index, getCellColor(AIR, colors[index]));
            }
        }
    }

    void addBlock(int mouseX, int mouseY, int brushSize, unsigned int BlockId) {
        for (int dy = -brushSize - 1; dy <= brushSize + 1; ++dy) {
            for (int dx = -brushSize - 1; dx <= brushSize + 1; ++dx) {
                int nx = mouseX + dx;
                int ny = mouseY + dy;
        
                if (!isVaild(nx, ny)) continue;
                int index = getIndex(nx, ny);

                bool isInsideBrush = (dy >= -brushSize && dy <= brushSize && dx >= -brushSize && dx <= brushSize);

                if (isInsideBrush) {
                    if (BlockId == FIRE) {
                        if (grid[index] == DIRT) {
                            grid[index] = FIRE;
                            activeGrid[index] = true;
                        }
                    }
                    else if (BlockId == AIR || grid[index] == AIR) {
                        grid[index] = BlockId;
                        colors[index] = fastRand() & 1;
                        updatePixelVertices(index, getCellColor(BlockId, colors[index]));
                        activeGrid[index] = true;

                        if (isVaild(nx, ny + 1)) {
                            activeGrid[getIndex(nx, ny + 1)] = true;
                        }
                    }
                } else {
                    activeGrid[index] = true; 
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
        bool leftToRight = (fastRand() & 1);

        for(int y = HEIGHT - 1; y >= 0; --y){
            int rowOffset = y * WIDTH;

            for(int step = 0; step < WIDTH; ++step){
                int x = leftToRight ? step : (WIDTH - 1 - step);
                int currentIndex = rowOffset + x;

                if (grid[currentIndex] == SAND && !activeGrid[currentIndex]) {
                    int dx[] = {0, 0, -1, 1};
                    int dy[] = {-1, 1, 0, 0};
                    for (int i = 0; i < 4; ++i) {
                        int tx = x + dx[i];
                        int ty = y + dy[i];
                        if (isVaild(tx, ty)) {
                            if (grid[ty * WIDTH + tx] == WATER) {
                                activeGrid[currentIndex] = true;
                                break;
                            }
                        }
                    }
                }

                if(!activeGrid[currentIndex]) continue;

                int type = grid[currentIndex];
                if (updateFunctions[type] != nullptr) {
                    int color = (fastRand() & 1);
                    (this->*updateFunctions[type])(x, y, currentIndex, color);
                }
            }
        }
    }


    void draw(sf::RenderWindow& win){
        texture.update(pixelBuffer.data());
        win.draw(sprite);
    }
};
