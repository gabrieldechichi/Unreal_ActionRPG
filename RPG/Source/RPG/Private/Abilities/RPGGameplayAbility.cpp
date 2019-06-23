// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameplayAbility.h"

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
