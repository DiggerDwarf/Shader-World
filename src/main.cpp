#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

#define WIDTH  1280
#define HEIGHT 720
#define MOV_SPEED 5
#define ROT_SPEED 10

int reflections = 3;

sf::Transform rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return sf::Transform(
        c, 0,-s,
        0, 1, 0,
        s, 0, c
    );
}
sf::Transform rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return sf::Transform(
        c, s, 0,
       -s, c, 0,
        0, 0, 1
    );
}
sf::Transform rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return sf::Transform(
        1, 0, 0,
        0, c, s,
        0,-s, c
    );
}

void Draw(sf::RenderWindow& window, sf::ConvexShape& quad, sf::Shader& shader, sf::Vector3f& pos, sf::Transform& rot) {
    shader.setUniform("u_resolution", sf::Glsl::Vec2(window.getSize()));
    shader.setUniform("u_camera_pos", sf::Glsl::Vec3(pos));
    shader.setUniform("nb_reflections", float(reflections));
    // shader.setUniform("camera_rot", sf::Glsl::Mat4(rot));
    // shader.setUniform("u_color", sf::Glsl::Vec4(sf::Color(106, 61, 158)));
    window.draw(quad, &shader);
    window.display();
}

void Update(sf::RenderWindow& window, sf::Vector3f& pos, sf::Transform& rot, sf::Clock& clock) {

    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        
        default:
            break;
        }
    }
    

    float dt = clock.restart().asSeconds();
    std::cout << 1/dt << "\n";


    /* Camera movement */ {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) { pos.z += MOV_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { pos.z -= MOV_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { pos.x -= MOV_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { pos.x += MOV_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { pos.y += MOV_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { pos.y -= MOV_SPEED * dt; }
    }

    

    /* Camera rotation */ {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) { rot*= rotation_x( ROT_SPEED * dt); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) { rot*= rotation_x(-ROT_SPEED * dt);; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) { pos.x -= ROT_SPEED * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) { pos.x += ROT_SPEED * dt; }
    }


}

int main(int argc, char const *argv[])
{
    if (argc >= 2) reflections = std::stoi(argv[1]);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Shader World !", sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(60U);
    sf::Clock clock;
    sf::Shader shader;
    if (!shader.loadFromFile("shaders/fragment.glsl", sf::Shader::Fragment)) {
        return 1;
    }
    sf::ConvexShape quad(4);
    quad.setPoint(0, sf::Vector2f(0,     0     ));
    quad.setPoint(1, sf::Vector2f(WIDTH, 0     ));
    quad.setPoint(2, sf::Vector2f(WIDTH, HEIGHT));
    quad.setPoint(3, sf::Vector2f(0,     HEIGHT));

    sf::Vector3f camera_pos(0, 0, -3);
    sf::Transform camera_rot;

    while (window.isOpen())
    {
        Draw(window, quad, shader, camera_pos, camera_rot);
        Update(window, camera_pos, camera_rot, clock);
    }
}