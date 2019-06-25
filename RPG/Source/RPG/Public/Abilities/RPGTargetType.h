// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "RPGTargetType.generated.h"

class ARPGCharacterBase;
class AActor;
struct FGameplayEventData;

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class RPG_API URPGTargetType : public UObject
{
	GENERATED_BODY()

public:
	URPGTargetType() {}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintable)
class RPG_API URPGTargetType_UseOwner : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_UseOwner() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(NotBlueprintable)
class RPG_API URPGTargetType_UseEventData : public URPGTargetType
{
	GENERATED_BODY()

public:
	URPGTargetType_UseEventData() {}

	virtual void GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
