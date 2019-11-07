#pragma once

#include <utility>

class Feistel_network
{

public:
	Feistel_network(short rounds_num) : rounds_num(rounds_num)
	{
	}

	template <typename R, typename F, typename K>
	std::pair<R, R>& round(F function, const std::pair<R, R>& pair, const K& sub_key)
	{
		R tmp = pair.first;
		pair.first = pair.second;
		pair.second = function(pair.second, sub_key) ^ tmp;
		return pair;
	}

	template <typename R, typename F, typename P, typename K>
	R encrypt(F function, const P& plain_text, const K& key);

private:
	short rounds_num;

};