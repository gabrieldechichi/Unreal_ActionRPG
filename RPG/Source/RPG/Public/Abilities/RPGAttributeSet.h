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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, float, PreviousValue, float, NewValue, const struct FGameplayTagContainer&, EventTags);

UCLASS()
class RPG_API URPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URPGAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetHealthValue)) 
	int K2_GetHealth() const { return GetHealth(); }
	UPROPERTY(BlueprintAssignable) 
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth)
	UFUNCTION(BlueprintCallable, meta = (DisplayName = GetMaxHealthValue))
	int K2_GetMaxHealth() const { return GetMaxHealth(); }

protected:
	void AdjustAttributeForNewMax(FGameplayAttributeData& AffectedAttribute, FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
	float GetPastAttributeValueFromModData(const FGameplayEffectModCallbackData& Data) const;
	void RaiseAttributeChangedEvent(const FGameplayEffectModCallbackData& Data, const FGameplayAttribute& AffectedAttribute, FOnAttributeChanged BroadcastEvent) const;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_MaxHealth();
};
