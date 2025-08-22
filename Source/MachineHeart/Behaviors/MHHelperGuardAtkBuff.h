// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHHelperGuardAtkBuff.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHHelperGuardAtkBuff : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float DamageToOthers = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float AttackBuff = 7.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    int32 BuffTurns = 2;



    virtual void ExecuteBehavior_Implementation(class AMHCharacterEnemy* Enemy, class AMHBattleGameMode* GM) override;
};
