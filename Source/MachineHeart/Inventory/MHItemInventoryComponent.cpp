// Fill out your copyright notice in the Description page of Project Settings.
#include "Inventory/MHItemInventoryComponent.h"
#include "GameInstance/MHGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/MHBattleGameMode.h"     
#include "GameFramework/GameModeBase.h"  
#include "CharacterStat/MHCharacterStatComponent.h"
void UMHItemInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    if (UMHGameInstance* GI = Cast<UMHGameInstance>(
        UGameplayStatics::GetGameInstance(GetWorld())))
    {
        // 게임 인스턴스에 저장된 인벤토리 로드
        Items = GI->SavedInventory;
        EquippedItems = GI->SavedEquippedItems;
        OnInventoryUpdated.Broadcast();  // UI 리프레시 트리거
    }
}

// Sets default values for this component's properties
UMHItemInventoryComponent::UMHItemInventoryComponent()
{

	// ...
}

void UMHItemInventoryComponent::AddItem(UMHItemDataAsset* Item)
{
    if (!Item) return;
    // 1) 같은 DataAsset 인스턴스 찾기
    for (FInventoryItem& Inst : Items)
    {
        if (Inst.ItemData == Item)
        {
            Inst.Quantity++;
            OnInventoryUpdated.Broadcast();
            SyncToGameInstance();
            return;
        }
    }
    // 2) 없으면 새로 추가
    FInventoryItem NewInst;
    NewInst.ItemData = Item;
    NewInst.Quantity = 1;
    NewInst.bEquipped = false;
    Items.Add(NewInst);
    SyncToGameInstance();
    OnInventoryUpdated.Broadcast();
    
}


bool UMHItemInventoryComponent::EquipItemByData(UMHItemDataAsset* ItemData)
{
    if (!ItemData || ItemData->EquipSlotType == EEquipmentSlot::None)
        return false;

    // 1) 슬롯 타입 미리 확보
    EEquipmentSlot SlotType = ItemData->EquipSlotType;

    // 2) 같은 슬롯 이미 장착 중이면 해제
    if (EquippedItems.Contains(SlotType))
        UnequipSlot(SlotType);

    // 3) Items 배열에서 해당 데이터 찾아 수량 차감 & 제거
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i].ItemData == ItemData)
        {
            Items[i].Quantity--;
            if (Items[i].Quantity <= 0)
            {
                Items.RemoveAt(i);
            }
            break;
        }
    }

    // 4) 장착 맵에 “복사해둔” ItemData만 추가
    EquippedItems.Add(SlotType, ItemData);

    // 5) 동기화 → 방송 순서로 호출
    SyncToGameInstance();
    OnInventoryUpdated.Broadcast();

    if (UMHGameInstance* GI = Cast<UMHGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GI->OnEquipmentChanged.Broadcast(SlotType);
    }

    /*if (ItemData->EquipSlotType == EEquipmentSlot::Armor)
    {
        if (AActor* Owner = GetOwner())
        {
            if (UMHCharacterStatComponent* StatComp =
                    Owner->FindComponentByClass<UMHCharacterStatComponent>())
            {
                StatComp->ApplyArmorStats();
            }
        }
    }*/

    if (AActor* Owner = GetOwner())
    {
        if (UMHCharacterStatComponent* StatComp =
            Owner->FindComponentByClass<UMHCharacterStatComponent>())
        {
            StatComp->ApplyEquipmentStats(); 
        }
    }

    return true;
}

bool UMHItemInventoryComponent::UnequipSlot(EEquipmentSlot Slot)
{
    if (UMHItemDataAsset** Found = EquippedItems.Find(Slot))
    {
        // 해제된 아이템은 다시 인벤토리에 복귀 → AddItem 대신 바로 복원
        AddItem(*Found);
        EquippedItems.Remove(Slot);

   

        

        SyncToGameInstance();

        if (UMHGameInstance* GI = Cast<UMHGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
        {
            GI->OnEquipmentChanged.Broadcast(Slot);
        }
        /*if (AActor* Owner = GetOwner())
        {
            if (UMHCharacterStatComponent* StatComp =
                Owner->FindComponentByClass<UMHCharacterStatComponent>())
            {
                StatComp->ApplyArmorStats();
            }
        }*/

        if (AActor* Owner = GetOwner())
        {
            if (UMHCharacterStatComponent* StatComp =
                Owner->FindComponentByClass<UMHCharacterStatComponent>())
            {
                StatComp->ApplyEquipmentStats(); 
            }
        }

        OnInventoryUpdated.Broadcast();
        
        return true;
    }
    return false;
}

bool UMHItemInventoryComponent::RemoveItemByData(UMHItemDataAsset* ItemData)
{
    if (!ItemData) return false;

    // 1) 목록에서 해당 ItemData 찾기
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i].ItemData == ItemData)
        {
            // 2) 수량 차감 & 0 이면 제거
            Items[i].Quantity--;
            if (Items[i].Quantity <= 0)
            {
                Items.RemoveAt(i);
            }
            // 3) 동기화 & UI 갱신 방송
            SyncToGameInstance();
            OnInventoryUpdated.Broadcast();
            return true;
        }
    }

    return false;
}

void UMHItemInventoryComponent::SyncToGameInstance()
{
    if (UMHGameInstance* GI = Cast<UMHGameInstance>(
        UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GI->SavedInventory = Items;
        GI->SavedEquippedItems = EquippedItems;
    }
}

bool UMHItemInventoryComponent::UseItem(UMHItemDataAsset* ItemData, AMHCharacterBase* OptionalTarget)
{
    if (!ItemData || !ItemData->bIsConsumable) return false;

    AMHCharacterBase* OwnerChar = Cast<AMHCharacterBase>(GetOwner());
    if (!OwnerChar) return false;

    AMHBattleGameMode* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this));
    const bool bIsBattle = (GM != nullptr);

    // AllEnemies는 OptionalTarget 불필요. Self는 내부에서 User 사용.
    const bool bApplied = ItemData->ApplyUse(OwnerChar, OptionalTarget, bIsBattle, GM);
    if (!bApplied) return false;

    // 수량 차감
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i].ItemData == ItemData)
        {
            Items[i].Quantity--;
            if (Items[i].Quantity <= 0) Items.RemoveAt(i);
            break;
        }
    }

    SyncToGameInstance();
    OnInventoryUpdated.Broadcast();

   
    return true;
}




