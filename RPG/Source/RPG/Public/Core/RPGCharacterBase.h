// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RPGCharacterBase.generated.h"

class URPGAbilitySystemComponent;
class URPGAttributeSet;
class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class RPG_API ARPGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARPGCharacterBase();

	virtual void PossessedBy(AController* NewController) override;

	// Inherited via IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//

	UFUNCTION(BlueprintCallable, Category = Abilities)
	bool ActivateAbility(TSubclassOf<UGameplayAbility> Ability, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable, Category = Abilities)
	void Temp_GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable)
	int GetHealth() const;

	UFUNCTION(BlueprintCallable)
	int GetMaxHealth() const;

protected:
	void AddStartupGameplayAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY()
	URPGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	URPGAttributeSet* AttributeSet;

	UPROPERTY()
	int32 bAbilitiesInitialized;
};
