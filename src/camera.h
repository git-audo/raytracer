#pragma once

#include "config.h"

struct Camera
{
    Vec3 pos;

    Vec3 X;
    Vec3 Y;
    Vec3 Z;

    Vec3* framebuffer;
    Material backgroundMaterial;

    const int MAX_NUMBER_OF_BOUNCES = 3;

Camera(Vec3 p, Config &config) : pos(p)
        {
            Vec3 worldZAxys = Vec3(0, 0, 1);

            Z = pos.normalize();
            X = Z.cross(worldZAxys).normalize();
            Y = Z.cross(X).normalize();

            framebuffer = new Vec3[config.WIDTH * config.HEIGHT];

            backgroundMaterial.emittedColor = Vec3(0.81f, 0.81f, 0.75f);
        };

    ~Camera()
        {
            delete[] framebuffer;
        }

    Vec3 raycast(Ray ray, Scene& scene)
        {
            float minDistance = FLT_MAX;
            Vec3 attenuation = Vec3(1, 1, 1);
            Vec3 finalColor = {};

            Material* nextMaterial = &backgroundMaterial;
            Vec3 nextOrigin = Vec3();
            Vec3 nextNormal = Vec3();

            for (uint8_t i = 0; i < MAX_NUMBER_OF_BOUNCES; i++)
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

                for (RectYZ r : scene.rects)
                {
                    float distance = 0;
                    if (r.intersectsRay(ray, distance))
                    {
                        nextOrigin = ray.origin + ray.direction * distance;
                        nextNormal = r.normal;
                        nextMaterial = r.material;
                        minDistance = distance;
                    }
                }

                if (minDistance < FLT_MAX)
                {
                    Vec3 adjacentSphereCenter = ray.origin + nextNormal;
                    Vec3 randomDirection = ((adjacentSphereCenter + Vec3::randomized()) - ray.origin).normalize();

                    ray.direction = lerp(randomDirection, nextNormal, nextMaterial->reflectance);
                    ray.origin = nextOrigin;

                    finalColor = finalColor + nextMaterial->emittedColor * attenuation;
                    attenuation = nextMaterial->reflectedColor * attenuation;

                    minDistance = FLT_MAX;
                }
                else
                {
                    finalColor = finalColor + backgroundMaterial.emittedColor * attenuation;
                    break;
                }
            }

            return finalColor;
        }
};
