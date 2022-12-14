// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMultiplayerCharacter.h"
#include "FPSMultiplayerProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "FPSWeapon.h"
#include "FPSHUD.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "FPSFloatingHPBar.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSMultiplayerCharacter

AFPSMultiplayerCharacter::AFPSMultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	RegainHealthColldown = 10;

	MaxHealth = 100;
	 
	bIsDead = false;

	WeaponSocketName = "GripPoint";

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	OnTakeAnyDamage.AddDynamic(this, &AFPSMultiplayerCharacter::HandleTakeAnyDamage);

	/*FloatingHPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP_BarWidget"));
	FloatingHPBar->SetupAttachment(GetCapsuleComponent());*/

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	if (GetCharacterMovement())
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AFPSMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority && WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AFPSWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}

		Health = MaxHealth;

		//MulticastUpdateHPFloatingBar();

		if (GetHUD())
		{
			HUD->Init();
			UpdateHUD();
		}
	}
	
	if (IsLocallyControlled())
	{
		GetMesh()->HideBoneByName("neck_01", EPhysBodyOp::PBO_None);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPSMultiplayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSMultiplayerCharacter::OnFire);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSMultiplayerCharacter::Interact);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSMultiplayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSMultiplayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSMultiplayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFPSMultiplayerCharacter::EndCrouch);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSMultiplayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSMultiplayerCharacter::LookUpAtRate);
}

void AFPSMultiplayerCharacter::OnFire()
{
	if (bIsDead || !CurrentWeapon || !CurrentWeapon->CanFire())
		return;

	CurrentWeapon->Fire();

	if (APlayerController* PlayerConroller = Cast<APlayerController>(GetController()))
	{
		PlayerConroller->ClientStartCameraShake(FireCameraShake);
	}

	if (FireAnimation)
	{
		GetMesh()->PlayAnimation(FireAnimation, false);
	}

	// TODO play character fire animation
}

void AFPSMultiplayerCharacter::Interact()
{
	if (!IsLocallyControlled())
		return;

	if (CurrentOverlappedProjectile)
	{
		if (CurrentWeapon)
		{
			CurrentOverlappedProjectile->SetOwner(this);
			CurrentWeapon->ServerAddAmmo();
			CurrentOverlappedProjectile->ServerPickUp();
		}
	}
}

void AFPSMultiplayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPSMultiplayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AFPSMultiplayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSMultiplayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSMultiplayerCharacter::StartCrouch()
{
	Crouch();
}

void AFPSMultiplayerCharacter::EndCrouch()
{
	UnCrouch();
}

void AFPSMultiplayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSMultiplayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFPSMultiplayerCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPSMultiplayerCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPSMultiplayerCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPSMultiplayerCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AFPSMultiplayerCharacter::OnRep_CurrentWeapon()
{
	if (GetHUD())
		HUD->Init();
}

//void AFPSMultiplayerCharacter::MulticastUpdateHPFloatingBar_Implementation()
//{
//	if (FloatingHPBar)
//	{
//		UFPSFloatingHPBar* Floating_HPWidget = Cast< UFPSFloatingHPBar>(FloatingHPBar->GetUserWidgetObject());
//		if (Floating_HPWidget)
//		{
//			Floating_HPWidget->UpdateHPBar(Health, MaxHealth);
//		}
//	}
//}

void AFPSMultiplayerCharacter::UpdateHUD()
{
	if (!HUD)
		return;

	HUD->OnHealthUpdated(Health,MaxHealth);
}

void AFPSMultiplayerCharacter::SetHUD(UFPSHUD* InHUD)
{
	if (InHUD)
		HUD = InHUD;

	UpdateHUD();
}

void AFPSMultiplayerCharacter::OnDeath()
{
	if (GetLocalRole() != ROLE_Authority || Health > 0 || bIsDead)
		return;

	UE_LOG(LogTemp, Log, TEXT("Current Health %f DEAAD"), Health);

	bIsDead = true;

	if (GetCharacterMovement())
		GetCharacterMovement()->StopActiveMovement();

	if (GetCapsuleComponent())
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FTimerHandle Respawn_TimeHandler;
	GetWorld()->GetTimerManager().SetTimer(Respawn_TimeHandler, this, &AFPSMultiplayerCharacter::Respawn, 5, true);

	if (GetWorld()->GetTimerManager().IsTimerActive(RegainHealth_TimeHandler))
		GetWorld()->GetTimerManager().ClearTimer(RegainHealth_TimeHandler);
}

void AFPSMultiplayerCharacter::Respawn()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetController())
	{
		GetWorld()->GetAuthGameMode()->RestartPlayer(GetController());

		if (GetCapsuleComponent())
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		bIsDead = false;
		Health = MaxHealth;

		UpdateHUD();
	}
}

void AFPSMultiplayerCharacter::RegainHealth()
{
	Health += 10;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);
	UpdateHUD();

	GetWorld()->GetTimerManager().SetTimer(RegainHealth_TimeHandler, this, &AFPSMultiplayerCharacter::RegainHealth, RegainHealthColldown, true);
}

void AFPSMultiplayerCharacter::OnRep_Health()
{
	UpdateHUD();
}

void AFPSMultiplayerCharacter::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (Damage <= 0)
		return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	if (Health <= 0)
	{
		OnDeath();
	}
	else
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(RegainHealth_TimeHandler))
			GetWorld()->GetTimerManager().ClearTimer(RegainHealth_TimeHandler);

		GetWorld()->GetTimerManager().SetTimer(RegainHealth_TimeHandler, this, &AFPSMultiplayerCharacter::RegainHealth, RegainHealthColldown, true);
	}

	//MulticastUpdateHPFloatingBar();

	UpdateHUD();
}

void AFPSMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSMultiplayerCharacter, CurrentWeapon);
	DOREPLIFETIME(AFPSMultiplayerCharacter, bIsDead);
	DOREPLIFETIME(AFPSMultiplayerCharacter, Health);
}
