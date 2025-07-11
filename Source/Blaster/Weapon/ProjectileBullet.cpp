// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (AController* OwnerController = Cast<AController>(OwnerCharacter->Controller))
		{
			UGameplayStatics::ApplyDamage(OtherActor,Damage,OwnerController,this,UDamageType::StaticClass());

		}
	}
	
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

