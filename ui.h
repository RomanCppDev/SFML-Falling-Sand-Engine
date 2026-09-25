#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include "engine.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>

struct UIElement {
    sf::Sprite sprite;
    sf::Texture texture;
    std::wstring name;
    float xOffset = 0.f;
    float yOffset = 0.f;
    unsigned int elementType;
};

enum orientation {
    HORIZONTAL = false,
    VERTICAL = true,
};

class RoundedRectangleShape : public sf::Shape {
private:
    sf::Vector2f size;
    float radius;
    std::size_t cornerPointCount;
public:
    RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0.f, 0.f), float radius = 0.f, std::size_t cornerPointCount = 30)
        : size(size), radius(radius), cornerPointCount(cornerPointCount) {
        setRadius(this->radius); 
    }

    void setSize(const sf::Vector2f& size) { 
        this->size = size; 
        setRadius(this->radius); 
    }
    void setRadius(float radius) { 
        float maxRadius = std::min(size.x, size.y) / 2.f;
        this->radius = std::min(radius, maxRadius); 
        update(); 
    }
    void setCornerPointCount(std::size_t count) { this->cornerPointCount = count; update(); }

    virtual std::size_t getPointCount() const override {
        return cornerPointCount * 4;
    }

    virtual sf::Vector2f getPoint(std::size_t index) const override {
        if(index >= cornerPointCount * 4) return sf::Vector2f(0.f, 0.f);

        float deltaAngel = 90.f / (cornerPointCount - 1);
        std::size_t centerIndex = index / cornerPointCount;
        std::size_t offsetIndex = index % cornerPointCount;

        sf::Vector2f center;
        float angle = 0.f;

        switch (centerIndex) {
        case 0: center = sf::Vector2f(size.x - radius, radius); angle = 270.f; break;
        case 1: center = sf::Vector2f(size.x - radius, size.y - radius); angle = 0.f; break;
        case 2: center = sf::Vector2f(radius, size.y - radius); angle = 90.f; break;
        case 3: center = sf::Vector2f(radius, radius); angle = 180.f; break;
        }

        float radians = (angle + offsetIndex * deltaAngel) * 3.141592654f / 180.f; 
        return sf::Vector2f(center.x + radius * std::cos(radians), center.y + radius * std::sin(radians));
    }
};

class Panel {
private:
    const std::vector<UIElement*>& panelElements;

    float minPanelSize = 0.f;
    float maxPanelSize = 0.f;
    bool orientation;

    bool isPanelMovingToUp = false;
    bool isPanelMoving = false;

    sf::Clock panelClock;
public:
    sf::RectangleShape panel;
    RoundedRectangleShape btn;

    Panel(const float minPanelSize, const float maxPanelSize, const bool orientation, const std::vector<UIElement*>& elements) : 
        panelElements(elements), minPanelSize(minPanelSize), maxPanelSize(maxPanelSize), orientation(orientation),
        panel(sf::Vector2f((orientation == HORIZONTAL) ? (WIDTH*CELL_SIZE) - 4.f : maxPanelSize,
            (orientation == HORIZONTAL) ? maxPanelSize : (HEIGHT*CELL_SIZE) - 4.f)),
        btn(sf::Vector2f(100.f, 20.f), 10.f, 30) {

            panel.setFillColor(sf::Color(40,40,40,240));
            panel.setPosition(sf::Vector2f(2.f, (HEIGHT*CELL_SIZE)));
            panel.setOutlineColor(sf::Color(169,169,169,240));
            panel.setOutlineThickness(2.f);

            btn.setFillColor(sf::Color(169, 169, 169));
    }

    float getMaxPanelSize() const{
        return maxPanelSize;
    }

    float getMinSize() const { return minPanelSize; }

