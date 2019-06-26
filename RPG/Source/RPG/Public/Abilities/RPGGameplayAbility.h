// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/RPGAbilityTypes.h"
#include "RPGGameplayAbility.generated.h"

class UGameplayEffect;

UCLASS()
class RPG_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URPGGameplayAbility() {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
	TMap<FGameplayTag, FRPGGameplayEffectContainer> EffectContainers;

	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag EventTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

protected:
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FRPGGameplayEffectContainerSpec ContainerSpec);

	FRPGGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag EventTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
};
