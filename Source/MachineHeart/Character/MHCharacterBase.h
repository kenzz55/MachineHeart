// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "MHCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API AMHCharacterBase : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	AMHCharacterBase();


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Turn")
	void OnTurnStarted();
	virtual void OnTurnStarted_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void FinishTurn();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float CalculateDamageAgainst(const AMHCharacterBase* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float CalculateDamageAgainstWithBonus(const AMHCharacterBase* Target, float BonusAttack) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamageTo(AMHCharacterBase* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamageToWithBonus(AMHCharacterBase* Target, float BonusAttack);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMHCharacterStatComponent> Stat;

};

