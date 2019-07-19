// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "RPG.h"

#define HANDLE_ATTRIBUTE_CHANGE(AttributeName) \
{ \
	RaiseAttributeChangedEvent(Data, Get ##AttributeName## Attribute(), On ##AttributeName## Changed); \
}

#define HANDLE_ATTRIBUTE_CHANGE_WITH_CLAMP(AttributeName) \
{ \
	if (Data.EvaluatedData.Attribute == Get ##AttributeName## Attribute()) \
	{ \
		Set ##AttributeName## (FMath::Clamp(Get##AttributeName##(), 0.0f, GetMax##AttributeName## ())); \
		HANDLE_ATTRIBUTE_CHANGE(AttributeName); \
	} \
}

#define HANDLE_CHANGE_OF_MAX_ATTRIBUTE(AttributeName) \
{ \
	if (Attribute == GetMax ##AttributeName## Attribute()) \
	{ \
		AdjustAttributeForNewMax(##AttributeName##, Max##AttributeName##, NewValue, Get ##AttributeName## Attribute()); \
	} \
}

URPGAttributeSet::URPGAttributeSet()
	: Health(1.f)
	, MaxHealth(1.f)
	, Mana(1.f)
	, MaxMana(1.f)
{
}

void URPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	HANDLE_CHANGE_OF_MAX_ATTRIBUTE(Health);
	HANDLE_CHANGE_OF_MAX_ATTRIBUTE(Mana);
}

void URPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	HANDLE_ATTRIBUTE_CHANGE_WITH_CLAMP(Health);
	HANDLE_ATTRIBUTE_CHANGE_WITH_CLAMP(Mana);
}

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void URPGAttributeSet::AdjustAttributeForNewMax(FGameplayAttributeData& AffectedAttribute, FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentRatio = CurrentMaxValue > 0.0f
			? AffectedAttribute.GetCurrentValue() / CurrentMaxValue
			: 1.0f;
		const float NewValue = NewMaxValue * CurrentRatio;
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Override, NewValue);
	}
}

float URPGAttributeSet::GetPastAttributeValueFromModData(const FGameplayEffectModCallbackData& Data) const
{
	float CurrentValue = Data.EvaluatedData.Attribute.GetNumericValueChecked(this);
	switch (Data.EvaluatedData.ModifierOp)
	{
	case EGameplayModOp::Additive:
		return CurrentValue - Data.EvaluatedData.Magnitude;
	case EGameplayModOp::Division:
		return CurrentValue * Data.EvaluatedData.Magnitude;
	case EGameplayModOp::Multiplicitive:
		return CurrentValue / Data.EvaluatedData.Magnitude;
	}
	return CurrentValue;
}

void URPGAttributeSet::RaiseAttributeChangedEvent(const FGameplayEffectModCallbackData& Data, const FGameplayAttribute& AffectedAttribute, FOnAttributeChanged BroadcastEvent) const
{
	if (Data.EvaluatedData.Attribute == AffectedAttribute)
	{
		const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
		
		float CurrentValue = Data.EvaluatedData.Attribute.GetNumericValueChecked(this);
		float PreviousValue = GetPastAttributeValueFromModData(Data);
		
		FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
		UAbilitySystemComponent* SourceAbilitySystemComponent = Context.GetOriginalInstigatorAbilitySystemComponent();
		AActor* Source = SourceAbilitySystemComponent != nullptr
			? SourceAbilitySystemComponent->AbilityActorInfo->AvatarActor.Get() :
			nullptr;
		
		BroadcastEvent.Broadcast(Source, PreviousValue, CurrentValue, Context, SourceTags);
	}
}

void URPGAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health);
}

void URPGAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth);
}

void URPGAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Mana);
}

void URPGAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxMana);
}

void URPGAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AttackPower);
}

void URPGAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, DefensePower);
}
