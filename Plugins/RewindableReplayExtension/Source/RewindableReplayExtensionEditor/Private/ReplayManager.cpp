// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "Engine.h"
#include "ReplayManager.h"
#include "RecordingComponent.h"
#include "RecordableEntry.h"
#include "StandardSlateWidget.h"
#include "ReplayStyle.h"

TMap<FString, class URecordingComponent*> AReplayManager::sRecordingComponents;
TArray<FString> AReplayManager::mNewlyAddedComponentIDs;
bool AReplayManager::sIsRecording = false;
bool AReplayManager::sIsPlayback = false;
AReplayManager* AReplayManager::sInstance = nullptr;

AReplayManager* AReplayManager::GetInstance()
{
	return sInstance;
}

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

	sInstance = this;

	frameCount = 0;

	ReplayMenu = SNew(SStandardSlateWidget);

	//EnableInput(GetWorld()->GetFirstPlayerController());
	
	/*InputComponent->BindAction("Record", IE_Pressed, this, &AReplayManager::ToggleRecording).bExecuteWhenPaused = true;
	InputComponent->BindAction("Playback", IE_Released, this, &AReplayManager::TogglePlayback).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubForward", IE_Repeat, this, &AReplayManager::ScrubForward).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubBackward", IE_Repeat, this, &AReplayManager::ScrubBackward).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubForward", IE_Pressed, this, &AReplayManager::RecordTime).bExecuteWhenPaused = true;
	InputComponent->BindAction("ScrubBackward", IE_Pressed, this, &AReplayManager::RecordTime).bExecuteWhenPaused = true;*/

	for (TObjectIterator<UCameraComponent> itr; itr; ++itr)
	{
		if (itr->GetWorld() != GetWorld())
			continue;

		mCamera = nullptr;
		if ((*itr)->GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			mCamera = *itr;
			break;
		}
	}

	if(!mCamera)
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("NO CAMERA!!!")));

	totalPlaybackTime = 0.0f;
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

	if (sIsPlayback)
	{
		if (mIterator != mFrameBuffer.end())
		{
			PlaybackData((*mIterator).GetDeltaTime()/2.0f, false);
			mIterator++;
			ReplayMenu.Get()->GetScrubber().Get()->SetValue((float)frameCount / mFrameBuffer.Size());
			frameCount++;
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"), (*mIterator).GetDeltaTime()));
		}
		else TogglePlayback();
	}
}

void AReplayManager::Begin()
{
	mIterator = mFrameBuffer.begin();
	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;
		ACharacter* character = Cast<ACharacter>(component->GetOwner());
		if (character)
		{
			USkeletalMeshComponent* mesh = character->GetMesh();
			if (mesh)
			{
				mesh->InitAnim(true);
			}
		}
	}
	PlaybackData(0.0f, false);
	frameCount = 0;
	ReplayMenu.Get()->GetScrubber().Get()->SetValue((float)frameCount / mFrameBuffer.Size());
}

void AReplayManager::End()
{
	mIterator = mFrameBuffer.end();
	mIterator--;
	PlaybackData(0.0f, false);
	frameCount = mFrameBuffer.Size() - 1;
	ReplayMenu.Get()->GetScrubber().Get()->SetValue((float)frameCount / mFrameBuffer.Size());
}

void AReplayManager::Scrub(bool reverse)
{
	DisableMotionBlur();
	frameCount = 0;
	int32 index = (int32)(mFrameBuffer.Size() * ReplayMenu.Get()->GetScrubber().Get()->GetValue());
	RingBuffer::Iterator it = mFrameBuffer.begin();

	if (!reverse)
	{
		for (int i = 0; i < index; i++)
		{
			it++;
			frameCount++;
		}
		mIterator = it;
		PlaybackData((*mIterator).GetDeltaTime(), false);
	}
	else
	{
		for (int i = 0; i < index; i++)
		{
			it++;
			frameCount++;
			mIterator = it;
			PlaybackData((*mIterator).GetDeltaTime(), false);
		}
	}

	EnableMotionBlur();
}

void AReplayManager::RecordData(float DeltaTime)
{
	if (sRecordingComponents.Num() == 0)
		return;

	GameFrame frame;

	//for (auto& id : mNewlyAddedComponentIDs)
	//{
		//RecordableEntry* entry = frame.GetRecordableEntry(id);
		//entry->SetEntryCommand(RecordableEntry::EntryCommands::SPAWN_ME);
	//}
	//mNewlyAddedComponentIDs.Empty();

	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;

		RecordableEntry* entry = frame.GetRecordableEntry(id);

		if (!component || !component->IsValidLowLevel() || component->IsPendingKill())
		{
			entry->SetEntryCommand(RecordableEntry::EntryCommands::DO_NOTHING);
			continue;
		}
		else
		{
			entry->SetEntryCommand(RecordableEntry::EntryCommands::SPAWN_ME);
		}

		entry->SetClass(component->GetOwner()->GetClass());

		//frame.InitMovementComponentProperties(id);
		TMap<FName, FString>& movementProperties = entry->GetMovementComponentProperties();
		component->CopyMovementComponentProperties(movementProperties);

		//frame.InitCharacterProperties(id);
		TMap<FName, FString>& characterProperties = entry->GetCharacterProperties();
		component->CopyCharacterProperties(characterProperties);

		FTransform transform = component->GetActorTransform();
		entry->AddTransform(transform);
	}

	frame.SaveDeltaTime(DeltaTime);
	frame.SaveDebugPrintMessages(GEngine->PriorityScreenMessages);
	frame.AddDebugDrawLines(GetWorld()->LineBatcher->BatchedLines);
	frame.AddDebugDrawLines(GetWorld()->ForegroundLineBatcher->BatchedLines);
	frame.AddDebugDrawLines(GetWorld()->PersistentLineBatcher->BatchedLines);

	mFrameBuffer.Insert(frame);
	mIterator = mFrameBuffer.begin();
	frameCount = 0;
}

