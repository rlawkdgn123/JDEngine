#pragma once
#include <cmath>


namespace JDUtility
{
    struct Vector2f
    {
        float x, y;

        Vector2f() : x(0), y(0) {}
        Vector2f(float value) : x(value), y(value) {}
        Vector2f(float a, float b) : x(a), y(b) {}
        Vector2f(const Vector2f& other) : x(other.x), y(other.y) {}

        Vector2f& operator=(const Vector2f& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
            }
            return *this;
        }
        Vector2f operator+(const Vector2f& other) const
        {
            return Vector2f(x + other.x, y + other.y);
        }

        Vector2f operator-(const Vector2f& other) const
        {
            return Vector2f(x - other.x, y - other.y);
        }

        Vector2f operator*(float scalar) const
        {
            return Vector2f(x * scalar, y * scalar);
        }

        Vector2f operator/(float scalar) const
        {
            if (scalar != 0)
                return Vector2f(x / scalar, y / scalar);
            else
                return Vector2f(0, 0); // Avoid division by zero
        }

        Vector2f& operator+=(const Vector2f& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2f& operator-=(const Vector2f& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2f& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2f& operator/=(float scalar)
        {
            if (scalar != 0)
            {
                x /= scalar;
                y /= scalar;
            }
            return *this;
        }

        bool operator==(const Vector2f& other) const
        {
            return (x == other.x && y == other.y);
        }

        bool operator!=(const Vector2f& other) const
        {
            return !(*this == other);
        }

        Vector2f operator-() const
        {
            return { -x, -y };
        }

        float Length() const
        {
            return sqrt(x * x + y * y);
        }

        float LengthSquared() const
        {
            return x * x + y * y;
        }

        void Normalize()
        {
            float length = Length();
            if (length > 0)
            {
                x /= length;
                y /= length;
            }
        }

        Vector2f Normalized() const
        {
            Vector2f result = *this;
            result.Normalize();
            return result;
        }

        float Dot(const Vector2f& other) const
        {
            return x * other.x + y * other.y;
        }

        float Cross(const Vector2f& other) const
        {
            return x * other.y - y * other.x;
        }

        float Distance(const Vector2f& other) const
        {
            return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
        }

        float DistanceSquared(const Vector2f& other) const
        {
            return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
        }
    };

	struct Vector3f {
		float x, y, z;
	};

};

