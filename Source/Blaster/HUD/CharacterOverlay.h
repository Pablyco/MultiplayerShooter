﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmountText;

	UPROPERTY(meta = (BindWidget))
	class UBorder* BorderEliminated;

	
	
private:

	
};
