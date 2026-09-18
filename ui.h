#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include "engine.h"
#include <map>

struct Button{
    sf::Sprite sprite;
    sf::Texture texture;
    std::wstring name;
};

struct ElementButton : Button{
    unsigned int elementType;
};


class RoundedRectangleShape : public sf::Shape{
private:
    sf::Vector2f size;
    float radius;
    std::size_t cornerPointCount;
public:
    RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0.f, 0.f), float radius = 0.f, std::size_t cornerPointCount = 30)
        : size(size), radius(radius), cornerPointCount(cornerPointCount) {
        setRadius(this->radius); 
    }

    void setSize(const sf::Vector2f& size) 
    { 
        this->size = size; 
        setRadius(this->radius); 
    }
    void setRadius(float radius) 
    { 
        float maxRadius = std::min(size.x, size.y) / 2.f;
        this->radius = std::min(radius, maxRadius); 
        update(); 
    }
    void setCornerPointCount(std::size_t count) { this->cornerPointCount = count; update(); }

    virtual std::size_t getPointCount() const override{
        return cornerPointCount*4;
    }

    virtual sf::Vector2f getPoint(std::size_t index) const override{
        if(index >= cornerPointCount*4 ) return sf::Vector2f(0.f,0.f);

        float deltaAngel = 90.f / (cornerPointCount -1);
        std::size_t centerIndex = index/cornerPointCount;
        std::size_t offsetIndex = index % cornerPointCount;

        sf::Vector2f center;
        float angle = 0.f;

        switch (centerIndex)
        {
        case 0:
            center = sf::Vector2f(size.x - radius, radius);
            angle = 270.f;
            break;
        case 1:
            center = sf::Vector2f(size.x- radius, size.y - radius);
            angle = 0.f;
            break;
        case 2:
            center = sf::Vector2f(radius, size.y - radius);
            angle = 90.f;
            break;
        case 3:
            center = sf::Vector2f(radius, radius);
            angle = 180.f;
            break;
        }

        float radians = (angle + offsetIndex * deltaAngel) * 3.141592654f/180.f;
        return sf::Vector2f(center.x + radius *std::cos(radians), center.y + radius * std::sin(radians));
    }
};

class UI {
private:
    static constexpr float minPanelHeight = 60.f;
    static constexpr float maxPanelHeight = 450.f;

    unsigned int currentItem = SAND;

    bool isPanelMovingToUp = false;
    bool isPanelMoving = false; 

    std::map<unsigned int, sf::Texture> uiTextures;

        void loadTexture(Button& btn, unsigned int elementType, std::string path, float sizeX = 80.f, float sizeY = 80.f) {
            if (uiTextures[elementType].loadFromFile("textures/" + path)) {
                uiTextures[elementType].setSmooth(false);   
                uiTextures[elementType].setRepeated(false); 
        
                btn.sprite.setTexture(uiTextures[elementType], true); 
                btn.sprite.setScale(sizeX / uiTextures[elementType].getSize().x, sizeY / uiTextures[elementType].getSize().y);
            }
        }
    
    ElementButton sandBtn;
    ElementButton waterBtn;
    ElementButton stoneBtn;
    ElementButton dirtBtn;
    ElementButton acidBtn;
    ElementButton fireBtn;
    Button trashBtn;
    RoundedRectangleShape btn;

