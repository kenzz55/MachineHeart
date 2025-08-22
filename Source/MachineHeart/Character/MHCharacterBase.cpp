// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MHCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/MHBattleGameMode.h"
AMHCharacterBase::AMHCharacterBase()
{
	Stat = CreateDefaultSubobject<UMHCharacterStatComponent>(TEXT("Stat"));
}

void AMHCharacterBase::OnTurnStarted_Implementation()
{
}

void AMHCharacterBase::FinishTurn()
{
    if (AMHBattleGameMode* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->EndTurn();
    }
}

float AMHCharacterBase::CalculateDamageAgainst(const AMHCharacterBase* Target) const
{
    return CalculateDamageAgainstWithBonus(Target, 0.f);
}

float AMHCharacterBase::CalculateDamageAgainstWithBonus(const AMHCharacterBase* Target, float BonusAttack) const
{
    if (!Target) return 0.f;

    const float Atk = Stat->GetAttackPower() + BonusAttack;
    const float Def = Target->Stat->GetDefense();

    float Damage = 0.f;
    if (Atk >= Def)
    {
        // 공격력 ±2
        Damage = FMath::RandRange(Atk - 2.f, Atk + 2.f);
    }
    else
    {
        // (공격력*0.8) ±2
        const float HalfAtk = Atk * 0.8f;
        Damage = FMath::RandRange(HalfAtk - 2.f, HalfAtk + 2.f);
    }

    Damage = FMath::RoundToFloat(Damage);

    // 음수가 나올 수 있으니 0 이상으로 클램프
    return FMath::Max(0.f, Damage);
}

void AMHCharacterBase::DealDamageTo(AMHCharacterBase* Target)
{
    if (!Target) return;
    const float Damage = CalculateDamageAgainst(Target);

    if (auto* TS = Target->FindComponentByClass<UMHCharacterStatComponent>())
    {
        TS->ApplyDamage(Damage);
    }
}

void AMHCharacterBase::DealDamageToWithBonus(AMHCharacterBase* Target, float BonusAttack)
{
    if (!Target) return;
    const float Damage = CalculateDamageAgainstWithBonus(Target, BonusAttack);
    if (auto* TS = Target->FindComponentByClass<UMHCharacterStatComponent>())
        TS->ApplyDamage(Damage);

}
