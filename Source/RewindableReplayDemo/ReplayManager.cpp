// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
#include "Engine.h"
#include "ReplayManager.h"
#include "RecordingComponent.h"
#include "RecordableEntry.h"

TMap<FString, class URecordingComponent*> AReplayManager::sRecordingComponents;
bool AReplayManager::sIsRecording = false;
bool AReplayManager::sIsPlayback = false;

// Sets default values
AReplayManager::AReplayManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AReplayManager::BeginPlay()
{
	Super::BeginPlay();	
	EnableInput(GetWorld()->GetFirstPlayerController());
	
	InputComponent->BindAction("Record", IE_Pressed, this, &AReplayManager::ToggleRecording);
	InputComponent->BindAction("Playback", IE_Released, this, &AReplayManager::TogglePlayback);
	InputComponent->BindAction("ScrubForward", IE_Repeat, this, &AReplayManager::ScrubForward);
	InputComponent->BindAction("ScrubBackward", IE_Repeat, this, &AReplayManager::ScrubBackward);
}

void AReplayManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	sRecordingComponents.Empty();
	sIsRecording = false;
	sIsPlayback = false;
}

// Called every frame
void AReplayManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (sIsRecording)
		RecordData();
	//else if (sIsPlayback)
		//PlaybackData();
}

void AReplayManager::RecordData()
{
	if (sRecordingComponents.Num() == 0)
		return;

	GameFrame frame;

	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		FTransform transform = pair.Value->GetActorTransform();

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f, z: %f"), transform.GetLocation().X, transform.GetLocation().Y, transform.GetLocation().Z));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("RECORDING")));
		frame.AddTransform(id, transform);
	}

	mFrameBuffer.Insert(frame);
	mIterator = mFrameBuffer.begin();
}

void AReplayManager::PlaybackData()
{
	//if (mIterator == mFrameBuffer.end())
		//return;

	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;

		FTransform transform = frame.GetTransform(id);

		component->SetActorTransform(transform);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("PLAYBACK")));
	}
}

void AReplayManager::ToggleRecording()
{
	AReplayManager::sIsRecording = !AReplayManager::sIsRecording;
	AReplayManager::sIsPlayback = false;
}

void AReplayManager::TogglePlayback()
{
	AReplayManager::sIsPlayback = !AReplayManager::sIsPlayback;
	AReplayManager::sIsRecording = false;
}

void AReplayManager::ScrubForward()
{
	if (!sIsPlayback)
		return;

	if (mIterator != mFrameBuffer.end())
	{
		mIterator++;
	}
	PlaybackData();
}

void AReplayManager::ScrubBackward()
{
	if (!sIsPlayback)
		return;

	if (mIterator != mFrameBuffer.begin())
	{
		mIterator--;
	}
	PlaybackData();
}

void AReplayManager::AddRecordingComponent(URecordingComponent * component, FString componentID)
{
	sRecordingComponents.Add(componentID, component);
}

bool AReplayManager::IsRecording()
{
	return sIsRecording;
}

