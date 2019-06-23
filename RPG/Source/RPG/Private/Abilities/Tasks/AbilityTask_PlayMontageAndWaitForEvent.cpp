// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_PlayMontageAndWaitForEvent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"


UAbilityTask_PlayMontageAndWaitForEvent::UAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitialized)
{
	Rate = 1.0f;
	bStopWhenAbilityEnds = true;
}

void UAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr || AbilitySystemComponent == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	UAnimInstance* AnimInstance = GetAnimInstanceFromActorInfo();

	//Play anim and bind events
	if (AnimInstance)
	{
		EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags,
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));

		if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.0f)
		{
			if (ShouldBroadcastAbilityTaskDelegates() == false)
			{
				return;
			}

			CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

			BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
			AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate);

			MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate);

			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->Role == ROLE_Authority ||
				(Character->Role == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
			}

			bPlayedMontage = true;
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	SetWaitingOnAvatar();
}

void UAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString UAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;

	if (Ability)
	{
		if (UAnimInstance* AnimInstance = GetAnimInstanceFromActorInfo())
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay)
				? MontageToPlay
				: AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);

	Super::OnDestroy(AbilityEnded);
}

UAbilityTask_PlayMontageAndWaitForEvent* UAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	//UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);
	UAbilityTask_PlayMontageAndWaitForEvent* NewTask = NewAbilityTask<UAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	NewTask->MontageToPlay = MontageToPlay;
	NewTask->EventTags = EventTags;
	NewTask->Rate = Rate;
	NewTask->StartSection = StartSection;
	NewTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	NewTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return NewTask;
}

bool UAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	UAnimInstance* AnimInstance = GetAnimInstanceFromActorInfo();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	if (AbilitySystemComponent && Ability
		&& AbilitySystemComponent->GetAnimatingAbility() == Ability
		&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
	{
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
		if (MontageInstance)
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
			MontageInstance->OnMontageEnded.Unbind();
		}

		AbilitySystemComponent->CurrentMontageStop();
		return true;

	}
	return false;
}

UAnimInstance* UAbilityTask_PlayMontageAndWaitForEvent::GetAnimInstanceFromActorInfo() const
{
	if (Ability == nullptr)
	{
		return nullptr;
	}
	if (const FGameplayAbilityActorInfo * ActorInfo = Ability->GetCurrentActorInfo())
	{
		return ActorInfo->GetAnimInstance();
	}
	return nullptr;
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay && Montage == MontageToPlay)
	{
		AbilitySystemComponent->ClearAnimatingAbility(Ability);

		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->Role == ROLE_Authority ||
			(Character->Role == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates() == false)
	{
		return;
	}

	if (bInterrupted)
	{
		OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	else
	{
		OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage() && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted && ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	EndTask();
}

void UAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;
		EventReceived.Broadcast(EventTag, TempData);
	}
}
