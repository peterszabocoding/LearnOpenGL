#pragma once

#include <xhash>

namespace LearnOpenGL {

	class UUID {

	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(std::string stringId);
		UUID(const UUID&);

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;
	};

}

namespace std {

	template<>
	struct hash<LearnOpenGL::UUID>
	{
		std::size_t operator()(const LearnOpenGL::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

}