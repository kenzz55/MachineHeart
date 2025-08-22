// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MHBattleGameMode.h"
#include "Character/MHCharacterEnemy.h"
#include "Character/MHCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MHBattleSpawnPoint.h"
#include "PaperCharacter.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"  
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AMHBattleGameMode::AMHBattleGameMode()
{
    BGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComp"));
    BGMComp->bAutoActivate = false;     
    BGMComp->bAllowSpatialization = false;
}

void AMHBattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (BattleMusic && BGMComp)
    {
        BGMComp->SetSound(BattleMusic);
        BGMComp->FadeIn(1.5f, 0.7f); 
    }
}

void AMHBattleGameMode::StartPlay()
{
    Super::StartPlay();
    
    // 플레이어 등록
    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        PlayerActor = Cast<AMHCharacterBase>(Pawn);
        
    }
   

    SpawnEnemiesFromPoints(); 
    TryStartBattle();
    //StartNewRound();
}

void AMHBattleGameMode::EndBattle(bool bPlayerWon)
{
    if (bPlayerWon)
    {
        if (AMHCharacterPlayer* Player = Cast<AMHCharacterPlayer>(
            UGameplayStatics::GetPlayerCharacter(this, 0)))
        {
            Player->SaveStateToGameInstance();
        }

        UGameplayStatics::OpenLevel(this, WorldMapLevelName);
    }
    else
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (!DefeatWidgetInstance && DefeatWidgetClass)
        {
            DefeatWidgetInstance = CreateWidget<UUserWidget>(PC, DefeatWidgetClass);
            if (DefeatWidgetInstance)
            {
                
                DefeatWidgetInstance->AddToViewport(/*ZOrder*/ 100);
            }
        }

        PC->bShowMouseCursor = true;
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);

        FInputModeUIOnly InputMode;
        if (DefeatWidgetInstance)
        {
            
            InputMode.SetWidgetToFocus(DefeatWidgetInstance->TakeWidget());
        }
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);

     
        
        if (BGMComp && BGMComp->IsPlaying())
        {
            UE_LOG(LogTemp, Warning, TEXT("fade out"));
            BGMComp->FadeOut(1.0f, 0.0f);
        }

        if (DefeatMusic && BGMComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("fade in"));
       
            BGMComp->bIsUISound = true;
            BGMComp->SetSound(DefeatMusic);
            BGMComp->FadeIn(1.5f, 0.7f);
        }

        UGameplayStatics::SetGamePaused(this, true);
    }
}

void AMHBattleGameMode::OnCharacterDeath(AMHCharacterBase* DeadCharacter)
{
    if (DeadCharacter == PlayerActor)
    {
        EndBattle(false);
        return;
    }

  
    if (AMHCharacterEnemy* DeadEnemy = Cast<AMHCharacterEnemy>(DeadCharacter))
    {
    
        EnemyActors.Remove(DeadEnemy);

   
        int32 RemovedIdx = TurnOrder.IndexOfByKey(DeadEnemy);
        if (RemovedIdx != INDEX_NONE)
        {
            TurnOrder.RemoveAt(RemovedIdx);
            if (RemovedIdx <= CurrentTurnIndex && CurrentTurnIndex > 0)
            {
                --CurrentTurnIndex;
            }
        }

       
    }
}

void AMHBattleGameMode::StartTurn(AMHCharacterBase* Char)
{
    if (!Char) return;
    
    if (!bFirstTurnDelayDone)
    {
        bFirstTurnDelayDone = true;

        FTimerHandle DelayHandle;
        GetWorldTimerManager().SetTimer(
            DelayHandle,
            FTimerDelegate::CreateLambda([Char]()
                {
                    Char->OnTurnStarted();
                }),
            3.0f,   // 첫 턴 딜레이
            false
        );
    }
    else
    {
        // 이후 턴은 바로 실행
        Char->OnTurnStarted();
    }
}

