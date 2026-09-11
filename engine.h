#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

const int WIDTH = 300;
const int HEIGHT = 225;
const int CELL_SIZE = 3;

enum Element{
    AIR = 0,
    SAND = 1,
    WATER = 2,
    STONE = 3,
    DIRT = 4,
    FIRE = 5,
    ACID_L = 6,
    ACID_R = 7,
    SMOKE = 8,
};

class SandBoxEngine{
private:
    std::vector<int> grid;
    std::vector<int> colors;
    sf::VertexArray pixels;

    int getIndex(int x, int y){
        return y*WIDTH+x;
    }

    bool isVaild(int x,int y) const{
        return (x>=0 && x <WIDTH && y >= 0 && y < HEIGHT);
    }
    
public:
    SandBoxEngine(){
        grid.assign(WIDTH*HEIGHT, AIR);
        colors.assign(WIDTH*HEIGHT, AIR);
        pixels.setPrimitiveType(sf::PrimitiveType::Quads);
        pixels.resize(WIDTH*HEIGHT*4);

        for(int i = 0; i < WIDTH * HEIGHT; ++i) {
            colors[i] = std::rand() % 2;
        }   
    }

    void addBlock(int mouseX, int mouseY, int brushSize, int BlockId) {
    for (int dy = -brushSize; dy <= brushSize; ++dy) {
        for (int dx = -brushSize; dx <= brushSize; ++dx) {
            int nx = mouseX + dx;
            int ny = mouseY + dy;
            
            if (!isVaild(nx, ny)) continue;
            int index = getIndex(nx, ny);

            if (BlockId == FIRE) {
                if (grid[index] == DIRT) {
                    grid[index] = FIRE;
                }
            }
            else if (BlockId == AIR || grid[index] == AIR) {
                if (BlockId == ACID_L) {
                    grid[index] = (std::rand() % 2 == 0) ? ACID_L : ACID_R;
                } else {
                    grid[index] = BlockId;
                    colors[index] = std::rand() % 2;
                }
            }
        }
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

                //Песок
                if(grid[currentIndex] == SAND){
                    if(isVaild(x,y+1)&& (grid[getIndex(x,y+1)] == ACID_L || grid[getIndex(x,y+1)] == ACID_R)){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x,y+1)] = AIR;

                        colors[currentIndex] = color;
                        colors[getIndex(x,y+1)] = 0;
                    }

                    if(isVaild(x,y+1) && grid[getIndex(x,y+1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x,y+1)] = SAND; 

                        colors[getIndex(x,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x-1,y+1) && grid[getIndex(x-1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x-1,y+1)] = SAND;

                        colors[getIndex(x-1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x+1,y+1) && grid[getIndex(x+1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x+1,y+1)] = SAND;

                        colors[getIndex(x+1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }

                    //Песок + Вода = Земля
                    int sideDir = (rand()%2==0)? -1 : 1;
                    if(isVaild(x,y+1) && grid[getIndex(x,y+1)]==WATER){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x,y+1)] = DIRT;
                    }
                    else if(isVaild(x,y-1) && grid[getIndex(x,y-1)] == WATER){
                        grid[getIndex(x,y-1)] = AIR;
                        grid[currentIndex] = DIRT;
                    }
                    else if(isVaild(x+sideDir,y) && grid[getIndex(x+sideDir,y)] == WATER){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x+sideDir,y)] = DIRT;
                    }
                    else if(isVaild(x-sideDir,y) && grid[getIndex(x-sideDir,y)]==WATER){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x-sideDir,y)] = DIRT;
                    }
                }
                //Вода
                else if(grid[currentIndex] == WATER){
                    if(isVaild(x,y+1) && (grid[getIndex(x,y+1)] == ACID_L || grid[getIndex(x,y+1)] == ACID_R)){
                            grid[currentIndex] = AIR;
                            grid[getIndex(x, y + 1)] = AIR;

                            colors[currentIndex] = color;
                            colors[getIndex(x,y+1)] = 0;
                    }
                    
                    if(isVaild(x, y + 1) && grid[getIndex(x, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x, y + 1)] = WATER;

                        colors[getIndex(x,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x - 1, y + 1) && grid[getIndex(x - 1, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x - 1, y + 1)] = WATER;

                        colors[getIndex(x-1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x + 1, y + 1) && grid[getIndex(x + 1, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x + 1, y + 1)] = WATER;

                        colors[getIndex(x+1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else {
                        int sideDir = (rand() % 2 == 0) ? -1 : 1;
    
                        int maxSteps = 3; 
                        int currentX = x;

                        for (int i = 0; i < maxSteps; ++i) {
                            int nextX = currentX + sideDir;

                            if (isVaild(nextX, y) && grid[getIndex(nextX, y)] == AIR) {
                                currentX = nextX;
                            } else {
                                break;
                            }
                        }

                        if (currentX != x) {
                            grid[currentIndex] = AIR;
                            grid[getIndex(currentX, y)] = WATER;

                            // colors[getIndex(currentX,y)] = colors[currentIndex];
                            // colors[currentIndex] = color;
                        }
                    }
                }
                //Земля
                else if(grid[currentIndex]==DIRT){
                    if( isVaild(x,y+1) && (grid[getIndex(x,y+1)] == ACID_L || grid[getIndex(x,y+1)] == ACID_R)){
                            grid[currentIndex] = AIR;
                            grid[getIndex(x, y + 1)] = AIR;

                            colors[currentIndex] = color;
                            colors[getIndex(x,y+1)] = 0;
                    }

                    if(isVaild(x,y+1) && grid[getIndex(x, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x, y + 1)] = DIRT;

                        colors[getIndex(x,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x-1,y+1) && grid[getIndex(x-1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x-1,y+1)] = DIRT;

                        colors[getIndex(x-1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                    else if(isVaild(x+1,y+1) && grid[getIndex(x+1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x+1,y+1)] = DIRT;

                        colors[getIndex(x+1,y+1)] = colors[currentIndex];
                        colors[currentIndex] = color;
                    }
                }
                //Кислота
                else if (grid[currentIndex] == ACID_L || grid[currentIndex] == ACID_R) {
                    int currentType = grid[currentIndex]; 
                    int downIndex = getIndex(x, y + 1);

                    if (isVaild(x, y + 1)) {
                        if (grid[downIndex] == AIR) {
                            grid[currentIndex] = AIR;
                            grid[downIndex] = currentType;

                            colors[downIndex] = colors[currentIndex];
                            colors[currentIndex] = color;
                            continue;
                        }
                    else if (grid[downIndex] != ACID_L && grid[downIndex] != ACID_R) {
                        grid[currentIndex] = AIR;
                        grid[downIndex] = AIR;

                        colors[currentIndex] = color;
                        colors[downIndex] = color;
                        continue;
                    }
                    }

                    int dir = (std::rand() % 2 == 0) ? -1 : 1;
    
                    int sideX = x + dir;
                    int sideIndex = getIndex(sideX, y);

                    if (isVaild(sideX, y)) {
                        if (grid[sideIndex] == AIR) {
                            grid[currentIndex] = AIR;
                            grid[sideIndex] = (dir == -1) ? ACID_L : ACID_R; 
            
                            colors[sideIndex] = colors[currentIndex];
                            colors[currentIndex] = color;
                            continue;
                        }
                    else if (grid[sideIndex] != ACID_L && grid[sideIndex] != ACID_R) {
                        grid[currentIndex] = AIR;
                        grid[sideIndex] = AIR;

                        colors[currentIndex] = color;
                        colors[sideIndex] = color;
                        continue;
                    }

                    int altX = x - dir; // Изменение направления на противоположное
                    int altIndex = getIndex(altX, y);

                    if (isVaild(altX, y)) {
                        if (grid[altIndex] == AIR) {
                            grid[currentIndex] = AIR;
                            grid[altIndex] = (dir == 1) ? ACID_L : ACID_R; 
            
                            colors[altIndex] = colors[currentIndex];
                            colors[currentIndex] = color;
                            continue;
                        }
                        else if (grid[altIndex] != ACID_L && grid[altIndex] != ACID_R) {
                            grid[currentIndex] = AIR;
                            grid[altIndex] = AIR;

                            colors[currentIndex] = color;
                            colors[altIndex] = color;
                            continue;}
                        }
                    }
                }

                //Огонь
                else if(grid[currentIndex] == FIRE){
                    if(std::rand()%100 < 5){
                        grid[currentIndex] = (std::rand() % 2 == 0) ? SMOKE : AIR;
                        continue;
                    }

                    //--Распростронение огня--
                    if(std::rand() % 5 == 0)
                    {
                        // Соседи для распростронения
                        int dx[] = {-1,  0,  1, -1, 1, -1, 0, 1};
                        int dy[] = {-1, -1, -1,  0, 0,  1, 1, 1};

                        int dir = std::rand() % 8;

                        for (int i = 0; i < 8; ++i) {
                            int currentDir = (dir + i) % 8;
                            int targetX = x + dx[currentDir];
                            int targetY = y + dy[currentDir];

                            if (isVaild(targetX, targetY)) {
                                int targetIndex = getIndex(targetX, targetY);

                                if (grid[targetIndex] == DIRT) {
                                    grid[targetIndex] = FIRE;
                                    break;
                                }
                            }
                        }
                    }
                        

                    //Тушение огня
                    // if(){

                    // }
                }

                //Дым
                else if (grid[currentIndex] == SMOKE) {
                    if (std::rand() % 100 < 1) { 
                        grid[currentIndex] = AIR;
                        colors[currentIndex] = 0;
                        continue;
                    }

                    if (y == 0) {
                        if (std::rand() % 100 < 15) {
                            grid[currentIndex] = AIR;
                            colors[currentIndex] = 0;
                        }    
                        continue;
                    }

                    if (std::rand() % 100 < 30) {
        
                    int dirX = (std::rand() % 3) - 1;
                    int tx = x + dirX;
                    int ty = y - 1; 

                    if (isVaild(tx, ty) && grid[getIndex(tx, ty)] == AIR) {
                        int targetIndex = getIndex(tx, ty);
                        grid[currentIndex] = AIR;
                        grid[targetIndex] = SMOKE;

                        colors[targetIndex] = colors[currentIndex];
                        colors[currentIndex] = 0;
                    }
                    else {
                        int sideX = x + ((std::rand() % 2 == 0) ? -1 : 1);
                        if (isVaild(sideX, y) && grid[getIndex(sideX, y)] == AIR) {
                            int targetIndex = getIndex(sideX, y);
                            grid[currentIndex] = AIR;
                            grid[targetIndex] = SMOKE;

                            colors[targetIndex] = colors[currentIndex];
                            colors[currentIndex] = 0;}
                        }
                    }
                }

            }
        }
    }


    void draw(sf::RenderWindow& win){
        for(int y = 0; y <HEIGHT; ++y){
            for(int x = 0; x < WIDTH; ++x){
                int index = getIndex(x,y);
                int vIndex = 4*index;

                float left   = x * CELL_SIZE;
                float top    = y * CELL_SIZE;
                float right  = left + CELL_SIZE;
                float bottom = top + CELL_SIZE;

                pixels[vIndex + 0].position = sf::Vector2f(left, top);      
                pixels[vIndex + 1].position = sf::Vector2f(right, top);    
                pixels[vIndex + 2].position = sf::Vector2f(right, bottom);   
                pixels[vIndex + 3].position = sf::Vector2f(left, bottom);

                sf::Color cellColor;
                switch (grid[index])
                {
                    case AIR:
                        cellColor = sf::Color(20,20,20);
                        break;
                    case SAND:
                        cellColor = (colors[index] == 0) ? sf::Color(235,190,85) : sf::Color(185,135,45);
                        break;
                    case WATER:
                        cellColor = (colors[index] == 0) ? sf::Color(40,160,175) : sf::Color(127,255,212);
                        break;
                    case DIRT:
                        cellColor = (colors[index] == 0) ? sf::Color(120,75,45) : sf::Color(85,50,30);
                        break;
                    case STONE:
                        cellColor = (colors[index] == 0) ? sf::Color(70,75,80) : sf::Color(110,115,120);
                        break;
                    case ACID_L:
                        cellColor = (std::rand() % 2 == 0) ? sf::Color(143,254,9) : sf::Color(45,140,5);                    
                        break;
                    case ACID_R:
                        cellColor = (std::rand() % 2 == 0) ? sf::Color(143,254,9) : sf::Color(45,140,5);  
                        break;
                    case FIRE:
                        cellColor = (std::rand() % 2 == 0) ? sf::Color(255, 69, 0) : sf::Color(255, 140, 0);
                        break;
                    case SMOKE:
                        cellColor = (colors[index] == 0) ? sf::Color(80, 80, 80) : sf::Color(120, 120, 120);
                        break;
                }
                pixels[vIndex + 0].color = cellColor;
                pixels[vIndex + 1].color = cellColor;
                pixels[vIndex + 2].color = cellColor;
                pixels[vIndex + 3].color = cellColor;
            }
        }

        win.draw(pixels);
    }
};
