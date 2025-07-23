#pragma once

#include <limits>
#include <algorithm>
#include <glm/glm.hpp>

namespace LearnOpenGL {


	class Bounds3
	{
	public:
		
		Bounds3()
		{
			float minNum = std::numeric_limits<float>::lowest();
			float maxNum = std::numeric_limits<float>::max();
			pMin = glm::vec3(maxNum, maxNum, maxNum);
			pMax = glm::vec3(minNum, minNum, minNum);
		}

		Bounds3(const glm::vec3 &p): pMin(p), pMax(p) {}

		Bounds3(const glm::vec3 &p1, const glm::vec3& p2): 
			pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)), 
			pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) {}

		const glm::vec3& operator[](int i) const {
			switch (i % 2)
			{
			case 0:
				return pMin;
			case 1:
				return pMax;
			default:
				return pMin;
			}
		}

		glm::vec3 &operator[](int i) {
			switch (i % 2)
			{
			case 0:
				return pMin;
			case 1:
				return pMax;
			default:
				return pMin;
			}
		}

	public:
		glm::vec3 pMin, pMax;
	};

	

}

