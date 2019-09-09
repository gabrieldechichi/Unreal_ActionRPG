// Fill out your copyright notice in the Description page of Project Settings.


#include "NWXGameplayAbilityTargetActor.h"

void ANWXGameplayAbilityTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	ReceiveStartTargeting(Ability);
}

void ANWXGameplayAbilityTargetActor::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (IsConfirmTargetingAllowed())
	{
		FGameplayAbilityTargetDataHandle TargetData;
		ConfirmAndSendTargetData(TargetData);
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
}

bool ANWXGameplayAbilityTargetActor::OnReplicatedTargetDataReceived(FGameplayAbilityTargetDataHandle& Data) const
{
	bool bIsTargetDataValid = false;
	ValidateClientTargetData(Data, bIsTargetDataValid);
	return Super::OnReplicatedTargetDataReceived(Data) && bIsTargetDataValid;
}

void ANWXGameplayAbilityTargetActor::ValidateClientTargetData_Implementation(const FGameplayAbilityTargetDataHandle& Data, bool& bIsTargetDataValid) const
{
	bIsTargetDataValid = true;
}
