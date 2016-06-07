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
