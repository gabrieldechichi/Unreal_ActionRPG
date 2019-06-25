// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

bool URPGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (SourceBlockedTags.Num() > 0)
	{
		if (IAbilitySystemInterface* const AbilitySystemInterface = Cast<IAbilitySystemInterface>(ActorInfo->AvatarActor.Get()))
		{
			FGameplayTagContainer OwnerActorTags;
			AbilitySystemInterface->GetAbilitySystemComponent()->GetOwnedGameplayTags(OwnerActorTags);
			SourceTags = &OwnerActorTags;
		}
	}

	return UGameplayAbility::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

TArray<FActiveGameplayEffectHandle> URPGGameplayAbility::ApplyEffectsToSelf()
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	for (TSubclassOf<UGameplayEffect> Effect : Effects)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Effect);
		AllEffects.Add(K2_ApplyGameplayEffectSpecToOwner(SpecHandle));
	}
	return AllEffects;
}
