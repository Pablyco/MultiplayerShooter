// Fill out your copyright notice in the Description page of Project Settings.

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
	
	void virtual OnPossess(APawn* InPawn) override;

private:

	class ABlasterHUD* BlasterHUD;
};
