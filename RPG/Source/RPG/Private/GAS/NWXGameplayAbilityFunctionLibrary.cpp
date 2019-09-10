// Fill out your copyright notice in the Description page of Project Settings.


#include "NWXGameplayAbilityFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GAS/NWXGameplayAbilityTargetTypes.h"

void UNWXGameplayAbilityFunctionLibrary::ConfirmAbilityTask(UAbilityTask* AbilityTask, bool bEndTask)
{
	AbilityTask->ExternalConfirm(bEndTask);
}

void UNWXGameplayAbilityFunctionLibrary::CancelAbilityTask(UAbilityTask* AbilityTask)
{
	AbilityTask->ExternalCancel();
}

FGameplayAbilityTargetDataHandle UNWXGameplayAbilityFunctionLibrary::AbilityTargetDataFromTransforms(const FGameplayAbilityTargetingLocationInfo& SourceLocation, const FGameplayAbilityTargetingLocationInfo& TargetLocation)
{
	FGameplayAbilityTargetData_TransformInfo* NewData = new FGameplayAbilityTargetData_TransformInfo();
	NewData->SourceLocation = SourceLocation;
	NewData->TargetLocation = TargetLocation;

	FGameplayAbilityTargetDataHandle	Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData_TransformInfo>(NewData));
	return Handle;
}
