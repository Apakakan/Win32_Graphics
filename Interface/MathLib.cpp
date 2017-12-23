#include "stdafx.h"
#include "MathLib.h"

namespace MathLib
{
	
	bool IntersectionTest::AABBvsAABB(AABB aabb1, AABB aabb2, v2 diffVec)
	{
		bool result = false;
		
		if((fabs(diffVec.x) < aabb1.width + aabb2.width) && (fabs(diffVec.y) < aabb1.height + aabb2.height))
		{
			result = true;
		}
		
		return result;
	}
	
	bool IntersectionTest::AABBvsOBB(AABB aabb, OBB obb, v2 toOBBVec)
	{
		v2 axises[4];
		axises[0] = v2(1,0);
		axises[1] = v2(0,1);
		axises[2] = obb.rightAxis;
		axises[3] = obb.upAxis;
		
		v2 AABBCorners[4];
		AABBCorners[0] = v2(aabb.width, aabb.height);
		AABBCorners[1] = v2(aabb.width, -aabb.height);
		AABBCorners[2] = v2(-aabb.width, aabb.height);
		AABBCorners[3] = v2(-aabb.width, -aabb.height);
		
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
				float aabbDot = AABBCorners[w].Dot(axises[i]);
				if(aabbDot > AABBMaxProjection)
				{
					AABBMaxProjection = aabbDot;
				}
				if(aabbDot < AABBMinProjection)
				{
					AABBMinProjection = aabbDot;
				}
				
				float obbDot = OBBCorners[w].Dot(axises[i]);
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
	
	bool IntersectionTest::AABBvsSphere(AABB aabb, Sphere sphere, v2 toSphereVec)
	{
		bool result = false;
		
		v2 pointInBox = toSphereVec;
		if(pointInBox.x < -aabb.width)
		{
			pointInBox.x = -aabb.width;
		}
		if(pointInBox.x > aabb.width)
		{
			pointInBox.x = aabb.width;
		}
		if(pointInBox.y < -aabb.height)
		{
			pointInBox.y = -aabb.height;
		}
		if(pointInBox.y > aabb.height)
		{
			pointInBox.y = aabb.height;
		}
		
		float distance = (toSphereVec - pointInBox).Length();
		
		if(distance < sphere.radius)
		{
			result = true;
		}
		
		return result;
	}
	
	bool IntersectionTest::OBBvsOBB(OBB box1, OBB box2, v2 diffVec)
	{
		return false;
	}
	
	bool IntersectionTest::OBBvsSphere(OBB box, Sphere sphere, v2 toSphereVec)
	{
		return false;
	}
	
	bool IntersectionTest::SpherevsSphere(Sphere sphere1, Sphere sphere2, v2 diffVec)
	{
		return false;
	}
	
}