// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMultiplayerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TextRenderComponent.h"
#include "FPSMultiplayerCharacter.h"

AFPSMultiplayerProjectile::AFPSMultiplayerProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSMultiplayerProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	OverlayBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlayBoxComp"));
	OverlayBox->SetBoxExtent(FVector(50, 50, 50));
	OverlayBox->OnComponentBeginOverlap.AddDynamic(this, &AFPSMultiplayerProjectile::OnBeginOverlap);
	OverlayBox->OnComponentEndOverlap.AddDynamic(this, &AFPSMultiplayerProjectile::OnEndOverlap);
	OverlayBox->AttachToComponent(CollisionComp, FAttachmentTransformRules::KeepRelativeTransform);

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	AnimationExplosionTime = 3;
	PlayerExplosionTime = 4;
	DefaultExplosionTime = 8;

	ExplosionSpeedParameterName = "Frequency";
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AFPSMultiplayerProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ExplosionAnimationDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Blink, DefaultExplosionTime - AnimationExplosionTime, true);
	}
}

void AFPSMultiplayerProjectile::Tick(float DeltaTime)
{
	if (ExplosionDynamicMaterial && GetWorld()->GetTimerManager().IsTimerActive(ExplosionDelay_TimeHandler))
	{
		ExplosionSpeedParameterValue += (DeltaTime / AnimationExplosionTime);
		ExplosionDynamicMaterial->SetScalarParameterValue(ExplosionSpeedParameterName, ExplosionSpeedParameterValue);
	}
}


void AFPSMultiplayerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherComp->IsSimulatingPhysics())
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		else if (GetWorld() && OtherActor != GetOwner())
		{
			if (AFPSMultiplayerCharacter* Player = Cast<AFPSMultiplayerCharacter>(OtherActor))
			{
				OverlayBox->DestroyComponent();

				// attach to player and disable interaction
				if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionAnimationDelay_TimeHandler) && GetWorld()->GetTimerManager().GetTimerRemaining(ExplosionAnimationDelay_TimeHandler) > PlayerExplosionTime)
				{
					GetWorld()->GetTimerManager().ClearTimer(ExplosionAnimationDelay_TimeHandler);
					GetWorld()->GetTimerManager().SetTimer(ExplosionAnimationDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Blink, PlayerExplosionTime - AnimationExplosionTime, true);
				}
			}
		}
	}
}

void AFPSMultiplayerProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComponent != nullptr && OtherActor->IsA(AFPSMultiplayerCharacter::StaticClass()))
	{
		if (UTextRenderComponent* TextRenderComp = Cast<UTextRenderComponent>(GetComponentByClass(UTextRenderComponent::StaticClass())))
		{
			TextRenderComp->SetText(FText::FromString("Press E to Pickup"));

			if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager (GetWorld(),0))
			{
				FVector CameraLocation = CameraManager->GetCameraLocation();
				FVector ProjectileLocation = GetActorLocation();

				FRotator BillboardRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileLocation, CameraLocation);
				TextRenderComp->SetWorldRotation(BillboardRotation);
			}
		}
	}
}

void AFPSMultiplayerProjectile::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComponent != nullptr && OtherActor->IsA(AFPSMultiplayerCharacter::StaticClass()))
	{
		if (UTextRenderComponent* TextRenderComp = Cast<UTextRenderComponent>(GetComponentByClass(UTextRenderComponent::StaticClass())))
		{
			TextRenderComp->SetText(FText::FromString(""));
		}
	}
}

void AFPSMultiplayerProjectile::Blink()
{
	if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		ExplosionDynamicMaterial = StaticMeshComp->CreateDynamicMaterialInstance(0, ExploadingMaterialInstance);

		if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionDelay_TimeHandler))
			GetWorld()->GetTimerManager().ClearTimer(ExplosionDelay_TimeHandler);

		ExplosionSpeedParameterValue = 0;
		GetWorld()->GetTimerManager().SetTimer(ExplosionDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Expload, AnimationExplosionTime, true);
	}
}

void AFPSMultiplayerProjectile::Expload()
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, IgnoreActors);

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}

	Destroy();
}

void AFPSMultiplayerProjectile::PickUp()
{
	// give t to the player + ammo
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ExplosionAnimationDelay_TimeHandler);
		GetWorld()->GetTimerManager().ClearTimer(ExplosionDelay_TimeHandler);
	}
}