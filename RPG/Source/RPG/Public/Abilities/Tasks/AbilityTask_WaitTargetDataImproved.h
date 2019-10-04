// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitTargetDataImproved.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataDelegate, const FGameplayAbilityTargetDataHandle &, Data);

UENUM(BlueprintType)
namespace EGameplayTargetConfirmation
{
	/** Describes how the targeting information is confirmed */
	enum Type
	{
		/** The targeting happens instantly without special logic or user input deciding when to 'fire' */
		Instant,

		/** Targeting actor is responsible for confirm/cancel targeting*/
		TargetActorConfirmed,
	};
}

/**
 * This offers the some function was Epic's WaitTargetData, but it doesn't memory leak and will support actor caching in the future
 */
UCLASS()
class RPG_API UAbilityTask_WaitTargetDataImproved : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate Cancelled;

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle &Data, FGameplayTag ActivationTag);

	UFUNCTION()
	void OnTargetDataReplicatedCancelledCallback();

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle &Data);

	UFUNCTION()
	void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle &Data);

	/** Spawns target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UAbilityTask_WaitTargetDataImproved *WaitTargetDataImproved(UGameplayAbility *OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, TSubclassOf<AGameplayAbilityTargetActor> Class);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	bool BeginSpawningActor(UGameplayAbility *OwningAbility, TSubclassOf<AGameplayAbilityTargetActor> Class, AGameplayAbilityTargetActor *&SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	void FinishSpawningActor(UGameplayAbility *OwningAbility, AGameplayAbilityTargetActor *SpawnedActor);

protected:
	bool ShouldSpawnTargetActor() const;
	void InitializeTargetActor(AGameplayAbilityTargetActor *SpawnedActor) const;
	void FinalizeTargetActor(AGameplayAbilityTargetActor *SpawnedActor) const;

	void RegisterTargetDataCallbacks();

	virtual void OnDestroy(bool AbilityEnded) override;

	bool ShouldReplicateDataToServer() const;

protected:
	UPROPERTY()
	TSubclassOf<AGameplayAbilityTargetActor> TargetClass;

	UPROPERTY()
	AGameplayAbilityTargetActor *TargetActor;

	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

	FDelegateHandle OnTargetDataReplicatedCallbackDelegateHandle;
};
