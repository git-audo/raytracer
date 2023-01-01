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
    Vec2 vertexA, vertexB;
    Vec3 normal;
    Material material;

    RectYZ(float x, Vec2 vertexA, Vec2 vertexB) : x(x), vertexA(vertexA), vertexB(vertexB) {
        normal = Vec3(1, 0, 0);
    };

    bool intersectsRay(Ray& r, float& d) {
        d = (x - r.origin.x) / r.direction.x;
        if (d < 0 || d > FLT_MAX){
            return false;
        }

        float y = r.origin.y + d * r.direction.y;
        float z = r.origin.z + d * r.direction.z;

        if (y < vertexA.u || y > vertexB.u || z < vertexA.v || z > vertexB.v) {
            return false;
        }

        return true;
    };
};

class Scene
{
public:
    Scene() {
        if (!loadFromFile()) {
            // Handle scene file error
        };
    };

    ~Scene() {};

    bool loadFromFile() {
        std::ifstream sceneFile("data/scene.txt");
        if (!sceneFile.is_open())
            return false;

        std::string objectsCount;
        sceneFile >> objectsCount;

        std::cout << objectsCount << std::endl;

        std::string line;
        std::string objectType;
        std::string x, y, z;

        // remove materials
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


        while ( !sceneFile.eof() ) {
            sceneFile >> objectType;

            if (!objectType.compare(std::string("PLANE"))) {
                std::string d;
                sceneFile >> x >> y >> z >> d;

                Plane plane = {};
                plane.normal = Vec3(stof(x), stof(y), stof(z));
                plane.distance = stof(d);
                plane.material = groundMaterial;
                planes.push_back(plane);
            }

            if (!objectType.compare(std::string("SPHERE"))) {
                std::string r;
                sceneFile >> x >> y >> z >> r;

                Sphere sphere = {};
                sphere.center = Vec3(stof(x), stof(y), stof(z));
                sphere.radius = stof(r);
                sphere.material = matA;
                spheres.push_back(sphere);
            }

            if (!objectType.compare(std::string("RECT"))) {
                std::string x1, y1, d;
                sceneFile >> x >> y >> x1 >> y1 >> d;

                RectYZ rect = RectYZ(stof(d), Vec2(stof(x), stof(y)), Vec2(stof(x1), stof(y1)));
                rect.material = emitter;
                rects.push_back(rect);
            }

            objectType.clear();
        }

        return true;
    }

    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
    std::vector<RectYZ> rects;
};
