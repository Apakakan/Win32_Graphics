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

	v2 operator==(const v2& a) const
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
};
struct OBB
{
	float width, height;
	struct v2 rightAxis, upAxis;
	OBB(v2 rightAxis, v2 upAxis, float width, float height)
	: rightAxis(rightAxis), upAxis(upAxis), width(width), height(height)
	{}
};
struct AABB
{
	float width,height;
	
	AABB(float width, float height)
	: width(width), height(height)
	{}
	bool AABBvsAABBTest(AABB box2, v2 diffVec)
	{
		bool result = false;
		
		if((fabs(diffVec.x) < width + box2.width) && (fabs(diffVec.y) < height + box2.height))
		{
			result = true;
		}
		
		return result;
	}
	bool AABBvsSphereTest(Sphere sphere, v2 toSphereVec)
	{
		bool result = false;
		
		v2 pointInBox = toSphereVec;
		if(pointInBox.x < -width)
		{
			pointInBox.x = -width;
		}
		if(pointInBox.x > width)
		{
			pointInBox.x = width;
		}
		if(pointInBox.y < -height)
		{
			pointInBox.y = -height;
		}
		if(pointInBox.y > height)
		{
			pointInBox.y = height;
		}
		
		float distance = (toSphereVec - pointInBox).Length();
		
		if(distance < sphere.radius)
		{
			result = true;
		}
		
		return result;
	}
	bool AABBvsOBBTest(OBB obb, v2 toOBBVec)
	{
		
		v2 axises[4];
		axises[0] = v2(1,0);
		axises[1] = v2(0,1);
		axises[2] = obb.rightAxis;
		axises[3] = obb.upAxis;
		
		v2 AABBCorners[4];
		AABBCorners[0] = width + height;
		AABBCorners[1] = width - height;
		AABBCorners[2] = -width + height;
		AABBCorners[3] = -width - height;
		
		v2 OBBCorners[4];
		OBBCorners[0] = toOBBVec + (obb.rightAxis.Scale(obb.width) + obb.upAxis.Scale(obb.height));
		OBBCorners[1] = toOBBVec + (obb.rightAxis.Scale(obb.width) + obb.upAxis.Scale(-obb.height));
		OBBCorners[2] = toOBBVec + (obb.rightAxis.Scale(-obb.width) + obb.upAxis.Scale(obb.height));
		OBBCorners[3] = toOBBVec + (obb.rightAxis.Scale(-obb.width) + obb.upAxis.Scale(-obb.height));
		
		for(int i = 0; i < 4; i++)
		{
			float AABBMinProjection = 0;
			float AABBMaxProjection = 0;
			
			float OBBMinProjection = axises[i].Dot(toOBBVec);
			float OBBMaxProjection = OBBMinProjection;
			
			for(int w = 0; w < 4; w++)
			{
				float aabbDot = axises[i].Dot(AABBCorners[w]);
				if(aabbDot > AABBMaxProjection)
				{
					AABBMaxProjection = aabbDot;
				}
				if(aabbDot < AABBMinProjection)
				{
					AABBMinProjection = aabbDot;
				}
				
				float obbDot = axises[i].Dot(OBBCorners[w]);
				if(obbDot > OBBMaxProjection)
				{
					OBBMaxProjection = obbDot;
				}
				if(obbDot < OBBMinProjection)
				{
					OBBMinProjection = obbDot;
				}
			}
			if((AABBMinProjection > OBBMaxProjection && AABBMinProjection > OBBMinProjection)
				|| (AABBMaxProjection < OBBMinProjection && AABBMaxProjection < OBBMaxProjection))
			{
				return false;
			}
			
		}
		
		return true;
	}
};



