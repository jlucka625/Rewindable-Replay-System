// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
#include "RecordableEntry.h"

void RecordableEntry::SetEntryCommand(EntryCommands command)
{
	mEntryCommand = command;
}

RecordableEntry::EntryCommands RecordableEntry::GetEntryCommand()
{
	return mEntryCommand;
}

void RecordableEntry::AddTransform(FTransform transform)
{
	mTransform = transform;
}

FTransform RecordableEntry::GetTransform()
{
	return mTransform;
}

void GameFrame::SaveDeltaTime(float DeltaTime)
{
	mDeltaTime = DeltaTime;
}

float GameFrame::GetDeltaTime()
{
	return mDeltaTime;
}

void GameFrame::SaveDebugPrintMessages(TArray<struct FScreenMessageString> PriorityScreenMessages)
{
	mDebugMessages = PriorityScreenMessages;
}

TArray<struct FScreenMessageString>& GameFrame::GetDebugMessages()
{
	return mDebugMessages;
}

void GameFrame::SaveCameraTransform(FTransform transform)
{
	mCameraTransform = transform;
}

FTransform & GameFrame::GetCameraTransform()
{
	return mCameraTransform;
}

void GameFrame::AddDebugDrawLines(TArray<FBatchedLine> BatchedLines)
{
	mBatchedLines.Append(BatchedLines);
}

TArray<FBatchedLine>& GameFrame::GetDebugDrawLines()
{
	return mBatchedLines;
}

RecordableEntry* GameFrame::GetRecordableEntry(const FString & id)
{
	RecordableEntry* entry = mRecordableEntries.Find(id);
	if (entry)
	{
		return entry;
	}
	else
	{
		return &mRecordableEntries.Add(id);
	}
}

/*void RecordableEntry::InitMovementComponentProperties()
{
	mMovementComponentProperties.Add(id, TMap<FName, FString>());
}*/

TMap<FName, FString>& RecordableEntry::GetMovementComponentProperties()
{
	return mMovementComponentProperties;
}

/*void GameFrame::InitCharacterProperties(const FString & id)
{
	mCharacterProperties.Add(id, TMap<FName, FString>());
}*/

TMap<FName, FString>& RecordableEntry::GetCharacterProperties()
{
	return mCharacterProperties;
	/*TMap<FName, FString>* properties = mCharacterProperties.Find(id);
	if (properties)
		return *properties;
	else
		throw std::exception("value doesn't exist at key!");*/
}

UClass * RecordableEntry::GetClass()
{
	return mClass;
}

void RecordableEntry::SetClass(UClass * uclass)
{
	mClass = uclass;
}
