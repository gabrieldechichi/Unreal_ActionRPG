// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGDamageCalculation.h"
#include "Abilities/RPGAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

struct FRPGDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	FRPGDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, AttackPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, Health, Target, false);
	}

	static const FRPGDamageStatics& DamageStatics()
	{
		static FRPGDamageStatics DamageStatics;
		return DamageStatics;
	}
};

URPGDamageCalculation::URPGDamageCalculation()
{
	RelevantAttributesToCapture.Add(FRPGDamageStatics::DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(FRPGDamageStatics::DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(FRPGDamageStatics::DamageStatics().HealthDef);

	InvalidScopedModifierAttributes.Add(FRPGDamageStatics::DamageStatics().HealthDef);
}

void URPGDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float DefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FRPGDamageStatics::DamageStatics().DefensePowerDef, EvaluationParameters, DefensePower);
	if (DefensePower == 0.0f)
	{
		DefensePower = 1.0f;
	}

	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FRPGDamageStatics::DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);

	float DamageDone = AttackPower / DefensePower;
	if (DamageDone > 0)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(FRPGDamageStatics::DamageStatics().HealthProperty, EGameplayModOp::Additive, -DamageDone));
	}
}

void URPGDamageCalculation::GetSourceAndTarget(const FGameplayEffectCustomExecutionParameters& ExecutionParams, AActor*& OutSource, AActor*& OutTarget)
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	OutSource = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	OutTarget = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
}
