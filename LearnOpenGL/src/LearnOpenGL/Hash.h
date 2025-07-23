#pragma once

namespace LearnOpenGL {

	class Hash
	{
	public:
		static constexpr uint32_t GenerateFNVHash(const char* str);
		static constexpr uint32_t GenerateFNVHash(std::string_view string);
	};

}

