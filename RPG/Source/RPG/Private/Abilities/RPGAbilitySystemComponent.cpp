// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAbilitySystemComponent.h"
#include "Abilities/RPGGameplayAbility.h"
#include "AbilitySystemGlobals.h"


void URPGAbilitySystemComponent::RegisterPredictionEventsForAbilityInstance(URPGGameplayAbility* Instance)
{
	//only on locally controlled clients
	if (GetOwner()->Role != ROLE_AutonomousProxy)
	{
		return;
	}

	FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
	if (PredictionKey.IsValidKey())
	{
		PredictionKey.NewCaughtUpDelegate().BindUObject(this, &URPGAbilitySystemComponent::OnServerAbilityCaughtUp, Instance);
		PredictionKey.NewRejectedDelegate().BindUObject(this, &URPGAbilitySystemComponent::OnServerAbilityRejected, Instance);
	}
}

URPGAbilitySystemComponent* URPGAbilitySystemComponent::GetAbilitySystemComonentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void URPGAbilitySystemComponent::OnServerAbilityCaughtUp(URPGGameplayAbility* Ability)
{
	Ability->OnServerAbilityActivationResponse(true);
}

void URPGAbilitySystemComponent::OnServerAbilityRejected(URPGGameplayAbility* Ability)
{
	Ability->OnServerAbilityActivationResponse(false);
}

void URPGAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<class URPGGameplayAbility*>& ActiveAbilities) const
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		TArray<UGameplayAbility*> AbilityInstance = Spec->GetAbilityInstances();
		for (UGameplayAbility* ActiveAbility : AbilityInstance)
		{
			ActiveAbilities.Add(Cast<URPGGameplayAbility>(ActiveAbility));
		}
	}
}