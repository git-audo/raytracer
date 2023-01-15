#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>

#include "math.h"
#include "scene.h"
#include "camera.h"
#include "config.h"
#include "rayTracer.h"

int main(int argc, char** argv)
{
    Config config;
    Scene scene;

    Vec3 cameraPos = Vec3(0, 7.0f, 1.5f);
    Camera camera = Camera(cameraPos, config);

    RayTracer rayTracer = RayTracer(config, scene, camera);
    rayTracer.renderScene();

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

    return 0;
}
