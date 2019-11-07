#pragma once

#include "gost_r3411_94.h"
#include "magma.h"

#include <map>
#include <vector>

namespace attack
{

	class Context
	{

	public:
		Context(uint64_t) noexcept;
		bool checkHashVal() const noexcept;
		bool findFixedPoints(uint16_t);

	private:
		uint64_t hashVal;
		std::multimap<uint16_t, uint32_t> keys;
		std::map<uint64_t, uint64_t> birthdayAttackTable;
	};

	class LinearTransformation
	{

	public:
		LinearTransformation() noexcept;
		uint16_t compute1(uint32_t key) const noexcept;
		uint16_t compute2(uint32_t key) const noexcept;
		uint64_t invPermutation(const Hash_block&) noexcept;
		uint16_t operatorA_(uint64_t key) noexcept;
		short invPhi_(short) noexcept;

	private:
		uint16_t vector[64] = { 0 };

		inline uint64_t invPsi_(uint64_t) noexcept;

	};

}

//y_0 + z_0 = c => z_0 = c + y_0 = c' => { Z = \psi^{-12}(M), M = perm^{-1}(k_0) + H_{i - 1} } => 
//=> k_0 = \perm(H_{i - 1} + \psi^{12}(C'))
//trunc(Z) = trunc(C') => trunc(psi^{-12}(perm^{-1}(k_0) + H_{i - 1}) = trunc(C')
//trunc(psi^{-12}(perm^{-1}(k_0)) = d => k_0 = k_{0}^{+} + k_{0}^{-}