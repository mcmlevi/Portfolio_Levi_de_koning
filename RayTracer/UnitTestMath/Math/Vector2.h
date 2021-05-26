#pragma once
#pragma warning(push)
#pragma warning(disable: 4201)
#include <cmath>

///
/// Entry class for some 2d math
///
class Vector2
{
public:		
	/// The union trick in not standard C++ but should work with any compiler
	/// and makes usage of C style libs a lot easier
	union
    {
		// Holds all the values 
		float f[2];
			
		struct
		{
			/// Holds the value along the x axis.
			float x;
				
			/// Holds the value along the y axis. */
			float y;				
		};
	};
				
public:
	/// Default 0 contructor
	Vector2(): x(0), y(0) { }
		
	/// Constructor from components
	Vector2(float x, float y) : x(x), y(y) {}
        
    /// Constructor from components
    explicit Vector2(float v) : x(v), y(v) {}
 
	/// Returns the value of the given vector added to this.
	Vector2 operator+(const Vector2& v) const
	{
		return Vector2(x+v.x, y+v.y);
	}
		
	/// Returns the value of the given vector subtracted from this.
	Vector2 operator-(const Vector2& v) const
	{
		return Vector2(x-v.x, y-v.y);
	}
		
	/// Returns a copy of this vector scaled the given value.
	Vector2 operator*(float value) const
	{
		return Vector2(x*value, y*value);
	}
        
    /// Returns a copy of this vector scaled the given value.
	Vector2 operator/(const float value) const
	{
		return Vector2(x/value, y/value);
	}
				
	/// Calculates and returns the dot product of this vector
    /// with the given vector.
	float operator *(const Vector2& vector) const
	{
		return x*vector.x + y*vector.y;
	}
		
	/// Adds the given vector to this.
	void operator+=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
	}		
				
    /// Subtracts the given vector from this
	void operator-=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
	}				
		
	/// Multiplies this vector by the given scalar
	void operator*=(const float value)
	{
		x *= value;
		y *= value;
	}
		
	/// Checks if the two vectors have identical components
	bool operator==(const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}
		
	/// Checks if the two vectors have non-identical components
	bool operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}
												
    /// Calculates and returns the dot product of this vector
	/// with the given vector.
	float Dot(const Vector2& vector) const
	{
		return x*vector.x + y*vector.y;
	}
        
    /// Cross product in 2D, look on mathworld
    float Cross(const Vector2& rhs) const
    {
        return x*rhs.y - y*rhs.x;
    }
		
	/// Gets the magnitude of this vector
	float Magnitude() const
	{		
		return sqrtf(x*x+y*y);
	}
		
	/// Gets the squared magnitude of this vector
	float SquareMagnitude() const
	{
		return x*x+y*y;
	}
		
	/// Limits the size of the vector to the given maximum
	void Trim(float size)
	{
		if (SquareMagnitude() > size*size)
		{
			Normalize();
			x *= size;
			y *= size;
		}
	}
		
	/// Turns a non-zero vector into a vector of unit length
    void Normalize()
	{
		float l = Magnitude();
		if (l > 0)
		{
			(*this) *= ((float)1)/l;
		}
	}	
		
	/// Returns the normalised version of a vector
	Vector2 Unit() const
	{
		Vector2 result = *this;	// Copy ctor
		result.Normalize();
		return result;
	}
        
    /// Returns the perpendicular of the vector
    Vector2 Perpendicular() const
    {
        Vector2 result(y, -x);
        return result;
    }
		
    /// Zero all the components of the vector
	void Clear()
	{
		x = y = 0;
	}
        
                
    /// Squared distance to another vector
    float SquareDistance(const Vector2& other) const
    {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
    }           
        
    static Vector2 Zero;
};
    
/// Multiplication with Rhs Vector
inline Vector2 operator*(float val, const Vector2& rhs)
{
    return Vector2(rhs.x * val, rhs.y * val);
}
#pragma warning(pop)