// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	static URPGAbilitySystemComponent* GetAbilitySystemComonentFromActor(const AActor* Actor, bool LookForComponent = false);

	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<class URPGGameplayAbility*>& ActiveAbilities) const;
	void RegisterPredictionEventsForAbilityInstance(URPGGameplayAbility* Instance);

protected:
	virtual void OnServerAbilityCaughtUp(URPGGameplayAbility* Ability);
	virtual void OnServerAbilityRejected(URPGGameplayAbility* Ability);
};
