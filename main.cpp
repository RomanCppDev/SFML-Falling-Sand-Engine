#include "engine.h"
#include "ui.h"

void usePanel(const int& maxPanelHeight, const int& minPanelHeight, sf::RectangleShape& panel, sf::Clock& animationClock, bool& isMovingToUp, bool& isPanelMoving);
void closePanel(sf::RectangleShape& panel, sf::Clock& animationClock);

int main() {
    sf::Clock clock;
    int frameCount = 0; 
    UI ui;

    // Окно
    sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, (HEIGHT - 1) * CELL_SIZE + ui.getMinPanelHeight()), "SandBox");
    window.setFramerateLimit(140);
    window.setVerticalSyncEnabled(false);

    SandBoxEngine engine;
    unsigned short brushSize = 2;

    while (window.isOpen()) {
        float frameTime = clock.restart().asSeconds();
        float fps = 1.0f / frameTime;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) brushSize = 0;  // Точечная кисть
                if (event.key.code == sf::Keyboard::Num2) brushSize = 5;  // Средняя
                if (event.key.code == sf::Keyboard::Num3) brushSize = 8;  // Большая
                if (event.key.code == sf::Keyboard::Num4) brushSize = 13; // Огромная
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                ui.MouseButtonPressed(engine, window);
            }
        }
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            ui.isLeftButtonPressed(engine, brushSize, mousePos);
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            ui.isRightButtonPressed(engine, brushSize, mousePos);
        }

        if (frameCount >= 140) {
            window.setTitle("SandBox | FPS: " + std::to_string(static_cast<int>(fps)));
            frameCount = 0;
        }

        ui.MovePanel();
        
        frameCount++;

        engine.update();

        window.clear();
        engine.draw(window);
        ui.draw(window);
        window.display();
    }

    return 0;
}
