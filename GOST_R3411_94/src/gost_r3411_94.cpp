#include "gost_r3411_94.h"

static uint64_t mapping_a(Hash_block y)
{
	Hash_block res;
	res[0] = y[2] ^ y[3];
	res[1] = y[0];
	res[2] = y[1];
	res[3] = y[2];
	return res;
}

static short mapping_phi(short x)
{
	short i = (x - 1) % 4;
	short k = (x - 1) / 4 + 1;
	return 8 * i + k;
}

uint64_t permutation(const Hash_block& block)
{
	Hash_block res;
	for (int i = 0; i < 32; ++i) {
		short new_ind = 32 - mapping_phi(32 - i);
		res[new_ind / 8][new_ind % 8] = block[i / 8][i % 8];
	}
	return res;
}

Hash_block::Hash_block() noexcept
{
}

Hash_block::Hash_block(uint64_t val) noexcept
{
	block[0] = val >> 48;
	block[1] = (val >> 32) & 0xFFFF;
	block[2] = (val >> 16) & 0xFFFF;
	block[3] = val & 0xFFFF;
}

Hash_block::Hash_block(const Block *block) noexcept
{
	for (int i = 0; i < 4; ++i) {
		(*this)[i] = block[i];
	}
}

Block& Hash_block::operator[](short i) noexcept
{
	return block[i];
}

const Block& Hash_block::operator[](short i) const noexcept
{
	return block[i];
}

Hash_block Hash_block::operator^(const Hash_block &block) const noexcept
{
	Hash_block res;
	for (int i = 0; i < 4; ++i) {
		res[i] = this->block[i] ^ block[i];
	}
	return res;
}

Hash_block::operator uint64_t() const noexcept
{
	return ((uint64_t)block[0] << 48) ^ ((uint64_t)block[1] << 32) ^ ((uint64_t)block[2] << 16) ^ (uint64_t)block[3];
}

std::ostream& operator<<(std::ostream &str, const Hash_block &block)
{
	for (int i = 0; i < 4; ++i) {
		str << block[i] << ' ';
	}
	return str;
}

gost::Context::Context(uint64_t iv) : hash_val(iv)
{
}

void gost::Context::update(uint64_t)
{
}

void gost::Context::keygen(uint64_t m)
{
	uint64_t consts[3];
	consts[1] = 0xFF00FFFF000000FF;
	uint64_t u = hash_val;
	uint64_t v = m;
	uint64_t w = u ^ v;
	keys[0] = permutation(w);
	for (int i = 0; i < 3; ++i) {
		u = (uint64_t)mapping_a(u) ^ consts[i];
		v = mapping_a(mapping_a(v));
		w = u ^ w;
		keys[1 + i] = permutation(w);
	}
}

void gost::Context::encrypt(uint64_t m)
{
	s[0] = magma::encrypt(m & 0xFFFF, keys[0]);
	s[1] = magma::encrypt((m >> 16) & 0xFFFF, keys[1]);
	s[2] = magma::encrypt((m >> 32) & 0xFFFF, keys[2]);
	s[3] = magma::encrypt((m >> 48) & 0xFFFF, keys[3]);
}
