

#include "Weapon.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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
// Sets the current state of the weapon and updates components accordingly
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false); // Hide the pickup widget
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable pickup detection
		break;
	}
}
// Called automatically when WeaponState is replicated
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);// Hide widget on clients
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
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
}
