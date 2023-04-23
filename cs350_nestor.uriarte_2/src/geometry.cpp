/**
* @file geometry.cpp
* @author Nestor Uriarte, 54000817, nestor.uriarte@digipen.edu
* @date 2020/10/05
* @brief The implementation for the basic geometrical 
*		calculus for the primitives
*
* This file serves as an example of a documented file .
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "geometry.hpp"
#include "renderer.hpp"

namespace cs350 {

	/**
	* @brief default constructor for the class
	* @param glm::vec3& p0
	* @param glm::vec3& p1
	* @return 
	*/
	segment::segment(glm::vec3 p0, glm::vec3 p1) : mP0(p0), mP1(p1)
	{
		mMesh = renderer::instance().resources().meshes.segment;

	}

	/**
	/* @brief default constructor for the class
	/* @param glm::vec3& pos
	/* @param glm::vec3& norm
	/* @return
	/*/
	plane::plane(glm::vec3 pos, glm::vec3 norm) : mPosition(pos), mNormal(norm)
	{
		mMesh = renderer::instance().resources().meshes.quad;
	}

	/**
	* @brief default constructor for the class
	* @param glm::vec3& v0
	* @param glm::vec3& v1
	* @param glm::vec3& v2
	* @return
	*/
	triangle::triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) : mV0(v0), mV1(v1), mV2(v2)
	{
		mMesh = renderer::instance().resources().meshes.triangle;
	}

	/**
	* @brief default constructor for the class
	* @param glm::vec3& p
	* @param glm::vec3& v
	* @return
	*/
	ray::ray(glm::vec3& p, glm::vec3 v)
	{
		mP = p;
		mVec = v;
	}

	/**
	* @brief default constructor for the class
	* @param glm::vec3& min
	* @param glm::vec3& max
	* @return
	*/
	aabb::aabb(glm::vec3 min, glm::vec3 max) : mMin(min), mMax(max)
	{
		mMesh = renderer::instance().resources().meshes.cube;
	}

	/**
	* @brief default constructor for the class
	* @param glm::vec3& min
	* @param glm::vec3& max
	* @return
	*/
	aabb::aabb(glm::vec3 pos, float scale) : mMin(pos - scale), mMax(pos + scale)
	{
		mMesh = renderer::instance().resources().meshes.cube;
	}

	/**
	* @brief default constructor for the class
	* @param glm::vec3& pos
	* @param float radius
	* @return
	*/
	sphere::sphere(glm::vec3 pos, float radius) : mPosition(pos)
	{
		mRadius = radius;

		mMesh = renderer::instance().resources().meshes.sphere;
	}

	/**
	* @brief default constructor for the class
	* @param planes*
	* @return
	*/
	frustum::frustum(plane* planes)
	{
		if (planes != nullptr)
		{
			for (int i = 0; i < 6; i++)
				mPlanes[i] = planes[i];
		}
	}

	/**
	* @brief overload of the [] operator for the segment
	* @param int i
	* @return a reference to the vec3 with index i
	*/
	const glm::vec3& segment::operator[](int i) const
	{
		assert(i <= 1 && i >= 0);

		if (i == 0)
			return mP0;
		
		return mP1;
	}

	/**
	* @brief overload of the [] operator for the triangle
	* @param int i
	* @return a reference to the vec3 with index i
	*/
	const glm::vec3& triangle::operator[](int i) const
	{
		assert(i <= 2 && i >= 0);

		if (i == 0)
			return mV0;
		if (i == 1)
			return mV1;

		return mV2;
	}

	/**
	* @brief overload of the << operator for the classification_t enum class
	* @param std::ostream& os
	* @param const classification_t& c
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const classification_t& c)
	{
		switch (c)
		{
		case classification_t::inside :
			os << "inside";
			break;
		case classification_t::outside:
			os << "outside";
			break;
		case classification_t::overlapping:
			os << "overlapping";
			break;
		default:
			os << "outside";
			break;
		}

		return os;
	}

	/**
	* @brief overload of the >> operator for the classification_t enum class
	* @param std::istream& is
	* @param classification_t& c
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, classification_t& c)
	{
		std::string text;

		is >> text;

		if (text == "inside")
			c = classification_t::inside;
		if (text == "outside")
			c = classification_t::outside;
		if (text == "overlapping")
			c = classification_t::overlapping;

		return is;
	}

	/**
	* @brief overload of the << operator for the ray class
	* @param std::ostream& os
	* @param const ray& r
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const ray& r)
	{
		os << r.mP;
		os << r.mVec;

		return os;
	}

	/**
	* @brief overload of the >> operator for the ray class
	* @param std::istream& is
	* @param ray& r
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, ray& r)
	{
		is >> r.mP;
		is >> r.mVec;

		return is;
	}

	/**
	* @brief overload of the << operator for the frustum class
	* @param std::ostream& os
	* @param const frustum& f
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const frustum& f)
	{
		for (int i = 0; i < 6; i++)
			os << f.mPlanes[i];

		return os;
	}

	/**
	* @brief overload of the >> operator for the frustum class
	* @param std::istream& is
	* @param frustum& f
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, frustum& f)
	{
		for (int i = 0; i < 6; i++)
			is >> f.mPlanes[i];

		return is;
	}

	/**
	* @brief overload of the << operator for the segment class
	* @param std::ostream& os
	* @param const segment& s
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const segment& s)
	{
		os << s.mP0;
		os << s.mP1;

		return os;
	}

	/**
	* @brief overload of the >> operator for the segment class
	* @param std::istream& is
	* @param segment& s
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, segment& s)
	{
		is >> s.mP0;
		is >> s.mP1;

		return is;
	}

	/**
	* @brief overload of the << operator for the plane class
	* @param std::ostream& os
	* @param const plane& p
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const plane& p)
	{
		os << p.mPosition;
		os << p.mNormal;

		return os;
	}

	/**
	* @brief overload of the >> operator for the plane class
	* @param std::istream& is
	* @param plane& p
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, plane& p)
	{
		is >> p.mPosition;
		is >> p.mNormal;

		return is;
	}

	/**
	* @brief overload of the << operator for the triangle class
	* @param std::ostream& os
	* @param const triangle& t
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const triangle& t)
	{
		os << t.mV0;
		os << t.mV1;
		os << t.mV2;

		return os;
	}

	/**
	* @brief overload of the >> operator for the triangle class
	* @param std::istream& is
	* @param triangle& t
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, triangle& t)
	{
		is >> t.mV0;
		is >> t.mV1;
		is >> t.mV2;

		return is;
	}

	/**
	* @brief overload of the << operator for the aabb class
	* @param std::ostream& os
	* @param const aabb& a
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const aabb& a)
	{
		os << a.mMin;
		os << a.mMax;

		return os;
	}

	/**
	* @brief overload of the >> operator for the aabb class
	* @param std::istream& is
	* @param aabb& a
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, aabb& a)
	{
		is >> a.mMin;
		is >> a.mMax;

		return is;
	}

	/**
	* @brief overload of the << operator for the sphere class
	* @param std::ostream& os
	* @param const sphere& s
	* @return std::ostream&
	*/
	std::ostream& operator<<(std::ostream& os, const sphere& s)
	{
		os << s.mPosition;
		os << s.mRadius;

		return os;
	}

	/**
	* @brief overload of the >> operator for the sphere class
	* @param std::istream& is
	* @param sphere& s
	* @return std::istream&
	*/
	std::istream& operator>>(std::istream& is, sphere& s)
	{
		is >> s.mPosition;
		is >> s.mRadius;

		return is;
	}

	/**
	* @brief checks the intersection between a plane and a ray
	* @param const ray& r
	* @param const plane& p
	* @return float representing the collision time
	*/
	float intersection_ray_plane(const ray& r, const plane& p)
	{
		//computing the dot product
		float dot = glm::dot(p.mNormal, r.mVec);

		//if the dot product is 0
		if (dot <= cEpsilon &&  dot >= -cEpsilon)
			return -1.0F;

		//using the formula to compute the time
		float d = (p.mNormal.x * p.mPosition.x) + (p.mNormal.y * p.mPosition.y) + (p.mNormal.z * p.mPosition.z);

		float t = (d - glm::dot(p.mNormal, r.mP)) / dot;

		//if the t is negative return -1
		if (t < 0.0F)
			return -1.0F;

		//returning the time
		return t;
	}

	/**
	* @brief checks the intersection between a aabb and a ray
	* @param const ray& r
	* @param const aabb& a
	* @return float representing the collision time
	*/
	float intersection_ray_aabb(const ray& r, const aabb& a)
	{
		if (intersection_point_aabb(r.mP, a))
			return 0.0F;
		
		float tMin = -FLT_MAX;
		float tMax = FLT_MAX;

		for (int i = 0; i < 3; i++)
		{
			if (r.mVec[i] <= cEpsilon && r.mVec[i] >= -cEpsilon)
			{
				if (r.mP[i] < a.mMin[i] || r.mP[i] > a.mMax[i])
					return -1.0F;
			}
			else
			{
				float divider = 1.0F / r.mVec[i];

				float t1 = (a.mMin[i] - r.mP[i]) * divider;
				float t2 = (a.mMax[i] - r.mP[i]) * divider;

				if (t1 > t2)
					std::swap(t1, t2);

				if (t1 > tMin)
					tMin = t1;

				if (t2 < tMax)
					tMax = t2;

				if (tMin > tMax)
					return -1.0F;
			}
		}

		if (tMin < 0.0F)
			return -1.0F;

		return tMin;
	}

	/**
	* @brief checks the intersection between a sphere and a ray
	* @param const ray& r
	* @param const sphere& s
	* @return float representing the collision time
	*/
	float intersection_ray_sphere(const ray& r, const sphere& s)
	{
		//if the point is inside the sphere return 0
		if (intersection_point_sphere(r.mP, s) == true)
			return 0.0F;

		//computing the a,b,c values of the quadratic equation to solve t
		float a = glm::length2(r.mVec);

		//if a is 0 return -1
		if (a <= cEpsilon && a >= -cEpsilon)
			return -1.0F;

		float b = 2.0F * (glm::dot(r.mVec, (r.mP - s.mPosition)));

		float c = glm::length2((r.mP - s.mPosition)) - (s.mRadius * s.mRadius);

		//computing the value inside of the square root
		float inRoot = (b * b) - (4 * a * c);
		
		//if is negative return -1
		if (inRoot < 0.0F)
			return -1.0F;

		//solve for both t
		float t1 = (-b + glm::sqrt(inRoot)) / (2 * a);
		float t2 = (-b - glm::sqrt(inRoot)) / (2 * a);

		//checking if are the same value
		float diff = glm::abs(t1 - t2);

		if (diff <= cEpsilon)
			return t1;

		//if both are negative return -1
		if (t1 < 0.0f && t2 < 0.0f)
			return -1.0F;

		//if t1 is negative
		if (t1 < 0.0F)
			return t2;

		//if t2 is negative
		if (t2 < 0.0F)
			return t1;

		//returning the lowest time
		float finalT = t1 < t2 ? t1 : t2;

		return finalT;
	}

	/**
	* @brief checks the intersection between a triangle and a ray
	* @param const ray& r
	* @param const triangle& t
	* @return float representing the collision time
	*/
	float intersection_ray_triangle(const ray& r, const triangle& t)
	{
		//checking if the ray intersects with the plane created by the tirangle
		glm::vec3 normal = glm::normalize(glm::cross(t.mV0 - t.mV1, t.mV0 - t.mV2));

		float time = intersection_ray_plane(r, plane{ t.mV0, normal });

		//if they intersect
		if (time != -1.0F)
		{
			//checking point containment using barycentric coordinates
			glm::vec3 point = r.mP + (time * r.mVec);

			glm::vec3 edge0(t.mV1 - t.mV0);
			glm::vec3 edge1(t.mV2 - t.mV0);
			glm::vec3 edge2(point - t.mV0);

			float dot00 = glm::dot(edge0, edge0);
			float dot01 = glm::dot(edge0, edge1);
			float dot02 = glm::dot(edge0, edge2);
			float dot11 = glm::dot(edge1, edge1);
			float dot12 = glm::dot(edge1, edge2);

			float divider = (dot00 * dot11) - (dot01 * dot01);

			if (divider == 0.0F)
				return -1.0F;

			float u = ((dot11 * dot02) - (dot01 * dot12)) / divider;
			float v = ((dot00 * dot12) - (dot01 * dot02)) / divider;

			if (u >= 0.0F && v >= 0 && (u + v <= 1.0F))
				return time;

		}

		//if they dont intersect return -1
		return -1.0F;
	}

	/**
	* @brief returns the closest segment between the given two
	* @param const segment& s0
	* @param const segment& s1
	* @return segment, a segment which represents the closes one
	*/
	segment closest_segment_segment(const segment& s0, const segment& s1)
	{
		//temporal variable to store the output
		segment closest;

		//following the process shown in class to compute
		glm::vec3 k(s0.mP0 - s1.mP0);
		glm::vec3 v(s0.mP1 - s0.mP0);
		glm::vec3 w(s1.mP1 - s1.mP0);

		float a = glm::dot(v, v);
		float b = glm::dot(v, w);
		float c = glm::dot(w, w);
		float d = glm::dot(v, k);
		float e = glm::dot(w, k);

		float t = 0.0F;
		float s = 0.0F;

		float divisor = ((b * b) - (a * c));

		//checking if the divisor is 0
		if (divisor == 0.0F)
		{
			//in this case we know that t will be 0
			t = 0.0F;

			//if a is not 0
			if (a != 0.0F)
			{
				//computing the s value
				s = (-d + (b * t)) / a;
				s = glm::clamp(s, 0.0F, 1.0F);
			}
			else//otherwise is 0
				s = 0.0F;
		}
		else//in this case we know t is not 0
		{
			//computing t
			t = ((b * d) - (a * e)) / divisor;
			t = glm::clamp(t, 0.0F, 1.0F);//clamping it 

			//if a is not 0
			if (a != 0.0F)
			{
				//compute the s value
				s = (-d + (b * t)) / a;
				s = glm::clamp(s, 0.0F, 1.0F);
			}
			else//otherwise is 0
				s = 0.0F;
		}

		//if divider is 0 set t to 0
		if (c == 0.0F)
			t = 0.0F;
		else//otherwise compute it
			t = (e + (s * b)) / c;

		//clamping the t value 
		t = glm::clamp(t, 0.0F, 1.0F);

		//setting the values
		closest.mP0 = s0.mP0 + (s * v);
		closest.mP1 = s1.mP0 + (t * w);

		//returning the segment
		return closest;
	}

	/**
	* @brief returns the closest point to a given plane
	* @param const glm::vec3& p
	* @param const plane& plane
	* @return glm::vec3, the projected point
	*/
	glm::vec3 closest_point_plane(const glm::vec3& p, const plane& plane)
	{
		//projecting the point onto the plane
		glm::vec3 closest;

		float lenghtSqr = glm::length2(plane.mNormal);

		closest = p - (glm::dot((p - plane.mPosition), plane.mNormal) / lenghtSqr) * plane.mNormal;

		//returning the projected point
		return closest;
	}

	/**
	* @brief checks a intersection between a point and an aabb
	* @param const glm::vec3& p
	* @param const aabb& b
	* @return bool, representing if intersecting or not
	*/
	bool intersection_point_aabb(const glm::vec3& p, const aabb& b)
	{
		//check if the point is inside the min and max values on the three axis
		const bool inX = p.x >= b.mMin.x && p.x <= b.mMax.x;

		const bool inY = p.y >= b.mMin.y && p.y <= b.mMax.y;

		const bool inZ = p.z >= b.mMin.z && p.z <= b.mMax.z;

		//if is inside all return true
		if (inX && inY && inZ)
			return true;

		return false;
	}

	/**
	* @brief checks a intersection between an aabb and an aabb
	* @param const aabb& a
	* @param const aabb& b
	* @return bool, representing if intersecting or not
	*/
	bool intersection_aabb_aabb(const aabb& a, const aabb& b)
	{
		//checking the min and max point intersect with the other abb
		const bool minA = intersection_point_aabb(a.mMin, b);
		const bool maxA = intersection_point_aabb(a.mMax, b);

		const bool minB = intersection_point_aabb(b.mMin, a);
		const bool maxB = intersection_point_aabb(b.mMax, a);

		//if any of the point is inside return true
		if (minA || minB || maxA || maxB)
			return true;

		return false;
	}

	/**
	* @brief checks a intersection between a point and a sphere
	* @param const glm::vec3& p
	* @param const sphere& s
	* @return bool, representing if intersecting or not
	*/
	bool intersection_point_sphere(const glm::vec3& p, const sphere& s)
	{
		//computing the distance from the point to the sphere
		float distance = glm::length2((p - s.mPosition));
		float radius = s.mRadius * s.mRadius;

		//if is lower or equal return true
		if (distance <= radius)
			return true;

		return false;
	}

	/**
	* @brief checks a intersection between a sphere and a sphere
	* @param const sphere& a
	* @param const sphere& b
	* @return bool, representing if intersecting or not
	*/
	bool intersection_sphere_sphere(const sphere& a, const sphere& b)
	{
		//computing the distance between the two centers	
		float distance = glm::length2((a.mPosition - b.mPosition));
		float radiusSqr = (a.mRadius + b.mRadius) * (a.mRadius + b.mRadius);

		//if the distance is lower than the addition of both return true
		if (distance <= radiusSqr)
			return true;

		return false;
	}

	/**
	* @brief classifies if a sphere is inside, outside or overlapping a frustum
	* @param const frustum& frustum
	* @param const sphere& s
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_frustum_sphere_naive(const frustum& frustum, const sphere& s)
	{
		//array cvontaining the result count
		int result[3]{ 0 };

		//cheking the sphere with all 6 planes of the frustum
		for (int i = 0; i < 6; i++)
			result[static_cast<unsigned>(classify_plane_sphere(frustum.mPlanes[i], s, cEpsilon))]++;

		//if any test gives is out return is out
		if(result[1] > 0)
			return classification_t::outside;

		//if all tests say is inside return is inside 
		if(result[0] == 6)
			return classification_t::inside;

		//otherwise return overlapping
		return classification_t::overlapping;

	}

	/**
	* @brief classifies if a aabb is inside, outside or overlapping a frustum
	* @param const frustum& frustum
	* @param const aabb& a
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_frustum_aabb_naive(const frustum& frustum, const aabb& a)
	{
		//array cvontaining the result count
		int result[3]{ 0 };

		//cheking the aabb with all 6 planes of the frustum
		for (int i = 0; i < 6; i++)
			result[static_cast<unsigned>(classify_plane_aabb(frustum.mPlanes[i], a, cEpsilon))]++;


		//if any test gives is out return is out
		if (result[1] > 0)
			return classification_t::outside;

		//if all tests say is inside return is inside 
		if (result[0] == 6)
			return classification_t::inside;

		//otherwise return overlapping
		return classification_t::overlapping;
	}

	/**
	* @brief classifies if a point is inside, outside or overlapping a plane
	* @param const plane& plane
	* @param const glm::vec3& p
	* @param const float thickness
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_plane_point(const plane& plane, const glm::vec3& p, const float thickness)
	{
		//compute the distance from the point to the closest poit on the plane
		float distance = glm::length2(closest_point_plane(p, plane) - p);

		//compute how thicc it is
		float thicSqr = thickness * thickness;

		//if the distance is lower, the point is overlapping
		if (distance <= thicSqr)
			return classification_t::overlapping;

		//computing the dot product to check if is inside ot outside 
		float dot = glm::dot((p - plane.mPosition), plane.mNormal);

		//if the dot product is 0 return overlapping
		if (glm::abs(dot) <= cEpsilon)
			return classification_t::overlapping;

		//if is negative means that the angle is > 90 so is outside
		if(dot > 0.0F)
			return classification_t::outside;

		//returning that is inside
		return classification_t::inside;
	}

	/**
	* @brief classifies if a triangle is inside, outside or overlapping a plane
	* @param const plane& plane
	* @param const triangle& t
	* @param const float thickness
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_plane_triangle(const plane& plane, const triangle& t, const float thickness)
	{
		//checking the state of the three vertices respect to the plane
		classification_t v0 = classify_plane_point(plane, t.mV0, thickness);
		classification_t v1 = classify_plane_point(plane, t.mV1, thickness);
		classification_t v2 = classify_plane_point(plane, t.mV2, thickness);
		
		//storing the results in a array to make the checking easier
		classification_t result[3] = { v0, v1, v2 };

		//variable storing the coincidence count
		int i = 0;

		//checking if all three vertices are overlapping
		for (i = 0; i < 3; i++)
			if (result[i] != classification_t::overlapping)
				break;

		//if all three are overlapping return it
		if(i == 3)
			return classification_t::overlapping;

		//checking if all three vertices are outside
		for (i = 0; i < 3; i++)
			if (result[i] == classification_t::inside)
				break;

		//if all three are outside return it
		if (i == 3)
			return classification_t::outside;

		//checking if all three vertices are inside
		for (i = 0; i < 3; i++)
			if (result[i] == classification_t::outside)
				break;

		//if all three are inside return it
		if (i == 3)
			return classification_t::inside;
		
		//if none of the previous tests have been succesfully met it means is overlaping
		return classification_t::overlapping;
	}

	/**
	* @brief classifies if a aabb is inside, outside or overlapping a plane
	* @param const plane& plane
	* @param const aabb& a
	* @param const float thickness
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_plane_aabb(const plane& plane, const aabb& a, const float thickness)
	{	
		//computing the missing 6 point that form the aabb
		glm::vec3 p0(a.mMax.x, a.mMin.y, a.mMin.z);
		glm::vec3 p1(a.mMax.x, a.mMin.y, a.mMax.z);
		glm::vec3 p2(a.mMin.x, a.mMin.y, a.mMax.z);

		glm::vec3 p3(a.mMin.x, a.mMax.y, a.mMin.z);
		glm::vec3 p4(a.mMax.x, a.mMax.y, a.mMin.z);
		glm::vec3 p5(a.mMin.x, a.mMax.y, a.mMax.z);

		//array storing the results
		classification_t result[8]
		{ 
			classify_plane_point(plane, a.mMin, thickness),
			classify_plane_point(plane, p0, thickness),
			classify_plane_point(plane, p1, thickness),
			classify_plane_point(plane, p2, thickness),
			classify_plane_point(plane, p3, thickness),
			classify_plane_point(plane, p4, thickness),
			classify_plane_point(plane, p5, thickness),
			classify_plane_point(plane, a.mMax, thickness)
		};

		//variable to store the coincidence count
		int i = 0;

		//same logic as the triangle but with the 8 vertices
		for (i = 0; i < 8; i++)
			if (result[i] != classification_t::overlapping)
				break;

		if (i == 8)
			return classification_t::overlapping;

		for (i = 0; i < 8; i++)
			if (result[i] == classification_t::inside)
				break;

		if (i == 8)
			return classification_t::outside;

		for (i = 0; i < 8; i++)
			if (result[i] == classification_t::outside)
				break;

		if (i == 8)
			return classification_t::inside;


		return classification_t::overlapping;
	}

	/**
	* @brief classifies if a sphere is inside, outside or overlapping a plane
	* @param const plane& plane
	* @param const sphere& s
	* @param const float thickness
	* @return classification_t, a enum value representing if inside outside or overlapping
	*/
	classification_t classify_plane_sphere(const plane& plane, const sphere& s, const float thickness)
	{
		//classifying how the center is respect to the plane wirh the thickness of the radius added
		classification_t center = classify_plane_point(plane, s.mPosition, thickness + s.mRadius);

		//returning the result
		return center;
	}

}

