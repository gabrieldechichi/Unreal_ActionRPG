// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAbilitySystemComponent.h"
#include "Abilities/RPGGameplayAbility.h"
#include "AbilitySystemGlobals.h"


URPGAbilitySystemComponent* URPGAbilitySystemComponent::GetAbilitySystemComonentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
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