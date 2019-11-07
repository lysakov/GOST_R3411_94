#pragma once

#include "block.h"
#include "magma.h"

class Hash_block
{

public:
	Hash_block() noexcept;
	Hash_block(uint64_t) noexcept;
	Hash_block(const Block*) noexcept;
	Hash_block(const std::string*) noexcept;
	Block& operator[](short) noexcept;
	const Block& operator[](short) const noexcept;
	Hash_block operator^(const Hash_block&) const noexcept;
	operator uint64_t() const noexcept;
	friend std::ostream& operator<<(std::ostream&, const Hash_block&);

private:
	Block block[4] = { 0 };

};

uint64_t permutation(const Hash_block& block);

namespace gost
{

	class Context
	{

	public:
		Context(uint64_t = 0) noexcept;
		uint64_t update(uint64_t);
		inline uint64_t mix(uint64_t);
		void test();

	private:
		uint64_t hash_val = 0;
		uint16_t s[4] = { 0 };
		uint64_t keys[4] = { 0 };

		void keygen(uint64_t);
		uint64_t encrypt();

	};

	uint64_t compress(uint64_t h, uint64_t m) noexcept;

}