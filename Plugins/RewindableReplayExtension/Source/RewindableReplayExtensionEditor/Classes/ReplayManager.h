// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RingBuffer.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"
#include "DrawDebugHelpers.h"
#include "ReplayManager.generated.h"

UCLASS()
class AReplayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	static void AddRecordingComponent(class URecordingComponent* component, FString componentID);

	bool IsRecording();

	void RecordData(float DeltaTime);

	void PlaybackData(float DeltaTime, bool reverse);

	void ToggleRecording();

	void TogglePlayback();

	void ScrubForward();

	void ScrubBackward();

	void RecordTime();

	void DisplayRecordedDebugInfo(GameFrame& frame);

	void DisplayRecordedDebugDrawLines(GameFrame& frame);

	void DisableMotionBlur();

	void EnableMotionBlur();

	static bool sIsRecording;

	static bool sIsPlayback;

	static AReplayManager* GetInstance();

	void ExecuteReplaySystem();

	void Scrub(bool reverse);

	void Begin();

	void End();

private:
	// Sets default values for this actor's properties
	AReplayManager();

	static AReplayManager* sInstance;

	TSharedPtr<class SStandardSlateWidget> ReplayMenu;
	uint32 frameCount;
	float mPreviousSeconds;
	RingBuffer::Iterator mIterator;
	RingBuffer mFrameBuffer;
	float totalPlaybackTime;
	TArray<RingBuffer::Iterator> mScrubbedFrames;
	static TMap<FString, class URecordingComponent*> sRecordingComponents;
	static TArray<FString> mNewlyAddedComponentIDs;
	UCameraComponent* mCamera;
};
