#pragma once

namespace FastEngine {
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float x, float y) : x(x), y(y) {}
        
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
        
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
        
        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        
        float length() const {
            return std::sqrt(x * x + y * y);
        }
        
        Vector2 normalize() const {
            float len = length();
            if (len == 0) return *this;
            return *this * (1.0f / len);
        }
    };
    
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
    };
    
    struct Vector4 {
        float x, y, z, w;
        
        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };
}
