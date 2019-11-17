#pragma once

#include "block.h"
#include "magma.h"

class HashBlock
{

public:
	HashBlock() noexcept;
	HashBlock(uint64_t) noexcept;
	HashBlock(const Block*) noexcept;
	HashBlock(const std::string*) noexcept;
	Block& operator[](short) noexcept;
	const Block& operator[](short) const noexcept;
	HashBlock operator^(const HashBlock&) const noexcept;
	operator uint64_t() const noexcept;
	friend std::ostream& operator<<(std::ostream&, const HashBlock&);

private:
	Block block[4] = { 0 };

};

namespace gost
{

	class Context
	{

	public:
		Context(uint64_t = 0) noexcept;
		uint64_t update(uint64_t);
		void test();

	private:
		uint64_t hashVal = 0;
		uint16_t s[4] = { 0 };
		uint64_t keys[4] = { 0 };

		void keygen_(uint64_t);
		uint64_t encrypt_();
		uint64_t permutation_(const HashBlock& block);
		inline uint64_t psi_(uint64_t);

	};

	uint64_t compress(uint64_t h, uint64_t m) noexcept;

}