// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RPGGameplayAbilityTargetActor.generated.h"

RPG_API DECLARE_LOG_CATEGORY_EXTERN(LogTargetActor, Display, All);

/**
 * Base class that allows TargetActors to be implemented in blueprint
 */
UCLASS(Blueprintable, abstract, notplaceable)
class RPG_API ARPGGameplayAbilityTargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()
	
public:
	//AGameplayAbilityTargetActor interface
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual bool OnReplicatedTargetDataReceived(FGameplayAbilityTargetDataHandle& Data) const override;
	//

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StartTargeting"))
	void ReceiveStartTargeting(UGameplayAbility* Ability);

	UFUNCTION(BlueprintImplementableEvent)
	void ConfirmAndSendTargetData(FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintNativeEvent)
	void ValidateClientTargetData(const FGameplayAbilityTargetDataHandle& Data, bool& bIsTargetDataValid) const;
};
