#include "block.h"
#include "magma.h"
#include "gost_r3411_94.h"
#include "cryptanalysis.h"

int main()
{
	Block block("0 1 2 3 0 3 2 1");
	const char* key = "12345678";
	//Block block_arr[4];
	//block_arr[0] = Block("0 3 1 2 1 1 0 2"); 
	//block_arr[1] = Block("0 3 3 0 1 1 2 0"); 
	//block_arr[2] = Block("0 3 1 2 1 1 0 2"); 
	//block_arr[3] = Block("0 3 3 0 1 1 2 0");
	//Hash_block hash_block(block_arr);

	Block cipher_text = magma::encrypt(block, (const uint8_t*)key);
	Block plain_text = magma::decrypt(cipher_text, (const uint8_t*)key);
	std::cout << cipher_text << std::endl;
	std::cout << plain_text << std::endl;

	std::string msg[] = {"0 3 1 2 1 1 0 2", "0 3 3 0 1 1 2 0", "0 3 1 2 1 1 0 2", "0 3 3 0 1 1 2 0"};
	Hash_block msg_block(msg);
	std::cout << (Hash_block)gost::compress(msg_block, 4) << std::endl;

	attack::Context ctx(0);
	std::cout << ctx.inv_permutation(permutation(115)) << std::endl;

	//std::cout << hash_block << std::endl;
	//std::cout << permutation(hash_block) << std::endl;

	/*uint64_t keys[4] = { 0 };
	keygen(keys, 0, 2);
	for (int i = 0; i < 4; ++i) {
		std::cout << Hash_block(keys[i]) << std::endl;
	}*/

	return 0;
}