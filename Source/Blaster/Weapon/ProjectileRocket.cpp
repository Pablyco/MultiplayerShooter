// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "RocketMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"


AProjectileRocket::AProjectileRocket()
{
	PrimaryActorTick.bCanEverTick = true;

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>("RocketMovementComponent");
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
	
}

void AProjectileRocket::Destroyed()
{
	
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&AProjectileRocket::OnHit);
	}
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, GetRootComponent(), FName(),
		                                                               GetActorLocation(),
		                                                               EAttachLocation::KeepWorldPosition, false, 1.f,
		                                                               1.f, 0.f, LoopingSoundAttenuation,
		                                                               nullptr, false);
	}

	
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner()) return;
	
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(),
				DamageInnerRadius,DamageOuterRadius, DamageFalloff, UDamageType::StaticClass(),TArray<AActor*>(), this, FiringController);
		}
	}
	
	GetWorldTimerManager().SetTimer(DestroyTimer,this,&AProjectileRocket::DestroyTimerFinished,DestroyTime,false);

	if (ImpactPlayerParticle && bPlayerImpact)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactPlayerParticle,GetActorLocation());
	}
	else
	{
		if (ImpactParticles)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactParticles,GetActorLocation());
		}
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TracerComponent)
	{
		if (auto SystemInstanceController = TracerComponent->GetSystemInstanceController())
		{
			SystemInstanceController->Deactivate();
		}
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

