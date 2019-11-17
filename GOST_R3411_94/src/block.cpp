#include "block.h"

Block::Block(const std::string& msg, size_t* sym_read)
{
	enum State {
		STATE_NUM,
		STATE_SPACE
	};
	State state = STATE_SPACE;
	uint16_t pow = 16384; //4^7
	uint16_t result = 0;
	uint16_t sym_cnt = 0;
	for (const auto &sym : msg) {
		if ((sym == '0' || sym == '1' || sym == '2' || 
			sym == '3') && state == STATE_SPACE) {
			state = STATE_NUM;
			result += (sym - '0') * pow;
			pow /= 4;
			++sym_cnt;
			continue;
		}
		if (sym == ' ') {
			state = STATE_SPACE;
			continue;
		}
		throw std::runtime_error("Undefine symbols sequence");
	}
	short padding_value = (BLOCK_SIZE - sym_cnt) % 4;
	if (sym_read) {
		*sym_read = sym_cnt;
	}
	while (sym_cnt != BLOCK_SIZE) {
		result += padding_value * pow;
		pow /= 4;
		++sym_cnt;
	}
	val = result;
}

Block::Block(uint16_t val) noexcept : val(val)
{
}

Ref Block::operator[](short i) noexcept
{
	return Ref(this, i);
}

ConstRef Block::operator[](short pos) const noexcept
{
	return ConstRef(this, pos);
}

bool Block::operator==(const Block& block) const noexcept
{
	return this->val == block.val;
}

bool Block::operator>(const Block& block) const noexcept
{
	return this->val > block.val;
}

bool Block::operator<(const Block& block) const noexcept
{
	return this->val < block.val;
}

Block Block::operator<<(short i) const noexcept
{
	return val << i;
}

Block Block::operator>>(short i) const noexcept
{
	return val >> i;
}

Block Block::operator^(const Block& block) const noexcept
{
	return Block(this->val ^ block.val);
}

Block Block::operator&(uint16_t i) const noexcept
{
	return val & i;
}

Block Block::operator+(const Block& block) const noexcept
{
	return this->val + block.val;
}

Block::operator uint16_t() const noexcept
{
	return val;
}

std::ostream& operator<<(std::ostream& str, const Block& block)
{
	for (short i = 0; i < Block::BLOCK_SIZE; ++i) {
		str << ((block.val >> 2 * (7 - i)) & 0b11);
		if (i != Block::BLOCK_SIZE - 1) {
			str << ' ';
		}
	}
	return str;
 }

Ref::Ref(Block* block, short pos) noexcept : block(block), pos(pos)
{
}

short Ref::operator=(short val) noexcept
{
	uint32_t tmp = val;
	uint32_t mask = 0b11;
	mask <<= 2 * (Block::BLOCK_SIZE - pos - 1);
	mask = ~mask;
	tmp &= 0b11;
	tmp <<= 2 * (Block::BLOCK_SIZE - pos - 1);
	block->val = (block->val & mask) ^ tmp;
	return val;
}

Ref::operator short() const noexcept
{
	return (block->val >> 2 * (Block::BLOCK_SIZE - pos - 1)) & 0b11;
}

ConstRef::ConstRef(const Block* block, short pos) noexcept : block(block), pos(pos)
{
}

ConstRef::operator short() const noexcept
{
	return (block->val >> 2 * (Block::BLOCK_SIZE - pos - 1)) & 0b11;
}