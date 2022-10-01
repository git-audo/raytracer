#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>

#include "math.h"
#include "scene.h"

struct Camera
{
    Vec3 pos;

    Vec3 X;
    Vec3 Y;
    Vec3 Z;

    Camera(Vec3 p, Vec3 zAxys) : pos(p)
    {
        Z = pos.normalize();
        X = Z.cross(zAxys).normalize();
        Y = Z.cross(X).normalize();
    };
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
            // we translate ray.origin to compensate for a sphere not in the origin
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

        if (minDistance < FLT_MAX)
        {
            Vec3 adiacentSphereCenter = ray.origin + nextNormal;
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
    const int WIDTH  = 1280;
    const int HEIGHT = 720;

    Vec3* framebuffer = new Vec3[WIDTH * HEIGHT];

    Vec3 xAxys = Vec3(1, 0, 0);
    Vec3 yAxys = Vec3(0, 1, 0);
    Vec3 zAxys = Vec3(0, 0, 1);

    Camera camera = Camera(Vec3(0, 7, 1), zAxys);
    camera.pos = camera.pos + Vec3(0, 0, 0.8f);

    Scene scene;

    float screenW = 1.0f;
    float screenH = 1.0f;
    float focalDistance = 1.0f;
    Vec3 screenCenter = camera.pos - (camera.Z * focalDistance);

    float pixelW = (screenW / (float)WIDTH);
    float pixelH = (screenH / (float)HEIGHT);

    if (WIDTH > HEIGHT)
    {
        screenH = (float)HEIGHT / (float)WIDTH;
    }

    Ray ray = {};
    uint8_t raysPerPixel = 255;

    for (size_t x = 0; x < WIDTH; x++)
    {
        float screenX = (float)x * 2.0f / (float)WIDTH - 1.0f;
        for (size_t y = 0; y < HEIGHT; y++)
        {
            float screenY = (float)y * 2.0f / (float)HEIGHT - 1.0f;

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
    for (size_t i = 0; i < HEIGHT * WIDTH; ++i)
    {
        ofs << (char)(255 * std::sqrt(framebuffer[i].x));
        ofs << (char)(255 * std::sqrt(framebuffer[i].y));
        ofs << (char)(255 * std::sqrt(framebuffer[i].z));
    }
    ofs.close();

    return 0;
}
