// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "GameInstance/MHGameInstance.h"
#include "Character/MHCharacterPlayer.h"
#include "CharacterStat/MHCharacterStatComponent.h"


UMHCharacterStatComponent::UMHCharacterStatComponent()
{
	MaxHp = 50.0f;
	CurrentHp = MaxHp;
	TurnSpeed = 7.0f;
	AttackPower = 10.0f;
	Defense = 15.0f;

	IgnoreNextAttackTurns = 0;
	bIgnoreNextAttack = false;
	IgnoreNextAttackChance = 0.f;
}



void UMHCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheBaseStats();

	SetHp(MaxHp);

	ApplyEquipmentStats();

	//ApplyArmorStats();
	
}

float UMHCharacterStatComponent::ApplyDamage(float InDamage)
{

	
	if (bIgnoreNextAttack && IgnoreNextAttackTurns > 0)
	{
		const float Rand = FMath::FRand();  
		if (Rand < IgnoreNextAttackChance)
		{
			UE_LOG(LogTemp, Warning, TEXT("StatComponent: miss! (miss %.2f)"), IgnoreNextAttackChance);
			// 버프 소모 및 즉시 만료
			bIgnoreNextAttack = false;
			IgnoreNextAttackTurns = 0;
			return 0.f;
		}
		// 실패해도 버프 소모
		bIgnoreNextAttack = false;
		IgnoreNextAttackTurns = 0;
	}

	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);

	if (ActualDamage > 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnDamaged] Owner=%s Damage=%.1f"),
			*GetOwner()->GetName(), ActualDamage);
		
		OnDamaged.Broadcast(ActualDamage, GetOwner());
	}

	UE_LOG(LogTemp, Warning, TEXT("StatComponent::ApplyDamage called: PrevHp=%f, Damage=%f, NewHp=%f"),
		PrevHp, ActualDamage, CurrentHp);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UMHCharacterStatComponent::InitCurrentHp(float NewHp)
{
	SetHp(NewHp);
}

void UMHCharacterStatComponent::AddIgnoreNextAttack(float Chance, int32 Turns)
{
	IgnoreNextAttackTurns = Turns;
	bIgnoreNextAttack = (Turns > 0);
	IgnoreNextAttackChance = FMath::Clamp(Chance, 0.f, 1.f);
}

void UMHCharacterStatComponent::OnNewTurn()
{
	if (IgnoreNextAttackTurns > 0)
	{
		--IgnoreNextAttackTurns;
		if (IgnoreNextAttackTurns <= 0)
		{
			bIgnoreNextAttack = false;
			IgnoreNextAttackChance = 0.f;
		}
	}

	if (DefenseDebuffTurns > 0)
	{
		--DefenseDebuffTurns;
		if (DefenseDebuffTurns <= 0)
		{
			
			DefenseDebuffTurns = 0;
			DefenseDebuffTotal = 0.f;   
			ApplyEquipmentStats();
		}
	}

	if (AttackBuffState.RemainingTurns > 0)
	{
		--AttackBuffState.RemainingTurns;
		if (AttackBuffState.RemainingTurns <= 0)
		{
			AttackPower -= AttackBuffState.Value;   // 롤백
			AttackBuffState = FTimedStatModifier{};
		}
	}

	// 턴속도 버프 만료 처리
	if (TurnSpeedBuffState.RemainingTurns > 0)
	{
		--TurnSpeedBuffState.RemainingTurns;
		if (TurnSpeedBuffState.RemainingTurns <= 0)
		{
			TurnSpeed -= TurnSpeedBuffState.Value;  // 롤백
			TurnSpeedBuffState = FTimedStatModifier{};
		}
	}

	
	if (IncomingDamageChanceState.RemainingTurns > 0)
	{
		--IncomingDamageChanceState.RemainingTurns;
		if (IncomingDamageChanceState.RemainingTurns <= 0)
		{
			IncomingDamageChanceState = FTimedStatModifier{}; 
		}
	}
}

//void UMHCharacterStatComponent::ApplyArmorStats()
//{
//	MaxHp = BaseMaxHp;
//	AttackPower = BaseAttackPower;
//	Defense = BaseDefense;
//	TurnSpeed = BaseTurnSpeed;
//
//
//
//	if (UMHGameInstance* GI = Cast<UMHGameInstance>(
//		UGameplayStatics::GetGameInstance(GetWorld())))
//	{
//		if (UMHItemDataAsset** ArmorPtr =
//			GI->SavedEquippedItems.Find(EEquipmentSlot::Armor))
//		{
//			UMHItemDataAsset* Armor = *ArmorPtr;
//			UE_LOG(LogTemp, Warning, TEXT("ApplyArmorStats(): Equipped Armor = %s"),
//				*Armor->GetName());
//	
//			for (const FItemStatModifier& Mod : Armor->StatModifiers)
//			{
//				ApplyModifier(Mod);
//			}
//		}
//	}
//}

