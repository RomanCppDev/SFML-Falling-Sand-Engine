#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

const int WIDTH = 100;
const int HEIGHT = 75;
const int CELL_SIZE = 10;

enum Element{
    AIR = 0,
    SAND = 1,
    WATER = 2,
    STONE = 3,
    DIRT = 4,
    FIRE = 5,
};

class SandBoxEngine{
private:
    std::vector<int> grid;
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
        pixels.setPrimitiveType(sf::PrimitiveType::Quads);
        pixels.resize(WIDTH*HEIGHT*4);
    }

    void addBlock(int mouseX, int mouseY, int brushSize, int BlockId){
        for(int dy = -brushSize; dy<=brushSize; ++dy){
            for(int dx=-brushSize; dx<=brushSize; ++dx){
                int nx = mouseX + dx;
                int ny = mouseY + dy;
                
                int index = getIndex(nx,ny);
                if(isVaild(nx, ny) && (BlockId == AIR || grid[index] == AIR)){
                    grid[index] = BlockId;
                }
            }
        }
    }

    void update(){
        for(int y = HEIGHT - 2; y >= 0; --y){
            for(int x = 0; x < WIDTH; ++x){
                int currentIndex = getIndex(x,y);

                //Песок (SAND)
                if(grid[currentIndex] == SAND){
                    if(isVaild(x,y+1) && grid[getIndex(x,y+1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x,y+1)] = SAND;
                    }
                    else if(isVaild(x-1,y+1) && grid[getIndex(x-1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x-1,y+1)] = SAND;
                    }
                    else if(isVaild(x+1,y+1) && grid[getIndex(x+1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x+1,y+1)] = SAND;
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
                    if(isVaild(x, y + 1) && grid[getIndex(x, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x, y + 1)] = WATER;
                    }
                    else if(isVaild(x - 1, y + 1) && grid[getIndex(x - 1, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x - 1, y + 1)] = WATER;
                    }
                    else if(isVaild(x + 1, y + 1) && grid[getIndex(x + 1, y + 1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x + 1, y + 1)] = WATER;
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
                                break; // Уперлись в препятствие, дальше течь нельзя
                            }
                        }

                        if (currentX != x) {
                            grid[currentIndex] = AIR;
                            grid[getIndex(currentX, y)] = WATER;
                        }
                    }
                }
                //Земля
                else if(grid[currentIndex]==DIRT){
                    if(isVaild(x,y+1) && grid[getIndex(x,y+1)] == AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x,y+1)] = DIRT;
                    }
                    else if(isVaild(x-1,y+1) && grid[getIndex(x-1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x-1,y+1)] = DIRT;
                    }
                    else if(isVaild(x+1,y+1) && grid[getIndex(x+1,y+1)]==AIR){
                        grid[currentIndex] = AIR;
                        grid[getIndex(x+1,y+1)] = DIRT;
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
                        cellColor = sf::Color(235,190,85);
                        break;
                    case WATER:
                        cellColor = sf::Color(127,255,212);
                        break;
                    case DIRT:
                        cellColor = sf::Color(139, 69, 19);
                        break;
                    case STONE:
                        cellColor = sf::Color(211,211,211);
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
