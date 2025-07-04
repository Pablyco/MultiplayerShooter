﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDSCore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDEliminated(bool Eliminated);
	
	void virtual OnPossess(APawn* InPawn) override;

private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
};
