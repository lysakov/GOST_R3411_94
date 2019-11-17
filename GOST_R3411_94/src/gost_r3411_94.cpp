#include "gost_r3411_94.h"

static uint64_t mappingA(uint64_t y)
{
	uint64_t tmp1 = y >> 16;
	uint64_t tmp2 = (y << 48) ^ ((y << 32) & 0xFFFF000000000000);
	return tmp1 ^ tmp2;
}

static short mappingPhi(short x)
{
	short i = (x - 1) % 4;
	short k = (x - 1) / 4 + 1;
	return 8 * i + k;
}


HashBlock::HashBlock() noexcept
{
}

HashBlock::HashBlock(uint64_t val) noexcept
{
	block[0] = val >> 48;
	block[1] = (val >> 32) & 0xFFFF;
	block[2] = (val >> 16) & 0xFFFF;
	block[3] = val & 0xFFFF;
}

HashBlock::HashBlock(const Block *block) noexcept
{
	for (int i = 0; i < 4; ++i) {
		(*this)[i] = block[i];
	}
}

HashBlock::HashBlock(const std::string *str) noexcept
{
	block[0] = Block(str[0]);
	block[1] = Block(str[1]);
	block[2] = Block(str[2]);
	block[3] = Block(str[3]);
}

Block& HashBlock::operator[](short i) noexcept
{
	return block[i];
}

const Block& HashBlock::operator[](short i) const noexcept
{
	return block[i];
}

HashBlock HashBlock::operator^(const HashBlock &block) const noexcept
{
	HashBlock res;
	for (int i = 0; i < 4; ++i) {
		res[i] = this->block[i] ^ block[i];
	}

	return res;
}

HashBlock::operator uint64_t() const noexcept
{
	return (static_cast<uint64_t>(block[0]) << 48) ^ (static_cast<uint64_t>(block[1]) << 32) ^ 
		(static_cast<uint64_t>(block[2]) << 16) ^ static_cast<uint64_t>(block[3]);
}

std::ostream& operator<<(std::ostream &str, const HashBlock &block)
{
	for (int i = 0; i < 4; ++i) {
		str << block[i] << ' ';
	}

	return str;
}

gost::Context::Context(uint64_t iv) noexcept : hashVal(iv)
{
}

uint64_t gost::Context::update(uint64_t m)
{
	keygen_(m);

	uint64_t s = encrypt_();

	for (int i = 0; i < 12; ++i) {
		s = psi_(s);
	}

	uint64_t res = hashVal ^ psi_(m ^ s);

	for (int i = 0; i < 61; ++i) {
		res = psi_(res);
	}

	return res;
}

void gost::Context::keygen_(uint64_t m)
{
	uint64_t consts[3] = { 0 };
	consts[1] = 0xFF00FFFF000000FF;
	uint64_t u = hashVal;
	uint64_t v = m;
	uint64_t w = u ^ v;

	keys[0] = permutation_(w);

	for (int i = 0; i < 3; ++i) {
		u = static_cast<uint64_t>(mappingA(u)) ^ consts[i];
		v = mappingA(mappingA(v));
		w = u ^ v;
		keys[1 + i] = permutation_(w);
	}
}

uint64_t gost::Context::encrypt_()
{
	s[0] = magma::encrypt(hashVal & 0xFFFF, keys[0]);
	s[1] = magma::encrypt((hashVal >> 16) & 0xFFFF, keys[1]);
	s[2] = magma::encrypt((hashVal >> 32) & 0xFFFF, keys[2]);
	s[3] = magma::encrypt((hashVal >> 48) & 0xFFFF, keys[3]);

	return static_cast<uint64_t>(s[0]) ^ (static_cast<uint64_t>(s[1]) << 16) ^ 
		(static_cast<uint64_t>(s[2]) << 32) ^ (static_cast<uint64_t>(s[3]) << 48);
}

uint64_t gost::Context::permutation_(const HashBlock& block)
{
	HashBlock res;
	for (int i = 0; i < 32; ++i) {
		short newInd = 32 - mappingPhi(32 - i);
		res[i / 8][i % 8] = block[newInd / 8][newInd % 8];
	}

	return res;
}

inline uint64_t gost::Context::psi_(uint64_t y)
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
	std::cout << "A(0x1111222233334444) = " << mappingA(0x1111222233334444) << std::endl;
	std::cout << "P(0x1111222233334444) = " << permutation_(0x1111222233334444) << std::endl;
	std::cout << "PSI(0x1111222233334444) = " << psi_(0x1111222233334444) << std::endl;
	std::cout << static_cast<HashBlock>(0x1111222233334444) << std::endl;
	std::cout << static_cast<HashBlock>(permutation_(0x1111222233334444)) << std::endl;
	std::cout << std::dec;
	for (int i = 32; i > 0; --i) {
		std::cout << mappingPhi(i) << ' ';
	}
	std::cout << std::endl;
}

uint64_t gost::compress(uint64_t h, uint64_t m) noexcept
{
	gost::Context ctx(h);
	return ctx.update(m);
}