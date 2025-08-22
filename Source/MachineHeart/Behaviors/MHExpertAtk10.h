// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHExpertAtk10.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHExpertAtk10 : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteBehavior_Implementation(AMHCharacterEnemy* Enemy, AMHBattleGameMode* GM) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float DefenseDown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 Turns = 2;

};
