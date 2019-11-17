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