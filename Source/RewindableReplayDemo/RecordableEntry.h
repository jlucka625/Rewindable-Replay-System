#pragma once
#include "Engine.h"
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

	void SaveDeltaTime(float DeltaTime);
	float GetDeltaTime();

	void SaveDebugPrintMessages(TArray<struct FScreenMessageString> PriorityScreenMessages);
	TArray<struct FScreenMessageString>& GetDebugMessages();

	void SaveCameraTransform(FTransform transform);
	FTransform& GetCameraTransform();

private:
	FTransform mCameraTransform;
	TMap<FString, FTransform> mTransforms;
	TArray<struct FScreenMessageString> mDebugMessages;
	float mDeltaTime;
};
