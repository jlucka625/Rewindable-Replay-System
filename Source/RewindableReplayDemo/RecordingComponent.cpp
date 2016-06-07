// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
#include "ReplayManager.h"
#include "RecordingComponent.h"

uint32 URecordingComponent::mInstanceCounter = 0;

// Sets default values for this component's properties
URecordingComponent::URecordingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URecordingComponent::BeginPlay()
{
	Super::BeginPlay();
	mID = "Actor";
	mID.AppendInt(mInstanceCounter++);
	AReplayManager::AddRecordingComponent(this, mID);
}


// Called every frame
void URecordingComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

FTransform URecordingComponent::GetActorTransform()
{
	return GetOwner()->GetTransform();
}

void URecordingComponent::SetActorTransform(FTransform transform)
{
	GetOwner()->SetActorTransform(transform);
}

const FString & URecordingComponent::GetID() const
{
	return mID;
}

