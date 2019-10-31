#include "cryptanalysis.h"

attack::Context::Context(uint64_t hash_val) noexcept : hash_val(hash_val)
{
}

uint64_t attack::Context::inv_permutation(const Hash_block &y) noexcept
{
	Hash_block res;
	for (int i = 0; i < 32; ++i) {
		short new_ind = 32 - inv_phi(32 - i);
		//std::cout << block[i / 8][i % 8] << '\n';
		res[new_ind / 8][new_ind % 8] = y[i / 8][i % 8];
	}

	return res;
}

inline uint64_t attack::Context::inv_psi(uint64_t y) noexcept
{
	uint64_t last_block = y >> 60;
	uint64_t mask = 0xF;

	for (int i = 0; i < 3; ++i) {
		last_block ^= (y >> 4 * i) & mask;
	}

	last_block ^= (y >> 44) & mask;
	last_block ^= (y >> 56) & mask;

	return (y << 4) ^ last_block;
}

short attack::Context::inv_phi(short x) noexcept
{
	short i = x / 8;
	short k = x % 8;

	if (!k) {
		k = 8;
		i -= 1;
	}

	return i + 1 + 4 * (k - 1);
}
