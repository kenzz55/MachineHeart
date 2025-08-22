// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHHelperGuardHeal.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHHelperGuardHeal : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float HealAmount = 20.f;


    virtual void ExecuteBehavior_Implementation(class AMHCharacterEnemy* Enemy, class AMHBattleGameMode* GM) override;

};
