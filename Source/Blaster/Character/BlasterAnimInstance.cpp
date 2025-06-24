// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouch = BlasterCharacter->bIsCrouched;
	bIsAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();

	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation,DeltaRot,DeltaSeconds,15.f);
	YawOffset = DeltaRotation.Yaw;
	

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();

	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds,6.f);
	Lean = FMath::Clamp(Interp,-90.f,90.f);

	AOYaw = BlasterCharacter->GetAOYaw();
	AOPitch = BlasterCharacter->GetAOPitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation );

		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = BlasterCharacter->GetMesh()->GetSocketTransform(FName("Hand_R"), RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(),RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
		}
		/*
		 * For debug: 
		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"),RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f,FColor::White);
		DrawDebugLine(GetWorld(),MuzzleTipTransform.GetLocation(),BlasterCharacter->GetHitTarget(),FColor::Purple);
		*/
	}
}
