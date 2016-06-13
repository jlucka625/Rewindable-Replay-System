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
	PrimaryActorTick.bTickEvenWhenPaused = true;

}

// Called when the game starts or when spawned
void AReplayManager::BeginPlay()
{
	Super::BeginPlay();	
	EnableInput(GetWorld()->GetFirstPlayerController());
	
	InputComponent->BindAction("Record", IE_Pressed, this, &AReplayManager::ToggleRecording).bExecuteWhenPaused = true;
	InputComponent->BindAction("Playback", IE_Released, this, &AReplayManager::TogglePlayback).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubForward", IE_Repeat, this, &AReplayManager::ScrubForward).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubBackward", IE_Repeat, this, &AReplayManager::ScrubBackward).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubForward", IE_Pressed, this, &AReplayManager::RecordTime).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubBackward", IE_Pressed, this, &AReplayManager::RecordTime).bExecuteWhenPaused = true;

	for (TObjectIterator<UCameraComponent> itr; itr; ++itr)
	{
		if (itr->GetWorld() != GetWorld())
			continue;

		mCamera = nullptr;
		if ((*itr)->GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			mCamera = *itr;
		}
	}
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
		RecordData(DeltaTime);
}

void AReplayManager::RecordData(float DeltaTime)
{
	if (sRecordingComponents.Num() == 0)
		return;

	GameFrame frame;

	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		FTransform transform = pair.Value->GetActorTransform();

		frame.AddTransform(id, transform);
		frame.SaveDeltaTime(DeltaTime);

		frame.SaveDebugPrintMessages(GEngine->PriorityScreenMessages);
	}

	mFrameBuffer.Insert(frame);
	mIterator = mFrameBuffer.begin();
}

void AReplayManager::PlaybackData()
{
	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;

		FTransform transform = frame.GetTransform(id);

		component->SetActorTransform(transform);

		DisplayRecordedDebugInfo(frame);

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->UpdateCamera(0.0f);
	}
}

void AReplayManager::DisplayRecordedDebugInfo(GameFrame& frame)
{
	TArray<struct FScreenMessageString>& debugMessages = frame.GetDebugMessages();
	GEngine->ClearOnScreenDebugMessages();
	for (auto& message : debugMessages)
	{
		float TimeToDisplay = message.TimeToDisplay;
		FColor DisplayColor = message.DisplayColor;
		const FString& DebugMessage = message.ScreenMessage;
		const FVector2D& TextScale = message.TextScale;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DisplayColor, DebugMessage, true, TextScale);
	}
}

void AReplayManager::DisableMotionBlur()
{
	if (mCamera)
		mCamera->PostProcessSettings.MotionBlurAmount = 0.0f;
	const_cast<float&>(GetWorld()->PostProcessVolumes[0]->GetProperties().Settings->MotionBlurAmount) = 0.0f;
}

void AReplayManager::EnableMotionBlur()
{
	if (mCamera)
		mCamera->PostProcessSettings.MotionBlurAmount = 0.5f;

	const_cast<float&>(GetWorld()->PostProcessVolumes[0]->GetProperties().Settings->MotionBlurAmount) = 0.5f;
}

void AReplayManager::ToggleRecording()
{
	AReplayManager::sIsRecording = !AReplayManager::sIsRecording;
	AReplayManager::sIsPlayback = false;
	EnableMotionBlur();
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AReplayManager::TogglePlayback()
{
	AReplayManager::sIsPlayback = !AReplayManager::sIsPlayback;
	AReplayManager::sIsRecording = false;
	
	if (sIsPlayback)
		DisableMotionBlur();
	else
		EnableMotionBlur();

	UGameplayStatics::SetGamePaused(GetWorld(), sIsPlayback);
}

void AReplayManager::RecordTime()
{
	mPreviousSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

void AReplayManager::ScrubForward()
{
	if (!sIsPlayback)
		return;

	float currentSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	float deltaSeconds = currentSeconds - mPreviousSeconds;

	float totalSeconds = 0.0f;
	while (totalSeconds < deltaSeconds)
	{
		if (mIterator != mFrameBuffer.end())
		{
			GameFrame& frame = *(mIterator++);
			totalSeconds += frame.GetDeltaTime();
		}
		else break;
	}

	PlaybackData();

	mPreviousSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

void AReplayManager::ScrubBackward()
{
	if (!sIsPlayback)
		return;

	float currentSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	float deltaSeconds = currentSeconds - mPreviousSeconds;

	float totalSeconds = 0.0f;
	while (totalSeconds < deltaSeconds)
	{
		if (mIterator != mFrameBuffer.begin())
		{
			GameFrame& frame = *(mIterator--);
			totalSeconds += frame.GetDeltaTime();
		}
		else break;
	}

	PlaybackData();

	mPreviousSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

void AReplayManager::AddRecordingComponent(URecordingComponent * component, FString componentID)
{
	sRecordingComponents.Add(componentID, component);
}

bool AReplayManager::IsRecording()
{
	return sIsRecording;
}

