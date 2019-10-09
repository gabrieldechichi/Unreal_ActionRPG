// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityTask_RPGWaitTargetData.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"

UAbilityTask_RPGWaitTargetData::UAbilityTask_RPGWaitTargetData(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilityTask_RPGWaitTargetData *UAbilityTask_RPGWaitTargetData::RPGWaitTargetData(UGameplayAbility *OwningAbility, FName TaskInstanceName, TEnumAsByte<ERPGGameplayTargetConfirmation::Type> ConfirmationType, TSubclassOf<AGameplayAbilityTargetActor> InTargetClass)
{
	UAbilityTask_RPGWaitTargetData *MyObj = NewAbilityTask<UAbilityTask_RPGWaitTargetData>(OwningAbility, TaskInstanceName); //Register for task list here, providing a given FName as a key
	MyObj->TargetClass = InTargetClass;
	MyObj->TargetActor = nullptr;
	MyObj->ConfirmationType = ConfirmationType;
	return MyObj;
}

bool UAbilityTask_RPGWaitTargetData::BeginSpawningActor(UGameplayAbility *OwningAbility, TSubclassOf<AGameplayAbilityTargetActor> InTargetClass, AGameplayAbilityTargetActor *&SpawnedActor)
{
	SpawnedActor = nullptr;

	if (Ability)
	{
		if (ShouldSpawnTargetActor())
		{
			UClass *Class = *InTargetClass;
			if (Class != nullptr)
			{
				if (UWorld *World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull))
				{
					SpawnedActor = World->SpawnActorDeferred<AGameplayAbilityTargetActor>(Class, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				}
			}

			if (SpawnedActor)
			{
				TargetActor = SpawnedActor;
				InitializeTargetActor(SpawnedActor);
			}
		}

		RegisterTargetDataCallbacks();
	}

	return (SpawnedActor != nullptr);
}

void UAbilityTask_RPGWaitTargetData::FinishSpawningActor(UGameplayAbility *OwningAbility, AGameplayAbilityTargetActor *SpawnedActor)
{
	if (SpawnedActor && !SpawnedActor->IsPendingKill())
	{
		check(TargetActor == SpawnedActor);

		const FTransform SpawnTransform = AbilitySystemComponent->GetOwner()->GetTransform();

		SpawnedActor->FinishSpawning(SpawnTransform);

		FinalizeTargetActor(SpawnedActor);
	}

	//K2Node_LatentGameplayTaskCall does not call ReadyForActivation if we spawn an actor, so this needs to be called manually
	ReadyForActivation();
}

bool UAbilityTask_RPGWaitTargetData::ShouldSpawnTargetActor() const
{
	check(TargetClass);
	check(Ability);

	// Spawn the actor if this is a locally controlled ability (always) or if this is a replicating targeting mode.
	// (E.g., server will spawn this target actor to replicate to all non owning clients)

	const AGameplayAbilityTargetActor *CDO = CastChecked<AGameplayAbilityTargetActor>(TargetClass->GetDefaultObject());

	const bool bReplicates = CDO->GetIsReplicated();
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = CDO->ShouldProduceTargetDataOnServer;

	return (bReplicates || bIsLocallyControlled || bShouldProduceTargetDataOnServer);
}

void UAbilityTask_RPGWaitTargetData::InitializeTargetActor(AGameplayAbilityTargetActor *SpawnedActor) const
{
	check(SpawnedActor);
	check(Ability);

	SpawnedActor->MasterPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// If we spawned the target actor, always register the callbacks for when the data is ready.
	SpawnedActor->TargetDataReadyDelegate.AddUObject(this, &UAbilityTask_RPGWaitTargetData::OnTargetDataReadyCallback);
	SpawnedActor->CanceledDelegate.AddUObject(this, &UAbilityTask_RPGWaitTargetData::OnTargetDataCancelledCallback);
}

void UAbilityTask_RPGWaitTargetData::FinalizeTargetActor(AGameplayAbilityTargetActor *SpawnedActor) const
{
	check(SpawnedActor);
	check(Ability);

	// User ability activation is inhibited while this is active
	AbilitySystemComponent->SpawnedTargetActors.Push(SpawnedActor);

	SpawnedActor->StartTargeting(Ability);

	if (SpawnedActor->ShouldProduceTargetData())
	{
		// If instant confirm, then stop targeting immediately.
		// Note this is kind of bad: we should be able to just call a static func on the CDO to do this.
		// But then we wouldn't get to set ExposeOnSpawnParameters.
		if (ConfirmationType == ERPGGameplayTargetConfirmation::Instant)
		{
			SpawnedActor->ConfirmTargeting();
		}
	}
}

void UAbilityTask_RPGWaitTargetData::RegisterTargetDataCallbacks()
{
	if (!ensure(IsPendingKill() == false))
	{
		return;
	}

	check(TargetClass);
	check(Ability);

	const AGameplayAbilityTargetActor *CDO = CastChecked<AGameplayAbilityTargetActor>(TargetClass->GetDefaultObject());

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = CDO->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_RPGWaitTargetData::OnTargetDataReplicatedCallback);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_RPGWaitTargetData::OnTargetDataReplicatedCancelledCallback);

			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}

/** Valid TargetData was replicated to use (we are server, was sent from client) */
void UAbilityTask_RPGWaitTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle &Data, FGameplayTag ActivationTag)
{
	check(AbilitySystemComponent);

	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	/**
	 *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	EndTask();
}

/** Client canceled this Targeting Task (we are the server) */
void UAbilityTask_RPGWaitTargetData::OnTargetDataReplicatedCancelledCallback()
{
	check(AbilitySystemComponent);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

/** The TargetActor we spawned locally has called back with valid target data */
void UAbilityTask_RPGWaitTargetData::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle &Data)
{
	check(AbilitySystemComponent);
	if (!Ability)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, ShouldReplicateDataToServer());

	const FGameplayAbilityActorInfo *Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			FGameplayTag ApplicationTag; // Fixme: where would this be useful?
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		else if (ConfirmationType == ERPGGameplayTargetConfirmation::TargetActorConfirmed)
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	EndTask();
}

/** The TargetActor we spawned locally has called back with a cancel event (they still include the 'last/best' targetdata but the consumer of this may want to discard it) */
void UAbilityTask_RPGWaitTargetData::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle &Data)
{
	check(AbilitySystemComponent);

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
}

void UAbilityTask_RPGWaitTargetData::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		TargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

bool UAbilityTask_RPGWaitTargetData::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server
	const FGameplayAbilityActorInfo *Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}

// --------------------------------------------------------------------------------------
