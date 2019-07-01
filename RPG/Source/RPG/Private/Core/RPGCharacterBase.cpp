// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacterBase.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/RPGGameplayAbility.h"
#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGAttributeSet.h"

// Sets default values
ARPGCharacterBase::ARPGCharacterBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));

	bAbilitiesInitialized = false;
}

void ARPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	AttributeSet->OnHealthChanged.AddDynamic(this, &ARPGCharacterBase::OnHealthChanged);
}

void ARPGCharacterBase::AddStartupGameplayAbilities()
{
	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect->GetDefaultObject<UGameplayEffect>(), 1.0f, EffectContext);
		}

		bAbilitiesInitialized = true;
	}
}

void ARPGCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

UAbilitySystemComponent* ARPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ARPGCharacterBase::ActivateAbility(TSubclassOf<UGameplayAbility> Ability, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilityByClass(Ability, bAllowRemoteActivation);
	}
	return false;
}

void ARPGCharacterBase::Temp_GiveAbility(TSubclassOf<UGameplayAbility> Ability)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1.0f, INDEX_NONE, this));
	}
}

bool ARPGCharacterBase::HasTags(const FGameplayTagContainer AbilityTags) const
{
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	return OwnedTags.HasAll(AbilityTags);
}

void ARPGCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

void ARPGCharacterBase::OnHealthChanged(AActor* SourceActor, float PreviousValue, float NewValue, FGameplayEffectContextHandle Context, const FGameplayTagContainer& EventTags)
{
	if (NewValue < PreviousValue)
	{
		OnCombatDamageReceived(SourceActor, PreviousValue - NewValue, Context, EventTags);
	}
}
