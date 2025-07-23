#pragma once

namespace LearnOpenGL {

	template<typename T>
	class Singleton {
	public:
		static T& Get() {
			static T singleton;
			return singleton;
		}
	};

}
