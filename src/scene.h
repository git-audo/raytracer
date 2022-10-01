#pragma once

struct Material
{
    Vec3 emittedColor;
    Vec3 reflectedColor;
    float reflectance;

    Material() {};
};

struct Plane
{
    Vec3  normal;
    float distance;
    Material material;

    Plane(Vec3 n, float d) : normal(n), distance(d) {};
    Plane() {};

    bool intersectsRay(Ray& r, float& d) {
        float numerator   = - distance - normal.inner(r.origin);
        float denominator = normal.inner(r.direction);

        float epsilon = 0.0001f;
        if (denominator < -epsilon || denominator > epsilon)
        {
            d = numerator / denominator;
            if (d > 0 && d < FLT_MAX)
            {
                return true;
            }
        }

        return false;
    };
};

struct Sphere
{
    Vec3  center;
    float radius;
    Material material;

    Sphere(Vec3 c, float r) : center(c), radius(r) {};
    Sphere() {};

    bool intersectsRay(Ray& r, float& d) {
        float a = r.direction.inner(r.direction);
        float b = (2 * r.direction.inner(r.origin));
        float c = r.origin.inner(r.origin) - radius * radius;

        float discriminant = b * b -  4.0f * a * c;
        if (discriminant >= 0)
        {
            float squared = sqrt(discriminant);
            float posNum = - b + squared;
            float negNum = - b - squared;

            float posRoot = posNum / 2 * a;
            float negRoot = negNum / 2 * a;

            d = posRoot < negRoot ? posRoot : negRoot;

            float epsilon = 0.0001f;
            if (d > epsilon)
            {
                return true;
            }
        }

        return false;
    };
};

class Scene
{
public:
    Scene() {
        Material groundMaterial = {};
        groundMaterial.reflectedColor = Vec3(0.2f, 0.3f, 0.5f);
        groundMaterial.reflectance = 0.2f;
        Material matA = {};
        matA.reflectedColor = Vec3(0.15f, 0.15f, 0.19f);
        matA.reflectance = 0.92f;
        Material matB = {};
        matB.reflectedColor = Vec3(0.98f, 0.98f, 0.98f);
        matB.reflectance = 0.2f;
        Material emitter = {};
        emitter.emittedColor = Vec3(0.50f, 0.70f, 0.88f);
        Material matD = {};
        matD.reflectedColor = Vec3(0.58f, 0.28f, 0.28f);
        matD.reflectance = 0.2f;

        Plane plane = {};
        plane.normal = Vec3(0, 0.02f, 1.0f);
        plane.distance = 0;
        plane.material = groundMaterial;

        Sphere sphereA = {};
        sphereA.center = Vec3(-1.2f, 0, 1.2f);
        sphereA.radius = 1.0f;
        sphereA.material = matA;

        Sphere sphereB = {};
        sphereB.center = Vec3(1.5f, 1, 0.6f);
        sphereB.radius = 1.0f;
        sphereB.material = matB;

        Sphere sphereC = {};
        sphereC.center = Vec3(0, 1.5f, 0);
        sphereC.radius = 0.45f;
        sphereC.material = emitter;

        Sphere sphereD = {};
        sphereD.center = Vec3(-1.5f, -5.2f, 0);
        sphereD.radius = 4.0f;
        sphereD.material = matD;

        planes.push_back(plane);
        spheres.push_back(sphereA);
        spheres.push_back(sphereB);
        spheres.push_back(sphereC);
        spheres.push_back(sphereD);
    };

    ~Scene() {};

    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
};
