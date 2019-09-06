// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "RPGCharacterBase.generated.h"

class URPGAbilitySystemComponent;
class URPGAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class URPGGameplayAbility;

UENUM(BlueprintType)
enum class EAbilityInputBindings : uint8
{
	UseAbility1 UMETA(DisplayName = "Use Spell 1"),
	UseAbility2 UMETA(DisplayName = "Use Spell 2"),
	UseAbility3 UMETA(DisplayName = "Use Spell 3"),
	UseAbility4 UMETA(DisplayName = "Use Spell 4"),
	WeaponAbility UMETA(DisplayName = "Use Weapon")
};

UCLASS()
class RPG_API ARPGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARPGCharacterBase();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	// Inherited via IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//

	UFUNCTION(BlueprintCallable, Category = Abilities)
	bool ActivateAbility(TSubclassOf<UGameplayAbility> Ability, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable, Category = Abilities)
	void Temp_GiveAbility(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, Category = Abilities)
	bool HasTags(const FGameplayTagContainer AbilityTags) const;

	UFUNCTION(BlueprintCallable, Category = Abilities)
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities) const;

	UFUNCTION()
	void OnHealthChanged(AActor* SourceActor, float PreviousValue, float NewValue, FGameplayEffectContextHandle Context, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCombatDamageReceived(AActor* SourceActor, float Damage, FGameplayEffectContextHandle Context, const struct FGameplayTagContainer& EventTags);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnCombatDamageReceived(AActor* SourceActor, float Damage, FGameplayEffectContextHandle Context, const struct FGameplayTagContainer& EventTags);

protected:
	void AddStartupGameplayAbilities();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY()
	URPGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly)
	URPGAttributeSet* AttributeSet;

	UPROPERTY()
	int32 bAbilitiesInitialized;
};
