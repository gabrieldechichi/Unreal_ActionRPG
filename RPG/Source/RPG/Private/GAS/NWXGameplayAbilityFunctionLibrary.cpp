// Fill out your copyright notice in the Description page of Project Settings.


#include "NWXGameplayAbilityFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"

void UNWXGameplayAbilityFunctionLibrary::ConfirmAbilityTask(UAbilityTask* AbilityTask, bool bEndTask)
{
	AbilityTask->ExternalConfirm(bEndTask);
}

void UNWXGameplayAbilityFunctionLibrary::CancelAbilityTask(UAbilityTask* AbilityTask)
{
	AbilityTask->ExternalCancel();
}
