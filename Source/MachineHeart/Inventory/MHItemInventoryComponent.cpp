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
        // ���� �ν��Ͻ��� ����� �κ��丮 �ε�
        Items = GI->SavedInventory;
        EquippedItems = GI->SavedEquippedItems;
        OnInventoryUpdated.Broadcast();  // UI �������� Ʈ����
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
    // 1) ���� DataAsset �ν��Ͻ� ã��
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
    // 2) ������ ���� �߰�
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

    // 1) ���� Ÿ�� �̸� Ȯ��
    EEquipmentSlot SlotType = ItemData->EquipSlotType;

    // 2) ���� ���� �̹� ���� ���̸� ����
    if (EquippedItems.Contains(SlotType))
        UnequipSlot(SlotType);

    // 3) Items �迭���� �ش� ������ ã�� ���� ���� & ����
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

    // 4) ���� �ʿ� �������صС� ItemData�� �߰�
    EquippedItems.Add(SlotType, ItemData);

    // 5) ����ȭ �� ��� ������ ȣ��
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
        // ������ �������� �ٽ� �κ��丮�� ���� �� AddItem ��� �ٷ� ����
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

    // 1) ��Ͽ��� �ش� ItemData ã��
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i].ItemData == ItemData)
        {
            // 2) ���� ���� & 0 �̸� ����
            Items[i].Quantity--;
            if (Items[i].Quantity <= 0)
            {
                Items.RemoveAt(i);
            }
            // 3) ����ȭ & UI ���� ���
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

    // AllEnemies�� OptionalTarget ���ʿ�. Self�� ���ο��� User ���.
    const bool bApplied = ItemData->ApplyUse(OwnerChar, OptionalTarget, bIsBattle, GM);
    if (!bApplied) return false;

    // ���� ����
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




