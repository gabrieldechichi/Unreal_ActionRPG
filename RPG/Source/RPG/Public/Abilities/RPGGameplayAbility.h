// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGameplayAbility.generated.h"

class UGameplayEffect;

UCLASS()
class RPG_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URPGGameplayAbility() {};

	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectsToSelf();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
	TArray<TSubclassOf<UGameplayEffect>> Effects;
};
