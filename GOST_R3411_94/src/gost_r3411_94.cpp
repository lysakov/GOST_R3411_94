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
		res[i / 8][i % 8] = block[new_ind / 8][new_ind % 8];
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

Hash_block::Hash_block(const std::string *str) noexcept
{
	block[0] = Block(str[0]);
	block[1] = Block(str[1]);
	block[2] = Block(str[2]);
	block[3] = Block(str[3]);
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
	return (static_cast<uint64_t>(block[0]) << 48) ^ (static_cast<uint64_t>(block[1]) << 32) ^ 
		(static_cast<uint64_t>(block[2]) << 16) ^ static_cast<uint64_t>(block[3]);
}

std::ostream& operator<<(std::ostream &str, const Hash_block &block)
{
	for (int i = 0; i < 4; ++i) {
		str << block[i] << ' ';
	}

	return str;
}

gost::Context::Context(uint64_t iv) noexcept : hash_val(iv)
{
}

uint64_t gost::Context::update(uint64_t m)
{
	keygen(m);

	uint64_t s = encrypt();
	for (int i = 0; i < 12; ++i) {
		s = mix(s);
	}

	uint64_t res = hash_val ^ mix(m ^ s);

	for (int i = 0; i < 61; ++i) {
		res = mix(res);
	}

	return res;
}

void gost::Context::keygen(uint64_t m)
{
	uint64_t consts[3] = { 0 };
	consts[1] = 0xFF00FFFF000000FF;
	//const char* const1[] = { "f f 0 0 f f f f", "0 0 0 0 0 0 f f", "f f 0 0 0 0 f f" "0 0 f f f f 0 0", "0 0 f f 0 0 f f", "0 0 f f 0 0 f f", "f f 0 0 f f 0 0", "f f 0 0 f f 0 0" };
	uint64_t u = hash_val;
	uint64_t v = m;
	uint64_t w = u ^ v;

	keys[0] = permutation(w);

	//std::cout << "key" << 0 << ": " << keys[0] << '\n';

	for (int i = 0; i < 3; ++i) {
		u = static_cast<uint64_t>(mapping_a(u)) ^ consts[i];
		v = mapping_a(mapping_a(v));
		w = u ^ w;
		keys[1 + i] = permutation(w);
		//std::cout << "key" << 1 + i << ": " << keys[1 + i] << '\n';
	}
}

uint64_t gost::Context::encrypt()
{
	s[0] = magma::encrypt(hash_val & 0xFFFF, keys[0]);
	s[1] = magma::encrypt((hash_val >> 16) & 0xFFFF, keys[1]);
	s[2] = magma::encrypt((hash_val >> 32) & 0xFFFF, keys[2]);
	s[3] = magma::encrypt((hash_val >> 48) & 0xFFFF, keys[3]);

	return static_cast<uint64_t>(s[0]) ^ (static_cast<uint64_t>(s[1]) << 16) ^ 
		(static_cast<uint64_t>(s[2]) << 32) ^ (static_cast<uint64_t>(s[3]) << 48);
}

inline uint64_t gost::Context::mix(uint64_t y)
{
	uint64_t mask = 0xF;
	uint64_t res = 0;

	for (int i = 0; i < 4; ++i) {
		uint64_t tmp = (y >> 4 * i) & mask;
		res ^= tmp;
	}

	res ^= (y >> 48) & mask;
	res ^= (y >> 60) & mask;

	return (y >> 4) ^ (res << 60);
}

void gost::Context::test()
{
	std::cout << std::hex;
	std::cout << "A(0x1111222233334444) = " << mapping_a(0x1111222233334444) << std::endl;
	std::cout << "P(0x1111222233334444) = " << permutation(0x1111222233334444) << std::endl;
	std::cout << "PSI(0x1111222233334444) = " << mix(0x1111222233334444) << std::endl;
	std::cout << static_cast<Hash_block>(0x1111222233334444) << std::endl;
	std::cout << static_cast<Hash_block>(permutation(0x1111222233334444)) << std::endl;
	std::cout << std::dec;
	for (int i = 32; i > 0; --i) {
		std::cout << mapping_phi(i) << ' ';
	}
	std::cout << std::endl;
}

uint64_t gost::compress(uint64_t h, uint64_t m) noexcept
{
	gost::Context ctx(h);
	return ctx.update(m);
}