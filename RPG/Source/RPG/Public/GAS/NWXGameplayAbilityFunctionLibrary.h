// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NWXGameplayAbilityFunctionLibrary.generated.h"

class UAbilityTask;

UCLASS()
class RPG_API UNWXGameplayAbilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static void ConfirmAbilityTask(UAbilityTask* AbilityTask, bool bEndTask);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static void CancelAbilityTask(UAbilityTask* AbilityTask);

};
