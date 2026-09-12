#include <SFML/Graphics.hpp>
#include "engine.h"
#include <cmath>
#include <thread>

struct ElementButton{
    int elementType;
    sf::Sprite sprite;
    sf::Texture texture;
    std::wstring name;
    bool isHovered = false;
};

void loadTexture(ElementButton& btn, std::string path){
    if(btn.texture.loadFromFile("textures/" + path)){
        btn.sprite.setTexture(btn.texture);
        btn.sprite.setScale(80.f / btn.texture.getSize().x, 80.f / btn.texture.getSize().y);
    }
}

class RoundedRectangleShape : public sf::Shape{
private:
    sf::Vector2f size;
    float radius;
    std::size_t cornerPointCount;
public:
    RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0.f, 0.f), float radius = 0.f, std::size_t cornerPointCount = 30)
        : size(size), cornerPointCount(cornerPointCount){
    setRadius(radius);
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
    void setCornerPointCount(std::size_t& count) {this->cornerPointCount = count; update();}

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

void usePanel(const int& maxPanelHeight, const int& minPanelHeight, sf::RectangleShape& panel, sf::Clock& animationClock, bool& isMovingToUp, bool& isPanelMoving);
void closePanel(sf::RectangleShape& panel, sf::Clock& animationClock);

int main() {
    sf::Clock clock;
    sf::Clock panelClock;
    static int frameCount = 0;

    //Панель и ее параметры

    const float minPanelHeight = 60.f;
    const float maxPanelHeight = 450.f;

    sf::RectangleShape panel(sf::Vector2f((WIDTH*CELL_SIZE) -4.f, maxPanelHeight));
    panel.setFillColor(sf::Color(40,40,40,240));
    panel.setPosition(sf::Vector2f(2.f, (HEIGHT*CELL_SIZE) ));
    panel.setOutlineColor(sf::Color(169,169,169,240));
    panel.setOutlineThickness(2.f);

    RoundedRectangleShape btn(sf::Vector2f(100.f,20.f), 10.f, 30);
    btn.setFillColor(sf::Color(169, 169, 169));
    btn.setPosition(sf::Vector2f(((WIDTH*CELL_SIZE) / 2) - 50.f, panel.getGlobalBounds().getPosition().y - 10.f));


    //Окно
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE + minPanelHeight), "SandBox");
    window.setFramerateLimit(140);

    SandBoxEngine engine;

    int brushSize = 2; 
    int currentItem = 1;

    //---КНОПКИ---//
    
    //Песок
    ElementButton sandBtn;
    sandBtn.elementType = SAND;
    sandBtn.name = L"Песок";
    loadTexture(sandBtn, "sand.png");
    sandBtn.sprite.setPosition(10.f, panel.getPosition().y - 3.f);

    //Вода
    ElementButton waterBtn;
    waterBtn.elementType = WATER;
    waterBtn.name = L"Вода";
    loadTexture(waterBtn, "water.png");
    waterBtn.sprite.setPosition(sf::Vector2f(90.f, panel.getPosition().y -3.f));

    //Земля
    ElementButton dirtBtn;
    dirtBtn.elementType = DIRT;
    dirtBtn.name = L"Земля";
    loadTexture(dirtBtn, "dirt.png");
    dirtBtn.sprite.setPosition(sf::Vector2f(170.f, panel.getPosition().y -3.f));

    //Камень
    ElementButton stoneBtn;
    stoneBtn.elementType = DIRT;
    stoneBtn.name = L"Камень";
    loadTexture(stoneBtn, "stone.png");
    stoneBtn.sprite.setPosition(sf::Vector2f(250.f, panel.getPosition().y -3.f));

    //Кислота
    ElementButton acidBtn;
    acidBtn.elementType = ACID_L;
    acidBtn.name = L"Кислота";
    loadTexture(acidBtn, "acid.png");
    acidBtn.sprite.setPosition(sf::Vector2f(330.f, panel.getPosition().y -3.f));

    //Огонь
    ElementButton fireBtn;
    fireBtn.elementType = FIRE;
    fireBtn.name = L"Огонь";
    loadTexture(fireBtn, "fire.png");
    fireBtn.sprite.setPosition(sf::Vector2f(410.f, panel.getPosition().y + 2.f));
    fireBtn.sprite.setScale(60.f / fireBtn.texture.getSize().x, 60.f / fireBtn.texture.getSize().y);

    bool isPanelMoving = false; 
    bool isPanelMovingToUp = false;
    
    while (window.isOpen()) {
        float frameTime = clock.restart().asSeconds();
        float fps = 1.0f / frameTime;

        sf::Event event; 
        
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); 
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) brushSize = 0; // Точечная кисть
                if (event.key.code == sf::Keyboard::Num2) brushSize = 5; // Средняя
                if (event.key.code == sf::Keyboard::Num3) brushSize = 8; // Большая
                if (event.key.code == sf::Keyboard::Num4) brushSize = 13; // Огромная
            }

            if(event.type == sf::Event::MouseButtonPressed){
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (btn.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if(!isPanelMoving){
                        if(panel.getPosition().y >= (HEIGHT * CELL_SIZE) - 5) {
                            isPanelMovingToUp = false; // Едем вверх, если мы внизу
                        }
                        else {
                            isPanelMovingToUp = true; // Едем вниз, если мы вверху
                        }
                    } 
                    isPanelMoving = true; 
                }
                else if(sandBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = SAND;
                }
                else if(waterBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = WATER;
                }
                else if(dirtBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = DIRT;
                }
                else if(stoneBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = STONE;
                }
                else if(acidBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = ACID_L;
                }
                else if(fireBtn.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)){
                    currentItem = FIRE;
                }
            }
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if(mousePos.y < HEIGHT * CELL_SIZE && (!sandBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y) &&
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
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if(mousePos.y < HEIGHT * CELL_SIZE){
                int gridX = mousePos.x / CELL_SIZE;
                int gridY = mousePos.y / CELL_SIZE;

                engine.addBlock(gridX,gridY,brushSize,AIR);
            }
        }

        switch (currentItem)
        {
        case SAND:
            sandBtn.sprite.setColor(sf::Color::White);
            waterBtn.sprite.setColor(sf::Color(120, 120, 120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
            break;
        case WATER:
            sandBtn.sprite.setColor(sf::Color(120, 120, 120));
            waterBtn.sprite.setColor(sf::Color::White);
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
            break;
        case DIRT:
            sandBtn.sprite.setColor(sf::Color(120, 120, 120));
            waterBtn.sprite.setColor(sf::Color(120, 120, 120));
            dirtBtn.sprite.setColor(sf::Color::White);
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
            break;
        case STONE:
            sandBtn.sprite.setColor(sf::Color(120, 120, 120));
            waterBtn.sprite.setColor(sf::Color(120, 120, 120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color::White);
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            fireBtn.sprite.setColor(sf::Color(120,120,120));
            break;
        case ACID_L:
            sandBtn.sprite.setColor(sf::Color(120, 120, 120));
            waterBtn.sprite.setColor(sf::Color(120, 120, 120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color::White);
            fireBtn.sprite.setColor(sf::Color(120,120,120));
            break;
        case FIRE:
            fireBtn.sprite.setColor(sf::Color::White);
            waterBtn.sprite.setColor(sf::Color(120, 120, 120));
            dirtBtn.sprite.setColor(sf::Color(120,120,120));
            stoneBtn.sprite.setColor(sf::Color(120,120,120));
            acidBtn.sprite.setColor(sf::Color(120,120,120));
            sandBtn.sprite.setColor(sf::Color(120, 120, 120));
        }

        if (frameCount >= 140) { 
            window.setTitle("SandBox | FPS: " + std::to_string(static_cast<int>(fps)));
            frameCount = 0;
        }     

        frameCount++;

        if(isPanelMoving){
            usePanel(maxPanelHeight, minPanelHeight, panel, panelClock, isPanelMovingToUp, isPanelMoving);

            btn.setPosition(sf::Vector2f(((WIDTH * CELL_SIZE) / 2) - 50.f, panel.getPosition().y - 10.f));
            sandBtn.sprite.setPosition(10.f, panel.getPosition().y -2.f);
            waterBtn.sprite.setPosition(90.f, panel.getPosition().y-2.f);
            dirtBtn.sprite.setPosition(170.f, panel.getPosition().y -2.f);
            stoneBtn.sprite.setPosition(250.f, panel.getPosition().y -2.f);
            acidBtn.sprite.setPosition(330.f, panel.getPosition().y -2.f);
            fireBtn.sprite.setPosition(410.f, panel.getPosition().y + 3.f);
        }

        engine.update();

        window.clear();
        engine.draw(window);

        window.draw(panel);
        window.draw(sandBtn.sprite);
        window.draw(waterBtn.sprite);
        window.draw(dirtBtn.sprite);
        window.draw(stoneBtn.sprite);
        window.draw(acidBtn.sprite);
        window.draw(fireBtn.sprite);
        window.draw(btn);

        window.display();
    }

    return 0;
}

void usePanel(const int& maxPanelHeight, const int& minPanelHeight, sf::RectangleShape& panel, sf::Clock& animationClock, bool& isMovingToUp, bool& isPanelMoving) {
    float currentY = panel.getPosition().y;
    
    if (isMovingToUp) {
        // Движение ВНИЗ: используем HEIGHT * CELL_SIZE для надежного ухода за экран
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
        // Движение ВВЕРХ (оставляем как есть, раз оно работало)
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
