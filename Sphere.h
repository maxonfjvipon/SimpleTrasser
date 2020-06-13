#ifndef TRASSER_SPHERE_H
#define TRASSER_SPHERE_H

#include "Material.h"

class Sphere {

    float radius;
    Material material;

public:
    Vec3f center;
    Sphere(const Vec3f &center, const float &radius, const Material &material);

    // луч исходит из orig в направлении dir
    // пересекается ли луч со сферой
    bool isRayIntersectSphere(const Vec3f &orig, const Vec3f &dir, float &inter0) const;

    Material getMaterial() {
        return material;
    }

    Vec3f getCenter() {
        return center;
    }

    float getRadius() {
        return radius;
    }

};


#endif //TRASSER_SPHERE_H
