// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MHItemTypes.h"
#include "Items/MHCombatTypes.h"
#include "MHItemStatModifier.h" 
#include "MHItemDataAsset.generated.h"

/**
 * 
 */

class AMHCharacterBase;
class AMHBattleGameMode;
class UPaperSprite;

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    None          UMETA(DisplayName = "None"),
    Sword         UMETA(DisplayName = "Sword"),
    Gun           UMETA(DisplayName = "Gun"),
    EnergyWeapon  UMETA(DisplayName = "EnergyWeapon"),
    Armor         UMETA(DisplayName = "Armor"),
};

UCLASS(BlueprintType)
class MACHINEHEART_API UMHItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

    virtual FMHAttackResult ComputeAttack(const FMHAttackContext& Ctx) const
    {
        FMHAttackResult R;
        R.bHit = true;
        R.Damage = Ctx.AttackerAttackPower; // 디폴트: 공격력 그대로
        return R;
    }
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemName;

   
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UPaperSprite* ItemSprite;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsEquipment;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Item|Equip")
    bool bIsEquipped = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TArray<FItemStatModifier> StatModifiers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* ItemIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Description", meta = (MultiLine = true))
    FText StatDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Description", meta = (MultiLine = true))
    FText EffectDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Equip")
    EEquipmentSlot EquipSlotType = EEquipmentSlot::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Use")
    bool bIsConsumable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Use")
    EItemUseContext UseContext = EItemUseContext::Anywhere;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Use")
    EItemUseTarget UseTarget = EItemUseTarget::Self;

    // 효과 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    float HealAmount = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    float AttackBuff = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    int32 AttackBuffTurns = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    float TurnSpeedBuff = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    int32 TurnSpeedBuffTurns = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    float EnemyDefenseDebuff = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    int32 EnemyDefenseDebuffTurns = 0;

    // 플레이어 피격 확률 감소(자기 자신용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect", meta = (ClampMin = "0.0"))
    float IncomingHitChanceScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effect")
    int32 IncomingHitChanceTurns = 0;

    // 사용 실행
    virtual bool ApplyUse(AMHCharacterBase* User, AMHCharacterBase* Target, bool bIsBattle, AMHBattleGameMode* GM) const;
  
};
