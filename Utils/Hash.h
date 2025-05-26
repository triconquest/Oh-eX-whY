#pragma once

#include <cstdint>
#include <cstring>

namespace FNV
{
	inline constexpr std::uint32_t ullBasis = 0x811C9DC5;
	inline constexpr std::uint32_t ullPrime = 0x1000193;

	// compile-time hashes
	constexpr std::uint32_t HashConst(const char* szString, const std::uint32_t uValue = ullBasis) noexcept
	{
		return (szString[0] == '\0') ? uValue : HashConst(&szString[1], (uValue ^ std::uint32_t(szString[0])) * ullPrime);
	}

	// runtime hashes
	inline std::uint32_t Hash(const char* szString)
	{
		std::uint32_t uHashed = ullBasis;

		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uHashed ^= szString[i];
			uHashed *= ullPrime;
		}

		return uHashed;
	}
}