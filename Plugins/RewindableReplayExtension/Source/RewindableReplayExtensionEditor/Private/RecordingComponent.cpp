// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayExtensionEditorPrivatePCH.h"
#include "ReplayManager.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "RecordingComponent.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/Animation/AnimNode_StateMachine.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

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
	mID = GetOwner()->GetName();
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

void URecordingComponent::SetActorTransform(FTransform& transform)
{
	if (!GetOwner()->GetRootComponent())
		return;

	if (GetOwner()->GetRootComponent()->Mobility == EComponentMobility::Static)
		return;

	GetOwner()->SetActorTransform(transform);
}

const FString & URecordingComponent::GetID() const
{
	return mID;
}

void URecordingComponent::SetID(const FString& id) 
{
	mID = id;
}

void URecordingComponent::UpdateAnimationState(float DeltaTime)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();
		if (mesh)
		{
			//mesh->TickAnimation(DeltaTime);
			mesh->TickAnimation(DeltaTime, false);
			mesh->TickPose(DeltaTime, false);
			mesh->TickComponent(DeltaTime, LEVELTICK_All, &mesh->PrimaryComponentTick);
		}
	}
}

void URecordingComponent::CopyProperties(UClass* OwnerClass, uint8* OwnerObject, TMap<FName, FString>& propertiesAsStrings)
{
	for (UProperty* Property = OwnerClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FString valueAsString;

		if (FBlueprintEditorUtils::PropertyValueToString_Direct(Property, Property->ContainerPtrToValuePtr<uint8>(OwnerObject), valueAsString))
			propertiesAsStrings.Add(Property->GetFName(), valueAsString);
	}
}

void URecordingComponent::SetProperties(UClass* OwnerClass, uint8* OwnerObject, const TMap<FName, FString>& propertiesAsStrings)
{
	for (UProperty* Property = OwnerClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FName name = Property->GetFName();
		const FString* valueAsString = propertiesAsStrings.Find(name);

		if (Property->IsA(UStructProperty::StaticClass()))
		{
			static UScriptStruct* VectorStruct = TBaseStructure<FVector>::Get();
			static UScriptStruct* RotatorStruct = TBaseStructure<FRotator>::Get();
			static UScriptStruct* TransformStruct = TBaseStructure<FTransform>::Get();
			static UScriptStruct* LinearColorStruct = TBaseStructure<FLinearColor>::Get();
			const UStructProperty* StructProperty = CastChecked<UStructProperty>(Property);

			if (StructProperty->Struct != VectorStruct && StructProperty->Struct != RotatorStruct
				&& StructProperty->Struct != TransformStruct && StructProperty->Struct != LinearColorStruct)
			{
				continue;
			}
		}

		if (valueAsString)
		{
			FBlueprintEditorUtils::PropertyValueFromString(Property, *valueAsString, OwnerObject);
		}
	}
}

void URecordingComponent::CopyMovementComponentProperties(TMap<FName, FString>& propertiesAsStrings)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UCharacterMovementComponent* movement = character->GetCharacterMovement();
		if (movement)
		{
			UClass* movementClass = movement->GetClass();
			CopyProperties(movementClass, (uint8*)movement, propertiesAsStrings);
		}
	}
}

void URecordingComponent::SetMovementComponentProperties(const TMap<FName, FString>& propertiesAsStrings)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UCharacterMovementComponent* movement = character->GetCharacterMovement();
		if (movement)
		{
			UClass* movementClass = movement->GetClass();
			SetProperties(movementClass, (uint8*)movement, propertiesAsStrings);
		}
	}
}

void URecordingComponent::CopyCharacterProperties(TMap<FName, FString>& propertiesAsStrings)
{
	AActor* character = GetOwner();
	if (character)
	{
		UClass* characterClass = character->GetClass();
		CopyProperties(characterClass, (uint8*)character, propertiesAsStrings);
	}
}

void URecordingComponent::SetCharacterProperties(const TMap<FName, FString>& propertiesAsStrings)
{
	AActor* character = GetOwner();
	if (character)
	{
		UClass* characterClass = character->GetClass();
		SetProperties(characterClass, (uint8*)character, propertiesAsStrings);
	}
}

void URecordingComponent::CopyAnimBPProperties(TMap<FName, FString>& propertiesAsStrings)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UAnimInstance* animInstance = character->GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			UClass* animClass = animInstance->GetClass();
			CopyProperties(animClass, (uint8*)animInstance, propertiesAsStrings);
		}
	}
}

void URecordingComponent::SetAnimBPProperties(const TMap<FName, FString>& propertiesAsStrings)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UAnimInstance* animInstance = character->GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			UClass* animClass = animInstance->GetClass();
			SetProperties(animClass, (uint8*)animInstance, propertiesAsStrings);
		}
	}
}

void URecordingComponent::GetComponentTransforms(TMap<FName, FTransform>& componentTransforms)
{
	const TSet<UActorComponent*> components = GetOwner()->GetComponents();
	for (auto& component : components)
	{
		USceneComponent* sceneComponent = Cast<USceneComponent>(component);
		if (sceneComponent)
		{
			if (sceneComponent->Mobility == EComponentMobility::Static)
				continue;
			componentTransforms.Add(sceneComponent->GetFName(), sceneComponent->GetComponentTransform());
		}
	}
}

void URecordingComponent::SetComponentTransforms(const TMap<FName, FTransform>& componentTransforms)
{
	const TSet<UActorComponent*> components = GetOwner()->GetComponents();
	for (auto& component : components)
	{
		const FTransform* transform = componentTransforms.Find(component->GetFName());
		if (transform)
		{
			USceneComponent* sceneComponent = Cast<USceneComponent>(component);
			if (sceneComponent->Mobility == EComponentMobility::Static)
				continue;
			
			sceneComponent->SetWorldTransform(*transform);
		}
	}
}