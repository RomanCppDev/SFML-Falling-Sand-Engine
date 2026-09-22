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
    struct ElementButton : Button{
        unsigned int elementType;
        float xOffset = 0.f;
        float yOffset = 0.f;
    };

    struct BtnConfig { unsigned int type; std::wstring name; std::string file; float size; float x; float y; };

    std::vector<ElementButton> elementButtons;

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

    void updatePositions(){
        float panelY = panel.getPosition().y;

        btn.setPosition((WIDTH*CELL_SIZE/2)-50.f, panelY - 10.f);
        for(auto& b : elementButtons){
            b.sprite.setPosition(b.xOffset, panelY + b.yOffset);
        }
        trashBtn.sprite.setPosition(panel.getPosition().x + (WIDTH * CELL_SIZE) - 100.f, panelY + maxPanelHeight - 190.f);
    }

    void updateButtonColors(){
        for(auto& b : elementButtons){
            if(b.elementType == currentItem){
                b.sprite.setColor(sf::Color::White);
            } else {
                b.sprite.setColor(sf::Color(120,120,120));
            }
        }
    }

    bool isClickOnUI(const sf::Vector2i& mousePos){
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if(panel.getGlobalBounds().contains(mousePosF) || btn.getGlobalBounds().contains(mousePosF)){
            return true;
        }
        return false;
    }
    
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

        std::vector<BtnConfig> configs = {
            {SAND, L"Песок", "sand.png", 80.f, 10.f, -3.f},
            {WATER, L"Вода", "water.png", 80.f, 90.f, -3.f}, 
            {DIRT, L"Земля", "dirt.png", 80.f, 170.f, -3.f},
            {STONE, L"Камень", "stone.png", 80.f, 250.f, -3.f},
            {ACID_L, L"Кислота", "acid.png", 80.f, 330.f, -3.f},
            {FIRE, L"Огонь", "fire.png", 60.f, 410.f, 3.f},
        };

        for(const auto& conf : configs){
            ElementButton b;
            b.elementType = conf.type;
            b.name = conf.name;
            b.xOffset = conf.x;
            b.yOffset = conf.y;
            loadTexture(b, b.elementType, conf.file, conf.size, conf.size);
            elementButtons.push_back(b);
        }

        // Кнопка-Очистить 
        loadTexture(trashBtn, 0, "trash.png", 100.f, 100.f);
        trashBtn.sprite.setColor(sf::Color(120,120,120));

        updatePositions();
        updateButtonColors();
    }


    float getMinPanelHeight() const {
        return minPanelHeight;
    }

    float getMaxPanelHeight() const {
        return maxPanelHeight;
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

        for(auto& b : elementButtons){
            if(b.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)){
                currentItem = b.elementType;
                updateButtonColors();
                break;
            }
        }
    }

    void isLeftButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos){
        if(!isClickOnUI(mousePos) && mousePos.y < HEIGHT*CELL_SIZE && mousePos.x >= 0 && mousePos.x < WIDTH*CELL_SIZE){
            int gridX = mousePos.x / CELL_SIZE;
            int gridY = mousePos.y / CELL_SIZE;

            engine.addBlock(gridX,gridY,brushSize,currentItem);
        }
    }

    void isRightButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos){
        if(!isClickOnUI(mousePos) && mousePos.y < HEIGHT * CELL_SIZE && mousePos.x >= 0 && mousePos.x < WIDTH * CELL_SIZE){
            if(mousePos.y < HEIGHT * CELL_SIZE){
                int gridX = mousePos.x / CELL_SIZE;
                int gridY = mousePos.y / CELL_SIZE;

                engine.addBlock(gridX,gridY,brushSize,AIR);
            }
        }
    }

    void draw(sf::RenderWindow& win){
        win.draw(panel);
        for(auto& b : elementButtons){
            win.draw(b.sprite);
        }
        win.draw(trashBtn.sprite);
        win.draw(btn);
    }

    void MovePanel(){
        if(isPanelMoving){
            usePanel(getMaxPanelHeight(), getMinPanelHeight(), panel, panelClock, isPanelMovingToUp, isPanelMoving);

            btn.setPosition(sf::Vector2f(((WIDTH * CELL_SIZE) / 2) - 50.f, panel.getPosition().y - 10.f));
            updatePositions();
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