void AMHBattleGameMode::EndTurn()
{
    // 다음 턴으로
    bool bAnyEnemyAlive = false;
    for (AMHCharacterEnemy* E : EnemyActors)
    {
        if (E && E->FindComponentByClass<UMHCharacterStatComponent>()->GetCurrentHp() > 0.f)
        {
            bAnyEnemyAlive = true;
            break;
        }
    }
    if (!bAnyEnemyAlive)
    {
        EndBattle(true);
        return;
    }

 
    ++CurrentTurnIndex;
    if (CurrentTurnIndex < TurnOrder.Num())
    {
        StartTurn(TurnOrder[CurrentTurnIndex]);
    }
    else
    {

        StartNewRound();
    }
}

void AMHBattleGameMode::GetAliveEnemies(TArray<AMHCharacterEnemy*>& OutEnemies) const
{
    OutEnemies.Reset();
    for (AMHCharacterEnemy* E : EnemyActors)
    {
        if (!E) continue;
        if (const UMHCharacterStatComponent* S = E->FindComponentByClass<UMHCharacterStatComponent>())
        {
            if (S->GetCurrentHp() > 0.f)
            {
                OutEnemies.Add(E);
            }
        }
    }
}





void AMHBattleGameMode::TryStartBattle()
{
    if (!PlayerActor)
    {
        if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
        {
            PlayerActor = Cast<AMHCharacterBase>(Pawn);
        }

        GetWorldTimerManager().SetTimerForNextTick(this, &AMHBattleGameMode::TryStartBattle);
        return;
    }

    StartNewRound();
}

void AMHBattleGameMode::SpawnEnemiesFromPoints()
{
    EnemyActors.Empty();

    // 레벨에 배치된 SpawnPoint BP 읽기
    TArray<AActor*> FoundPoints;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AMHBattleSpawnPoint::StaticClass(),
        FoundPoints
    );

    for (AActor* Actor : FoundPoints)
    {
        AMHBattleSpawnPoint* SP = Cast<AMHBattleSpawnPoint>(Actor);
        if (SP && SP->EnemyClass)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

           
            if (AMHCharacterEnemy* E = GetWorld()->SpawnActor<AMHCharacterEnemy>(
                SP->EnemyClass, SP->GetActorTransform(), Params))
            {
                EnemyActors.Add(E);
                
                
            }
        }
    }
}

void AMHBattleGameMode::StartNewRound()
{
    TurnOrder.Empty();
    if (PlayerActor &&
        PlayerActor->FindComponentByClass<UMHCharacterStatComponent>()->GetCurrentHp() > 0.f)
    {
        TurnOrder.Add(PlayerActor);
    }
    for (AMHCharacterEnemy* E : EnemyActors)
    {
        if (E && E->FindComponentByClass<UMHCharacterStatComponent>()->GetCurrentHp() > 0.f)
        {
            TurnOrder.Add(E);
        }
    }

    // Speed 내림차순 정렬
    TurnOrder.Sort([](AMHCharacterBase& A, AMHCharacterBase& B) {
        return A.Stat->GetSpeed() > B.Stat->GetSpeed();
        });

    // 인덱스 초기화 후 첫 캐릭터 행동 시작
    CurrentTurnIndex = 0;
    if (TurnOrder.Num() > 0)
    {
        StartTurn(TurnOrder[CurrentTurnIndex]);


        UE_LOG(LogTemp, Warning, TEXT("=== TurnOrder BEFORE sort ==="));
        for (auto* C : TurnOrder)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s Speed=%.1f"),
                *GetNameSafe(C), C && C->Stat ? C->Stat->GetSpeed() : -1.f);
        }

   
        UE_LOG(LogTemp, Warning, TEXT("=== TurnOrder AFTER sort ==="));
        for (auto* C : TurnOrder)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s Speed=%.1f"),
                *GetNameSafe(C), C && C->Stat ? C->Stat->GetSpeed() : -1.f);
        }
    }
    else
    {
        // 참가자 전멸 시 자동 승리
        EndBattle(true);
    }
}
