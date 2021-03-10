// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Containers/UnrealString.h"
#include "Styling/SlateColor.h"
#include "Styling/WidgetStyle.h"
#include "ShooterCharacter.generated.h"

class AGun;

UCLASS()
class TPS_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
		bool IsDead() const;

	UFUNCTION(BlueprintPure)
		float GetHealthPercent() const;
	UFUNCTION(BlueprintPure)
		FString ShowAid() const;
	UFUNCTION(BlueprintPure)
		float GetAllAmmo() const;
	UFUNCTION(BlueprintPure)
		float GetLoadedAmmo() const;

	void StartShooting();
	void StopShooting();
	
private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	void RunOrWalk();
	void UseAid();
	bool CanUseAid() const;
	void SwitchWeaponFirst();
	void SwitchWeaponSecond();
	void Reload();

	float Speed = 1.f;
	void StartRun();
	void StopRun();

	AController* GetOwnerController() const;

	FTimerHandle ShootTimer;

	AGun* ActiveGun;

	UPROPERTY(EditAnywhere)
		float RotationRate = 25.f;

	UPROPERTY(EditDefaultsOnly)
		float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere)
		float HP;

	UPROPERTY(EditAnywhere)

		float HPAid = 3;

	UPROPERTY(EditAnywhere)
		float AidPercentValue = 35.f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AGun> GunClass;

	UPROPERTY()
		AGun* MainGun;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AGun> SecondGunClass;

	UPROPERTY()
		AGun* SecondGun;

	UPROPERTY(EditAnywhere)
		USoundBase* WeaponSwapSound;

};
