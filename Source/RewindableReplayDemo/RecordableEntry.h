#pragma once
#include "Engine.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"

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

	void AddAnimLength(FString id, float length);
	float GetAnimLength(const FString& id);

	void AddAnimTime(FString id, float time);
	float GetAnimTime(const FString& id);

	void AddVelocity(FString id, FVector velocity);
	FVector& GetVelocity(const FString& id);

	void AddMovementMode(FString id, BYTE mode);
	BYTE& GetMovementMode(const FString& id);

	void SaveDeltaTime(float DeltaTime);
	float GetDeltaTime();

	void SaveDebugPrintMessages(TArray<struct FScreenMessageString> PriorityScreenMessages);
	TArray<struct FScreenMessageString>& GetDebugMessages();

	void SaveCameraTransform(FTransform transform);
	FTransform& GetCameraTransform();

	void AddDebugDrawLines(TArray<FBatchedLine> BatchedLines);
	TArray<struct FBatchedLine>& GetDebugDrawLines();

	//void AddMovementComponentProperties(const FString& id, void* buffer);
	//void* GetMovementComponentProperties(const FString& id);

	void InitMovementComponentProperties(const FString& id);
	TMap<FName, FString>& GetMovementComponentProperties(const FString& id);

	void InitCharacterProperties(const FString& id);
	TMap<FName, FString>& GetCharacterProperties(const FString& id);

private:
	FTransform mCameraTransform;
	TMap<FString, FTransform> mTransforms;
	TMap<FString, FVector> mVelocities;
	TMap<FString, BYTE> mMovement;
	TMap<FString, float> mAnimLengths;
	TMap<FString, float> mAnimTimes;
	TMap<FString, TMap<FName, FString>> mMovementComponentProperties;
	TMap<FString, TMap<FName, FString>> mCharacterProperties;

	TArray<struct FScreenMessageString> mDebugMessages;
	TArray<struct FBatchedLine> mBatchedLines;
	TArray<struct FBatchedLine> mBatchedForegroundLines;
	TArray<struct FBatchedLine> mBatchedPersistentLines;
	float mDeltaTime;
};
