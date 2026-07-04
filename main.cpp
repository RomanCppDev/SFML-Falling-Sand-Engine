#include <SFML/Graphics.hpp>
#include "engine.h"

int main() {
    sf::Clock clock;
    static int frameCount = 0;

    int panelHeight = 60;
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE +60), "SandBox");
    window.setFramerateLimit(60);

    SandBoxEngine engine;
    int brushSize = 2; 
    int currentItem = 1;

    //---КНОПКИ---//
    //Песок
    sf::RectangleShape Sandbtn(sf::Vector2f(60.f,40.f));
    Sandbtn.setPosition(sf::Vector2f(20.f, (HEIGHT*CELL_SIZE)+10.f));
    Sandbtn.setFillColor(sf::Color(235,190,85));
    Sandbtn.setOutlineThickness(3.f);
    Sandbtn.setOutlineColor(sf::Color::Black);

    //Вода
    sf::RectangleShape Waterbtn(sf::Vector2f(60.f,40.f));
    Waterbtn.setPosition(sf::Vector2f(100.f,(HEIGHT*CELL_SIZE)+10.f)); 
    Waterbtn.setFillColor(sf::Color(127,255,212));
    Waterbtn.setOutlineThickness(3.0f);
    Waterbtn.setOutlineColor(sf::Color::Black);

    //Земля
    sf::RectangleShape Dirtbtn(sf::Vector2f(60.f,40.f));
    Dirtbtn.setPosition(sf::Vector2f(180.f,(HEIGHT*CELL_SIZE)+10.f)); 
    Dirtbtn.setFillColor(sf::Color(139, 69, 19));                   
    Dirtbtn.setOutlineThickness(3.0f);
    Dirtbtn.setOutlineColor(sf::Color::Black);

    //Камень
    sf::RectangleShape Stonebtn(sf::Vector2f(60.f,40.f));
    Stonebtn.setPosition(sf::Vector2f(260.f,(HEIGHT*CELL_SIZE)+10.f)); 
    Stonebtn.setFillColor(sf::Color(211, 211, 211));                
    Stonebtn.setOutlineThickness(3.f);
    Stonebtn.setOutlineColor(sf::Color::Black);


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
                if (event.key.code == sf::Keyboard::Num2) brushSize = 2; // Средняя
                if (event.key.code == sf::Keyboard::Num3) brushSize = 5; // Большая
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
                if(Sandbtn.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = SAND;
                }
                else if(Waterbtn.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = WATER;
                }
                else if(Dirtbtn.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = DIRT;
                }
                else if(Stonebtn.getGlobalBounds().contains(mousePos.x,mousePos.y)){
                    currentItem = STONE;
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
            Sandbtn.setOutlineColor(sf::Color::White);
            Waterbtn.setOutlineColor(sf::Color::Black);
            Dirtbtn.setOutlineColor(sf::Color::Black);
            Stonebtn.setOutlineColor(sf::Color::Black);
            break;
        case WATER:
            Sandbtn.setOutlineColor(sf::Color::Black);
            Waterbtn.setOutlineColor(sf::Color::White);
            Dirtbtn.setOutlineColor(sf::Color::Black);
            Stonebtn.setOutlineColor(sf::Color::Black);
            break;
        case DIRT:
            Sandbtn.setOutlineColor(sf::Color::Black);
            Waterbtn.setOutlineColor(sf::Color::Black);
            Dirtbtn.setOutlineColor(sf::Color::White);
            Stonebtn.setOutlineColor(sf::Color::Black);
            break;
        case STONE:
            Sandbtn.setOutlineColor(sf::Color::Black);
            Waterbtn.setOutlineColor(sf::Color::Black);
            Dirtbtn.setOutlineColor(sf::Color::Black);
            Stonebtn.setOutlineColor(sf::Color::White);
            break;
        }

        if (frameCount >= 60) { 
            window.setTitle("SandBox | FPS: " + std::to_string(static_cast<int>(fps)));
            frameCount = 0;
        }     

        frameCount++;

        engine.update();

        window.clear();
        engine.draw(window);

        window.draw(Sandbtn);
        window.draw(Waterbtn);
        window.draw(Dirtbtn);
        window.draw(Stonebtn);

        window.display();
    }

    return 0;
}
