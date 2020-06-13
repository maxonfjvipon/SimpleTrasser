#include "Sphere.h"

Sphere::Sphere(const Vec3f &center, const float &radius, const Material &material) {
    this->center = center;
    this->radius = radius;
    this->material = material;
}

bool Sphere::isRayIntersectSphere(const Vec3f &original, const Vec3f &direction,
                                  float &distance) const {
    Vec3f fromOrigToCenter = center - original; // from original to center vector
    float fromOrigToProjDistance = fromOrigToCenter * direction; // from original to ray projection
    // distance
    // from center to ray projection distance ^ 2
    float fromCenterToProjDistance = fromOrigToCenter * fromOrigToCenter
                                     - fromOrigToProjDistance * fromOrigToProjDistance;
    if (fromCenterToProjDistance > radius * radius) {
        return false;
    }
    float fromIntersectionPointToProjectionPointDistance = sqrtf(radius * radius
            - fromCenterToProjDistance);
    // distance from original to intersection point 0
    distance = fromOrigToProjDistance - fromIntersectionPointToProjectionPointDistance;
    // distance from original to intersection point 1
    float distance2 = fromOrigToProjDistance + fromIntersectionPointToProjectionPointDistance;
    // check distance > 0
    if (distance < 0) {
        distance = distance2;
    }
    return distance >= 0;
}


