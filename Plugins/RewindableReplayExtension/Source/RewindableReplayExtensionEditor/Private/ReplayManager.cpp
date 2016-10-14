// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "Engine.h"
#include "ReplayManager.h"
#include "RecordingComponent.h"
#include "RecordableEntry.h"
#include "StandardSlateWidget.h"
#include "ReplayStyle.h"

//TMap<FString, class URecordingComponent*> AReplayManager::sRecordingComponents;
TArray<TPair<FString, class URecordingComponent*>> AReplayManager::mNewlyAddedComponentIDs;
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
	mNewlyAddedComponentIDs.Empty();
	ComponentClasses.Empty();
	sIsRecording = false;
	sIsPlayback = false;
	sInstance = nullptr;
}

// Called every frame
void AReplayManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (sIsRecording)
		RecordData(DeltaTime);

	if (sIsPlayback)
	{
		if (PlayInReverse)
		{
			if (mIterator != mFrameBuffer.begin())
			{
				PlaybackData(DeltaTime / 2.0f, false);
				mIterator--;
				ReplayMenu.Get()->GetScrubber().Get()->SetValue((float)frameCount / mFrameBuffer.Size());
				frameCount--;
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"), (*mIterator).GetDeltaTime()));
			}
			else TogglePlayback(1.0f);
		}
		else
		{
			if (mIterator != mFrameBuffer.end())
			{
				PlaybackData(DeltaTime / 2.0f, false);
				mIterator++;
				ReplayMenu.Get()->GetScrubber().Get()->SetValue((float)frameCount / mFrameBuffer.Size());
				frameCount++;
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%f"), (*mIterator).GetDeltaTime()));
			}
			else TogglePlayback(1.0f);
		}
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
		if ((component && component->IsValidLowLevel() && !component->IsPendingKill()))
		{
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

void AReplayManager::Scrub()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	DisableMotionBlur();
	frameCount = 0;
	int32 index = (int32)(mFrameBuffer.Size() * ReplayMenu.Get()->GetScrubber().Get()->GetValue());
	RingBuffer::Iterator it = mFrameBuffer.begin();

	for (int i = 0; i < index; i++)
	{
		it++;
		frameCount++;
	}
	mIterator = it;
	PlaybackData((*mIterator).GetDeltaTime(), false);

	EnableMotionBlur();
}

void AReplayManager::RecordData(float DeltaTime)
{
	GameFrame frame;

	for (auto& pair : mNewlyAddedComponentIDs)
	{
		sRecordingComponents.Add(pair.Key, pair.Value);
		ComponentClasses.Add(pair.Key, pair.Value->GetOwner()->GetClass());
		//StopMapGarbageCollection.Add(pair.Value);
	}
	mNewlyAddedComponentIDs.Empty();

	if (sRecordingComponents.Num() == 0)
		return;

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

		FTransform transform = component->GetActorTransform();
		entry->AddTransform(transform);

		TMap<FName, FTransform>& componentTransforms = entry->GetComponentTransforms();
		component->GetComponentTransforms(componentTransforms);

		if (!component->IsDeterministic)
		{
			TMap<FName, FString>& movementProperties = entry->GetMovementComponentProperties();
			component->CopyMovementComponentProperties(movementProperties);

			TMap<FName, FString>& characterProperties = entry->GetCharacterProperties();
			component->CopyCharacterProperties(characterProperties);

			TMap<FName, FString>& animBPProperties = entry->GetAnimBPProperties();
			component->CopyAnimBPProperties(animBPProperties); 
		}
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

bool AReplayManager::IsComponentValid(URecordingComponent* component)
{
	return (component && component->IsValidLowLevel() && !component->IsPendingKill());
}

void AReplayManager::PlaybackData(float DeltaTime, bool reverse)
{
	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;
		RecordableEntry* entry = frame.GetRecordableEntry(id);

		if (!IsComponentValid(component))
		{
			FTransform* transform = &entry->GetTransform();
			AActor* spawned = GetWorld()->SpawnActor(ComponentClasses[id], transform);
			component = Cast<URecordingComponent>(spawned->GetComponentByClass(URecordingComponent::StaticClass()));
			component->SetID(id);
			sRecordingComponents[id] = component;

			if (entry->GetEntryCommand() == RecordableEntry::EntryCommands::DO_NOTHING)
				component->GetOwner()->SetActorHiddenInGame(true);
		}
		else
		{
			if (entry->GetEntryCommand() == RecordableEntry::EntryCommands::SPAWN_ME)
				component->GetOwner()->SetActorHiddenInGame(false);
			else if (entry->GetEntryCommand() == RecordableEntry::EntryCommands::DO_NOTHING)
			{
				component->GetOwner()->SetActorHiddenInGame(true);
				continue;
			}

			component->SetActorTransform(entry->GetTransform());
			component->SetComponentTransforms(entry->GetComponentTransforms());

			if (!component->IsDeterministic)
			{
				component->SetCharacterProperties(entry->GetCharacterProperties());
				component->SetMovementComponentProperties(entry->GetMovementComponentProperties());
				component->SetAnimBPProperties(entry->GetAnimBPProperties());
				component->UpdateAnimationState(DeltaTime);
			}
			else
			{
				component->GetOwner()->Tick(frame.GetDeltaTime());
				const TArray<UActorComponent*> actorComponents = component->GetOwner()->GetComponents();
				for (auto& actorComponent : actorComponents)
					if(&actorComponent->PrimaryComponentTick && !actorComponent->IsA(UMeshComponent::StaticClass()))
						actorComponent->TickComponent(frame.GetDeltaTime(), ELevelTick::LEVELTICK_PauseTick, &actorComponent->PrimaryComponentTick);
			}
		}
	}

	DisplayRecordedDebugInfo(frame);
	DisplayRecordedDebugDrawLines(frame);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->UpdateCamera(0.0f);

		//if ((component && component->IsValidLowLevel() && !component->IsPendingKill()))
			//if (component->GetOwner()->GetLastRenderTime() > 0.01f)
				//continue;

		/*if (entry->GetEntryCommand() == RecordableEntry::EntryCommands::SPAWN_ME)
		{
			if ((!component || !component->IsValidLowLevel() || component->IsPendingKill()))
			{
				FTransform transform = entry->GetTransform();
				AActor* spawned = GetWorld()->SpawnActor(entry->GetClass(), &transform);
				component = Cast<URecordingComponent>(spawned->GetComponentByClass(URecordingComponent::StaticClass()));

				component->SetID(id);
				sRecordingComponents[id] = component;
				//StopMapGarbageCollection.Remove(component);
				//StopMapGarbageCollection.Add(recordingComponent);
				//continue;
			}
			else if ((component && component->IsValidLowLevel() && !component->IsPendingKill()))
			{
				component->SetActorTransform(entry->GetTransform());

				//component->SetComponentTransforms(entry->GetComponentTransforms());

				if (!component->IsDeterministic)
				{
					/*component->SetCharacterProperties(entry->GetCharacterProperties());

					component->SetMovementComponentProperties(entry->GetMovementComponentProperties());

					component->SetAnimBPProperties(entry->GetAnimBPProperties());

					component->UpdateAnimationState(DeltaTime);*/
				/*}
				else
				{
					/*component->GetOwner()->Tick(frame.GetDeltaTime());
					const TArray<UActorComponent*> actorComponents = component->GetOwner()->GetComponents();
					for (auto& actorComponent : actorComponents)
					{
						actorComponent->TickComponent(frame.GetDeltaTime(), ELevelTick::LEVELTICK_PauseTick, &actorComponent->PrimaryComponentTick);
					}*/
				/*}
			}
		}
		else
		{
			if ((component && component->IsValidLowLevel() && !component->IsPendingKill()))
			{
				ComponentsToDelete.Add(component);
				sRecordingComponents[id] = nullptr;

				//component->GetOwner()->K2_DestroyActor();
				//component->GetOwner()->ConditionalBeginDestroy();
				//GetWorld()->ForceGarbageCollection(false);
			}	
		}
	}

	for (auto& comp : ComponentsToDelete)
	{
		comp->GetOwner()->Destroy();
		delete comp->GetOwner();
		comp = nullptr;
	}
	ComponentsToDelete.Empty();
	//GetWorld()->ForceGarbageCollection(true);

	//DisplayRecordedDebugInfo(frame);

	//DisplayRecordedDebugDrawLines(frame);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->UpdateCamera(0.0f);*/
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
		//const FVector2D& TextScale = message.TextScale;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, DisplayColor, DebugMessage/*, true, TextScale*/);
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

void AReplayManager::TogglePlayback(float TimeDilation)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
	AReplayManager::sIsPlayback = !AReplayManager::sIsPlayback;
	AReplayManager::sIsRecording = false;
	
	if (sIsPlayback)
		DisableMotionBlur();
	else
		EnableMotionBlur();

	if (!sIsPlayback)
	{
		ReplayMenu.Get()->GetPlayButtonImage().Get()->SetImage(FReplayStyle::Get()->GetBrush("PlayButton"));
	}
	else
	{
		ReplayMenu.Get()->GetPlayButtonImage().Get()->SetImage(FReplayStyle::Get()->GetBrush("PauseButton"));
		if (TimeDilation < 0.0f)
			PlayInReverse = true;
		else PlayInReverse = false;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), FMath::Abs(TimeDilation));
		GetWorld()->ForceGarbageCollection(true);
	}
}

