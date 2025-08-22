// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHIdleWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MHCharacterPlayer.h"
#include "CharacterStat/MHCharacterStatComponent.h"

void UMHIdleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (AMHCharacterBase* C = Cast<AMHCharacterBase>(
            UGameplayStatics::GetPlayerCharacter(this, 0)))
        {
      
            StatComp = C->FindComponentByClass<UMHCharacterStatComponent>();
            if (StatComp)
            {
           
                StatComp->OnHpChanged.AddUObject(this, &UMHIdleWidget::HandleHpChanged);
           
                RefreshHealthBar();
            }
        }
    }
}

void UMHIdleWidget::HandleHpChanged(float NewHp)
{
    RefreshHealthBar();
}

void UMHIdleWidget::RefreshHealthBar()
{
    if (!HealthBar || !StatComp)
        return;

    const float Current = StatComp->GetCurrentHp();
    const float Max = StatComp->GetMaxHp();
    const float Percent = (Max > 0.f) ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;
    HealthBar->SetPercent(Percent);
}
