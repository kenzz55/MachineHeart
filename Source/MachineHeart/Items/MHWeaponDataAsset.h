#pragma once

#include "CoreMinimal.h"
#include "Items/MHItemDataAsset.h"
#include "Character/MHCharacterBase.h"
#include "MHWeaponDataAsset.generated.h"

UCLASS(BlueprintType)
class MACHINEHEART_API UMHWeaponDataAsset : public UMHItemDataAsset
{
    GENERATED_BODY()
public:
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float AddAttack = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float HitChance = 1.0f;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float GaugeBonusDamage = 2.0f;

    virtual FMHAttackResult ComputeAttack(const FMHAttackContext& Ctx) const override
    {
        FMHAttackResult R;

        // 명중 판정
        R.bHit = (FMath::FRand() <= HitChance);
        if (!R.bHit)
        {
            R.Damage = 0.f;
            return R;
        }

        // 보너스 공격력 합산: 무기 고정 + 게이지 보너스(성공일 때만)
        float BonusAttack = AddAttack;
        if (Ctx.bGaugeSuccess)
        {
            BonusAttack += GaugeBonusDamage;
        }

        // 캐릭터 베이스의 공격자 vs 피해자 공식 사용 (공/방, 난수, 반올림 포함)
        if (Ctx.Attacker && Ctx.Target)
        {
            // 공격자 공식은 내부에서 Stat->GetAttackPower()와 대상 방어력을 고려
            R.Damage = Ctx.Attacker->CalculateDamageAgainstWithBonus(Ctx.Target, BonusAttack);
        }
        else
        {
            R.Damage = 0.f; // 안전
        }

        return R;
    }
};