void AReplayManager::AddRecordingComponent(URecordingComponent * component, FString componentID)
{
	if (sIsRecording)
	{
		int32 index = mNewlyAddedComponentIDs.AddDefaulted();
		mNewlyAddedComponentIDs[index].Key = componentID;
		mNewlyAddedComponentIDs[index].Value = component;
		//mNewlyAddedComponentIDs.Add(TPair<FString, URecordingComponent*>(componentID, component));
	}
}

bool AReplayManager::IsRecording()
{
	return sIsRecording;
}

void AReplayManager::AddPreloadedComponents()
{
	for (TObjectIterator<URecordingComponent> itr; itr; ++itr)
	{
		if ((*itr)->GetWorld() != GetWorld())
			continue;

		URecordingComponent* component = *itr;

		AddRecordingComponent(component, component->GetID());
	}
}

void AReplayManager::ExecuteReplaySystem()
{
	if (!sIsRecording)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		ToggleRecording();
		GEngine->GameViewport->RemoveViewportWidgetContent(ReplayMenu.ToSharedRef());
		AddPreloadedComponents();
	}
	else
	{
		GEngine->GameViewport->AddViewportWidgetContent(ReplayMenu.ToSharedRef(), 1000);
		ToggleRecording();
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void AReplayManager::Stop()
{
	sIsRecording = false;
	sIsPlayback = false;
	GEngine->GameViewport->RemoveViewportWidgetContent(ReplayMenu.ToSharedRef());

	End();
	GameFrame& frame = *mIterator;
	for (auto& pair : sRecordingComponents)
	{
		FString id = pair.Key;
		URecordingComponent* component = pair.Value;
		RecordableEntry* entry = frame.GetRecordableEntry(id);
		if (entry->GetEntryCommand() == RecordableEntry::EntryCommands::DO_NOTHING)
			if (IsComponentValid(component))
				component->GetOwner()->Destroy();
	}
	sRecordingComponents.Empty();
	mNewlyAddedComponentIDs.Empty();
	ComponentClasses.Empty();
	mFrameBuffer.Clear();

	GetWorld()->ForceGarbageCollection(true);

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
}

