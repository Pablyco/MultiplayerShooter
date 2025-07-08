// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"


USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	class UTexture2D* CrosshairsCenter;
	
	UPROPERTY()
	UTexture2D* CrosshairsLeft;
	
	UPROPERTY()
	UTexture2D* CrosshairsRight;
	
	UPROPERTY()
	UTexture2D* CrosshairsTop;
	
	UPROPERTY()
	UTexture2D* CrosshairsBottom;
	
	UPROPERTY()
	float CrosshairSpread;
	
	UPROPERTY()
	FLinearColor CrosshairColor;
};
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;

protected:
	virtual void BeginPlay() override;
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter,FVector2d Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	void AddCharacterOverlay();
	
	void AddAnnouncement();
	void SetAnnouncementVisibility(ESlateVisibility Visibility);

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
