// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MHGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/MHGameInstance.h"

void AMHGameMode::StartPlay()
{
    Super::StartPlay();

    // PendingReturnLocation에 값이 있으면 플레이어 스폰 후 위치 옮기기
    UMHGameInstance* GI = Cast<UMHGameInstance>(GetGameInstance());
    if (GI && !GI->PendingReturnLocation.IsZero())
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                Pawn->SetActorLocation(GI->PendingReturnLocation);
                GI->PendingReturnLocation = FVector::ZeroVector;
            }
        }
    }
}
