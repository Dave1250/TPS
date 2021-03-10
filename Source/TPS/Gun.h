// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gun.generated.h"

UCLASS()
class TPS_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void PullTrigger();
	void Reload();
	float ShowAllAmmo() const;
	float ShowLoadedAmmo() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

	bool CheckIfAutomatic() const;
	float GetShootCooldown() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool CanShoot() const;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
		USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)
		USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
		UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere)
		USoundBase* NoAmmoSound;

	UPROPERTY(EditAnywhere)
		USoundBase* ReloadSound;


	UPROPERTY(EditAnywhere)
		float MaxRange = 5000.f;

	UPROPERTY(EditAnywhere)
		float Damage = 50.f;

	UPROPERTY(EditAnywhere)
		bool IsAutomatic = false;

	UPROPERTY(EditAnywhere)
		int32 MaxAmmo = 60;

	UPROPERTY(EditAnywhere)
		int32 MagazineAmmo = 15;

	UPROPERTY(EditAnywhere)
		float ShootCooldown = 0.15f;
	
	UPROPERTY(EditAnywhere)
		float ReloadCooldown = 0.5f;

	int32 Ammo;

	float ShootTimer;


	AController* GetOwnerController() const;
};
