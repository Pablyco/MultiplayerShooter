﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	ACasing();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void DestroyShell();
	
private:
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;


};
