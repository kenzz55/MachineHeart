// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHInventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "MHInventorySlot.h"
#include "GameInstance/MHGameInstance.h"
#include "Inventory/MHItemInventoryComponent.h"
#include "Items/MHItemDataAsset.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "Components/ProgressBar.h"
#include "Character/MHCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h" 
#include "Components/Button.h" 
#include "GameMode/MHBattleGameMode.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
void UMHInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
        {
            InvComp->OnInventoryUpdated.RemoveDynamic(this, &UMHInventoryWidget::RefreshInventory);
            InvComp->OnInventoryUpdated.AddDynamic(this, &UMHInventoryWidget::RefreshInventory);
        }

        if (UMHCharacterStatComponent* StatComp = C->FindComponentByClass<UMHCharacterStatComponent>())
        {
            StatComp->OnHpChanged.RemoveAll(this);
            StatComp->OnSkillGaugeChanged.RemoveAll(this);
            StatComp->OnHpChanged.AddUObject(this, &UMHInventoryWidget::HandleHpChanged);
            StatComp->OnSkillGaugeChanged.AddUObject(this, &UMHInventoryWidget::HandleSkillGaugeChanged);

            if (UMHGameInstance* GI = Cast<UMHGameInstance>(
                UGameplayStatics::GetGameInstance(GetWorld())))
            {
                int32 G = GI->SavedSkillGauge;             // 0~6
                SkillGaugeBar->SetPercent(G / 6.0f);       // 0.0~1.0
            }

            // 1) 텍스트 초기 세팅
            AttackPowerText->SetText(FText::AsNumber(StatComp->GetAttackPower()));
            DefenseText->SetText(FText::AsNumber(StatComp->GetDefense()));
            TurnSpeedText->SetText(FText::AsNumber(StatComp->GetSpeed()));
            HealthText->SetText(FText::AsNumber(StatComp->GetCurrentHp()));

        }
   
    }

    if (UnequipButton)
    {
        UnequipButton->OnClicked.Clear();
        UnequipButton->OnClicked.AddDynamic(this, &UMHInventoryWidget::HandleUnequipButtonClicked);
    }

    if (DiscardButton)
    {
        DiscardButton->OnClicked.Clear();
        DiscardButton->OnClicked.AddDynamic(this, &UMHInventoryWidget::HandleDiscardButtonClicked);
    }

    if (UseButton)
    {
        UseButton->OnClicked.Clear();
        UseButton->OnClicked.AddDynamic(this, &UMHInventoryWidget::HandleUseButtonClicked);
       
    }

	RefreshInventory();
    RefreshStatsDisplay();
}

void UMHInventoryWidget::NativeDestruct()
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
        {
            InvComp->OnInventoryUpdated.RemoveDynamic(this, &UMHInventoryWidget::RefreshInventory);
        }
        if (UMHCharacterStatComponent* StatComp = C->FindComponentByClass<UMHCharacterStatComponent>())
        {
            StatComp->OnHpChanged.RemoveAll(this);
            StatComp->OnSkillGaugeChanged.RemoveAll(this);
        }
    }

    if (UnequipButton) { UnequipButton->OnClicked.RemoveDynamic(this, &UMHInventoryWidget::HandleUnequipButtonClicked); }
    if (DiscardButton) { DiscardButton->OnClicked.RemoveDynamic(this, &UMHInventoryWidget::HandleDiscardButtonClicked); }
    if (UseButton) { UseButton->OnClicked.RemoveDynamic(this, &UMHInventoryWidget::HandleUseButtonClicked); }
    if (EquipButton) { EquipButton->OnClicked.RemoveDynamic(this, &UMHInventoryWidget::HandleEquipButtonClicked); }

    Super::NativeDestruct();
}

void UMHInventoryWidget::HandleSkillGaugeChanged(int32 NewGauge)
{
    SkillGaugeBar->SetPercent(NewGauge / 6.0f);
}