    sf::Clock panelClock;

public:    
    sf::RectangleShape panel; 
        UI() : 
    panel(sf::Vector2f((WIDTH * CELL_SIZE) - 4.f, maxPanelHeight)),
    btn(sf::Vector2f(100.f,20.f), 10.f, 30) {

        // Панель
        panel.setFillColor(sf::Color(40,40,40,240));
        panel.setPosition(sf::Vector2f(2.f, (HEIGHT*CELL_SIZE) ));
        panel.setOutlineColor(sf::Color(169,169,169,240));
        panel.setOutlineThickness(2.f);

        // Кнопка выдвижения панели
        btn.setFillColor(sf::Color(169, 169, 169));
        btn.setPosition(sf::Vector2f(((WIDTH*CELL_SIZE) / 2) - 50.f, panel.getPosition().y - 10.f));

        // Кнопка-Песок
        sandBtn.elementType = SAND;
        sandBtn.name = L"Песок";
        loadTexture(sandBtn, SAND, "sand.png");    
        sandBtn.sprite.setPosition(10.f, panel.getPosition().y - 3.f);

        // Кнопка-Вода
        waterBtn.elementType = WATER;
        waterBtn.name = L"Вода";
        loadTexture(waterBtn, WATER, "water.png");
        waterBtn.sprite.setPosition(sf::Vector2f(90.f, panel.getPosition().y - 3.f));
        waterBtn.sprite.setColor(sf::Color(120,120,120));

        // Кнопка-Земля
        dirtBtn.elementType = DIRT;
        dirtBtn.name = L"Земля";
        loadTexture(dirtBtn, DIRT, "dirt.png"); // Теперь DIRT автоматически найдет свое место в памяти!
        dirtBtn.sprite.setPosition(sf::Vector2f(170.f, panel.getPosition().y - 3.f));
        dirtBtn.sprite.setColor(sf::Color(120,120,120));

        // Кнопка-Камень
        stoneBtn.elementType = STONE;
        stoneBtn.name = L"Камень";
        loadTexture(stoneBtn, STONE, "stone.png");
        stoneBtn.sprite.setPosition(sf::Vector2f(250.f, panel.getPosition().y - 3.f));
        stoneBtn.sprite.setColor(sf::Color(120,120,120));

        // Кнопка-Кислота
        acidBtn.elementType = ACID_L;
        acidBtn.name = L"Кислота";
        loadTexture(acidBtn, ACID_L, "acid.png");
        acidBtn.sprite.setPosition(sf::Vector2f(330.f, panel.getPosition().y - 3.f));
        acidBtn.sprite.setColor(sf::Color(120,120,120));

        // Кнопка-Огонь
        fireBtn.elementType = FIRE;
        fireBtn.name = L"Огонь";
        loadTexture(fireBtn, FIRE, "fire.png", 60.f, 60.f); 
        fireBtn.sprite.setPosition(sf::Vector2f(410.f, panel.getPosition().y - 3.f));
        fireBtn.sprite.setColor(sf::Color(120,120,120));

        // Кнопка-Очистить 
        loadTexture(trashBtn, 0, "trash.png", 100.f, 100.f);
        trashBtn.sprite.setPosition(panel.getPosition().x + (WIDTH*CELL_SIZE) - 100.f, panel.getPosition().y + getMaxPanelHeight() - 190.f);
        trashBtn.sprite.setColor(sf::Color(120,120,120));

    }


    float getMinPanelHeight() const {
        return minPanelHeight;
    }

    float getMaxPanelHeight() const {
        return maxPanelHeight;
    }

    short getCurrentItem() const{
        return currentItem;
    }

