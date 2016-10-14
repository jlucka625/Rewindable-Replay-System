#pragma once
#include "Engine.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"

/**
 * Handles interfaces for storing the necessary data to be recorded.
 */

class RecordableEntry
{
public:
	enum class EntryCommands
	{
		SPAWN_ME,
		DESTROY_ME,
		DO_NOTHING
	};

	RecordableEntry() : mEntryCommand(EntryCommands::DO_NOTHING) {};
	~RecordableEntry() = default;

	void SetEntryCommand(EntryCommands command);
	EntryCommands GetEntryCommand();

	void AddTransform(FTransform transform);
	FTransform& GetTransform();

	//void InitMovementComponentProperties();
	TMap<FName, FString>& GetMovementComponentProperties();

	//void InitCharacterProperties();
	TMap<FName, FString>& GetCharacterProperties();

	TMap<FName, FString>& GetAnimBPProperties();

	TMap<FName, FTransform>& GetComponentTransforms();

	UClass* GetClass();
	void SetClass(UClass* uclass);

private:
	UClass* mClass;
	EntryCommands mEntryCommand;
	FTransform mTransform;
	TMap<FName, FString> mMovementComponentProperties;
	TMap<FName, FTransform> mComponentTransforms;
	TMap<FName, FString> mCharacterProperties;
	TMap<FName, FString> mAnimBPProperties;
};

class GameFrame
{
public:
	GameFrame() = default;
	~GameFrame() = default;

	void SaveDeltaTime(float DeltaTime);
	float GetDeltaTime();

	void SaveDebugPrintMessages(TArray<struct FScreenMessageString> PriorityScreenMessages);
	TArray<struct FScreenMessageString>& GetDebugMessages();

	void SaveCameraTransform(FTransform transform);
	FTransform& GetCameraTransform();

	void AddDebugDrawLines(TArray<FBatchedLine> BatchedLines);
	TArray<struct FBatchedLine>& GetDebugDrawLines();

	RecordableEntry* GetRecordableEntry(const FString& id);

private:
	FTransform mCameraTransform;
	//TMap<FString, FTransform> mTransforms;
	//TMap<FString, TMap<FName, FString>> mMovementComponentProperties;
	//TMap<FString, TMap<FName, FString>> mCharacterProperties;
	TMap<FString, RecordableEntry> mRecordableEntries;

	TArray<struct FScreenMessageString> mDebugMessages;
	TArray<struct FBatchedLine> mBatchedLines;
	TArray<struct FBatchedLine> mBatchedForegroundLines;
	TArray<struct FBatchedLine> mBatchedPersistentLines;
	float mDeltaTime;
};