void UMHInventoryWidget::ShowInventoryItemMenu(UMHItemDataAsset* ClickedItem)
{
    if (!ClickedItem) return;

    // 패널 초기화
    ItemPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquipmentPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquippedPanel->SetVisibility(ESlateVisibility::Collapsed);

    LastClickedData = ClickedItem;

    // 공통 정보 세팅
    ItemNameText->SetText(ClickedItem->ItemName);
    StatsText->SetText(ClickedItem->StatDescription);
    EffectText->SetText(ClickedItem->EffectDescription);

    Item_ItemNameText->SetText(ClickedItem->ItemName);
    Item_StatsText->SetText(ClickedItem->StatDescription);
    Item_EffectText->SetText(ClickedItem->EffectDescription);
    const bool bIsBattle = IsBattleContext();

    if (ClickedItem->bIsEquipment)
    {
        EquipmentPanel->SetVisibility(ESlateVisibility::Visible);
        EquipButton->OnClicked.Clear();
        EquipButton->OnClicked.AddDynamic(this, &UMHInventoryWidget::HandleEquipButtonClicked);
    }
    else
    {
        // 소비용 등 일반 아이템
        ItemPanel->SetVisibility(ESlateVisibility::Visible);

        if (UseButton)
        {
            // 컨텍스트 가드
            bool bContextOK = true;
            if (ClickedItem->UseContext == EItemUseContext::BattleOnly && !bIsBattle) bContextOK = false;
            if (ClickedItem->UseContext == EItemUseContext::FieldOnly && bIsBattle) bContextOK = false;

            // 타겟이 AllEnemies면 배틀에서만 사용 가능
            if (ClickedItem->UseTarget == EItemUseTarget::AllEnemies && !bIsBattle)
                bContextOK = false;

            const bool bCanUse = ClickedItem->bIsConsumable && bContextOK;

            UseButton->SetIsEnabled(bCanUse);
            UseButton->SetVisibility(bCanUse ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }

    }
}

void UMHInventoryWidget::HandleUseButtonClicked()
{
    if (!LastClickedData) return;

    AMHCharacterBase* Player = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player) return;

    if (UMHItemInventoryComponent* Inv = Player->FindComponentByClass<UMHItemInventoryComponent>())
    {
        // 적 대상은 전부 AllEnemies로 통일했으니 OptionalTarget = nullptr
        const bool bUsed = Inv->UseItem(LastClickedData, /*OptionalTarget=*/nullptr);
        if (bUsed)
        {
            // 사용 성공 → UI 정리/갱신
            LastClickedData = nullptr;
            if (ItemPanel)     ItemPanel->SetVisibility(ESlateVisibility::Collapsed);
            if (EquipmentPanel)EquipmentPanel->SetVisibility(ESlateVisibility::Collapsed);
            if (EquippedPanel) EquippedPanel->SetVisibility(ESlateVisibility::Collapsed);

            RefreshInventory();
            RefreshStatsDisplay();
        }
    }
}

