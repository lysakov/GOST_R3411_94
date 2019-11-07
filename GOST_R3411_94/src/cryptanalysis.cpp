#include "cryptanalysis.h"

attack::Context::Context(uint64_t hash_val) noexcept : hashVal(hash_val)
{
}

bool attack::Context::checkHashVal() const noexcept
{
	if ((hashVal & 0xFF) != ((hashVal & 0xFF00) >> 8)) {
		return false;
	}

	return true;
}

bool attack::Context::findFixedPoints(uint16_t d1)
{
	std::pair<uint8_t, uint8_t> h0;
	h0.first = (hashVal & 0xFF00) >> 8;
	h0.second = hashVal & 0xFF;
	uint64_t size = 0;
	std::cout << keys.max_size() << '\n';
	LinearTransformation A;

	#pragma loop(hint_parallel(0))
	for (uint32_t i = 0; i < UINT32_MAX; ++i) {

		if (A.compute1(i) == d1) {
			std::pair<uint8_t, uint8_t> plainText = h0;

			for (int j = 0; j < 4; ++j) {
				magma::round(plainText, (i >> 8 * (3 - j)) & 0xFF);
			}
			uint16_t res = magma::connect(plainText);

			//++size;

			keys.insert(std::make_pair(res, i));
		}

		if (!(i % 100000000)) {
			std::cout << i << '\n';
		}
	}

	std::cout << "First cycle for d1 = " << d1 << " finished" << std::endl;
	//std::cout << keys.size() << '\n';

	for (uint32_t i = 0; i < UINT32_MAX; ++i) {

		if (!(i % 100000000)) {
			std::cout << i << '\n';
		}

		if (A.compute2(i) == d1) {

			++size;

			std::pair<uint8_t, uint8_t> plainText = h0;

			for (int j = 0; j < 4; ++j) {
				magma::round(plainText, (i >> 8 * j) & 0xFF);
			}

			uint16_t res = ((uint16_t)plainText.first << 8) ^ plainText.second;

			auto iter = keys.find(res);

			if (iter == keys.end()) {
				//std::cerr << "Error\n";
				continue;
			}

			uint64_t key = ((uint64_t)iter->second << 32) ^ i;
			std::cout << key << std::endl;
			uint64_t m = A.invPermutation(key) ^ hashVal;
			uint64_t newHashVal = gost::compress(hashVal, m);

			keys.erase(iter);

			if (birthdayAttackTable.count(newHashVal)) {
				std::cout << m << std::endl;
				std::cout << birthdayAttackTable[newHashVal] << std::endl;
				return true;
			}

			birthdayAttackTable[newHashVal] = m;
		}

	}

	//std::cout << size << '\n';
	//std::cout << keys.size() << '\n';
	keys.clear();
	std::cout << birthdayAttackTable.size() << '\n';

	return false;
}

uint64_t attack::LinearTransformation::invPermutation(const Hash_block &y) noexcept
{
	Hash_block res;

	for (int i = 0; i < 32; ++i) {
		short new_ind = 32 - invPhi_(32 - i);
		res[new_ind / 8][new_ind % 8] = y[i / 8][i % 8];
	}

	return res;
}

inline uint64_t attack::LinearTransformation::invPsi_(uint64_t y) noexcept
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

uint16_t attack::LinearTransformation::operatorA_(uint64_t key) noexcept
{
	uint64_t tmp = invPermutation(key);

	//std::cout << "InvPerm: " << (Hash_block)tmp << '\n';

	for (int i = 0; i < 12; ++i) {
		tmp = invPsi_(tmp);
		//std::cout << "InvPsi" << i << ": " << (Hash_block)tmp << '\n';
	}

	return tmp & 0xFFFF;
}

attack::LinearTransformation::LinearTransformation() noexcept
{
	for (int i = 0; i < 64; ++i) {
		vector[i] = operatorA_((uint64_t)1 << (63 - i));
	}
}

uint16_t attack::LinearTransformation::compute1(uint32_t key) const noexcept
{
	uint16_t res = 0;

	for (int i = 0; i < 32; ++i) {
		if (key & ((uint32_t)1 << (31 - i))) {
			res ^= vector[i];
		}
	}

	return res;
}

uint16_t attack::LinearTransformation::compute2(uint32_t key) const noexcept
{
	uint16_t res = 0;

	for (int i = 0; i < 32; ++i) {
		if (key & ((uint32_t)1 << (31 - i))) {
			res ^= vector[32 + i];
		}
	}

	return res;
}

short attack::LinearTransformation::invPhi_(short x) noexcept
{
	short i = x / 8;
	short k = x % 8;

	if (!k) {
		k = 8;
		i -= 1;
	}

	return i + 1 + 4 * (k - 1);
}
