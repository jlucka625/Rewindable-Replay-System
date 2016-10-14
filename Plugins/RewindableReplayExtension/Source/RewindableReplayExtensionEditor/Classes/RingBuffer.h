#pragma once

/**
 * Circular buffer that will store frame information
 */
class GameFrame;
class RingBuffer
{
public:
	class Iterator
	{
		friend class RingBuffer;
	public:
		Iterator();
		bool operator==(const Iterator& rhs);
		bool operator!=(const Iterator& rhs);
		Iterator& operator++();
		Iterator operator++(int);
		Iterator& operator--();
		Iterator operator--(int);
		GameFrame& operator*();
		const GameFrame& operator*() const;
		const uint32 GetIndex() const { return mIndex; }
	private:
		Iterator(uint32 index, const RingBuffer* buffer);
		uint32 mIndex;
		const RingBuffer* mBuffer;
	};
	RingBuffer(uint32 capacity = 1000000);
	~RingBuffer();

	void Insert(const GameFrame& value);
	GameFrame& At(uint32 index) const;

	Iterator begin() const;
	Iterator end() const;

	const uint32& Size() const;
	const uint32& Capacity() const;

	void Clear();

private:
	GameFrame* mContainer;

	uint32 mSize;
	uint32 mCapacity;
	uint32 mCurrentIndex;

};
