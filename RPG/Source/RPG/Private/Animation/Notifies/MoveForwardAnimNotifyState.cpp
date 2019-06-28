// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveForwardAnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"

void UMoveForwardAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Speed = MoveAmount / TotalDuration;
	UAnimNotifyState::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UMoveForwardAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (ACharacter * Owner = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		FVector FrameMoveAmount = Owner->GetArrowComponent()->GetForwardVector() * Speed * FrameDeltaTime;
		Owner->AddActorWorldOffset(FrameMoveAmount);
	}
	
	UAnimNotifyState::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}
