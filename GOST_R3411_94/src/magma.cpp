#include "magma.h"

magma::Context::Context(const uint8_t *key) noexcept
{
	keygen(key);
}

uint16_t magma::Context::encrypt(uint16_t plain_text) const noexcept
{
	enum {
		ROUNDS_NUM = 32
	};
	auto pair = split(plain_text);
	for (int i = 0; i < ROUNDS_NUM; ++i) {
		round(pair, key_set[i]);
	}
	return connect(pair);
}

uint16_t magma::Context::decrypt(uint16_t cipher_text) const noexcept
{
	enum {
		ROUNDS_NUM = 32
	};
	auto pair = split(cipher_text);
	for (int i = ROUNDS_NUM - 1; i >= 0; --i) {
		round(pair, key_set[i]);
	}
	return connect(pair);
}

uint8_t magma::Context::substitution(const uint8_t part) const noexcept
{
	short subst[] = { 1, 3, 0, 2 };
	Block block = (uint16_t)part << 8;
	for (int i = 0; i < 4; ++i) {
		block[i] = subst[block[i]];
	}
	return (uint8_t)((uint16_t)block >> 8);
}

uint8_t magma::Context::addition(uint8_t part, const uint8_t key) const noexcept
{
	part += key;
	part = substitution(part);
	part = (part << 3) ^ (part >> 5);
	return part;
}

std::pair<uint8_t, uint8_t>& magma::Context::round(std::pair<uint8_t, uint8_t> &pair, const uint8_t k) const noexcept
{
	uint8_t tmp = pair.first;
	pair.first = pair.second;
	pair.second = addition(pair.second, k) ^ tmp;
	return pair;
}

uint16_t magma::Context::connect(std::pair<uint8_t, uint8_t> &pair) const noexcept
{
	uint16_t res = pair.first;
	res = res ^ ((uint16_t)pair.second << 8);
	return res;
}

void magma::Context::keygen(const uint8_t *key) noexcept
{
	for (int i = 0; i < 8; ++i) {
		key_set[i] = key[i];
		key_set[i + 8] = key[i];
		key_set[i + 16] = key[i];
	}
	for (int i = 0; i < 8; ++i) {
		key_set[24 + i] = key[7 - i];
	}
}

std::pair<uint8_t, uint8_t> magma::Context::split(const uint16_t block) const noexcept
{
	std::pair<uint8_t, uint8_t> pair;
	pair.first = (uint8_t)(block >> 8);
	pair.second = (uint8_t)(block & 0xFF);
	return pair;
}

uint16_t magma::encrypt(uint16_t plain_text, const uint8_t *key)
{
	Context ctx(key);
	return ctx.encrypt(plain_text);
}

uint16_t magma::decrypt(uint16_t cipher_text, const uint8_t *key)
{
	Context ctx(key);
	return ctx.decrypt(cipher_text);
}

uint16_t magma::encrypt(uint16_t m, uint64_t key)
{
	return magma::encrypt(m, (uint8_t*)(&key));
}

uint16_t magma::decrypt(uint16_t m, uint64_t key)
{
	return magma::decrypt(m, (uint8_t*)(&key));
}
