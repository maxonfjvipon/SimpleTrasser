#include <iostream>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Sphere.h"
#include "Light.h"
#include "stb-master/stb_image.h"
#include "stb-master/stb_image_write.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

bool sceneIntersect(const Vec3f &original, const Vec3f &direction, std::vector<Sphere> &spheres,
                    Vec3f &fromOrigToIntPointVector, Vec3f &intPointNormVector,
                    Material &material) {
    float spheresDist = std::numeric_limits<float>::max(); // so much number
    for (auto &sphere : spheres) {
        float currentDist; // from from sphere's center to intersection point distance
        if (sphere.isRayIntersectSphere(original, direction, currentDist)
            && currentDist < spheresDist) {
            spheresDist = currentDist; // the smallest distance
            // set from original to intersection point vector
            fromOrigToIntPointVector = original + direction * spheresDist;
            // set intersection point normalize vector
            intPointNormVector = (fromOrigToIntPointVector - sphere.getCenter()).normalize();
            material = sphere.getMaterial(); // set result material
        }
    }
    // distance from original to board
    float boardDist = std::numeric_limits<float>::max(); // so much number
    if (fabs(direction.y) > 1e-3) { // if direction.y != 0
        float boardD = -(original.y + 4) / direction.y;
        Vec3f fromOriginalToBoardVector = original + direction * boardD;
        if (boardD > 0 && fabs(fromOriginalToBoardVector.x) < 10
            && fromOriginalToBoardVector.z < -10 && fromOriginalToBoardVector.z > -30
            && boardD < spheresDist) {
            boardDist = boardD;
            fromOrigToIntPointVector = fromOriginalToBoardVector;
            intPointNormVector = Vec3f(0, 1, 0);
            material.getColor() = (int(.5 * fromOrigToIntPointVector.x + 1000) +
                                   int(.5 * fromOrigToIntPointVector.z)) & 1 ? Vec3f(.3, .3, .3)
                                                                             : Vec3f(.3, .2, .1);
        }
    }
    return std::min(spheresDist, boardDist) < 1000;
}

Vec3f reflect(const Vec3f &negLigthDir, const Vec3f &intPointNormVector) { // return reflect vector
    return negLigthDir - intPointNormVector * 2 * (negLigthDir * intPointNormVector);
}

// original = {0,0,0}, direction - from original to current point on screen
// directional.length = 1
Vec3f setPixelColor(const Vec3f &original, const Vec3f &direction, std::vector<Sphere> &spheres,
                    std::vector<Light> &lights, uint depth = 0) {
    Vec3f fromOrigToIntPointVector; // from orig to intersection point
    Vec3f intPointNormVector; // intersection point normalize vector
    // inpv - from center to intersection point vector with size of one(1)
    Material material; // result material
    // if ray doesnt intersect any object -> return background color
    if (depth > 5 || !sceneIntersect(original, direction, spheres, fromOrigToIntPointVector,
                                     intPointNormVector,
                                     material)) {
        return {0.2, 0.7, 0.6};
    }
    //
    float diffuseLightIntensity = 0;
    float specularLightIntensity = 0;
    float fromIPToLightDistance = 0;
    Vec3f lightDirection, fromShadowOrigToLightOrigVector, shadowNormVector;
    // direction for reflect ray of intersection point
    Vec3f reflectDirection = reflect(direction, intPointNormVector).normalize();

    // skew intersection point offset for reflect
    Vec3f reflectOriginal = reflectDirection * intPointNormVector < 0 ?
                            fromOrigToIntPointVector - intPointNormVector * 1e-3 :
                            fromOrigToIntPointVector + intPointNormVector * 1e-3;
    Vec3f reflectColor = setPixelColor(reflectOriginal, reflectDirection, spheres, lights,
                                       depth + 1);

    // skew intersection point offset for shadows
    Vec3f shadowOriginal = lightDirection * intPointNormVector < 0 ?
                           fromOrigToIntPointVector - intPointNormVector * 1e-3 :
                           fromOrigToIntPointVector + intPointNormVector * 1e-3;
//    Vec3f reflect_color = fillPixel(reflect_orig, reflectDir, spheres, lights, depth + 1);
    for (auto light : lights) {
        // set from intersection point to light distance
        fromIPToLightDistance = (light.getPosition() - fromOrigToIntPointVector).norm();
        // set from light.pos to intersection point direction
        lightDirection = (light.getPosition() - fromOrigToIntPointVector).normalize();
        Material tempMaterial;
        if (sceneIntersect(shadowOriginal, lightDirection, spheres,
                           fromShadowOrigToLightOrigVector, shadowNormVector, tempMaterial)
            && (fromShadowOrigToLightOrigVector - shadowOriginal).norm() < fromIPToLightDistance) {
            continue;
        }
        // set diffuse intensity
        diffuseLightIntensity += light.getIntensity()
                                 * std::max(0.f, lightDirection * intPointNormVector);
        // set specular intensity
        specularLightIntensity += powf(-reflect(-lightDirection, intPointNormVector) * direction,
                                       material.getSpecExp()) * light.getIntensity();
    }
    return material.getColor() * diffuseLightIntensity * material.getAlbedo()[0] +
           Vec3f(1, 1, 1) * specularLightIntensity * material.getAlbedo()[1] +
           reflectColor * material.getAlbedo()[2];
}

