// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RingBuffer.h"
#include "ReplayManager.generated.h"

UCLASS()
class REWINDABLEREPLAYDEMO_API AReplayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReplayManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	static void AddRecordingComponent(class URecordingComponent* component, FString componentID);

	bool IsRecording();

	void RecordData(float DeltaTime);

	void PlaybackData();

	void ToggleRecording();

	void TogglePlayback();

	void ScrubForward();

	void ScrubBackward();

	void RecordTime();

	void DisplayRecordedDebugInfo(GameFrame& frame);

	void DisableMotionBlur();

	void EnableMotionBlur();

	static bool sIsRecording;

	static bool sIsPlayback;

private:
	float mPreviousSeconds;
	RingBuffer::Iterator mIterator;
	RingBuffer mFrameBuffer;
	static TMap<FString, class URecordingComponent*> sRecordingComponents;
	UCameraComponent* mCamera;
};