    void updatePositions(sf::Sprite& trashBtn) {
        float panelPos = (orientation == HORIZONTAL) ? panel.getPosition().y : panel.getPosition().x;

        if (orientation == HORIZONTAL) {
            btn.setPosition((WIDTH * CELL_SIZE / 2.f) - 50.f, panelPos - 10.f);
        } else {
            btn.setPosition(panelPos - 10.f, (HEIGHT * CELL_SIZE / 2.f) - 50.f);
        }

        for(auto* b : panelElements) {
            if (!b) continue;
            float x = (orientation == HORIZONTAL) ? b->xOffset : panelPos + b->xOffset;
            float y = (orientation == HORIZONTAL) ? panelPos + b->yOffset : b->yOffset;
            b->sprite.setPosition(x, y);
        }
        trashBtn.setPosition(panel.getPosition().x + (WIDTH * CELL_SIZE) - 100.f, panelPos + maxPanelSize - 190.f);
    }

    void updateButtonColors(unsigned int currentItem) {
        for(auto* b : panelElements) {
            if (!b) continue;
            if(b->elementType == currentItem) {
                b->sprite.setColor(sf::Color::White);
            } else {
                b->sprite.setColor(sf::Color(120,120,120));
            }
        }
    }

    void MouseButtonPressed(sf::RenderWindow& win, unsigned int& currentItem) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(win);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if(btn.getGlobalBounds().contains(mousePosF)) {
            if(!isPanelMoving) {
                float currentPos = (orientation == HORIZONTAL) ? panel.getPosition().y : panel.getPosition().x;
                float openLimit = (orientation == HORIZONTAL) ? (HEIGHT * CELL_SIZE) : (WIDTH * CELL_SIZE);
                
                if(currentPos >= openLimit - 5.f) {
                    isPanelMovingToUp = true; 
                } else {
                    isPanelMovingToUp = false; 
                }
            }
            isPanelMoving = true;
        }

        for(auto* b : panelElements) {
            if (!b) continue;
            if(b->sprite.getGlobalBounds().contains(mousePosF)) {
                currentItem = b->elementType;
                updateButtonColors(currentItem);
                break;
            }
        }
    }

    void usePanel() {
        if (!isPanelMoving) return;

        float currentPos = (orientation == HORIZONTAL) ? panel.getPosition().y : panel.getPosition().x;
        float openLimit = (orientation == HORIZONTAL) ? static_cast<float>(HEIGHT * CELL_SIZE) : static_cast<float>(WIDTH * CELL_SIZE);
        float closedLimit = maxPanelSize - minPanelSize;

        if (panelClock.getElapsedTime().asMilliseconds() >= 10) {
            panelClock.restart();

            if (isPanelMovingToUp) { 
                float moveStep = -10.f;
                float nextPos = currentPos + moveStep;

                if (nextPos <= closedLimit) {
                    if (orientation == HORIZONTAL) panel.setPosition(panel.getPosition().x, closedLimit);
                    else panel.setPosition(closedLimit, panel.getPosition().y);
                    isPanelMoving = false;
                } else {
                    if (orientation == HORIZONTAL) panel.move(0.f, moveStep);
                    else panel.move(moveStep, 0.f);
                }
            } 
            else { 
                float moveStep = 10.f;
                float nextPos = currentPos + moveStep;

                if (nextPos >= openLimit) {
                    if (orientation == HORIZONTAL) panel.setPosition(panel.getPosition().x, openLimit);
                    else panel.setPosition(openLimit, panel.getPosition().y);
                    isPanelMoving = false;
                } else {
                    if (orientation == HORIZONTAL) panel.move(0.f, moveStep);
                    else panel.move(moveStep, 0.f);
                }
            }
        }
    }

    void MovePanel(sf::Sprite& trashBtn) {
        if(isPanelMoving) {
            usePanel();
            updatePositions(trashBtn);
        }
    }

    void draw(sf::RenderWindow& win) {
        win.draw(panel);
        win.draw(btn);
        for(auto* b : panelElements) {
            if (b) win.draw(b->sprite);
        }
    }
};

class UI {
private:
    struct BtnConfig { 
        unsigned int type; 
        std::wstring name; 
        std::string file; 
        float size; 
        float x; 
        float y; 
    };

    std::vector<UIElement*> ItemPanelElement;

public:    
    Panel ItemPanel;

private:
    std::vector<UIElement> elementButtons;
    unsigned int currentItem = SAND;

    std::map<unsigned int, sf::Texture> uiTextures;
    UIElement trashBtn;

