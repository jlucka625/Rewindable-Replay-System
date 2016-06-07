// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
#include "RingBuffer.h"
#include "RecordableEntry.h"

#pragma region Iterator

RingBuffer::Iterator::Iterator() :
	mIndex(0), mBuffer(nullptr)
{}

RingBuffer::Iterator::Iterator(uint32 index, const RingBuffer * buffer) :
	mIndex(index), mBuffer(buffer)
{}

bool RingBuffer::Iterator::operator==(const Iterator & rhs)
{
	return ((mBuffer == rhs.mBuffer) && (mIndex == rhs.mIndex));
}

bool RingBuffer::Iterator::operator!=(const Iterator & rhs)
{
	return !operator==(rhs);
}

RingBuffer::Iterator& RingBuffer::Iterator::operator++()
{
	if (mBuffer != nullptr && *this != mBuffer->end())
	{
		mIndex++;
		if (mIndex >= mBuffer->Size())
		{
			mIndex = 0;
		}
	}
	else
	{
		*this = mBuffer->end();
	}

	return *this;
}

RingBuffer::Iterator& RingBuffer::Iterator::operator--()
{
	if (mBuffer != nullptr && *this != mBuffer->begin())
	{
		mIndex--;
		if (mIndex < 0)
		{
			mIndex = mBuffer->Size() - 1;
		}
	}
	else
	{
		*this = mBuffer->begin();
	}

	return *this;
}

RingBuffer::Iterator RingBuffer::Iterator::operator++(int)
{
	Iterator it = *this;
	operator++();
	return it;
}

RingBuffer::Iterator RingBuffer::Iterator::operator--(int)
{
	Iterator it = *this;
	operator--();
	return it;
}

GameFrame& RingBuffer::Iterator::operator*()
{
	return mBuffer->At(mIndex);
}

const GameFrame& RingBuffer::Iterator::operator*() const
{
	return mBuffer->At(mIndex);
}

#pragma endregion

#pragma region Ring Buffer

RingBuffer::RingBuffer(uint32 capacity) :
	mCapacity(capacity)
{
	mContainer = reinterpret_cast<GameFrame*>(malloc(sizeof(GameFrame) * mCapacity));
}

void RingBuffer::Insert(const GameFrame& value)
{
	if (mCurrentIndex >= mCapacity)
	{
		mCurrentIndex = 0;
	}

	if (mSize == mCapacity)
	{
		mContainer[mCurrentIndex].~GameFrame();
		mSize--;
	}

	new(&mContainer[mCurrentIndex++]) GameFrame(value);
	mSize++;
}

GameFrame& RingBuffer::At(uint32 index) const
{
	if (index >= mSize)
		throw std::exception("Invalid index");

	return mContainer[index];
}

RingBuffer::Iterator RingBuffer::begin() const
{
	uint32 beginIndex = mCurrentIndex + 1;
	if (beginIndex >= mSize)
		beginIndex = 0;
	return Iterator(beginIndex, this);
}

RingBuffer::Iterator RingBuffer::end() const
{
	uint32 endIndex = mCurrentIndex - 1;
	if (endIndex < 0)
		endIndex = mSize;

	return Iterator(endIndex, this);
}

const uint32& RingBuffer::Size() const
{
	return mSize;
}

const uint32& RingBuffer::Capacity() const
{
	return mCapacity;
}

RingBuffer::~RingBuffer()
{
	while (mSize > 0)
	{
		mContainer[--mSize].~GameFrame();
	}
	free(mContainer);
}

#pragma endregion