void AReplayManager::PlaybackData(float DeltaTime, bool reverse)
{
	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;
		RecordableEntry* entry = frame.GetRecordableEntry(id);

		if ((!component || !component->IsValidLowLevel() || component->IsPendingKill())
			&& entry->GetEntryCommand() == RecordableEntry::EntryCommands::SPAWN_ME)
		{
			FTransform transform = entry->GetTransform();
			AActor* spawned = GetWorld()->SpawnActor(entry->GetClass(), &transform);
			URecordingComponent* recordingComponent = Cast<URecordingComponent>(spawned->GetComponentByClass(URecordingComponent::StaticClass()));
			//FString oldID = recordingComponent->GetID();
			//mNewlyAddedComponentIDs.Empty();
			recordingComponent->SetID(id);
			sRecordingComponents[id] = recordingComponent;
		}
		//else if(component && entry->GetEntryCommand() == RecordableEntry::EntryCommands::DESTROY_ME)

		component->SetActorTransform(entry->GetTransform());

		component->SetCharacterProperties(entry->GetCharacterProperties());

		component->SetMovementComponentProperties(entry->GetMovementComponentProperties());

		component->UpdateAnimationState(DeltaTime);
	}

	DisplayRecordedDebugInfo(frame);

	DisplayRecordedDebugDrawLines(frame);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->UpdateCamera(0.0f);
}

void AReplayManager::DisplayRecordedDebugDrawLines(GameFrame& frame)
{
	GetWorld()->LineBatcher->Flush();
	GetWorld()->ForegroundLineBatcher->Flush();
	GetWorld()->PersistentLineBatcher->Flush();

	for (auto& line : frame.GetDebugDrawLines())
	{
		FVector lineStart = line.Start;
		FVector lineEnd = line.End;
		FColor color = line.Color.ToFColor(true);
		float lifetime = 100.0f;
		uint8 depthPriority = line.DepthPriority;
		float thickness = line.Thickness;

		DrawDebugLine(GetWorld(), lineStart, lineEnd, color, true, lifetime, depthPriority, thickness);
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
}

void AReplayManager::TogglePlayback()
{
	AReplayManager::sIsPlayback = !AReplayManager::sIsPlayback;
	AReplayManager::sIsRecording = false;
	
	if (sIsPlayback)
		DisableMotionBlur();
	else
		EnableMotionBlur();

	if (!sIsPlayback)
		ReplayMenu.Get()->GetPlayButtonImage().Get()->SetImage(FReplayStyle::Get()->GetBrush("PlayButton"));
	else
		ReplayMenu.Get()->GetPlayButtonImage().Get()->SetImage(FReplayStyle::Get()->GetBrush("PauseButton"));

	//UGameplayStatics::SetGamePaused(GetWorld(), sIsPlayback);
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
			GameFrame& frame = *mIterator;
			totalSeconds += frame.GetDeltaTime();
			mIterator++;
		}
		else return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"), (*mIterator).GetDeltaTime()));
	totalPlaybackTime += (*mIterator).GetDeltaTime();

	//GEngine->AddOnScreenDebugMessage(-1, 40.0f, FColor::Red, FString::Printf(TEXT("TIME: %f"), totalPlaybackTime));
	mScrubbedFrames.Add(mIterator);
	PlaybackData((*mIterator).GetDeltaTime(), false);

	mPreviousSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
}

void AReplayManager::ScrubBackward()
{
	if (!sIsPlayback)
		return;

	if(mScrubbedFrames.Num() > 0)
		mScrubbedFrames.RemoveAt(mScrubbedFrames.Num() - 1, 1, false);

	if (mScrubbedFrames.Num() == 0)
	{
		mIterator = mFrameBuffer.begin();
		PlaybackData(0.0f, true);
		return;
	}

	mIterator = mFrameBuffer.begin();

	for (auto& pair : sRecordingComponents)
	{
		ACharacter* character = Cast<ACharacter>(pair.Value->GetOwner());
		//character->SetActorHiddenInGame(true);
		if (character)
		{
			USkeletalMeshComponent* mesh = character->GetMesh();
			if (mesh)
				mesh->InitAnim(true);
		}
	}

	for (auto& it : mScrubbedFrames)
	{
		PlaybackData((*mIterator).GetDeltaTime(), true);
		mIterator = it;
	}
	PlaybackData((*mIterator).GetDeltaTime(), true);
}

void AReplayManager::AddRecordingComponent(URecordingComponent * component, FString componentID)
{
	if (!sIsPlayback)
	{
		sRecordingComponents.Add(componentID, component);
		mNewlyAddedComponentIDs.Add(componentID);
	}
}

bool AReplayManager::IsRecording()
{
	return sIsRecording;
}

void AReplayManager::ExecuteReplaySystem()
{
	if (!sIsRecording)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		ToggleRecording();
		GEngine->GameViewport->RemoveViewportWidgetContent(ReplayMenu.ToSharedRef());
	}
	else
	{
		GEngine->GameViewport->AddViewportWidgetContent(ReplayMenu.ToSharedRef());
		ToggleRecording();
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