    void loadTexture(UIElement& btn, unsigned int elementType, std::string path, float sizeX = 80.f, float sizeY = 80.f) {
        if (uiTextures[elementType].loadFromFile("textures/" + path)) {
            uiTextures[elementType].setSmooth(false);   
            uiTextures[elementType].setRepeated(false); 
            btn.sprite.setTexture(uiTextures[elementType], true); 
            btn.sprite.setScale(sizeX / uiTextures[elementType].getSize().x, sizeY / uiTextures[elementType].getSize().y);
        }
    }

    bool isClickOnUI(const sf::Vector2i& mousePos) {
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        return (ItemPanel.panel.getGlobalBounds().contains(mousePosF) || ItemPanel.btn.getGlobalBounds().contains(mousePosF));
    }
    
public:    
    UI() : ItemPanel(60.f, 450.f, HORIZONTAL, ItemPanelElement) {
        std::vector<BtnConfig> configs = {
            {SAND, L"Песок", "sand.png", 80.f, 10.f, -3.f},
            {WATER, L"Вода", "water.png", 80.f, 90.f, -3.f}, 
            {DIRT, L"Земля", "dirt.png", 80.f, 170.f, -3.f},
            {STONE, L"Камень", "stone.png", 80.f, 250.f, -3.f},
            {ACID_L, L"Кислота", "acid.png", 80.f, 330.f, -3.f},
            {FIRE, L"Огонь", "fire.png", 60.f, 410.f, 3.f},
        };

        elementButtons.reserve(configs.size());

        for (const auto& conf : configs) {
            UIElement b;
            b.elementType = conf.type;
            b.name = conf.name;
            b.xOffset = conf.x;
            b.yOffset = conf.y;
            loadTexture(b, b.elementType, conf.file, conf.size, conf.size);
            elementButtons.push_back(b);
        }

        loadTexture(trashBtn, 0, "trash.png", 100.f, 100.f);
        trashBtn.sprite.setColor(sf::Color(120, 120, 120));

        for (size_t i = 0; i < elementButtons.size(); ++i) {
            ItemPanelElement.push_back(&elementButtons[i]);
        }

        ItemPanel.updatePositions(trashBtn.sprite);
        trashBtn.sprite.setPosition(ItemPanel.panel.getPosition().x + (WIDTH * CELL_SIZE) - 100.f, ItemPanel.panel.getPosition().y + ItemPanel.getMaxPanelSize() - 190.f);
        ItemPanel.updateButtonColors(currentItem);
    }

    float getMinPanelHeight() const {
        return ItemPanel.getMinSize();
    }

    void MovePanel() {
        ItemPanel.MovePanel(trashBtn.sprite);
    }

    void MouseButtonPressed(SandBoxEngine& engine, sf::RenderWindow& win) {
        ItemPanel.MouseButtonPressed(win, currentItem);
        sf::Vector2i mP = sf::Mouse::getPosition(win);
        if(trashBtn.sprite.getGlobalBounds().contains(mP.x, mP.y)){
            engine.FillGrid(AIR);
        }
    }

    void isLeftButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos) {
        if (!isClickOnUI(mousePos) && mousePos.y < HEIGHT * CELL_SIZE && mousePos.x >= 0 && mousePos.x < WIDTH * CELL_SIZE) {
            int gridX = mousePos.x / CELL_SIZE;
            int gridY = mousePos.y / CELL_SIZE;
            engine.addBlock(gridX, gridY, brushSize, currentItem);
        }
    }

    void isRightButtonPressed(SandBoxEngine& engine, unsigned short& brushSize, sf::Vector2i& mousePos) {
        if (!isClickOnUI(mousePos) && mousePos.y < HEIGHT * CELL_SIZE && mousePos.x >= 0 && mousePos.x < WIDTH * CELL_SIZE) {
            int gridX = mousePos.x / CELL_SIZE;
            int gridY = mousePos.y / CELL_SIZE;
            engine.addBlock(gridX, gridY, brushSize, AIR); 
        }
    }

    void draw(sf::RenderWindow& win) {
        ItemPanel.draw(win);      
        win.draw(trashBtn.sprite);
    }
};