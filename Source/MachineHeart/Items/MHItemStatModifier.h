#pragma once

#include "CoreMinimal.h"
#include "MHItemStatModifier.generated.h"

UENUM(BlueprintType)
enum class EItemStatType : uint8
{
    Health     UMETA(DisplayName = "Health"),
    AttackPower     UMETA(DisplayName = "AttackPower"),
    Defense    UMETA(DisplayName = "Defense"),
    TurnSpeed  UMETA(DisplayName = "Turn Speed"),

    
};

USTRUCT(BlueprintType)
struct FItemStatModifier
{
    GENERATED_BODY()

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Stat")
    EItemStatType StatType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Stat")
    float         DeltaValue;

    FItemStatModifier()
        : StatType(EItemStatType::Health)
        , DeltaValue(0.f)
    {
    }
};