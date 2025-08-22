// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MHSkillWidget.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class MACHINEHEART_API UMHSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    
    UPROPERTY(meta = (BindWidget))
    UButton* SkillButton;

private:
    UFUNCTION()
    void OnSkillButtonClicked();

    UFUNCTION()
    void UpdateSkillButtonState(int32 NewGauge);

};
