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

int main(int argc, char **argv)
{
    Config config;
    Scene scene;

    Vec3 cameraPos = Vec3(0, 7.0f, 1.5f);
    Camera camera = Camera(cameraPos, config);

    RayTracer rayTracer = RayTracer(config, scene, camera);
    rayTracer.renderScene();

    rayTracer.saveOutputFile();

    return 0;
}
