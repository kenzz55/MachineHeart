// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHBattleUIWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Character/MHCharacterEnemy.h"
#include "CharacterStat/MHCharacterStatComponent.h"
#include "Character/MHCharacterPlayer.h"
#include "MHSkillSlotWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/EngineTypes.h"
#include "MHGaugeWidget.h"
#include "GameMode/MHBattleGameMode.h"
#include "Blueprint/DragDropOperation.h"  
#include "Components/CanvasPanel.h"

bool UMHBattleUIWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
{
    if (Cast<UMHSkillSlotWidget>(InOperation->Payload))
    {
        
        return true;
    }
    return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UMHBattleUIWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InEvent, UDragDropOperation* InOp)
{
    if (UMHSkillSlotWidget* SkillSlotWidget = Cast<UMHSkillSlotWidget>(InOp->Payload))
    {
      
        FHitResult Hit;

        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {

            ECollisionChannel ECC_Enemy = ECollisionChannel::ECC_GameTraceChannel1;

            PC->bShowMouseCursor = true;
            PC->bEnableClickEvents = true;
            PC->bEnableMouseOverEvents = true;

            FInputModeGameAndUI Mode;
            Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            Mode.SetWidgetToFocus(TakeWidget());
            PC->SetInputMode(Mode);

            FVector2D ViewportPos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
          

            if (PC->GetHitResultAtScreenPosition(ViewportPos, ECollisionChannel::ECC_Pawn, false, Hit))
            {
                FVector HitLoc = Hit.ImpactPoint;

                if (AMHCharacterEnemy* Enemy = Cast<AMHCharacterEnemy>(Hit.GetActor()))
                {
                  
                    
                    PendingTarget = Enemy;
                    PendingItem = SkillSlotWidget->CurrentItemData;
                    PendingSlot = SkillSlotWidget->DesiredEquipSlot;
                    
                    
                    if (GaugeWidget && CombatPanel )
                    {
                        CombatPanel->SetVisibility(ESlateVisibility::Hidden);
                        GaugeWidget->SetVisibility(ESlateVisibility::Visible);
                        GaugeWidget->StartGauge();
                        GaugeWidget->OnGaugeResult.AddDynamic(this, &UMHBattleUIWidget::HandleGaugeResult);
                    }
                    return true;
                }
            }
        }
        return true;
    }
    return Super::NativeOnDrop(InGeometry, InEvent, InOp);
}

void UMHBattleUIWidget::HandleGaugeResult(bool bSuccess)
{
    if (AMHCharacterPlayer* Player = Cast<AMHCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->PlayAttackOverlayOnce(PendingSlot);  
    }

    // 보정 데미지 계산
    float ActualDamage = PendingDamage + (bSuccess ? 1.f : 0.f);
    if (GaugeWidget)
    {
        GaugeWidget->SetVisibility(ESlateVisibility::Hidden);
        GaugeWidget->OnGaugeResult.RemoveAll(this);
    }

    float FinalDamage = 0.f;
    bool  bHit = true;

    if (PendingTarget && PendingItem)
    {
        // 공격자 스탯
        AMHCharacterBase* Player = Cast<AMHCharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
        float Atk = 0.f;
        if (Player && Player->Stat) Atk = Player->Stat->GetAttackPower(); 

        // 컨텍스트 구성
        FMHAttackContext Ctx;
        Ctx.Attacker = Player;
        Ctx.Target = PendingTarget;
        Ctx.AttackerAttackPower = Atk;
        Ctx.bGaugeSuccess = bSuccess;

        // 아이템 효과 계산
        const FMHAttackResult R = PendingItem->ComputeAttack(Ctx);
        bHit = R.bHit;
        FinalDamage = R.Damage;

        if (bHit && FinalDamage > 0.f)
        {
            if (UMHCharacterStatComponent* Stat = PendingTarget->FindComponentByClass<UMHCharacterStatComponent>())
            {
                Stat->ApplyDamage(FinalDamage);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Attack missed."));
        }
    }

    // 턴 종료
    if (AMHBattleGameMode* GM = Cast<AMHBattleGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GM->EndTurn();
    }


    // 상태 초기화
    PendingTarget = nullptr;
    PendingItem = nullptr;
    PendingSlot = EEquipmentSlot::None;
}
