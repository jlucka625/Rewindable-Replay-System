#pragma once

/**
 * Handles interfaces for storing the necessary data to be recorded.
 */

class REWINDABLEREPLAYDEMO_API RecordableEntry
{
public:
	RecordableEntry() = default;
	~RecordableEntry() = default;

};

class REWINDABLEREPLAYDEMO_API GameFrame
{
public:
	GameFrame() = default;
	~GameFrame() = default;

	void AddTransform(FString id, FTransform transform);
	FTransform GetTransform(const FString& id);

private:
	TMap<FString, FTransform> mTransforms;
};
