#pragma once

#include <thread>

struct RayTracer
{
    Config &config;
    Scene &scene;
    Camera &camera;

    RayTracer(Config &config, Scene &scene, Camera &camera): config(config), scene(scene), camera(camera) {};

    void renderTile(const int minX, const int maxX, const int minY, const int maxY)
    {
        float screenW = 1.0f;
        float screenH = 1.0f;
        float focalDistance = 1.0f;
        Vec3 screenCenter = camera.pos - (camera.Z * focalDistance);

        float pixelW = (screenW / (float) config.WIDTH);
        float pixelH = (screenH / (float) config.HEIGHT);

        if (config.WIDTH > config.HEIGHT)
        {
            screenH = (float) config.HEIGHT / (float) config.WIDTH;
        }

        Ray ray = {};

        for (size_t x = minX; x < maxX; x++)
        {
            float screenX = (float) x * 2.0f / (float) config.WIDTH - 1.0f;
            for (size_t y = minY; y < maxY; y++)
            {
                float screenY = (float) y * 2.0f / (float) config.HEIGHT - 1.0f;

                Vec3 pixelColor = Vec3(0, 0, 0);
                for (uint16_t i = 0; i < config.RAYS_PER_PIXEL; i++)
                {
                    float xAntialiasJitter = (randomFloat() * 2 - 1) * pixelW;
                    float yAntialiasJitter = (randomFloat() * 2 - 1) * pixelH;
                    float xOffset = screenX * 0.5f * screenW + xAntialiasJitter;
                    float yOffset = screenY * 0.5f * screenH + yAntialiasJitter;
                    Vec3 screenP = screenCenter + camera.X * xOffset + camera.Y * yOffset;

                    Vec3 rayOrigin = camera.pos;
                    Vec3 rayDirection = (screenP - rayOrigin).normalize();

                    ray = Ray(rayOrigin, rayDirection);
                    pixelColor = pixelColor + camera.raycast(ray, scene);
                }

                camera.framebuffer[x+y*config.WIDTH] = pixelColor / (float) config.RAYS_PER_PIXEL;
            }
        }
    }

    void renderScene()
    {
        std::vector<std::thread> threads;

        for (uint8_t i = 0; i < config.NUM_OF_CORES; i++)
        {
            int tileHeight = config.HEIGHT / config.NUM_OF_CORES;
            threads.emplace_back(std::thread(&RayTracer::renderTile, this, 0, config.WIDTH, i * tileHeight, (i+1) * tileHeight));
        }

        for (auto& t : threads)
        {
            t.join();
        }
    }

    void saveOutputFile()
    {
        std::ofstream ofs;
        ofs.open("./output.ppm");
        ofs << "P6\n" << config.WIDTH << " " << config.HEIGHT << "\n255\n";
        for (size_t i = 0; i < config.HEIGHT * config.WIDTH; ++i)
        {
            ofs << (char)(255 * std::sqrt(camera.framebuffer[i].x));
            ofs << (char)(255 * std::sqrt(camera.framebuffer[i].y));
            ofs << (char)(255 * std::sqrt(camera.framebuffer[i].z));
        }
        ofs.close();
    }
};