void UMHInventoryWidget::RefreshInventory()
{
    if (!InventoryGrid || !SlotWidgetClass) return;
    InventoryGrid->ClearChildren();

    // 컴포넌트에서 최신 아이템 수량 가져오기
    AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!C) return;
    auto* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>();
    if (!InvComp) return;

    // 비장착 아이템 카운트
    TMap<UMHItemDataAsset*, int32> CountMap;
    for (const FInventoryItem& Inst : InvComp->Items)
    {
        if (Inst.ItemData && !Inst.bEquipped)
            CountMap.FindOrAdd(Inst.ItemData) += Inst.Quantity;
    }

 
    TArray<UMHItemDataAsset*> UniqueItems;
    UniqueItems.Reserve(CountMap.Num());
    for (auto& Pair : CountMap)
        UniqueItems.Add(Pair.Key);

    const int32 MaxItems = MaxRows * NumColumns;
    const int32 NumItems = UniqueItems.Num();

    // 슬롯 생성
    for (int32 Index = 0; Index < MaxItems; ++Index)
    {
        UMHInventorySlot* ISlot = CreateWidget<UMHInventorySlot>(this, SlotWidgetClass);
        ISlot->OnSlotClicked.AddDynamic(this, &UMHInventoryWidget::ShowInventoryItemMenu);
        int32 Row = Index / NumColumns;
        int32 Col = Index % NumColumns;
        InventoryGrid->AddChildToUniformGrid(ISlot, Row, Col);

        if (Index < NumItems)
        {
            UMHItemDataAsset* Data = UniqueItems[Index];
            int32 Quantity = CountMap[Data];
            ISlot->SetupSlot(Data, Quantity);
        }
        else
        {
            ISlot->SetupSlot(nullptr, 0);
        }
    }

    auto BindEquip = [&](EEquipmentSlot SlotType, UImage* Img, const FName& FunctionName)
        {
            Img->OnMouseButtonDownEvent.Clear();
            UMHItemDataAsset** Found = InvComp->EquippedItems.Find(SlotType);
            if (Found && *Found && (*Found)->ItemIcon)
            {
                Img->SetBrushFromTexture((*Found)->ItemIcon, false);
                Img->SetVisibility(ESlateVisibility::Visible);
                Img->SetIsEnabled(true);
                Img->OnMouseButtonDownEvent.BindUFunction(this, FunctionName);
            }
            else
            {
                Img->SetVisibility(ESlateVisibility::Hidden);
                Img->SetIsEnabled(false);
            }
        };

    BindEquip(EEquipmentSlot::Sword, SwordSlotImage, FName(TEXT("OnSwordSlotMouseDown")));
    BindEquip(EEquipmentSlot::Gun, GunSlotImage, FName(TEXT("OnGunSlotMouseDown")));
    BindEquip(EEquipmentSlot::EnergyWeapon, EnergySlotImage, FName(TEXT("OnEnergySlotMouseDown")));
    BindEquip(EEquipmentSlot::Armor, ArmorSlotImage, FName(TEXT("OnArmorSlotMouseDown")));

}

void UMHInventoryWidget::RefreshStatsDisplay()
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(
        UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UMHCharacterStatComponent* StatComp =
            C->FindComponentByClass<UMHCharacterStatComponent>())
        {
           
            HealthText->SetText(FText::AsNumber(StatComp->GetCurrentHp()));
            AttackPowerText->SetText(FText::AsNumber(StatComp->GetAttackPower()));
            DefenseText->SetText(FText::AsNumber(StatComp->GetDefense()));
            TurnSpeedText->SetText(FText::AsNumber(StatComp->GetSpeed()));
        }
    }
}

