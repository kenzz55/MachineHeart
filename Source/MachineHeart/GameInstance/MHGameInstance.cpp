// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MHItemDataAsset.h"
#include "MHGameInstance.h"

const UMHItemDataAsset* UMHGameInstance::GetEquippedItem(EEquipmentSlot Slot) const
{
    if (const UMHItemDataAsset* const* Found = SavedEquippedItems.Find(Slot))
    {
        return *Found;
    }

	return nullptr;
}

void UMHGameInstance::ResetPersistData()
{
    SavedCurrentHp = 0.f;
    SavedInventory.Empty();
    SavedEquippedItems.Empty();
    PendingReturnLocation = FVector::ZeroVector;
    ClearedBattleZones.Empty();
    SavedSkillGauge = 0;
}
