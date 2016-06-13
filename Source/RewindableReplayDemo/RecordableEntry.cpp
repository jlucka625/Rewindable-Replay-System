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
