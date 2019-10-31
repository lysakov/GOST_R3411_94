#pragma once

#include "gost_r3411_94.h"
#include <unordered_set>

namespace attack
{

	class Context
	{

	public:
		Context(uint64_t) noexcept;
		uint64_t inv_permutation(const Hash_block&) noexcept;

	private:
		uint64_t hash_val;
		short inv_phi(short) noexcept;
		inline uint64_t inv_psi(uint64_t) noexcept;

	};

}

//y_0 + z_0 = c => z_0 = c + y_0 = c' => { Z = \psi^{-12}(M), M = perm^{-1}(k_0) + H_{i - 1} } => 
//=> k_0 = \perm(H_{i - 1} + \psi^{12}(C'))
//trunc(Z) = trunc(C') => trunc(psi^{-12}(perm^{-1}(k_0) + H_{i - 1}) = trunc(C')
//trunc(psi^{-12}(perm^{-1}(k_0)) = d => k_0 = k_{0}^{+} + k_{0}^{-}