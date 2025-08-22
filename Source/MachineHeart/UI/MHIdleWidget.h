// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MHIdleWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UMHCharacterStatComponent;


UCLASS()
class MACHINEHEART_API UMHIdleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

protected:
    virtual void NativeConstruct() override;

private:

    UMHCharacterStatComponent* StatComp = nullptr;

    UFUNCTION()
    void HandleHpChanged(float NewHp);


    void RefreshHealthBar();

};
