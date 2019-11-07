#include "block.h"
#include "magma.h"
#include "gost_r3411_94.h"
#include "cryptanalysis.h"

int main()
{
	/*Block block("0 1 2 3 0 3 2 1");
	const char* key = "12345678";*/
	//Block block_arr[4];
	//block_arr[0] = Block("0 3 1 2 1 1 0 2"); 
	//block_arr[1] = Block("0 3 3 0 1 1 2 0"); 
	//block_arr[2] = Block("0 3 1 2 1 1 0 2"); 
	//block_arr[3] = Block("0 3 3 0 1 1 2 0");
	//Hash_block hash_block(block_arr);

	//uint64_t key = 0xdefb28e837c5cb41;

	uint64_t key = 18569995496846673;
	Block cipher_text = magma::encrypt(0, key);
	Block plain_text = magma::decrypt(cipher_text, key);
	std::cout << (uint16_t)cipher_text << std::endl;
	std::cout << plain_text << std::endl;

	//std::string msg[] = {"0 3 1 2 1 1 0 2", "0 3 3 0 1 1 2 0", "0 3 1 2 1 1 0 2", "0 3 3 0 1 1 2 0"};
	//Hash_block msg_block(msg);

	//std::cout << std::hex << std::endl;

	uint64_t h = 0x1a2b3c4d5e6f7788;
	uint64_t m = 0x1212121234343434;
	std::cout << std::hex << gost::compress(h, m) << std::dec << std::endl;

	attack::Context ctx(0);

	gost::Context gostCtx(0);
	//gostCtx.test();

	//attack::LinearTransformation A;
	//std::cout << A.invPermutation(permutation(935)) << std::endl;

	uint32_t i = 0;
	while (!ctx.findFixedPoints(i)) {
		++i;
	}

	//attack::LinearTransformation A;
	//for (int i = 0; i < 10000; ++i) {
	//	if (A.compute1(i) != A.operatorA_((uint64_t)i << 32)) {
	//		std::cout << i << '\n';
	//	}
	//}

	//std::cout << hash_block << std::endl;
	//std::cout << permutation(hash_block) << std::endl;

	/*uint64_t keys[4] = { 0 };
	keygen(keys, 0, 2);
	for (int i = 0; i < 4; ++i) {
		std::cout << Hash_block(keys[i]) << std::endl;
	}*/

	return 0;
}

//Fixed points for 0:
//1018925882268576149
//12200624335459838630
//6801463072697478340
//18060295913004461525
//17424694629266222822
//653681597128765440
//12142445185209594129
//6589510705837371938
//12142279854738502451
//535943001513522240
//18569995496846673
//306430660863719283
//17424439546863548544
//12295391100923935667

//h = 0x1a2b3c4d5e6f7788;
//m = 0x1212121234343434;
//
//const3 = 0xFF00FFFF000000FF;
//
//key0: 0x1563993afa31bbc
//key1 : 0x31439f55355c60a7
//key2 : 0xbd41fa3af3f00815
//key3 : 0x95816c6c9f9a9e9e
//
//A(0x1111222233334444) = 0x7777111122223333
//P(0x1111222233334444) = 0x55af000055af0
//PSI(0x1111222233334444) = 0x111122223333444