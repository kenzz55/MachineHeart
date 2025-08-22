#pragma once

#include "CoreMinimal.h"
#include "MHCombatTypes.generated.h"

class AMHCharacterBase;

USTRUCT(BlueprintType)
struct FMHAttackContext
{
    GENERATED_BODY()

    UPROPERTY() AMHCharacterBase* Attacker = nullptr;
    UPROPERTY() AMHCharacterBase* Target = nullptr;

    // 입력 파라미터
    UPROPERTY() float AttackerAttackPower = 0.f;   
    UPROPERTY() bool  bGaugeSuccess = false;       
};

USTRUCT(BlueprintType)
struct FMHAttackResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) bool  bHit = true;
    UPROPERTY(BlueprintReadOnly) float Damage = 0.f;
};