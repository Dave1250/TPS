// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "TPSGameModeBase.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	HP = MaxHP;

	MainGun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	MainGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	MainGun->SetOwner(this);

	ActiveGun = MainGun;

	SecondGun = GetWorld()->SpawnActor<AGun>(SecondGunClass);
	SecondGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	if (SecondGun != nullptr)
	{
		SecondGun->SetHidden(true);
		SecondGun->SetOwner(this);
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Movement
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Pressed, this, &AShooterCharacter::StartRun);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Released, this, &AShooterCharacter::StopRun);
	PlayerInputComponent->BindAction(TEXT("Run/Walk"), EInputEvent::IE_Pressed, this, &AShooterCharacter::RunOrWalk);
	//Look
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	//Shoot
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::StartShooting);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &AShooterCharacter::StopShooting);
	//Actions
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Reload);
	PlayerInputComponent->BindAction(TEXT("MainWeapon"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SwitchWeaponFirst);
	PlayerInputComponent->BindAction(TEXT("SecondaryWeapon"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SwitchWeaponSecond);
	PlayerInputComponent->BindAction(TEXT("UseAid"), EInputEvent::IE_Pressed, this, &AShooterCharacter::UseAid);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
	class AController* EventInstigator, AActor* DamageCauser) 
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(DamageToApply, HP);
	HP -= DamageToApply;

	if (IsDead())
	{
		ATPSGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ATPSGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->StopShooting();
		DetachFromControllerPendingDestroy();
	}
	return DamageToApply;
}

void AShooterCharacter::MoveForward(float AxisValue) 
{
	AddMovementInput(GetActorForwardVector() * AxisValue, Speed);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue, Speed);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::StartRun()
{
	if (Speed == 1.f)
	{
		Speed = 0.25f;
	}
	else Speed = 1.f;
}

void AShooterCharacter::StopRun()
{
	if (Speed == 0.25f)
	{
		Speed = 1.f;
	}
	else Speed = 0.25f;
}

bool AShooterCharacter::IsDead() const
{
	return HP <= 0;
}

void AShooterCharacter::RunOrWalk()
{
	if (Speed == 1)
	{
		Speed = 0.25f;
	}
	else if (Speed == 0.25f)
	{
		Speed = 1.f;
	}
}

void AShooterCharacter::StartShooting()
{
	if (ActiveGun != nullptr)
	{
		ActiveGun->PullTrigger();
		if (ActiveGun->CheckIfAutomatic())
		{
			GetWorldTimerManager().SetTimer(ShootTimer, ActiveGun, &AGun::PullTrigger, ActiveGun->GetShootCooldown(), true);
		}
	}
}

void AShooterCharacter::StopShooting()
{
	GetWorldTimerManager().ClearTimer(ShootTimer);
}

float AShooterCharacter::GetHealthPercent() const
{
	return (HP / MaxHP);
}

bool AShooterCharacter::CanUseAid() const
{
	if (HP < MaxHP && HPAid > 0)
	{
		return true;
	}
	else return false;
}

void AShooterCharacter::UseAid()
{
	if (CanUseAid())
	{
		HPAid--;
		float AidValue = HP + (MaxHP / (AidPercentValue / 100));
		HP = FMath::Min(MaxHP, AidValue);
	}
}

FString AShooterCharacter::ShowAid() const
{
		FString Result{};
	if (HPAid > 0 && HPAid < 5)
	{
		for (int i = 0; i < HPAid; i++)
		{
			Result += TEXT("+ ");
		}
		return Result;
	}
	else if (HPAid > 10)
	{
		Result = FString::SanitizeFloat(HPAid);
		return Result;
	}
	else return TEXT("---");
}

float AShooterCharacter::GetAllAmmo() const
{
	if (ActiveGun != nullptr)
	{
		return  ActiveGun->ShowAllAmmo();
	}
	else return 0;
}

float AShooterCharacter::GetLoadedAmmo() const
{
	if (ActiveGun != nullptr)
	{
		return ActiveGun->ShowLoadedAmmo();
	}
	else return 0;
}

AController* AShooterCharacter::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;
	return OwnerPawn->GetController();

}

void AShooterCharacter::SwitchWeaponFirst()
{
	if (MainGun != nullptr)
	{
		ActiveGun = MainGun;
		ActiveGun->SetActorHiddenInGame(false);
		UGameplayStatics::SpawnSoundAttached(WeaponSwapSound, GetOwner()->GetRootComponent());
		if (SecondGun != nullptr)
		{
			SecondGun->SetActorHiddenInGame(true);
		}
	}
}

void AShooterCharacter::SwitchWeaponSecond()
{
	if (SecondGun != nullptr)
	{
		ActiveGun = SecondGun;
		ActiveGun->SetActorHiddenInGame(false);
		UGameplayStatics::SpawnSoundAttached(WeaponSwapSound, GetOwner()->GetRootComponent());
		if (MainGun != nullptr)
		{
			MainGun->SetActorHiddenInGame(true);
		}
	}
}

void AShooterCharacter::Reload()
{
	if (ActiveGun != nullptr)
	{
		ActiveGun->Reload();
	}
}