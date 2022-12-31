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

struct RectYZ
{
    float x;
    float y0, z0, y1, z1;
    Vec3 normal;
    Material material;

    RectYZ(float x, float y0, float z0, float y1, float z1) : x(x), y0(y0), z0(z0), y1(y1), z1(z1) {
        normal = Vec3(1, 0, 0);
    };

    bool intersectsRay(Ray& r, float& d) {
        d = (x - r.origin.x) / r.direction.x;
        if (d < 0 || d > FLT_MAX){
            return false;
        }

        float y = r.origin.y + d * r.direction.y;
        float z = r.origin.z + d * r.direction.z;

        if (y < y0 || y > y1 || z < z0 || z > z1) {
            return false;
        }

        return true;
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
        Material greenEmitter = {};
        greenEmitter.emittedColor = Vec3(0.50f, 0.70f, 0.58f);
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

        Sphere sphereD = {};
        sphereD.center = Vec3(-1.5f, -5.2f, 0);
        sphereD.radius = 4.0f;
        sphereD.material = matD;

        Sphere lightA = {};
        lightA.center = Vec3(0, 1.5f, 0);
        lightA.radius = 0.45f;
        lightA.material = emitter;

        Sphere lightB = {};
        lightB.center = Vec3(1.2f, 0.5f, 2.2f);
        lightB.radius = 0.35f;
        lightB.material = greenEmitter;

        RectYZ rect = RectYZ(2, 1.0f, 1.5f, 3.0f, 4.5f);
        rect.material = emitter;

        planes.push_back(plane);
        spheres.push_back(sphereA);
        spheres.push_back(sphereB);
        spheres.push_back(sphereD);
        spheres.push_back(lightA);
        spheres.push_back(lightB);
        rects.push_back(rect);
    };

    ~Scene() {};

    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
    std::vector<RectYZ> rects;
};
