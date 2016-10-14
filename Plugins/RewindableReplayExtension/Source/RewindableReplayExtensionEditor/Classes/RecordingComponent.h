// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "RecordingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URecordingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecordingComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	FTransform GetActorTransform();

	void SetActorTransform(FTransform& transform);

	const FString& GetID() const;

	void SetID(const FString& id);

	void UpdateAnimationState(float DeltaTime);

	void CopyProperties(UClass* OwnerClass, uint8* OwnerObject, TMap<FName, FString>& propertiesAsStrings);
	
	void SetProperties(UClass* OwnerClass, uint8* OwnerObject, const TMap<FName, FString>& propertiesAsStrings);

	void CopyMovementComponentProperties(TMap<FName, FString>& propertiesAsStrings);

	void SetMovementComponentProperties(const TMap<FName, FString>& propertiesAsStrings);

	void CopyCharacterProperties(TMap<FName, FString>& propertiesAsStrings);

	void SetCharacterProperties(const TMap<FName, FString>& propertiesAsStrings);

	void CopyAnimBPProperties(TMap<FName, FString>& propertiesAsStrings);

	void SetAnimBPProperties(const TMap<FName, FString>& propertiesAsStrings);

	void GetComponentTransforms(TMap<FName, FTransform>& componentTransforms);

	void SetComponentTransforms(const TMap<FName, FTransform>& componentTransforms);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Determinism)
		bool IsDeterministic;

private:
	static uint32 mInstanceCounter;
	FString mID;
};
