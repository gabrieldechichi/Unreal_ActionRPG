// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnAttributeChanged, AActor*, SourceActor, float, PreviousValue, float, NewValue, FGameplayEffectContextHandle, Context, const struct FGameplayTagContainer&, EventTags);

UCLASS()
class RPG_API URPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URPGAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetHealth)) 
	int K2_GetHealth() const { return GetHealth(); }
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetMaxHealth))
	int K2_GetMaxHealth() const { return GetMaxHealth(); }

	UPROPERTY(ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Mana)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetMana)) 
	int K2_GetMana() const { return GetMana(); }
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnManaChanged;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxMana)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetMaxMana))
	int K2_GetMaxMana() const { return GetMaxMana(); }

	UPROPERTY(ReplicatedUsing=OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, AttackPower)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetAttackPower))
	int K2_GetAttackPower() const { return GetAttackPower(); }

	UPROPERTY(ReplicatedUsing=OnRep_DefensePower)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, DefensePower)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetDefensePower))
	int K2_GetDefensePower() const { return GetDefensePower(); }

protected:
	void AdjustAttributeForNewMax(FGameplayAttributeData& AffectedAttribute, FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	float GetPastAttributeValueFromModData(const FGameplayEffectModCallbackData& Data) const;

	void RaiseAttributeChangedEvent(const FGameplayEffectModCallbackData& Data, const FGameplayAttribute& AffectedAttribute, FOnAttributeChanged BroadcastEvent) const;

	UFUNCTION()
	void OnRep_Health() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health); }
	UFUNCTION()
	void OnRep_MaxHealth() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth); }

	UFUNCTION()
	void OnRep_Mana() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Mana); }
	UFUNCTION()
	void OnRep_MaxMana() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxMana); }

	UFUNCTION()
	void OnRep_AttackPower() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AttackPower); }

	UFUNCTION()
	void OnRep_DefensePower() { GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, DefensePower); }
};
