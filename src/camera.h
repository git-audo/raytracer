#pragma once

struct Camera
{
    Vec3 pos;

    Vec3 X;
    Vec3 Y;
    Vec3 Z;

    const int MAX_NUMBER_OF_BOUNCES = 3;

    Camera(Vec3 p, Vec3 zAxys) : pos(p)
        {
            Z = pos.normalize();
            X = Z.cross(zAxys).normalize();
            Y = Z.cross(X).normalize();
        };

    Vec3 raycast(Ray ray, Scene& scene)
        {
            Material background = {};
            background.emittedColor = Vec3(0.03f, 0.03f, 0.12f);

            float minDistance = FLT_MAX;
            Vec3 attenuation = Vec3(1, 1, 1);
            Vec3 finalColor = {};

            Material previousMaterial = {};
            Material nextMaterial = background;
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
};
