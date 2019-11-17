#include "block.h"
#include "magma.h"
#include "gost_r3411_94.h"
#include "cryptanalysis.h"

int main()
{
	attack::Context ctx(0);

	uint32_t i = 0;
	while (!ctx.findCollision(i)) {
		++i;
	}

	return 0;
}

//=============COLLISION=============
//h = 0, m_1 = 11190048871221131458
//h = 0, m_2 = 6632472470296812780