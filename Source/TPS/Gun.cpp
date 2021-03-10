// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Ammo = FMath::Min(MaxAmmo, MagazineAmmo);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	Ammo = FMath::Min(MaxAmmo, MagazineAmmo);
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::PullTrigger()
{
	if (Ammo > 0)
	{
		if (CanShoot())
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

			Ammo--;
			ShootTimer = GetWorld()->GetTimeSeconds() + ShootCooldown;

			FHitResult Hit;
			FVector ShootDirection;
			bool bSuccess = GunTrace(Hit, ShootDirection);
			if (bSuccess)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.Location, ShootDirection.Rotation());
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
				AActor* HitActor = Hit.GetActor();
				if (HitActor)
				{
					FPointDamageEvent DamageEvent(Damage, Hit, ShootDirection, nullptr);
					AController* OwnerController = GetOwnerController();
					HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
				}
			}
		}
	}
	else if (MaxAmmo > 0)
	{
		Reload();
	}
	else
	{
		UGameplayStatics::SpawnSoundAttached(NoAmmoSound, Mesh, TEXT("MuzzleFlashSocket"));
	}
}

bool AGun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{

	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr) return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector End = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;
	return OwnerPawn->GetController();

}

bool AGun::CheckIfAutomatic() const
{
	return IsAutomatic;
}

float AGun::GetShootCooldown() const
{
	return ShootCooldown;
}

void AGun::Reload()
{
	if (MaxAmmo > 0)
	{
		MaxAmmo += Ammo;
		Ammo = FMath::Min(MaxAmmo, MagazineAmmo);
		MaxAmmo -= Ammo;
		UGameplayStatics::SpawnSoundAttached(ReloadSound, Mesh, TEXT("MuzzleFlashSocket"));
		ShootTimer = GetWorld()->GetTimeSeconds() + ReloadCooldown;
	}
}

float AGun::ShowAllAmmo() const
{
	return MaxAmmo;
}

float AGun::ShowLoadedAmmo() const
{
	return Ammo;
}

bool AGun::CanShoot() const
{
	if (GetWorld()->GetTimeSeconds() > ShootTimer)
	{
		return true;
	}
	else return false;
}