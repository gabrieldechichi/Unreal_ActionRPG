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
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<class URPGGameplayAbility*>& ActiveAbilities) const;

	static URPGAbilitySystemComponent* GetAbilitySystemComonentFromActor(const AActor* Actor, bool LookForComponent = false);

};
