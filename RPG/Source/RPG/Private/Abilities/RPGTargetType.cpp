// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/RPGTargetType.h"
#include "Abilities/RPGGameplayAbility.h"
#include "Core/RPGCharacterBase.h"

void URPGTargetType::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void URPGTargetType_UseOwner::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void URPGTargetType_UseEventData::GetTargets_Implementation(ARPGCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (const FHitResult * FoundHitResult = EventData.ContextHandle.GetHitResult())
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{	
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}
}