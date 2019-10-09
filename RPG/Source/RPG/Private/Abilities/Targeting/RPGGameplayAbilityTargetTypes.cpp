// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGGameplayAbilityTargetTypes.h"

bool FGameplayAbilityTargetData_TransformInfo::NetSerialize(FArchive &Ar, UPackageMap *Map, bool &bOutSuccess)
{
	SourceLocation.NetSerialize(Ar, Map, bOutSuccess);
	TargetLocation.NetSerialize(Ar, Map, bOutSuccess);

	bOutSuccess = true;
	return true;
}

bool FGameplayAbilityTargetData_Class::NetSerialize(FArchive & Ar, UPackageMap * Map, bool & bOutSuccess)
{
	Ar << TargetClass;
	bOutSuccess = true;
	return true;
}
