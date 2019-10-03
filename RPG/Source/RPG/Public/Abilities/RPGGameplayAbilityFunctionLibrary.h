// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "RPGGameplayAbilityFunctionLibrary.generated.h"

class UAbilityTask;

UCLASS()
class RPG_API URPGGameplayAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static void ConfirmAbilityTask(UAbilityTask* AbilityTask, bool bEndTask);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static void CancelAbilityTask(UAbilityTask* AbilityTask);

	/** Creates a target data with a source and destination location */
	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static FGameplayAbilityTargetDataHandle	AbilityTargetDataFromTransforms(const FGameplayAbilityTargetingLocationInfo& SourceLocation, const FGameplayAbilityTargetingLocationInfo& TargetLocation);
};