    void MouseButtonPressed(SandBoxEngine& engine, sf::RenderWindow& win){
        sf::Vector2i mousePos = sf::Mouse::getPosition(win);

        if(btn.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))){
            if(!isPanelMoving){
                if(panel.getPosition().y >= (HEIGHT * CELL_SIZE) - 5) {
                    isPanelMovingToUp = false; 
                }
                else {
                    isPanelMovingToUp = true; 
                }
            }
            isPanelMoving = true;
        }

        if(sandBtn.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)){
            currentItem = sandBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color::White);
            waterBtn.sprite.setColor(sf::Color(120,120,120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
        }
        else if(waterBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
            currentItem = waterBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color(120,120,120));
            waterBtn.sprite.setColor(sf::Color::White);
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
        }
        else if(dirtBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
            currentItem = dirtBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color(120,120,120));
            waterBtn.sprite.setColor(sf::Color(120,120,120));
            dirtBtn.sprite.setColor(sf::Color::White);
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
        }
        else if(stoneBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
            currentItem = stoneBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color(120,120,120));
            waterBtn.sprite.setColor(sf::Color(120,120,120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color::White);
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
        }
        else if(acidBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
            currentItem = acidBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color(120,120,120));
            waterBtn.sprite.setColor(sf::Color(120,120,120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color::White);
            fireBtn.sprite.setColor(sf::Color(120,120,120));
        }
        else if(fireBtn.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)){
            currentItem = fireBtn.elementType;

            //Перекраска кнопок
            sandBtn.sprite.setColor(sf::Color(120,120,120));
            waterBtn.sprite.setColor(sf::Color(120,120,120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color::White);
        }
        else if(trashBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
            engine.FillGrid(AIR);
        }
    }

    void isLeftButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos){
        if(mousePos.y < HEIGHT * CELL_SIZE && ( !sandBtn.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y) &&
            !waterBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !dirtBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !stoneBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !acidBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !fireBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !btn.getGlobalBounds().contains(mousePos.x,mousePos.y))){
            int gridX = mousePos.x / CELL_SIZE;
            int gridY = mousePos.y / CELL_SIZE;

            engine.addBlock(gridX,gridY,brushSize,currentItem);
        }
    }

    void isRightButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos){
        if(!waterBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !dirtBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !stoneBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !acidBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !fireBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
            !btn.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    if(mousePos.y < HEIGHT * CELL_SIZE){
                        int gridX = mousePos.x / CELL_SIZE;
                        int gridY = mousePos.y / CELL_SIZE;

                        engine.addBlock(gridX,gridY,brushSize,AIR);
                    }
        }
    }

    void draw(sf::RenderWindow& win){
        win.draw(panel);
        win.draw(sandBtn.sprite);
        win.draw(waterBtn.sprite);
        win.draw(dirtBtn.sprite);
        win.draw(stoneBtn.sprite);
        win.draw(acidBtn.sprite);
        win.draw(fireBtn.sprite);
        win.draw(trashBtn.sprite);
        win.draw(btn);
    }

    void MovePanel(){
        if(isPanelMoving){
            usePanel(getMaxPanelHeight(), getMinPanelHeight(), panel, panelClock, isPanelMovingToUp, isPanelMoving);

            btn.setPosition(sf::Vector2f(((WIDTH * CELL_SIZE) / 2) - 50.f, panel.getPosition().y - 10.f));
            sandBtn.sprite.setPosition(10.f, panel.getPosition().y - 2.f);
            waterBtn.sprite.setPosition(90.f, panel.getPosition().y-2.f);
            dirtBtn.sprite.setPosition(170.f, panel.getPosition().y -2.f);
            stoneBtn.sprite.setPosition(250.f, panel.getPosition().y -2.f);
            acidBtn.sprite.setPosition(330.f, panel.getPosition().y -2.f);
            fireBtn.sprite.setPosition(410.f, panel.getPosition().y + 3.f);
            trashBtn.sprite.setPosition(panel.getPosition().x + (WIDTH*CELL_SIZE) - 100.f, panel.getPosition().y + getMaxPanelHeight() - 190.f);
        }
    }

    void usePanel(const int& maxPanelHeight, const int& minPanelHeight, sf::RectangleShape& panel, sf::Clock& animationClock, bool& isMovingToUp, bool& isPanelMoving) {
        float currentY = panel.getPosition().y;
    
        if (isMovingToUp) {
            float bottomLimit = static_cast<float>(HEIGHT * CELL_SIZE);
            if (currentY < bottomLimit) {
                if (animationClock.getElapsedTime().asMilliseconds() >= 10) {
                    panel.move(0.f, 10.f); 
                    animationClock.restart(); 
                
                    if (panel.getPosition().y >= bottomLimit) {
                        panel.setPosition(panel.getPosition().x, bottomLimit);
                        isPanelMoving = false; 
                    }
                }
            } else {
                isPanelMoving = false; 
            }
        } else {
            int topLimit = maxPanelHeight - minPanelHeight;
            if (currentY > topLimit) {
                if (animationClock.getElapsedTime().asMilliseconds() >= 10) {
                    panel.move(0.f, -10.f); 
                    animationClock.restart(); 
                
                    if (panel.getPosition().y <= topLimit) {
                        panel.setPosition(panel.getPosition().x, static_cast<float>(topLimit));
                        isPanelMoving = false;
                    }
                }
            } else {
                isPanelMoving = false; 
            }
        }
    }
};
