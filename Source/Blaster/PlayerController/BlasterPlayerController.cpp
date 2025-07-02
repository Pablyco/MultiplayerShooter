// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD
		->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health),FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDSCore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD
		->CharacterOverlay->ScoreAmountText;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("Score: %d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmountText->SetText(FText::FromString(ScoreText));
	}

}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD
		->CharacterOverlay->DefeatAmountText;

	if (bHUDValid)
	{
		FString DefeatText = FString::Printf(TEXT("Defeats: %d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatAmountText->SetText(FText::FromString(DefeatText));
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD
		->CharacterOverlay->AmmoAmountText;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("Ammo: %d"), Ammo);
		BlasterHUD->CharacterOverlay->AmmoAmountText->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD
		->CharacterOverlay->CarriedAmountText;

	if (bHUDValid)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("/%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmountText->SetText(FText::FromString(CarriedAmmoText));
	}
}

void ABlasterPlayerController::SetHUDEliminated(bool Eliminated)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD
		->CharacterOverlay->BorderEliminated;
	
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->BorderEliminated->SetVisibility(Eliminated ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		SetHUDHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
		SetHUDEliminated(false);
	}
}
