// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MHSkillWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MHCharacterPlayer.h"

void UMHSkillWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SkillButton)
    {
        SkillButton->OnClicked.AddDynamic(this, &UMHSkillWidget::OnSkillButtonClicked);
        //SkillButton->SetIsEnabled(false);
        if (AMHCharacterPlayer* Player = Cast<AMHCharacterPlayer>(
            UGameplayStatics::GetPlayerCharacter(this, 0)))
        {
            if (UMHCharacterStatComponent* StatComp = Player->FindComponentByClass<UMHCharacterStatComponent>())
            {
            
                StatComp->OnSkillGaugeChanged.AddUObject(this, &UMHSkillWidget::UpdateSkillButtonState);
            }
        }
    }
}

void UMHSkillWidget::OnSkillButtonClicked()
{
    if (AMHCharacterPlayer* PC = Cast<AMHCharacterPlayer>(
        UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        PC->UseAllEnemySkill();
    }
}

void UMHSkillWidget::UpdateSkillButtonState(int32 NewGauge)
{
    if (SkillButton)
    {
        SkillButton->SetIsEnabled(NewGauge >= 6);
    }
}