void fillPicture(const uint width, const uint height, std::vector<Vec3f> &frameBuffer) {
    std::vector<unsigned char> pixmap(width * height * 3);
    for (uint i = 0; i < height * width; i++) {
        for (uint j = 0; j < 3; j++) {
            pixmap[i * 3 + j] = (u_char) (255 * std::max(0.f, std::min(1.f, frameBuffer[i][j])));
        }
    }
    stbi_write_jpg("pict.jpg", width, height, 3, pixmap.data(), 100);
//    stbi_write_png("picture.png",width,height,3,pixmap.data(),100);
}

void render(ushort width, ushort height, std::vector<Sphere> &spheres, std::vector<Light> &lights) {
    const int fov = static_cast<const int>(M_PI_2); // fov - угол обзора(90гр)
    double x, y;
    Vec3f direction; // direction
    std::vector<Vec3f> frameBuffer(width * height);
    for (uint j = 0; j < height; j++) {
        for (uint i = 0; i < width; i++) {
            // set x and y of point on screen
            x = (2 * (i + 0.5) / (float) width - 1) * tan(fov / 2.) * width / (float) height;
            y = -(2 * (j + 0.5) / (float) height - 1) * tan(fov / 2.);
            // set direction, length = 1
            direction = Vec3f(static_cast<float>(x), static_cast<float>(y), -1).normalize();
            frameBuffer[i + j * width] = setPixelColor(Vec3f(0, 0, 0), direction, spheres, lights);
        }
    }
    fillPicture(width, height, frameBuffer);
}

void restartClock(sf::Clock &clock, float &time) {
    time += clock.getElapsedTime().asSeconds();
    clock.restart();
}

int main() {

    const ushort screenWidth = 100;
    const ushort screenHeight = 70;

    Material ivory(Vec3f(0.6, 0.3, 0.1), Vec3f(0.4, 0.4, 0.3), 50.);
    Material redRubber(Vec3f(0.9, 0.1, 0.0), Vec3f(0.3, 0.1, 0.1), 10.);
    Material mirror(Vec3f(0.0, 10.0, 0.8), Vec3f(1.0, 1.0, 1.0), 1425.);

    std::vector<Sphere> spheres;

    spheres.emplace_back(Vec3f(-3, 0, -16), 2, ivory);
    spheres.emplace_back(Vec3f(-1.0, -1.5, -12), 2, mirror);
    spheres.emplace_back(Vec3f(1.5, -0.5, -18), 3, redRubber);
    spheres.emplace_back(Vec3f(7, 5, -18), 3, mirror);

    std::vector<Light> lights;

    lights.emplace_back(Vec3f(-20, 20, 20), 1.5);
    lights.emplace_back(Vec3f(30, 50, -25), 1.8);
    lights.emplace_back(Vec3f(30, 20, 30), 1.7);

    render(screenWidth, screenHeight, spheres, lights);

    sf::Texture t;
    t.loadFromFile("pict.jpg");
    sf::Sprite sprite;
    sprite.setTexture(t);
    sprite.setTextureRect(sf::IntRect(0, 0, 100, 70));

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Trasser");
    sf::Event event {};
    sf::Clock clock;

    float time = 0; // time
    float delay = 0.12; // time delay

    while (window.isOpen()) {
        restartClock(clock, time);
        window.clear();
        if (time > delay) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    for (auto &sphere : spheres) {
                        sphere.center.x--;
                    }
                    for (auto &light : lights) {
                        light.position.x--;
                    }
                    render(screenWidth, screenHeight, spheres, lights);
                    t.loadFromFile("pict.jpg");
                    sprite.setTexture(t);
                    sprite.setTextureRect(sf::IntRect(0, 0, screenWidth, screenHeight));
                }
            }
            window.draw(sprite);
            window.display();
        }
    }

    return 0;
}