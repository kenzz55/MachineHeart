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

        // ���� ����
        R.bHit = (FMath::FRand() <= HitChance);
        if (!R.bHit)
        {
            R.Damage = 0.f;
            return R;
        }

        // ���ʽ� ���ݷ� �ջ�: ���� ���� + ������ ���ʽ�(������ ����)
        float BonusAttack = AddAttack;
        if (Ctx.bGaugeSuccess)
        {
            BonusAttack += GaugeBonusDamage;
        }

        // ĳ���� ���̽��� ������ vs ������ ���� ��� (��/��, ����, �ݿø� ����)
        if (Ctx.Attacker && Ctx.Target)
        {
            // ������ ������ ���ο��� Stat->GetAttackPower()�� ��� ������ ���
            R.Damage = Ctx.Attacker->CalculateDamageAgainstWithBonus(Ctx.Target, BonusAttack);
        }
        else
        {
            R.Damage = 0.f; // ����
        }

        return R;
    }
};