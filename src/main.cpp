#define SFML_STATIC
#include <iostream>
#include <streambuf>
#include <math.h>
// #include <string>
// #include <shobjidl.h>

#include <windows.h>

// #include "earth.h"

#include <SFML/Graphics.hpp>

#define WIDTH  1280
#define HEIGHT 720
#define MOV_SPEED 5
#define ROT_SPEED 2
#define SENSITIVITY 0.005

#define NB_SHADERS 1

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

// sf::Image createHeaderImage()
// {
//     sf::Image returnImage;
//     returnImage.create(width, height);

// }

void Draw(sf::RenderWindow* window, sf::ConvexShape* quad, sf::Shader* (& shaders)[NB_SHADERS], sf::Vector3f* pos, float rot[], sf::Texture* tex) {
    window->clear();
    shaders[0]->setUniform("u_resolution", sf::Glsl::Vec2(window->getSize()));
    // shader->setUniform("u_camera_pos", sf::Glsl::Vec3(*pos));
    // shader->setUniform("nb_reflections", float(reflections));
    shaders[0]->setUniform("u_camera_rot", sf::Glsl::Mat3(  rotation_x(rot[1]) * rotation_y(rot[0])  ));
    shaders[0]->setUniform("u_texture", *tex);
    
    // shader.setUniform("u_color", sf::Glsl::Vec4(sf::Color(106, 61, 158)));
    window->draw(*quad, shaders[0]);
    window->display();
}

void Update(sf::RenderWindow& window, sf::Vector3f& pos, float rot[], sf::Clock& clock, sf::Vector2i& mpos, bool& mpressed, sf::Texture* tex, sf::Shader* (& shaderArray)[NB_SHADERS]) {
    
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) window.close();
                // else if (event.key.code == sf::Keyboard::Enter)
                // {
                //     sf::Shader* temp = shaderArray[0];
                //     shaderArray[0] = shaderArray[1];
                //     shaderArray[1] = temp;
                // }
                
                else if (event.key.code == sf::Keyboard::O && event.key.control)
                {
                    char filename[ MAX_PATH ];

                    OPENFILENAMEA ofn;
                        ZeroMemory( &filename, sizeof( filename ) );
                        ZeroMemory( &ofn,      sizeof( ofn ) );
                        ofn.lStructSize  = sizeof( ofn );
                        ofn.hwndOwner    = window.getSystemHandle();  // If you have a window to center over, put its HANDLE here
                        ofn.lpstrFilter  = "Any File\0*.*\0";
                        ofn.lpstrFile    = filename;
                        ofn.lpstrInitialDir = "%homePath%/Pictures";
                        ofn.nMaxFile     = MAX_PATH;
                        ofn.lpstrTitle   = "Select an image to open :";
                        ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameA( &ofn ))
                    {
                        if (!tex->loadFromFile(filename))
                        {
                            // std::cout << "Error while trying to load file.\n";
                        }
                    }
                    else
                    {
                        // std::cout << "Error while trying to open file.\n";
                    }
                }
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


    // /* Camera movement */ {
    //     float s = sin(rot[0]);
    //     float c = cos(rot[0]);
    //     float dm = dt * MOV_SPEED;

    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) { pos += sf::Vector3f( s, 0, c) * dm; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { pos -= sf::Vector3f( s, 0, c) * dm; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { pos += sf::Vector3f(-c, 0, s) * dm; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { pos -= sf::Vector3f(-c, 0, s) * dm; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { pos.y += dm; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { pos.y -= dm; }
    // }

    

    /* Camera rotation */ {
        float dr = dt * ROT_SPEED;

        if (mpressed)
        {
            sf::Vector2i npos(sf::Mouse::getPosition());
            sf::Vector2i moffset(mpos - npos);
            mpos = npos;
            rot[0] -= moffset.x * SENSITIVITY;
            rot[1] -= moffset.y * SENSITIVITY;
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
    extern const char _binary_resources_earth_jpg_start[], _binary_resources_earth_jpg_size[];
    const char* binImgStart = _binary_resources_earth_jpg_start;
    size_t binImgSize = (size_t)_binary_resources_earth_jpg_size;

    
    sf::String temp("earth.jpeg");
    // if (argc >= 2) reflections = std::stoi(argv[1]);
    if (argc >= 2) temp = argv[1];
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Shader World !", sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(60U);
    sf::Clock clock;
    sf::Shader shader;
    sf::Texture image;
    sf::Clock clock2;
    // image.loadFromFile("resources/" + temp);

    image.loadFromMemory(binImgStart, binImgSize);

    sf::Shader* shaders[NB_SHADERS];

    sf::Shader sphereShader;
    // sf::Shader panoShader;

    const char sphereShaderCode[907] = \
        "uniform vec2        u_resolution; \n\
        uniform mat3        u_camera_rot; \n\
        uniform sampler2D   u_texture; \n\
        #define M_PI		3.14159265358979323846 \n\
        #define M_PI_2		1.57079632679489661923 \n\
        #define M_1_PI		0.31830988618379067154 \n\
        vec2 coord(in vec2 p) { \n\
            p = p / u_resolution.xy *2.0 -1.0; \n\
            if (u_resolution.x > u_resolution.y) { \n\
                p.x *= u_resolution.x / u_resolution.y; \n\
            } else { \n\
                p.y *= u_resolution.y / u_resolution.x; \n\
            } \n\
            return p; \n\
        } \n\
        void main() { \n\
            vec2 uv = -coord(gl_FragCoord.xy); \n\
            vec3 rd = normalize(vec3(uv*0.8, 1)) * u_camera_rot; \n\
            float theta = (atan(rd.z, rd.x)+M_PI)/(2*M_PI); \n\
            float phi   = (asin(rd.y)+M_PI_2)*M_1_PI; \n\
            gl_FragColor = texture(u_texture, vec2(theta, phi)); \n\
        }";

    if (!sphereShader.loadFromMemory(sphereShaderCode, sf::Shader::Fragment)) {
        return 1;
    }
    // if (!panoShader.loadFromFile("shaders/panorama.glsl", sf::Shader::Fragment)) {
    //     return 1;
    // }

    shaders[0] = &sphereShader;
    // shaders[1] = &panoShader;

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
        Draw(&window, &quad, shaders, &camera_pos, camera_rot, &image);
        Update(window, camera_pos, camera_rot, clock, mouse_pos, is_mouse_pressed, &image, shaders);
    }
}