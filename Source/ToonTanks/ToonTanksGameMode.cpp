// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameMode.h"

#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "ToonTankPlayerController.h"

void AToonTanksGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Tank)
	{
		Tank->HandleDestruction();
		if (ToonTankPlayerController)
		{
			ToonTankPlayerController->SetPlayerEnableState(false);
		}

		GameOver(false);
	}
	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))
	{
		DestroyedTower->HandleDestruction();
		--TargetTowers;
		if (TargetTowers == 0)
		{
			GameOver(true);
		}
	}
}

void AToonTanksGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	FTimerHandle SoundTimerHandle;
	GetWorldTimerManager().SetTimer(SoundTimerHandle, this, &AToonTanksGameMode::StartSoundTimerHandle, StartDelay);
}

void AToonTanksGameMode::HandleGameStart()
{
	TargetTowers = GetTargetTowerCount();

	Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	ToonTankPlayerController = Cast<AToonTankPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	StartGame();

	if (ToonTankPlayerController)
	{
		ToonTankPlayerController->SetPlayerEnableState(false);
		
		FTimerHandle PlayerEnableTimerHandle;

		FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(ToonTankPlayerController, 
			&AToonTankPlayerController::SetPlayerEnableState, 
			true);

		GetWorldTimerManager().SetTimer(PlayerEnableTimerHandle, 
			PlayerEnableTimerDelegate, 
			StartDelay,
			false);
	}
}

int32 AToonTanksGameMode::GetTargetTowerCount()
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), Towers);
	
	return Towers.Num();
}

void AToonTanksGameMode::StartSoundTimerHandle()
{
	if (BackgroundSound)
	{
		UGameplayStatics::PlaySound2D(this, BackgroundSound, 0.25f);
	}
}
