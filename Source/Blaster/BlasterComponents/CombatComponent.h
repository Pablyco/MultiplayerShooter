// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatStates.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGHT 80000.f

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	friend class ABlasterCharacter;
	
	void EquipWeapon(AWeapon* WeaponToEquip);

	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void Fire();

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION()
	void HandleReload();
	
private:

	UPROPERTY()
	class ABlasterCharacter* Character;
	
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	
	UPROPERTY()
	class ABlasterHUD* HUD;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	UPROPERTY()
	bool bFireButtonPressed;

	/**
	 * HUD and Crosshairs
	 **/

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	float CrosshairEnemyFactor;

	bool bEnemyInScope;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	/**
	 * Aiming and FOV
	 **/

	//FOV When not aiming; set to the camera's base FOV
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category="Combat")
	float ZoomedFOV = 30.f;

	
	float CurrentFOV; 

	UPROPERTY(EditAnywhere, Category="Combat")
	float ZoomInterpSpeed = 20.f;


	void InterpFOV(float DeltaTime);

	
	/*
	 * Automatic fire
	 */
	
	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
	void OnRep_CombatState();
};


