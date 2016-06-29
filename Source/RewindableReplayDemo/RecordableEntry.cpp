// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
#include "RecordableEntry.h"

void GameFrame::AddTransform(FString id, FTransform transform)
{
	mTransforms.Add(id, transform);
}

FTransform GameFrame::GetTransform(const FString& id)
{
	FTransform* transform = mTransforms.Find(id);
	if (transform)
		return *transform;
	else
		throw std::exception("Transform doesn't exist at key!");
}

void GameFrame::AddAnimLength(FString id, float length)
{
	mAnimLengths.Add(id, length);
}

float GameFrame::GetAnimLength(const FString & id)
{
	float* length = mAnimLengths.Find(id);
	if (length)
		return *length;
	else
		throw std::exception("value doesn't exist at key!");
}

void GameFrame::AddAnimTime(FString id, float time)
{
	mAnimTimes.Add(id, time);
}

float GameFrame::GetAnimTime(const FString & id)
{
	float* time = mAnimTimes.Find(id);
	if (time)
		return *time;
	else
		throw std::exception("value doesn't exist at key!");
}

void GameFrame::AddVelocity(FString id, FVector velocity)
{
	mVelocities.Add(id, velocity);
}

FVector& GameFrame::GetVelocity(const FString& id)
{
	FVector* velocity = mVelocities.Find(id);
	if (velocity)
		return *velocity;
	else
		throw std::exception("Vector doesn't exist at key!");
}

void GameFrame::AddMovementMode(FString id, BYTE mode)
{
	mMovement.Add(id, mode);
}

BYTE& GameFrame::GetMovementMode(const FString& id)
{
	BYTE* mode = mMovement.Find(id);
	if (mode)
		return *mode;
	else
		throw std::exception("Byte doesn't exist at key!");
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

void GameFrame::InitMovementComponentProperties(const FString & id)
{
	mMovementComponentProperties.Add(id, TMap<FName, FString>());
}

TMap<FName, FString>& GameFrame::GetMovementComponentProperties(const FString& id)
{
	TMap<FName, FString>* properties = mMovementComponentProperties.Find(id);
	if (properties)
		return *properties;
	else
		throw std::exception("value doesn't exist at key!");
}

void GameFrame::InitCharacterProperties(const FString & id)
{
	mCharacterProperties.Add(id, TMap<FName, FString>());
}

TMap<FName, FString>& GameFrame::GetCharacterProperties(const FString& id)
{
	TMap<FName, FString>* properties = mCharacterProperties.Find(id);
	if (properties)
		return *properties;
	else
		throw std::exception("value doesn't exist at key!");
}