void UMHCharacterStatComponent::ApplyEquipmentStats()
{
	MaxHp = BaseMaxHp;
	AttackPower = BaseAttackPower;
	Defense = BaseDefense;
	TurnSpeed = BaseTurnSpeed;

	AActor* Owner = GetOwner();
	AMHCharacterPlayer* Player = Owner ? Cast<AMHCharacterPlayer>(Owner) : nullptr;
	if (!Player)
	{
		
		return;
	}

	
	UMHGameInstance* GI = Cast<UMHGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI) return;


	if (UMHItemDataAsset** ArmorPtr = GI->SavedEquippedItems.Find(EEquipmentSlot::Armor))
	{
		if (UMHItemDataAsset* Armor = *ArmorPtr)
		{
			for (const FItemStatModifier& Mod : Armor->StatModifiers)
			{
				ApplyModifier(Mod);
			}
		}
	}


	auto ApplyOnlyTurnSpeed = [this](UMHItemDataAsset* Item)
		{
			if (!Item) return;
			for (const FItemStatModifier& Mod : Item->StatModifiers)
			{
				if (Mod.StatType == EItemStatType::TurnSpeed)
				{
					TurnSpeed += Mod.DeltaValue;
				}
			}
		};

	if (UMHItemDataAsset** SwordPtr = GI->SavedEquippedItems.Find(EEquipmentSlot::Sword))
		ApplyOnlyTurnSpeed(*SwordPtr);
	if (UMHItemDataAsset** GunPtr = GI->SavedEquippedItems.Find(EEquipmentSlot::Gun))
		ApplyOnlyTurnSpeed(*GunPtr);
	if (UMHItemDataAsset** EnergyPtr = GI->SavedEquippedItems.Find(EEquipmentSlot::EnergyWeapon))
		ApplyOnlyTurnSpeed(*EnergyPtr);

	if (AttackBuffState.RemainingTurns > 0)
		AttackPower += AttackBuffState.Value;      

	if (TurnSpeedBuffState.RemainingTurns > 0)
		TurnSpeed += TurnSpeedBuffState.Value;    

	if (DefenseDebuffTurns > 0 && DefenseDebuffTotal > 0.f)
		Defense -= DefenseDebuffTotal;
}

void UMHCharacterStatComponent::CacheBaseStats()
{
	BaseMaxHp = GetMaxHp();
	BaseAttackPower = GetAttackPower();
	BaseDefense = GetDefense();
	BaseTurnSpeed = GetSpeed();
}

void UMHCharacterStatComponent::AddDefenseDebuff(float Amount, int32 Turns)
{
	if (Turns <= 0 || Amount <= 0.f) return;

	// 즉시 방어력 감소
	Defense -= Amount;
	// 토큰 저장
	DefenseDebuffTotal += Amount;
	DefenseDebuffTurns = Turns;
	ApplyEquipmentStats();
}

void UMHCharacterStatComponent::AddAttackBuff(float Delta, int32 Turns)
{
	if (Turns <= 0 || FMath::IsNearlyZero(Delta)) return;

	
	AttackPower += Delta;
	AttackBuffState.Value += Delta;
	AttackBuffState.RemainingTurns = FMath::Max(AttackBuffState.RemainingTurns, Turns);
}

void UMHCharacterStatComponent::AddTurnSpeedBuff(float Delta, int32 Turns)
{
	if (Turns <= 0 || FMath::IsNearlyZero(Delta)) return;

	TurnSpeed += Delta;
	TurnSpeedBuffState.Value += Delta;
	TurnSpeedBuffState.RemainingTurns = FMath::Max(TurnSpeedBuffState.RemainingTurns, Turns);
}

void UMHCharacterStatComponent::AddIncomingDamageChanceScale(float Scale, int32 Turns)
{
	if (Turns <= 0 || Scale <= 0.f) return;

	if (IncomingDamageChanceState.Value <= 0.f)
	{
		IncomingDamageChanceState.Value = Scale;
	}
	else
	{
		IncomingDamageChanceState.Value *= Scale;
	}
	IncomingDamageChanceState.RemainingTurns = FMath::Max(IncomingDamageChanceState.RemainingTurns, Turns);
}

float UMHCharacterStatComponent::GetIncomingDamageChanceScale() const
{
	return (IncomingDamageChanceState.RemainingTurns > 0 && IncomingDamageChanceState.Value > 0.f)
		? IncomingDamageChanceState.Value
		: 1.0f;
}

void UMHCharacterStatComponent::Heal(float Amount)
{
	if (Amount <= 0.f) return;
	SetHp(CurrentHp + Amount);

}

void UMHCharacterStatComponent::IncrementSkillGauge()
{
	if (UMHGameInstance* GI = Cast<UMHGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->SavedSkillGauge = FMath::Clamp(GI->SavedSkillGauge + 1, 0, 6);

		
		OnSkillGaugeChanged.Broadcast(GI->SavedSkillGauge);
	}
}

void UMHCharacterStatComponent::ChargeSkillGuage()
{
	if (UMHGameInstance* GI = Cast<UMHGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->SavedSkillGauge = 6;

	
		OnSkillGaugeChanged.Broadcast(GI->SavedSkillGauge);
	}
}

void UMHCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	OnHpChanged.Broadcast(CurrentHp);
}

void UMHCharacterStatComponent::ApplyModifier(const FItemStatModifier& Mod)
{
	switch (Mod.StatType)
	{
	case EItemStatType::Health:
		// 최대 HP에 보정치 추가
		MaxHp += Mod.DeltaValue;
		break;

	case EItemStatType::AttackPower:
		AttackPower += Mod.DeltaValue;
		break;

	case EItemStatType::Defense:
		Defense += Mod.DeltaValue;
		break;

	case EItemStatType::TurnSpeed:
		TurnSpeed += Mod.DeltaValue;
		break;

	default:
		
		break;
	}
}




