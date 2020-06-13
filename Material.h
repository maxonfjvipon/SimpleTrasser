#ifndef TRASSER_MATERIAL_H
#define TRASSER_MATERIAL_H

#include "geometry.h"

class Material {

    Vec3f albedo;
    Vec3f color;
    float specularExponent {};


public:

    Material(const Vec3f &a, const Vec3f &color, const float &spec) {
        this->albedo = a;
        this->color = color;
        this->specularExponent = spec;
    }

    Material() {
        albedo = Vec3f(1.0,0.0,0.0);
    }

    Vec3f getAlbedo() {
        return albedo;
    }

    float getSpecExp() {
        return specularExponent;
    }

    Vec3f &getColor() {
        return color;
    }

};


#endif //TRASSER_MATERIAL_H
