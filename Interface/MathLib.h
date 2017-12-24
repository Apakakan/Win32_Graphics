#include <math.h>



#ifdef MATHLIB_EXPORTS  
#define MATHLIB_API __declspec(dllexport)   
#else  
#define MATHLIB_API __declspec(dllimport)   
#endif


struct v2
{
	float x,y;

	v2(float x=0, float y=0)
		:x(x),y(y)
	{

	}
	
	v2 Scale(float scalar)
	{
		return v2(x*scalar, y*scalar);
	}
	float Dot(const v2& a)
	{
		return (x*a.x + y*a.y);
	}
	v2 Normalize()
	{
		//float sum = this->Length();
		//return v2(x/sum,y/sum);
		float length = this->Length(); 
		return v2(x / length, y / length);
	}
	float Length()
	{
		return sqrt(x*x + y*y); 
	}
	float LengthPow2()
	{
		return x*x + y*y; 
	}
	v2 projectOnto(v2 a)
	{
		v2 result;
		float magnitude = 0;
		float dot = 0;

		dot = this->Dot(a);
		magnitude = a.x * a.x + a.y * a.y;

		return result = a.Scale(dot/magnitude);

	}
	v2 projectOntoClamped(v2 a)
	{
		v2 result;
		float lengthOfA = a.Length();
		float magnitude = 0;
		float dot = 0;

		dot = this->Dot(a);
		magnitude = a.x * a.x + a.y * a.y;
		float scalar = dot/magnitude;
		if(scalar < 0)
		{
			scalar = 0;
		}
		else if(scalar > 1.0)
		{
			scalar = 1.0;
		}

		return result = a.Scale(scalar);

	}

	v2 getPerpendicular()
	{
		v2 vecDir = v2(x,y).Normalize();
		float a[3];
		a[0] = vecDir.x;
		a[1] = vecDir.y;
		a[2] = 0;
		
		float b[3];
		b[0] = 0;
		b[1] = 0;
		b[2] = -1;
		
		
		
		return v2(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2]).Normalize();
	}
	
	v2 rotate(float radi)
	{
		return v2(x*cos(radi)-y*sin(radi),x*sin(radi)+y*cos(radi));
	}
	
	v2& operator=(const v2& a)
	{
		x=a.x;
		y=a.y;
		return*this;
	}

	v2 operator+(const v2& a)
	{
		return v2(x+a.x,y+a.y);
	}

	v2 operator-(const v2& a)
	{
		return v2(x-a.x,y-a.y);
	}

	bool operator==(const v2& a) const
	{
		return (a.x == x && a.y == y);
	}
};

struct Sphere
{
	float radius;
	Sphere(float radius)
	: radius(radius)
	{}
	
	Sphere& operator=(const Sphere& a)
	{
		radius = a.radius;
		return*this;
	}

	bool operator==(const Sphere& a) const
	{
		return (radius == a.radius);
	}
};
struct OBB
{
	float width, height;
	struct v2 rightAxis, upAxis;
	OBB(v2 rightAxis, v2 upAxis, float width, float height)
	: rightAxis(rightAxis), upAxis(upAxis), width(width), height(height)
	{}
	
	OBB& operator=(const OBB& a)
	{
		rightAxis = a.rightAxis;
		upAxis = a.upAxis;
		width = a.width;
		height = a.height;
		return*this;
	}

	bool operator==(const OBB& a) const
	{
		return (rightAxis == a.rightAxis
		&& upAxis == a.upAxis
		&& width == a.width
		&& height == a.height);
	}
};
struct AABB
{
	float width,height;
	
	AABB(float width, float height)
	: width(width), height(height)
	{}
	
	AABB& operator=(const AABB& a)
	{
		width = a.width;
		height = a.height;
		return*this;
	}

	bool operator==(const AABB& a) const
	{
		return (width == a.width && height == a.height);
	}
};
struct Line
{
	v2 startPoint;
	v2 endPoint;
	int width;
	Line(v2 startPoint,v2 endPoint, int width)
	: startPoint(startPoint), endPoint(endPoint),width(width)
	{
	}
	
	v2 getLineDir()
	{
		return (endPoint - startPoint).Normalize();
	}
	float getLength()
	{
		return (endPoint - startPoint).Length();
	}
	Line operator=(const Line& a)
	{
		startPoint = a.startPoint;
		endPoint = a.endPoint;
		width = a.width;
		return *this;
	}
	bool operator==(const Line& a) const
	{
		return (startPoint == a.startPoint && endPoint == a.endPoint && width == a.width);
	}
};

namespace MathLib
{
	class IntersectionTest
	{
		public:
			static MATHLIB_API bool AABBvsAABB(AABB box1, AABB box2, v2 diffVec);
			static MATHLIB_API bool AABBvsOBB(AABB box1, OBB box2, v2 toOBBVec);
			static MATHLIB_API bool AABBvsSphere(AABB box, Sphere sphere, v2 toSphereVec);
			static MATHLIB_API bool AABBvsLine(AABB box, Line line, v2 toAABB);
			static MATHLIB_API bool OBBvsOBB(OBB box1, OBB box2, v2 diffVec);
			static MATHLIB_API bool OBBvsSphere(OBB box, Sphere sphere, v2 toSphereVec);
			static MATHLIB_API bool OBBvsLine(OBB box, Line line, v2 toLineVec);
			static MATHLIB_API bool SpherevsSphere(Sphere sphere1, Sphere sphere2, v2 diffVec);
			static MATHLIB_API bool SpherevsLine(Sphere sphere, Line line, v2 toLineVec);
		
	};
}

