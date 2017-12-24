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
	
	bool IntersectionTest::AABBvsLine(AABB box, Line line, v2 toAABBVec)
	{
		v2 lineDir = line.getLineDir();
		v2 lineCenter = line.startPoint + lineDir.Scale(line.getLength() / 2);
		v2 lineCenterToAABB = toAABBVec - (lineCenter - line.startPoint);
		v2 diff = line.endPoint - lineCenter;
		if(!MathLib::IntersectionTest::AABBvsAABB(AABB(diff.x + line.width,diff.y+line.width), box,lineCenterToAABB))
		{
			return false;
		}
		
		v2 AABBCorners[4];
		AABBCorners[0] = toAABBVec + v2( box.width,  box.height);
		AABBCorners[1] = toAABBVec + v2( box.width, -box.height);
		AABBCorners[2] = toAABBVec + v2(-box.width,  box.height);
		AABBCorners[3] = toAABBVec + v2(-box.width, -box.height);
		
		float projectionMin = toAABBVec.Dot(lineDir);
		float projectionMax = projectionMin;
		for(int i = 0; i < 4; i++)
		{
			float dot = AABBCorners[i].Dot(lineDir);
			if(dot > projectionMax)
			{
				projectionMax = dot;
			}
			if(dot < projectionMin)
			{
				projectionMin = dot;
			}
		}
		if((projectionMax > -line.width && projectionMax < line.getLength() + line.width)
			|| (projectionMin > -line.width && projectionMin < line.getLength() + line.width)
			|| (projectionMin < -line.width && projectionMax > line.getLength() + line.width))
		{
			v2 lineDirPerp = lineDir.getPerpendicular();
			projectionMin = toAABBVec.Dot(lineDirPerp);
			projectionMax = projectionMin;
			for(int i = 0; i < 4; i++)
			{
				float dot = AABBCorners[i].Dot(lineDirPerp);
				if(dot > projectionMax)
				{
					projectionMax = dot;
				}
				if(dot < projectionMin)
				{
					projectionMin = dot;
				}
			}
			if((projectionMax > -line.width && projectionMax < line.width)
			|| (projectionMin > -line.width && projectionMin < line.width)
			|| (projectionMin < -line.width && projectionMax > line.width))
			{
				
				return true;
			}
			
			
		}
		return false;
	}
	
	bool IntersectionTest::OBBvsOBB(OBB box1, OBB box2, v2 diffVec)
	{
		float rotation = acos(box1.rightAxis.x);
		
		v2 diffVecRotated = diffVec.rotate(-rotation);
		
		OBB box2Rotated = OBB(box2.rightAxis.rotate(-rotation) ,box2.upAxis.rotate(-rotation) ,box2.width ,box2.height);
		
		
		AABB box1LocalSpace(box1.width,box1.height);
		
		return AABBvsOBB(box1LocalSpace,box2Rotated,diffVecRotated);
		
	}
	
	bool IntersectionTest::OBBvsSphere(OBB box, Sphere sphere, v2 toSphereVec)
	{
		float rotation = acos(box.rightAxis.x);
		
		AABB boxLocalSpace = AABB(box.width,box.height);
		
		return AABBvsSphere(boxLocalSpace, sphere, toSphereVec.rotate(-rotation));
		
	}
	
	bool IntersectionTest::OBBvsLine(OBB box, Line line, v2 toLineVec)
	{
		return false;
	}
	
	bool IntersectionTest::SpherevsSphere(Sphere sphere1, Sphere sphere2, v2 diffVec)
	{
		
		float distance = diffVec.Length();
		
		return ((sphere1.radius + sphere2.radius) > distance);
	}
	
	bool IntersectionTest::SpherevsLine(Sphere sphere, Line line, v2 toLineVec)
	{
		return false;
	}
}