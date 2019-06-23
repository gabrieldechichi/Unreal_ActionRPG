// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacterBase.h"
#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGAttributeSet.h"

// Sets default values
ARPGCharacterBase::ARPGCharacterBase()
{
	AttributeSet = CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));
}

int ARPGCharacterBase::GetHealth() const
{
	return AttributeSet->GetHealth();
}

int ARPGCharacterBase::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}
