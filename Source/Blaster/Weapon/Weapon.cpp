

#include "Weapon.h"

#include "Casing.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"




AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false; // Disable ticking, weapon doesn't need per-frame updates
	bReplicates = true; // Enable replication for multiplayer

	// Create and configure the weapon mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh); // Set mesh as the root component

	// Configure mesh collision
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block); // Block all channels by default
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // Ignore player collisions
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable collision initially

	// Create and configure the area detection sphere
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignore all channels by default
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable collision initially

	// Create and attach the pickup UI widget
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	PickupWidget->SetupAttachment(RootComponent); // Attached to root (mesh)
	
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	// Hide pickup widget by default
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// Setup overlap events only on the server
	if (HasAuthority())
	{
		// Enable collision for the detection sphere
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap with players

		// Bind overlap begin and end events
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the WeaponState variable across the network
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

// Called when a pawn enters the detection sphere
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		// Let the character know they are overlapping this weapon
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}
// Called when a pawn exits the detection sphere
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		// Clear the weapon overlap on the character
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}


void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	
	if (BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr
			                         ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->GetController())
			                         : BlasterOwnerController;

		if (BlasterOwnerController)
		{
			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SpendRound()
{
	UE_LOG(LogTemp,Warning,TEXT("ammo %d"),Ammo);
	Ammo = FMath::Clamp(Ammo - 1,0,MagCapacity);
	UE_LOG(LogTemp,Warning,TEXT("ammo %d"),Ammo);
	SetHUDAmmo();
	
}

void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}


// Sets the current state of the weapon and updates components accordingly
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false); // Hide the pickup widget
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable pickup detection
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
		
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

// Called automatically when WeaponState is replicated
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		
		ShowPickupWidget(false);// Hide widget on clients
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
		
	case EWeaponState::EWS_Dropped:
		
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
	
}



// Show or hide the pickup widget
void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	SpendRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}

void AWeapon::FireFx()
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
}

void AWeapon::SpawnCasing()
{
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(),SocketTransform.GetRotation().Rotator());
			}
		}
	}
}
