#include <SFML/Graphics.hpp>
#include "engine.h"

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

int main() {
    sf::Clock clock;
    static int frameCount = 0;

    int panelHeight = 60;
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE +60), "SandBox");
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
    sandBtn.sprite.setPosition(10.f, (HEIGHT * CELL_SIZE) - 5.f);

    //Вода
    ElementButton waterBtn;
    waterBtn.elementType = WATER;
    waterBtn.name = L"Вода";
    loadTexture(waterBtn, "water.png");
    waterBtn.sprite.setPosition(sf::Vector2f(90.f, (HEIGHT*CELL_SIZE) -5.f));

    //Земля
    ElementButton dirtBtn;
    dirtBtn.elementType = DIRT;
    dirtBtn.name = L"Земля";
    loadTexture(dirtBtn, "dirt.png");
    dirtBtn.sprite.setPosition(sf::Vector2f(170.f, (HEIGHT*CELL_SIZE) -5.f));

    //Камень
    ElementButton stoneBtn;
    stoneBtn.elementType = DIRT;
    stoneBtn.name = L"Камень";
    loadTexture(stoneBtn, "stone.png");
    stoneBtn.sprite.setPosition(sf::Vector2f(250.f, (HEIGHT*CELL_SIZE) - 5.f));

    //Кислота
    ElementButton acidBtn;
    acidBtn.elementType = ACID_L;
    acidBtn.name = L"Кислота";
    loadTexture(acidBtn, "acid.png");
    acidBtn.sprite.setPosition(sf::Vector2f(330.f, (HEIGHT*CELL_SIZE) - 5.f));

    //Огонь
    ElementButton fireBtn;
    fireBtn.elementType = FIRE;
    fireBtn.name = L"Огонь";
    loadTexture(fireBtn, "fire.png");
    fireBtn.sprite.setPosition(sf::Vector2f(410.f, (HEIGHT*CELL_SIZE)));
    fireBtn.sprite.setScale(60.f / fireBtn.texture.getSize().x, 60.f / fireBtn.texture.getSize().y);

    // sf::RectangleShape Firebtn(sf::Vector2f(60.f,40.f));
    // Firebtn.setPosition(sf::Vector2f(420.f, (HEIGHT*CELL_SIZE) + 10.f));
    // Firebtn.setFillColor(sf::Color(255, 69, 0));
    // Firebtn.setOutlineThickness(3.f);
    // Firebtn.setOutlineColor(sf::Color::Black);

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
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if(mousePos.y < HEIGHT * CELL_SIZE){
                int gridX = mousePos.x / CELL_SIZE;
                int gridY = mousePos.y / CELL_SIZE;

                engine.addBlock(gridX,gridY,brushSize,currentItem);
            }
            else{
                if(sandBtn.sprite.getGlobalBounds().contains(mousePos.x,mousePos.y)){
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

        engine.update();

        window.clear();
        engine.draw(window);

        window.draw(sandBtn.sprite);
        window.draw(waterBtn.sprite);
        window.draw(dirtBtn.sprite);
        window.draw(stoneBtn.sprite);
        window.draw(acidBtn.sprite);
        window.draw(fireBtn.sprite);

        window.display();
    }

    return 0;
}
