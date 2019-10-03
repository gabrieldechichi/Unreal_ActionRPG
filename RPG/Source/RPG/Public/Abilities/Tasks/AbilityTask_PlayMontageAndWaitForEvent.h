// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlayMontageAndWaitForEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/**
 * This task combines PlayMontageAndWait and WaitForEvent into one task, so you can wait for multiple types of activations such as from a melee combo
 * DOES NOT WORK PREDICTIVELY IN MULTIPLAYER (YET)
 */
UCLASS()
class RPG_API UAbilityTask_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitialized);
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	//Begin task events
	UPROPERTY(BlueprintAssignable)
	FPlayerMontageAndWaitForEventDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FPlayerMontageAndWaitForEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FPlayerMontageAndWaitForEventDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FPlayerMontageAndWaitForEventDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FPlayerMontageAndWaitForEventDelegate EventReceived;
	//End task events

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* MontageToPlay,
		FGameplayTagContainer EventTags,
		float Rate = 1.0f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScale = 1.f);

private:
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	bool StopPlayingMontage();
	UAnimInstance* GetAnimInstanceFromActorInfo() const;

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
