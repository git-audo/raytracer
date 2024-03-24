#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>
#include <chrono>

#include "math.h"
#include "scene.h"
#include "camera.h"
#include "config.h"
#include "rayTracer.h"

int main(int argc, char **argv)
{
    Config config;
    Scene scene;

    Vec3 cameraPos = Vec3(0, 7.0f, 1.5f);
    Camera camera = Camera(cameraPos, config);

    RayTracer rayTracer = RayTracer(config, scene, camera);

    printf("Rendering scene...\n");
    auto startTime = std::chrono::high_resolution_clock::now();

    rayTracer.renderScene();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto renderingTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    printf("Rendering time: %lld ms\n", renderingTime.count());

    rayTracer.saveOutputFile();

    return 0;
}
