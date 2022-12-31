#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>

#include "math.h"
#include "scene.h"
#include "camera.h"

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
    uint8_t raysPerPixel = 4;

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
