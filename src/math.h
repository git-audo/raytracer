
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

    float length() { return std::sqrt(x*x + y*y + z*z); };

    Vec3 normalize() {
        float l = length();
        Vec3 v = {};

        if (l != 0)
        {
            v.x = x / l;
            v.y = y / l;
            v.z = z / l;
        }

        return v;
    }

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

    static Vec3 randomized() {
        Vec3 v = {};

        v.x = randomFloat() * 2.0f - 1;
        v.y = randomFloat() * 2.0f - 1;
        v.z = randomFloat() * 2.0f - 1;

        return v;
    }
};

Vec3 lerp(Vec3 start, Vec3 end, float t)
{
    return start * (1 - t) + end * t;
}
