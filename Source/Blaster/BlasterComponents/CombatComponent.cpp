// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BaseWalkSpeed = 600.f;   // Default walk speed when not aiming
	AimWalkSpeed = 450.f;    // Slower walk speed while aiming

}
void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate EquippedWeapon and aiming state to clients
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set the character's walk speed to the default value
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr || Character->Controller == nullptr) return;
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->GetController()) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			
			if (EquippedWeapon)
			{
				
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			//Calculate crosshair spread

			//Map to determinate crosshair spread affected by character walk speed.
			FVector2d WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2d VelocityMultiplierRange(0.f,1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,VelocityMultiplierRange,Velocity.Size());


			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,0.f,DeltaTime,30.f);
			}
			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.58f,DeltaTime,30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,0.f,DeltaTime,30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,0.f,DeltaTime,30.f);

			 
			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,EquippedWeapon->GetZoomedFOV(),DeltaTime,EquippedWeapon->ZoomInterpSpeed);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV,DeltaTime,ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	// Locally update the aiming state
	bAiming = bIsAiming;

	// Send aiming state to the server
	ServerSetAiming(bIsAiming);

	// Adjust movement speed based on aiming
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	// Server-side implementation of aiming
	bAiming = bIsAiming;

	// Adjust walk speed on the server
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	// Called on clients when EquippedWeapon is updated via replication
	if (EquippedWeapon && Character)
	{
		// Update character rotation behavior to aim with the weapon
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}



void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrossHairLocation(ViewportSize.X/2, ViewportSize.Y/2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrossHairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
		);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection*(DistanceToCharacter + 50.f);
		}

		
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGHT;


		GetWorld()->LineTraceSingleByChannel(TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;
	}
}


void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		ServerFire(HitResult.ImpactPoint);
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 1.f;
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	EquippedWeapon->Fire(TraceHitTarget);
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->FireFx();
	}
}



void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	// Safety check: ensure both character and weapon exist
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Equipped Error");
		return;
	}

	// Debug message for successful weapon equip
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Equipped"));

	// Assign and configure the equipped weapon
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	// Attach the weapon to the character’s right hand socket
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	// Set the character as the owner of the weapon
	EquippedWeapon->SetOwner(Character);

	// Update rotation settings for aiming
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

