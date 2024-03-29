#pragma once

#include <map>

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
    Material* material;

    Plane(Vec3 n, float d) : normal(n), distance(d) {};
    Plane() {};

    bool intersectsRay(Ray& r, float& d)
    {
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
    Material* material;

    Sphere(Vec3 c, float r) : center(c), radius(r) {};
    Sphere() {};

    bool intersectsRay(Ray& r, float& d)
    {
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
    Material* material;

    RectYZ(float x, Vec2 vertexA, Vec2 vertexB) : x(x), vertexA(vertexA), vertexB(vertexB)
    {
        normal = Vec3(1, 0, 0);
    };

    bool intersectsRay(Ray& r, float& d)
    {
        d = (x - r.origin.x) / r.direction.x;
        if (d < 0 || d > FLT_MAX)
        {
            return false;
        }

        float y = r.origin.y + d * r.direction.y;
        float z = r.origin.z + d * r.direction.z;

        if (y < vertexA.u || y > vertexB.u || z < vertexA.v || z > vertexB.v)
        {
            return false;
        }

        return true;
    };
};

class Scene
{
private:
    bool loadFromFile()
    {
        std::ifstream sceneFile("data/scene.txt");
        if (!sceneFile.is_open())
            return false;

        std::string objectType;
        std::string x, y, z, m;

        while (!sceneFile.eof())
        {
            sceneFile >> objectType;

            if (!objectType.compare(std::string("MAT")))
            {
                std::string r;
                sceneFile >> m >> x >> y >> z >> r;

                Material material = {};
                material.reflectedColor = Vec3(stof(x), stof(y), stof(z));
                material.reflectance = stof(r);

                sceneFile >> x >> y >> z;
                material.emittedColor = Vec3(stof(x), stof(y), stof(z));

                materials.insert(std::pair<int, Material>(stoi(m), material));
            }

            if (!objectType.compare(std::string("PLANE")))
            {
                std::string d;
                sceneFile >> x >> y >> z >> d >> m;

                Plane plane = {};
                plane.normal = Vec3(stof(x), stof(y), stof(z));
                plane.distance = stof(d);
                plane.material = &(materials.at(stoi(m)));
                planes.push_back(plane);
            }

            if (!objectType.compare(std::string("SPHERE")))
            {
                std::string r;
                sceneFile >> x >> y >> z >> r >> m;

                Sphere sphere = {};
                sphere.center = Vec3(stof(x), stof(y), stof(z));
                sphere.radius = stof(r);
                sphere.material = &(materials.at(stoi(m)));
                spheres.push_back(sphere);
            }

            if (!objectType.compare(std::string("RECT")))
            {
                std::string x1, y1, d;
                sceneFile >> x >> y >> x1 >> y1 >> d >> m;

                RectYZ rect = RectYZ(stof(d), Vec2(stof(x), stof(y)), Vec2(stof(x1), stof(y1)));
                rect.material = &(materials.at(stoi(m)));
                rects.push_back(rect);
            }

            if (!objectType.compare(std::string("#")))
            {
                std::getline(sceneFile, m);
            }

            objectType.clear();
        }

        return true;
    }

public:
    Scene()
    {
        if (!loadFromFile())
        {
            std::cerr << "ERROR: could not open the scene file\n";
            std::exit(EXIT_FAILURE);
        };
    };

    ~Scene() {};

    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
    std::vector<RectYZ> rects;
    std::map<int, Material> materials;
};
