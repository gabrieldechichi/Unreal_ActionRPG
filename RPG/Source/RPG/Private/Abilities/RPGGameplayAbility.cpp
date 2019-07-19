// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGameplayAbility.h"
#include "Core/RPGCharacterBase.h"
#include "Abilities/RPGTargetType.h"
#include "Abilities/RPGAbilitySystemComponent.h"

TArray<FActiveGameplayEffectHandle> URPGGameplayAbility::ApplyEffectContainer(FGameplayTag EventTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	return ApplyEffectContainerSpec(MakeEffectContainerSpec(EventTag, EventData, OverrideGameplayLevel));
}

TArray<FActiveGameplayEffectHandle> URPGGameplayAbility::ApplyEffectContainerSpec(const FRPGGameplayEffectContainerSpec ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> EffectHandles;
	for (const FGameplayEffectSpecHandle& EffectHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		EffectHandles.Append(K2_ApplyGameplayEffectSpecToTarget(EffectHandle, ContainerSpec.TargetData));
	}
	return EffectHandles;
}

void URPGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (ActorInfo->AvatarActor.IsValid() == false)
	{
		return;
	}

	if (URPGAbilitySystemComponent * AbilitySystem = URPGAbilitySystemComponent::GetAbilitySystemComonentFromActor(ActorInfo->AvatarActor.Get(), true))
	{
		AbilitySystem->RegisterPredictionEventsForAbilityInstance(this);
	}
}

FRPGGameplayEffectContainerSpec URPGGameplayAbility::MakeEffectContainerSpec(FGameplayTag EventTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FRPGGameplayEffectContainer* Container = EffectContainers.Find(EventTag);
	if (Container == nullptr)
	{
		return FRPGGameplayEffectContainerSpec();
	}

	ARPGCharacterBase* OwningCharacter = Cast<ARPGCharacterBase>(GetOwningActorFromActorInfo());
	URPGAbilitySystemComponent* AbilitySystem = URPGAbilitySystemComponent::GetAbilitySystemComonentFromActor(OwningCharacter);

	if (OwningCharacter == nullptr || AbilitySystem == nullptr)
	{
		return FRPGGameplayEffectContainerSpec();
	}

	FRPGGameplayEffectContainerSpec ReturnSpec;
	if (Container->TargetType.Get())
	{
		TArray<FHitResult> HitResults;
		TArray<AActor*> TargetActors;
		const URPGTargetType* TargetTypeCDO = Container->TargetType->GetDefaultObject<URPGTargetType>();
		TargetTypeCDO->GetTargets(OwningCharacter, GetAvatarActorFromActorInfo(), EventData, HitResults, TargetActors);

		ReturnSpec.AddTargets(HitResults, TargetActors);
	}

	float EffectLevel = OverrideGameplayLevel == INDEX_NONE ? GetAbilityLevel() : OverrideGameplayLevel;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : Container->TargetGameplayEffects)
	{
		ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, EffectLevel));
	}

	return ReturnSpec;
}

void URPGGameplayAbility::OnServerAbilityActivationResponse(bool bAccepted)
{
	Receive_ServerAbilityActivationResponse(bAccepted);
}
