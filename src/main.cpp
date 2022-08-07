#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>

float randomFloat()
{
    return (float)rand() / (float)RAND_MAX;
}

struct Vec3
{
    float x=0, y=0, z=0;

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {};
    Vec3() {};

    Vec3 operator+(const Vec3& v) const { return {x+v.x, y+v.y, z+v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x-v.x, y-v.y, z-v.z}; }
    Vec3 operator*(const Vec3& v) const { return {x*v.x, y*v.y, z*v.z}; }
    Vec3 operator+(const float d) const { return {x+d, y+d, z+d}; }
    Vec3 operator-(const float d) const { return {x-d, y-d, z-d}; }
    Vec3 operator*(const float d) const { return {x*d, y*d, z*d}; }
    Vec3 operator/(const float d) const { return {x/d, y/d, z/d}; }

    bool operator!=(const Vec3& v) const {
        if (x == v.x && y == v.y && z == v.z) return false;
        return true;
    }

    float length() { return std::sqrt(x*x + y*y + z*z); };

    Vec3 normalize() {
        float l = length();
        Vec3 v = {};

        if (l != 0)
        {
            v.x = x / l;
            v.y = y / l;
            v.z = z / l;
        }

        return v;
    }

    Vec3 cross(const Vec3& v) {
        Vec3 result = {};
        result.x = y * v.z - z * v.y;
        result.y = x * v.z - z * v.x;
        result.z = x * v.y - y * v.x;

        return result;
    }

    float inner(const Vec3& v) {
        return x * v.x + y * v.y + z * v.z;
    }

    static Vec3 randomized() {
        Vec3 v = {};

        v.x = randomFloat() * 2.0f - 1;
        v.y = randomFloat() * 2.0f - 1;
        v.z = randomFloat() * 2.0f - 1;

        return v;
    }
};

Vec3 lerp(Vec3 start, Vec3 end, float t)
{
    return start * (1 - t) + end * t;
}

struct Camera
{
    Vec3 pos;

    Vec3 X;
    Vec3 Y;
    Vec3 Z;

    Camera(Vec3 p, Vec3 zAxys) : pos(p) {
        Z = pos.normalize();
        X = Z.cross(zAxys).normalize();
        Y = Z.cross(X).normalize();
    };
};

struct Material
{
    Vec3 emittedColor;
    Vec3 reflectedColor;
    float reflectance;

    Material() {};
};

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() {};
    Ray(Vec3 o, Vec3 d) : origin(o), direction(d) {};
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

struct Scene
{
    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
};

Vec3 raycast(Ray ray, Scene& scene)
{
    Material background = {};
    background.emittedColor = Vec3(0.6f, 0.6f, 0.9f);

    float minDistance = FLT_MAX;
    Vec3 attenuation = Vec3(1, 1, 1);
    Vec3 finalColor = {};

    Material previousMaterial = {};
    Material nextMaterial = background;
    Vec3 nextOrigin = Vec3();
    Vec3 nextNormal = Vec3();

    uint8_t castingDistance = 4;
    for (uint8_t i = 0; i < castingDistance; i++)
    {
        for (Plane p : scene.planes)
        {
            float distance = 0;
            if (p.intersectsRay(ray, distance))
            {
                nextOrigin = ray.origin + ray.direction * distance;
                nextNormal = p.normal;
                nextMaterial = p.material;
                minDistance = distance;
            }
        }

        for (Sphere s : scene.spheres)
        {
            // we translate the rayOrigin to compensate for a sphere not in the origin
            Vec3 relativeRayOrigin = ray.origin - s.center;
            Ray r = Ray(relativeRayOrigin, ray.direction);

            float distance = 0;
            if (s.intersectsRay(r, distance) && distance < minDistance)
            {
                nextOrigin = ray.origin + ray.direction * distance;
                nextNormal = (nextOrigin - s.center).normalize();
                nextMaterial = s.material;
                minDistance = distance;
            }
        }

        if (minDistance != FLT_MAX)
        {
            Vec3 adiacentSphereCenter = ray.origin + nextNormal * 1.2;
            Vec3 randomDirection = ((adiacentSphereCenter + Vec3::randomized()) - ray.origin).normalize();

            ray.direction = lerp(randomDirection, nextNormal, nextMaterial.reflectance);
            ray.origin = nextOrigin;

            finalColor = finalColor + nextMaterial.emittedColor * attenuation;
            attenuation = nextMaterial.reflectedColor * attenuation;
            previousMaterial = nextMaterial;

            minDistance = FLT_MAX;
        }
        else
        {
            finalColor = finalColor + background.emittedColor * attenuation;
            break;
        }
    }

    return finalColor;
}

