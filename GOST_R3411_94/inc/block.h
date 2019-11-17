#pragma once

#include <iostream>
#include <cstdint>

class Block;

class Ref
{

public:
	Ref(Block*, short) noexcept;
	short operator= (short) noexcept;
	operator short() const noexcept;

private:
	Block* block;
	short pos;

};

class ConstRef
{

public:
	ConstRef(const Block*, short) noexcept;
	operator short() const noexcept;

private:
	const Block* block;
	short pos;

};

class Block
{

public:
	enum {
		BLOCK_SIZE = 8
	};
	Block(const std::string&, size_t* = nullptr);
	Block(uint16_t = 0) noexcept;
	Ref operator[] (short) noexcept;
	ConstRef operator[] (short) const noexcept;
	bool operator== (const Block&) const noexcept;
	bool operator> (const Block&) const noexcept;
	bool operator< (const Block&) const noexcept; 
	Block operator<< (short) const noexcept;
	Block operator>> (short) const noexcept;
	Block operator^ (const Block&) const noexcept;
	Block operator& (uint16_t) const noexcept;
	Block operator+ (const Block&) const noexcept;
	operator uint16_t() const noexcept;
	friend std::ostream& operator<< (std::ostream&, const Block&);
	friend short Ref::operator= (short) noexcept;
	friend Ref::operator short() const noexcept;
	friend ConstRef::operator short() const noexcept;
	

private:
	uint16_t val = 0;

};

