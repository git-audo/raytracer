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

    void print() { printf("X: %f  Y: %f  Z: %f\n", x, y, z); };

    float len() { return std::sqrt(x*x + y*y + z*z); };

    void normalize() {
	float l = len();
        if (l != 0)
        {
            x /= l;
            y /= l;
            z /= l;
        }
    };

    void randomize() {
        x = randomFloat() * 2.0f - 1;
        y = randomFloat() * 2.0f - 1;
        z = randomFloat() * 2.0f - 1;
    };

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

    Camera() {};
};

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
};

struct Sphere
{
    Vec3  center;
    float radius;
    Material material;

    Sphere(Vec3 c, float r) : center(c), radius(r) {};
    Sphere() {};
};

struct Scene
{
    std::vector<Plane>  planes;
    std::vector<Sphere> spheres;
};

Vec3 raycast(Vec3 rayOrigin, Vec3 rayDirection, Scene scene)
{
    Material background = {};
    background.emittedColor = Vec3(0.6f, 0.6f, 0.9f);

    float minDistance = FLT_MAX;
    float epsilon = 0.0001f;
    Vec3 finalColor = {};
    Vec3 attenuation = Vec3(1, 1, 1);

    Material previousMaterial = {};
    Material nextMaterial = background;
    Vec3 nextOrigin = Vec3();
    Vec3 nextNormal = Vec3();

    uint8_t castingDistance = 4;
    for (uint8_t i = 0; i < castingDistance; i++)
    {
        for (Plane p : scene.planes)
        {
            float numerator   = - p.distance - p.normal.inner(rayOrigin);
            float denominator = p.normal.inner(rayDirection);

            if (denominator < -epsilon || denominator > epsilon)
            {
                float dis = numerator / denominator;
                if (dis > 0 && dis < FLT_MAX)
                {
                    nextOrigin = rayOrigin + rayDirection * dis;
                    nextNormal = p.normal;
                    nextMaterial = p.material;
                    minDistance = dis;
                }
            }
        }

        for (Sphere s : scene.spheres)
        {
            // Understand why we have to translate rayO to compensate for sphere not in the origin
            Vec3 relativeRayOrigin = rayOrigin - s.center;
            float a = rayDirection.inner(rayDirection);
            float b = (2 * rayDirection.inner(relativeRayOrigin));
            float c = relativeRayOrigin.inner(relativeRayOrigin) - s.radius * s.radius;

            float squaredTerm = b * b -  4.0f * a * c;
            if (squaredTerm >= 0)
            {
                // Todo: fix naming
                float squared = sqrt(squaredTerm);
                float posNum = - b + squared;
                float negNum = - b - squared;

                float posRoot = posNum / 2 * a;
                float negRoot = negNum / 2 * a;

                float dis = posRoot < negRoot ? posRoot : negRoot;

                if (dis > epsilon && dis < minDistance)
                {
                    nextOrigin = rayOrigin + rayDirection * dis;
                    nextNormal = nextOrigin - s.center;
                    nextNormal.normalize();
                    nextMaterial = s.material;
                    minDistance = dis;
                }
            }
        }

        if (minDistance != FLT_MAX)
        {
            // Compute random reflection direction
            Vec3 adiacentSphereCenter = rayOrigin + nextNormal * 1.2;
            Vec3 displacement = Vec3(0, 0, 0);
            displacement.randomize();
            Vec3 randomDirection = (adiacentSphereCenter + displacement) - rayOrigin;
            randomDirection.normalize();

            rayDirection = lerp(randomDirection, nextNormal, nextMaterial.reflectance);
            rayOrigin = nextOrigin;

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
    const int width   = 1280;
    const int height  = 720;

    Vec3* framebuffer = (Vec3*) malloc(width * height * sizeof(Vec3));

    Vec3 xAxys = Vec3(1, 0, 0);
    Vec3 yAxys = Vec3(0, 1, 0);
    Vec3 zAxys = Vec3(0, 0, 1);

    Camera camera = {};
    camera.pos = Vec3(0, 7, 1);
    // Todo: clean camera space setup
    camera.Z = camera.pos;
    camera.Z.normalize();
    camera.X = camera.Z.cross(zAxys);
    camera.X.normalize();
    camera.Y = camera.Z.cross(camera.X);
    camera.Y.normalize();
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

    float pixelW = (screenW / (float)width);
    float pixelH = (screenH / (float)height);

    if (width > height)
    {
        screenH = (float) height / (float) width;
    }

    uint8_t raysPerPixel = 4;
    for (size_t x = 0; x<width; x++)
    {
        float screenX = (float) x * 2.0f / (float) width - 1.0f;
	for (size_t y = 0; y<height; y++)
        {
            float screenY = (float) y * 2.0f / (float) height - 1.0f;

            Vec3 pixelColor = Vec3(0, 0, 0);
            for (uint8_t i = 0; i < raysPerPixel; i++)
            {
                float xAntialiasJitter = (randomFloat() * 2 - 1) * pixelW;
                float yAntialiasJitter = (randomFloat() * 2 - 1) * pixelH;
                float xOffset = screenX * 0.5f * screenW + xAntialiasJitter;
                float yOffset = screenY * 0.5f * screenH + yAntialiasJitter;
                Vec3 screenP = screenCenter + camera.X * xOffset + camera.Y * yOffset;

                Vec3 rayOrigin = camera.pos;
                Vec3 rayDirection = screenP - rayOrigin;
                rayDirection.normalize();

                pixelColor = pixelColor + raycast(rayOrigin, rayDirection, scene);
            }

	    framebuffer[x+y*width] = pixelColor / (float)raysPerPixel;
        }
    }

    std::ofstream ofs;
    ofs.open("./output.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i)
    {
	ofs << (char)(255 * framebuffer[i].x);
	ofs << (char)(255 * framebuffer[i].y);
	ofs << (char)(255 * framebuffer[i].z);
    }
    ofs.close();

    return 0;
}
