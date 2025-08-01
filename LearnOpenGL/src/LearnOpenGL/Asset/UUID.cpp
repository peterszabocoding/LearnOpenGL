#include "pch.h"
#include <random>
#include "UUID.h"

namespace LearnOpenGL {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID(): m_UUID(s_UniformDistribution(s_Engine)) {}
	UUID::UUID(uint64_t uuid): m_UUID(uuid) {}
	UUID::UUID(std::string stringId): m_UUID(std::stoull(stringId)) {}
	UUID::UUID(const UUID& other): m_UUID(other.m_UUID) {}

}
