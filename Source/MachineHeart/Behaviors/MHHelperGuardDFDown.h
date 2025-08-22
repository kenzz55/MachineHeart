// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Behaviors/EnemyBehaviorBase.h"
#include "MHHelperGuardDFDown.generated.h"

/**
 * 
 */
UCLASS()
class MACHINEHEART_API UMHHelperGuardDFDown : public UEnemyBehaviorBase
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float DefenseDown = 4.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    int32 Turns = 2;

    
    virtual void ExecuteBehavior_Implementation(class AMHCharacterEnemy* Enemy, class AMHBattleGameMode* GM) override;
};
