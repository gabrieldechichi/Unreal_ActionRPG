// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGGameplayAbilityFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Targeting/RPGGameplayAbilityTargetTypes.h"

void URPGGameplayAbilityFunctionLibrary::ConfirmAbilityTask(UAbilityTask *AbilityTask, bool bEndTask)
{
	AbilityTask->ExternalConfirm(bEndTask);
}

void URPGGameplayAbilityFunctionLibrary::CancelAbilityTask(UAbilityTask *AbilityTask)
{
	AbilityTask->ExternalCancel();
}

FGameplayAbilityTargetDataHandle URPGGameplayAbilityFunctionLibrary::AbilityTargetDataFromTransforms(const FGameplayAbilityTargetingLocationInfo &SourceLocation, const FGameplayAbilityTargetingLocationInfo &TargetLocation)
{
	FGameplayAbilityTargetData_TransformInfo *NewData = new FGameplayAbilityTargetData_TransformInfo();
	NewData->SourceLocation = SourceLocation;
	NewData->TargetLocation = TargetLocation;

	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData_TransformInfo>(NewData));
	return Handle;
}
