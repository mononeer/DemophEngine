
#pragma once

#include <cmath>
#include <iostream>
#include <array>

namespace Demoph {

/**
 * @brief 3D Vector class with comprehensive mathematical operations
 */
class DEMOPH_API Vec3 {
public:
    float x, y, z;

    // Constructors
    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
    constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    constexpr Vec3(const std::array<float, 3>& arr) : x(arr[0]), y(arr[1]), z(arr[2]) {}

    // Copy constructor and assignment
    constexpr Vec3(const Vec3& other) = default;
    constexpr Vec3& operator=(const Vec3& other) = default;

    // Move constructor and assignment
    constexpr Vec3(Vec3&& other) noexcept = default;
    constexpr Vec3& operator=(Vec3&& other) noexcept = default;

    // Destructor
    ~Vec3() = default;

    // Array access
    constexpr float& operator[](size_t index) {
        return (&x)[index];
    }
    
    constexpr const float& operator[](size_t index) const {
        return (&x)[index];
    }

    // Arithmetic operators
    constexpr Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    constexpr Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    constexpr Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    constexpr Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }

    constexpr Vec3 operator/(float scalar) const {
        float inv = 1.0f / scalar;
        return Vec3(x * inv, y * inv, z * inv);
    }

    constexpr Vec3 operator/(const Vec3& other) const {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }

    constexpr Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    // Assignment operators
    constexpr Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vec3& operator*=(const Vec3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    constexpr Vec3& operator/=(float scalar) {
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }

    constexpr Vec3& operator/=(const Vec3& other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    // Comparison operators
    constexpr bool operator==(const Vec3& other) const {
        return std::abs(x - other.x) < 1e-6f && 
               std::abs(y - other.y) < 1e-6f && 
               std::abs(z - other.z) < 1e-6f;
    }

    constexpr bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }

    // Vector operations
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    constexpr float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vec3 Normalized() const {
        float len = Length();
        if (len > 1e-6f) {
            float inv = 1.0f / len;
            return Vec3(x * inv, y * inv, z * inv);
        }
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    Vec3& Normalize() {
        float len = Length();
        if (len > 1e-6f) {
            float inv = 1.0f / len;
            x *= inv;
            y *= inv;
            z *= inv;
        } else {
            x = y = z = 0.0f;
        }
        return *this;
    }

    constexpr float Dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr Vec3 Cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float Distance(const Vec3& other) const {
        return (*this - other).Length();
    }

    constexpr float DistanceSquared(const Vec3& other) const {
        return (*this - other).LengthSquared();
    }

    Vec3 Lerp(const Vec3& other, float t) const {
        return *this + (other - *this) * t;
    }

    Vec3 Slerp(const Vec3& other, float t) const {
        float dot = this->Dot(other);
        dot = std::clamp(dot, -1.0f, 1.0f);
        float theta = std::acos(dot) * t;
        Vec3 relative = (other - *this * dot).Normalized();
        return (*this * std::cos(theta)) + (relative * std::sin(theta));
    }

    Vec3 Reflect(const Vec3& normal) const {
        return *this - normal * (2.0f * this->Dot(normal));
    }

    Vec3 Project(const Vec3& onto) const {
        float dot = this->Dot(onto);
        float lenSq = onto.LengthSquared();
        if (lenSq > 1e-6f) {
            return onto * (dot / lenSq);
        }
        return Vec3(0.0f);
    }

    Vec3 Reject(const Vec3& onto) const {
        return *this - Project(onto);
    }

    // Utility functions
    constexpr Vec3 Abs() const {
        return Vec3(std::abs(x), std::abs(y), std::abs(z));
    }

    constexpr Vec3 Min(const Vec3& other) const {
        return Vec3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }

    constexpr Vec3 Max(const Vec3& other) const {
        return Vec3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }

    constexpr Vec3 Clamp(const Vec3& min, const Vec3& max) const {
        return Vec3(
            std::clamp(x, min.x, max.x),
            std::clamp(y, min.y, max.y),
            std::clamp(z, min.z, max.z)
        );
    }

    // Constants
    static constexpr Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
    static constexpr Vec3 One() { return Vec3(1.0f, 1.0f, 1.0f); }
    static constexpr Vec3 Up() { return Vec3(0.0f, 1.0f, 0.0f); }
    static constexpr Vec3 Down() { return Vec3(0.0f, -1.0f, 0.0f); }
    static constexpr Vec3 Left() { return Vec3(-1.0f, 0.0f, 0.0f); }
    static constexpr Vec3 Right() { return Vec3(1.0f, 0.0f, 0.0f); }
    static constexpr Vec3 Forward() { return Vec3(0.0f, 0.0f, -1.0f); }
    static constexpr Vec3 Back() { return Vec3(0.0f, 0.0f, 1.0f); }

    // String representation
    std::string ToString() const {
        return std::format("Vec3({:.3f}, {:.3f}, {:.3f})", x, y, z);
    }
};

// Global operators
constexpr Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}

// Stream operators
inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    return os << vec.ToString();
}

} // namespace Demoph
