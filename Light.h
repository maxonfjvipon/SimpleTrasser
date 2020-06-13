#ifndef TRASSER_LIGTH_H
#define TRASSER_LIGTH_H

#include "Sphere.h"

class Light {

    float intensity{};

public:
    Vec3f position;

    Light() = default;

    Light(const Vec3f &pos, const float &intensity) : position(pos), intensity(intensity) {}

    Vec3f getPosition() {
        return position;
    }

    float getIntensity() {
        return intensity;
    }

};


#endif //TRASSER_LIGTH_H