void UMHInventoryWidget::ShowItemMenu(UMHItemDataAsset* ClickedItem)
{
    if (!ClickedItem) return;

 
    AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
    UMHItemInventoryComponent* InvComp = C ? C->FindComponentByClass<UMHItemInventoryComponent>() : nullptr;
    bool bIsEquipped = InvComp && InvComp->EquippedItems.Contains(ClickedItem->EquipSlotType);


    ItemPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquipmentPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquippedPanel->SetVisibility(ESlateVisibility::Collapsed);

    LastClickedData = ClickedItem;

   
    ItemNameText->SetText(ClickedItem->ItemName);
    StatsText->SetText(ClickedItem->StatDescription);
    EffectText->SetText(ClickedItem->EffectDescription);

    const bool bIsBattle = IsBattleContext();

   
    if (ClickedItem->bIsEquipment)
    {
        if (bIsEquipped)
        {
          
            EquippedPanel->SetVisibility(ESlateVisibility::Visible);
            Equipped_ItemNameText->SetText(ClickedItem->ItemName);
            Equipped_StatsText->SetText(ClickedItem->StatDescription);
            Equipped_EffectText->SetText(ClickedItem->EffectDescription);
        }
        else
        {
        
            EquipmentPanel->SetVisibility(ESlateVisibility::Visible);
            EquipButton->OnClicked.Clear();
            EquipButton->OnClicked.AddDynamic(this, &UMHInventoryWidget::HandleEquipButtonClicked);
        }
    }
    else
    {
       
        ItemPanel->SetVisibility(ESlateVisibility::Visible);
        Item_ItemNameText->SetText(ClickedItem->ItemName);
        Item_StatsText->SetText(ClickedItem->StatDescription);
        Item_EffectText->SetText(ClickedItem->EffectDescription);

        if (UseButton)
        {
            bool bContextOK = true;
            if (ClickedItem->UseContext == EItemUseContext::BattleOnly && !bIsBattle) bContextOK = false;
            if (ClickedItem->UseContext == EItemUseContext::FieldOnly && bIsBattle) bContextOK = false;
            if (ClickedItem->UseTarget == EItemUseTarget::AllEnemies && !bIsBattle)
                bContextOK = false;

            const bool bCanUse = ClickedItem->bIsConsumable && bContextOK;

            UseButton->SetIsEnabled(bCanUse);
            UseButton->SetVisibility(bCanUse ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }
    }
}

void UMHInventoryWidget::HandleEquipButtonClicked()
{
    if (!LastClickedData) return;
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
        {
          
            InvComp->EquipItemByData(LastClickedData);
            RefreshStatsDisplay();
        }
}

void UMHInventoryWidget::HandleUnequipButtonClicked()
{
    if (!LastClickedData)
        return;

   
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
        {
            InvComp->UnequipSlot(LastClickedData->EquipSlotType);
            RefreshStatsDisplay();
        }
    }

    EquippedPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquipmentPanel->SetVisibility(ESlateVisibility::Collapsed);
    ItemPanel->SetVisibility(ESlateVisibility::Collapsed);

    RefreshInventory();
    
}

void UMHInventoryWidget::HandleDiscardButtonClicked()
{
    if (!LastClickedData) return;

    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(
        UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        if (UMHItemInventoryComponent* InvComp =
            C->FindComponentByClass<UMHItemInventoryComponent>())
        {
            InvComp->RemoveItemByData(LastClickedData);
        }
    }

    ItemPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquipmentPanel->SetVisibility(ESlateVisibility::Collapsed);
    EquippedPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UMHInventoryWidget::HandleHpChanged(float NewHp)
{
    RefreshStatsDisplay();
}

bool UMHInventoryWidget::IsBattleContext() const
{
    return (Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(this)) != nullptr);
}

FEventReply UMHInventoryWidget::OnSwordSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
            if (UMHItemDataAsset** Found = InvComp->EquippedItems.Find(EEquipmentSlot::Sword))
                ShowItemMenu(*Found);
    return UWidgetBlueprintLibrary::Handled();
}

FEventReply UMHInventoryWidget::OnGunSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
            if (UMHItemDataAsset** Found = InvComp->EquippedItems.Find(EEquipmentSlot::Gun))
                ShowItemMenu(*Found);
    return UWidgetBlueprintLibrary::Handled();
}

FEventReply UMHInventoryWidget::OnEnergySlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
            if (UMHItemDataAsset** Found = InvComp->EquippedItems.Find(EEquipmentSlot::EnergyWeapon))
                ShowItemMenu(*Found);
    return UWidgetBlueprintLibrary::Handled();
}

FEventReply UMHInventoryWidget::OnArmorSlotMouseDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
    if (AMHCharacterBase* C = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        if (UMHItemInventoryComponent* InvComp = C->FindComponentByClass<UMHItemInventoryComponent>())
            if (UMHItemDataAsset** Found = InvComp->EquippedItems.Find(EEquipmentSlot::Armor))
                ShowItemMenu(*Found);
    return UWidgetBlueprintLibrary::Handled();
}

