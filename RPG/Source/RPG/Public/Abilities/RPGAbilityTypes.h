// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "RPGAbilityTypes.generated.h"

class URPGAbilitySystemComponent;
class UGameplayEffect;
class URPGTargetType;

USTRUCT(BlueprintType)
struct FRPGGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FRPGGameplayEffectContainer() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<URPGTargetType> TargetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffects;
};

USTRUCT(BlueprintType)
struct FRPGGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FRPGGameplayEffectContainerSpec() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	bool HasValidEffects() const;

	bool HasValidTargets() const;

	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};
