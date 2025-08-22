// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHEnemyHpWidget.h"
#include "MHEnemyHpWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h" 

void UMHEnemyHpWidget::SetHPPercent(float Percent)
{
    if (HPBar)
    {
        HPBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
    }
}

void UMHEnemyHpWidget::SetHPText(float CurrentHP)
{
    if (HPText)
    {
        int32 HPValue = FMath::RoundToInt(CurrentHP);
        HPText->SetText(FText::AsNumber(HPValue));
    }
}
