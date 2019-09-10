// Fill out your copyright notice in the Description page of Project Settings.

#include "NWXGameplayAbilityTargetActor.h"
#include "Abilities/GameplayAbility.h"

DEFINE_LOG_CATEGORY(LogTargetActor);

ANWXGameplayAbilityTargetActor::ANWXGameplayAbilityTargetActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetReplicates(true);
}

void ANWXGameplayAbilityTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
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
	else
	{
		UE_LOG(LogTargetActor, Warning, TEXT("ConfirmAndSendTarget not called for %s. ConfirmTargetingAllowed() = false. Ability: %s, SourceActor: %s"), 
			*GetName(),
			OwningAbility ? *OwningAbility->GetName() : TEXT("NULL"),
			SourceActor ? *SourceActor->GetName() : TEXT("NULL"));
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
