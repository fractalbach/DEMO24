#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
  public:
    float x;
    float y;
    float z;

    vec3() : x{0}, y{0}, z{0} {}
    vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}

    vec3 operator-() const { return vec3(-x, -y, -z); }
    // float operator[](int i) const { return e[i]; }
    // float &operator[](int i) { return e[i]; }

    vec3 &operator+=(const vec3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    vec3 &operator*=(float t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    vec3 &operator/=(float t) { return *this *= 1 / t; }

    float length() const { return sqrt(length_squared()); }

    float length_squared() const {
        return x * x + y * y + z * z;
    }
};


// Vector Utility Functions
//
//inline std::ostream &operator<<(std::ostream &out, const vec3 &v) {
//    return out << v.x << ' ' << v.y << ' ' << v.z;
//}
//
//inline vec3 operator+(const vec3 &u, const vec3 &v) {
//    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
//}
//
//inline vec3 operator-(const vec3 &u, const vec3 &v) {
//    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
//}
//
//inline vec3 operator*(const vec3 &u, const vec3 &v) {
//    return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
//}
//
//inline vec3 operator*(float t, const vec3 &v) {
//    return vec3(t * v.x, t * v.y, t * v.z);
//}
//
//inline vec3 operator*(const vec3 &v, float t) { return t * v; }
//
//inline vec3 operator/(vec3 v, float t) { return (1 / t) * v; }
//
//inline float dot(const vec3 &u, const vec3 &v) {
//    return u.x * v.x + u.y * v.y + u.z * v.z;
//}
//
//inline vec3 cross(const vec3 &u, const vec3 &v) {
//    return vec3(u.y * v.z - u.z * v.y,
//                u.z * v.x - u.x * v.z,
//                u.x * v.y - u.y * v.x);
//}

//inline vec3 unit_vector(vec3 v) { return v / v.length(); }

#endif