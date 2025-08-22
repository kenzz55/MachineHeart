// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/MHItemStatModifier.h"
#include "MHCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillGaugeChangedDelegate, int32 /*NewGauge*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDamaged, float /*Damage*/, AActor* /*DamagedActor*/);

USTRUCT()
struct FTimedStatModifier
{
	GENERATED_BODY()

	float Value = 0.f;
	int32 RemainingTurns = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MACHINEHEART_API UMHCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMHCharacterStatComponent();

protected:

	virtual void BeginPlay() override;

public:	
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnSkillGaugeChangedDelegate OnSkillGaugeChanged;
	FOnDamaged OnDamaged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }

	FORCEINLINE float GetCurrentHp() { return CurrentHp; }

	FORCEINLINE float GetSpeed() const { return TurnSpeed; }

	FORCEINLINE float GetAttackPower() const { return AttackPower; }
	FORCEINLINE float GetDefense()     const { return Defense; }

	float ApplyDamage(float InDamage);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void InitCurrentHp(float NewHp);

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentHp() const { return CurrentHp; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHp() const { return MaxHp; }


	void AddIgnoreNextAttack(float Chance, int32 Turns);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void OnNewTurn();

	//void ApplyArmorStats();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ApplyEquipmentStats();

	void CacheBaseStats();

	void AddDefenseDebuff(float Amount, int32 Turns);

	void AddAttackBuff(float Delta, int32 Turns);
	void AddTurnSpeedBuff(float Delta, int32 Turns);

	void AddIncomingDamageChanceScale(float Scale, int32 Turns);
	float GetIncomingDamageChanceScale() const;


	UFUNCTION(BlueprintCallable, Category = "Stat")
	void Heal(float Amount);


	UFUNCTION(BlueprintCallable, Category = "Stat")
	void IncrementSkillGauge();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ChargeSkillGuage();

protected:
	void SetHp(float NewHp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float TurnSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackPower = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Defense = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;


	int32 IgnoreNextAttackTurns = 0;

	bool bIgnoreNextAttack = false;

	float IgnoreNextAttackChance = 0.f;

private:

	float BaseMaxHp;
	float BaseAttackPower;
	float BaseDefense;
	float BaseTurnSpeed;

	int32 DefenseDebuffTurns = 0;
	//float DefenseDebuffValue = 0.f;
	float DefenseDebuffTotal = 0.f;

	void ApplyModifier(const FItemStatModifier& Mod);

	FTimedStatModifier AttackBuffState;
	FTimedStatModifier TurnSpeedBuffState;

	
	FTimedStatModifier IncomingDamageChanceState;
		
};
