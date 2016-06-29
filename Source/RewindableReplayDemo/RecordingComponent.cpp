// Fill out your copyright notice in the Description page of Project Settings.

#include "RewindableReplayDemo.h"
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
	mID = "Actor";
	mID.AppendInt(mInstanceCounter++);
	AReplayManager::AddRecordingComponent(this, mID);



	for (TFieldIterator<UProperty> PropIt(GetOwner()->GetClass()); PropIt; ++PropIt)
	{
		UProperty* Property = *PropIt;
		if (Property->GetClass() == UCameraComponent::StaticClass())
		{

		}
	}
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

FVector URecordingComponent::GetActorVelocity()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UMovementComponent* movement = character->FindComponentByClass<UMovementComponent>();
		if (movement)
		{
			return movement->Velocity;
		}
	}

	return FVector::ZeroVector;

	/*UClass* AnimClass = character->GetMesh()->GetAnimInstance()->GetClass();
	for (TFieldIterator<UProperty> PropIt(AnimClass); PropIt; ++PropIt)
	{
		UProperty* Property = *PropIt;
		Property->ElementSize;
		FVector* v = Property->ContainerPtrToValuePtr<FVector>(character->GetMesh()->GetAnimInstance());
		*v = FVector::ZeroVector;
	}*/
		/*(UProperty* Property = *PropIt;
		if (Property->GetFName() == PropName)
		{
			UFloatProperty* floatProperty = Cast<UFloatProperty>(Property);
			return floatProperty->GetPropertyValue(Property->ContainerPtrToValuePtr<float>(character->GetMesh()->GetAnimInstance()));
		}*/
	//}
	/*static const FName PropName(TEXT("Speed"));
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UClass* AnimClass = character->GetMesh()->GetAnimInstance()->GetClass();
		for (TFieldIterator<UProperty> PropIt(AnimClass); PropIt; ++PropIt)
		{
			UProperty* Property = *PropIt;
			if (Property->GetFName() == PropName)
			{
				UFloatProperty* floatProperty = Cast<UFloatProperty>(Property);
				return floatProperty->GetPropertyValue(Property->ContainerPtrToValuePtr<float>(character->GetMesh()->GetAnimInstance()));
			}
		}*/

		//UMovementComponent* movement = character->FindComponentByClass<UMovementComponent>();
		//if (movement)
		//{
			//return movement->Velocity;
		//}
	//}
}

void URecordingComponent::SetActorVelocity(FVector& velocity)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UMovementComponent* movement = character->FindComponentByClass<UMovementComponent>();
		if (movement)
		{
			movement->Velocity = velocity;
		}
	}
	/*static const FName PropName(TEXT("Speed"));
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UClass* AnimClass = character->GetMesh()->GetAnimInstance()->GetClass();
		for (TFieldIterator<UProperty> PropIt(AnimClass); PropIt; ++PropIt)
		{
			UProperty* Property = *PropIt;
			if (Property->GetFName() == PropName)
			{
				UFloatProperty* floatProperty = Cast<UFloatProperty>(Property);
				if (floatProperty)
				{
					floatProperty->SetPropertyValue(Property->ContainerPtrToValuePtr<float>(character->GetMesh()->GetAnimInstance()), prop);
				}
			}
		}
	}*/

	/*ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		int index = 0;
		UClass* AnimClass = character->GetMesh()->GetAnimInstance()->GetClass();
		for (UProperty* Property = AnimClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
		{
			UFloatProperty* floatProperty = Cast<UFloatProperty>(Property);
			UFloatProperty* floatPropertyCopy = Cast<UFloatProperty>(&properties[index]);
			if (floatProperty && floatPropertyCopy)
			{
				floatProperty->SetPropertyValue(Property->ContainerPtrToValuePtr<float>(AnimClass),
					floatPropertyCopy->GetPropertyValue(Property->ContainerPtrToValuePtr<float>(AnimClass)));
			}*/

			//UNumer
			//UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property);

			//*(Property->ContainerPtrToValuePtr<void>(AnimClass)) = properties[index];
			//UIntProperty* IntProperty = Cast<UIntProperty>(Property);
			//IntProperty->SetVal
			//properties[index];
		//}
		/*UMovementComponent* movement = character->FindComponentByClass<UMovementComponent>();
		if (movement)
		{
			movement->Velocity = velocity;
		}*/
	//}
}

BYTE URecordingComponent::GetMovementMode()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UCharacterMovementComponent* movement = character->GetCharacterMovement();
		return movement->MovementMode;
	}
	return 0;
}

void URecordingComponent::SetMovementMode(BYTE & mode)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UCharacterMovementComponent* movement = character->GetCharacterMovement();
		movement->SetMovementMode(EMovementMode(mode));
	}
}

const FString & URecordingComponent::GetID() const
{
	return mID;
}

float URecordingComponent::GetAnimLength()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();
		if (mesh)
		{
			IAnimClassInterface* anim = IAnimClassInterface::GetFromClass(mesh->AnimClass);
			const TArray<UStructProperty*>& properties = anim->GetAnimNodeProperties();
			for (int32 MachineIndex = 0; MachineIndex < properties.Num(); MachineIndex++)
			{
				UStructProperty* Property = properties[properties.Num() - 1 - MachineIndex];
				if (Property && Property->Struct == FAnimNode_StateMachine::StaticStruct())
				{
					FAnimNode_StateMachine* StateMachine = Property->ContainerPtrToValuePtr<FAnimNode_StateMachine>(mesh->GetAnimInstance());
					if (StateMachine)
					{
						int32 stateIndex = StateMachine->GetCurrentState();
						float length = mesh->GetAnimInstance()->GetRelevantAnimLength(MachineIndex, stateIndex);
						if (length != 0.0f)
							return length;
					}
				}
			}
		}
	}
	return 1.0f;
}

float URecordingComponent::GetAnimTime()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();
		if (mesh)
		{
			IAnimClassInterface* anim = IAnimClassInterface::GetFromClass(mesh->AnimClass);
			const TArray<UStructProperty*>& properties = anim->GetAnimNodeProperties();
			for (int32 MachineIndex = 0; MachineIndex < properties.Num(); MachineIndex++)
			{
				UStructProperty* Property = properties[properties.Num() - 1 - MachineIndex];
				if (Property && Property->Struct == FAnimNode_StateMachine::StaticStruct())
				{
					FAnimNode_StateMachine* StateMachine = Property->ContainerPtrToValuePtr<FAnimNode_StateMachine>(mesh->GetAnimInstance());
					if (StateMachine)
					{
						int32 stateIndex = StateMachine->GetCurrentState();
						float length = mesh->GetAnimInstance()->GetRelevantAnimTime(MachineIndex, stateIndex);
						if (length != 0.0f)
							return length;
					}
				}
			}
		}
	}
	return 1.0f;
}

void URecordingComponent::UpdateAnimationState(float DeltaTime)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();
		if (mesh)
		{
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
			movement->IsA(movementClass);
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
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UClass* characterClass = character->GetClass();
		CopyProperties(characterClass, (uint8*)character, propertiesAsStrings);
	}
}

void URecordingComponent::SetCharacterProperties(const TMap<FName, FString>& propertiesAsStrings)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		UClass* characterClass = character->GetClass();
		SetProperties(characterClass, (uint8*)character, propertiesAsStrings);
	}
}