int main(int argc, char** argv)
{
    const int WIDTH   = 1280;
    const int HEIGHT  = 720;

    Vec3* framebuffer = new Vec3[WIDTH * HEIGHT];

    Vec3 xAxys = Vec3(1, 0, 0);
    Vec3 yAxys = Vec3(0, 1, 0);
    Vec3 zAxys = Vec3(0, 0, 1);

    Camera camera = Camera(Vec3(0, 7, 1), zAxys);
    camera.pos = camera.pos + Vec3(0, 0, 0.3f);

    Material groundMaterial = {};
    groundMaterial.reflectedColor = Vec3(0.3f, 0.3f, 0.35f);
    groundMaterial.reflectance = 0.1f;
    Material matA = {};
    matA.reflectedColor = Vec3(0.2f, 0.25f, 0.29f);
    matA.reflectance = 0.92f;
    Material matB = {};
    matB.reflectedColor = Vec3(0.98f, 0.98f, 0.98f);
    matB.reflectance = 0.2f;
    Material emitter = {};
    emitter.emittedColor = Vec3(0.50f, 0.70f, 0.88f);

    Plane plane = {};
    plane.normal = Vec3(0, 0, 1);
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

    Scene scene = {};
    scene.planes.push_back(plane);
    scene.spheres.push_back(sphereA);
    scene.spheres.push_back(sphereB);
    scene.spheres.push_back(sphereC);

    float screenW = 1.0f;
    float screenH = 1.0f;
    float focalDistance = 1.0f;
    Vec3 screenCenter = camera.pos - (camera.Z * focalDistance);

    float pixelW = (screenW / (float)WIDTH);
    float pixelH = (screenH / (float)HEIGHT);

    if (WIDTH > HEIGHT)
    {
        screenH = (float) HEIGHT / (float) WIDTH;
    }

    Ray ray = {};
    uint8_t raysPerPixel = 8;

    for (size_t x = 0; x<WIDTH; x++)
    {
        float screenX = (float) x * 2.0f / (float) WIDTH - 1.0f;
        for (size_t y = 0; y<HEIGHT; y++)
        {
            float screenY = (float) y * 2.0f / (float) HEIGHT - 1.0f;

            Vec3 pixelColor = Vec3(0, 0, 0);
            for (uint8_t i = 0; i < raysPerPixel; i++)
            {
                float xAntialiasJitter = (randomFloat() * 2 - 1) * pixelW;
                float yAntialiasJitter = (randomFloat() * 2 - 1) * pixelH;
                float xOffset = screenX * 0.5f * screenW + xAntialiasJitter;
                float yOffset = screenY * 0.5f * screenH + yAntialiasJitter;
                Vec3 screenP = screenCenter + camera.X * xOffset + camera.Y * yOffset;

                Vec3 rayOrigin = camera.pos;
                Vec3 rayDirection = (screenP - rayOrigin).normalize();

                ray = Ray(rayOrigin, rayDirection);
                pixelColor = pixelColor + raycast(ray, scene);
            }

            framebuffer[x+y*WIDTH] = pixelColor / (float)raysPerPixel;
        }
    }

    std::ofstream ofs;
    ofs.open("./output.ppm");
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (size_t i = 0; i < HEIGHT*WIDTH; ++i)
    {
        ofs << (char)(255 * framebuffer[i].x);
        ofs << (char)(255 * framebuffer[i].y);
        ofs << (char)(255 * framebuffer[i].z);
    }
    ofs.close();

    return 0;
}
