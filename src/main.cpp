#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

#define WIDTH  1280
#define HEIGHT 720
#define MOV_SPEED 5
#define ROT_SPEED 2
#define SENSITIVITY 0.005 

int reflections = 3;

template <typename T>
void clamp(T& val, T low, T high){
    val = std::min(std::max(low, val), high);
}

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

void Draw(sf::RenderWindow& window, sf::ConvexShape& quad, sf::Shader& shader, sf::Vector3f& pos, float rot[]) {
    shader.setUniform("u_resolution", sf::Glsl::Vec2(window.getSize()));
    shader.setUniform("u_camera_pos", sf::Glsl::Vec3(pos));
    shader.setUniform("nb_reflections", float(reflections));
    shader.setUniform("u_camera_rot", sf::Glsl::Mat3(  rotation_x(rot[1]) * rotation_y(rot[0])  ));
    // shader.setUniform("u_color", sf::Glsl::Vec4(sf::Color(106, 61, 158)));
    window.draw(quad, &shader);
    window.display();
}

void Update(sf::RenderWindow& window, sf::Vector3f& pos, float rot[], sf::Clock& clock, sf::Vector2i& mpos, bool& mpressed) {

    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                mpressed = true;
                mpos = sf::Mouse::getPosition();
                break;
            case sf::Event::MouseButtonReleased:
                mpressed = false;
                break;
            default:
                break;
        }
    }
    

    float dt = clock.restart().asSeconds();
    std::cout << 1/dt << "\n";


    /* Camera movement */ {
        float s = sin(rot[0]);
        float c = cos(rot[0]);
        float dm = dt * MOV_SPEED;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) { pos += sf::Vector3f( s, 0, c) * dm; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { pos -= sf::Vector3f( s, 0, c) * dm; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { pos += sf::Vector3f(-c, 0, s) * dm; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { pos -= sf::Vector3f(-c, 0, s) * dm; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { pos.y += dm; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { pos.y -= dm; }
    }

    

    /* Camera rotation */ {
        float dr = dt * ROT_SPEED;

        if (mpressed)
        {
            sf::Vector2i npos(sf::Mouse::getPosition());
            sf::Vector2i moffset(mpos - npos);
            mpos = npos;
            rot[0] += moffset.x * SENSITIVITY;
            rot[1] += moffset.y * SENSITIVITY;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) { rot[1] += dr; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) { rot[1] -= dr; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) { rot[0] -= dr; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) { rot[0] += dr; }
    }
    clamp(rot[1], (float)-M_PI_2, (float)M_PI_2);

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
    float camera_rot[2] = {0.F,0.F};

    sf::Vector2i mouse_pos(0, 0);
    bool is_mouse_pressed(false);

    while (window.isOpen())
    {
        Draw(window, quad, shader, camera_pos, camera_rot);
        Update(window, camera_pos, camera_rot, clock, mouse_pos, is_mouse_pressed);
    }
}