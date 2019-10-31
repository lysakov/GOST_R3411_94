#pragma once

#include "block.h"

namespace magma
{

	class Context
	{

	public:
		Context(const uint8_t*) noexcept;
		uint16_t encrypt(uint16_t) const noexcept;
		uint16_t decrypt(uint16_t) const noexcept;

	private:
		uint8_t key_set[32] = { 0 };

		std::pair<uint8_t, uint8_t> split(const uint16_t) const noexcept;
		uint8_t substitution(const uint8_t) const noexcept;
		uint8_t addition(uint8_t, const uint8_t) const noexcept;
		std::pair<uint8_t, uint8_t>& round(std::pair<uint8_t, uint8_t>&, const uint8_t) const noexcept;
		uint16_t connect(std::pair<uint8_t, uint8_t>&) const noexcept;
		void keygen(const uint8_t*) noexcept;

	};

	uint16_t encrypt(uint16_t, const uint8_t*);
	uint16_t decrypt(uint16_t, const uint8_t*);

	uint16_t encrypt(uint16_t, uint64_t);
	uint16_t decrypt(uint16_t, uint64_t);

}

