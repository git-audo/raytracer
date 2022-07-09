
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <float.h>

struct Vec3 {
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

struct Camera
{
    Vec3 pos;

    Vec3 X;
    Vec3 Y;
    Vec3 Z;

    Camera() {};
};

struct Plane
{
    Vec3  normal;
    float distance;

    Plane(Vec3 n, float d) : normal(n), distance(d) {};
    Plane() {};
};

struct Sphere
{
    Vec3  center;
    float radius;

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
    Vec3 backgroundColor = Vec3(0.6, 0.6, 0.9);
    Vec3 groundColor = Vec3(0.7, 0.6, 0.4);
    Vec3 sphereColor = Vec3(0.4, 0.5, 0.6);

    Vec3 currentColor = backgroundColor;
    float minDistance = FLT_MAX;
    float epsilon = 0.0001f;

    Vec3 nextOrigin = Vec3();
    Vec3 nextDirection = Vec3();

    uint8_t castingDistance = 3;
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
                    if (currentColor != backgroundColor)
                    {
                        currentColor = currentColor * (groundColor * 0.3);
                        currentColor.normalize();
                    }
                    else
                    {
                        currentColor = groundColor;
                    }

                    minDistance = dis;

                    nextOrigin = rayOrigin + rayDirection * dis;
                    nextDirection = p.normal;
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
                    currentColor = sphereColor;

                    minDistance = dis;

                    nextOrigin = rayOrigin + rayDirection * dis;
                    nextDirection = nextOrigin - s.center;
                }
            }
        }

        if (minDistance != FLT_MAX)
        {
            rayOrigin = nextOrigin;
            rayDirection = nextDirection;
            minDistance = FLT_MAX;
        }
        else
        {
            break;
        }
    }

    return currentColor;
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
    camera.pos = Vec3(0, 10, 1);
    // Todo: clean camera space setup
    camera.Z = camera.pos;
    camera.Z.normalize();
    camera.X = camera.Z.cross(zAxys);
    camera.X.normalize();
    camera.Y = camera.Z.cross(camera.X);
    camera.Y.normalize();

    Plane plane = {};
    plane.normal = Vec3(0, 0, 1);
    plane.distance = 0;

    Sphere sphereA = {};
    sphereA.center = Vec3(0, 0, 1.2f);
    sphereA.radius = 1.0f;

    Sphere sphereB = {};
    sphereB.center = Vec3(2, 1, 0.6f);
    sphereB.radius = 1.0f;

    Scene scene = {};
    scene.planes.push_back(plane);
    scene.spheres.push_back(sphereA);
    scene.spheres.push_back(sphereB);

    float screenW = 1.0f;
    float screenH = 1.0f;
    float focalDistance = 1.0f;
    Vec3 screenCenter = camera.pos - (camera.Z * focalDistance);

    if (width > height)
    {
        screenH = (float) height / (float) width;
    }

    for (size_t x = 0; x<width; x++)
    {
        float screenX = (float) x * 2.0f / (float) width - 1.0f;
	for (size_t y = 0; y<height; y++)
        {
            float screenY = (float) y * 2.0f / (float) height - 1.0f;
            Vec3 screenP = screenCenter + camera.X * screenX * 0.5f * screenW + camera.Y * screenY * 0.5f * screenH;

            Vec3 rayOrigin = camera.pos;
            Vec3 rayDirection = screenP - rayOrigin;
            rayDirection.normalize();

	    Vec3 pixelColor = raycast(rayOrigin, rayDirection, scene);
	    framebuffer[x+y*width] = pixelColor;
        }
    }

    std::ofstream ofs;
    ofs.open("./out.ppm");
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
