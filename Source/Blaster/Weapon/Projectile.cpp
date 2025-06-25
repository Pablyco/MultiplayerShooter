// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Blaster/Blaster.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
	


	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	
	
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactParticles,GetActorLocation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(Tracer,CollisionBox,FName(),GetActorLocation(),GetActorRotation(),EAttachLocation::KeepWorldPosition,false,true);
	}
	
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		if (BlasterCharacter->HasAuthority())
		{
			BlasterCharacter->MulticastHit();
		}
	}
	
	Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

