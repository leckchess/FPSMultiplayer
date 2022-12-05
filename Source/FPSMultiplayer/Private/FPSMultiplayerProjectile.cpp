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
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

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
	bIsExploaded = false;

	bCanInteract = false;

	ExplosionSpeedParameterName = "Frequency";
	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

	SetReplicates(true);
}

void AFPSMultiplayerProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetLocalRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(ExplosionAnimationDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Blink, DefaultExplosionTime - AnimationExplosionTime, true);
	}
}

void AFPSMultiplayerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ExplosionDynamicMaterial && GetLocalRole() == ROLE_Authority)
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
		bCanInteract = true;

		if (OtherComp->IsSimulatingPhysics())
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		else if (GetWorld() && OtherActor != GetOwner())
		{
			if (AFPSMultiplayerCharacter* Player = Cast<AFPSMultiplayerCharacter>(OtherActor))
			{
				OverlayBox->DestroyComponent();
				GetProjectileMovement()->bShouldBounce = false;
				AttachToActor(Player, FAttachmentTransformRules::KeepWorldTransform);

				if (GetLocalRole() == ROLE_Authority)
				{
					if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionAnimationDelay_TimeHandler) && GetWorld()->GetTimerManager().GetTimerRemaining(ExplosionAnimationDelay_TimeHandler) > PlayerExplosionTime)
					{
						GetWorld()->GetTimerManager().ClearTimer(ExplosionAnimationDelay_TimeHandler);
						GetWorld()->GetTimerManager().SetTimer(ExplosionAnimationDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Blink, PlayerExplosionTime - AnimationExplosionTime, true);
					}
				}
			}
		}
	}
}

void AFPSMultiplayerProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComponent != nullptr && bCanInteract)
	{
		if (AFPSMultiplayerCharacter* Player = Cast<AFPSMultiplayerCharacter>(OtherActor))
		{
			if (!Player->GetCurrentOverlappedProjectile())
			{
				Player->SetCurrentOverlappedProjectile(this);

				if (UTextRenderComponent* TextRenderComp = Cast<UTextRenderComponent>(GetComponentByClass(UTextRenderComponent::StaticClass())))
				{
					TextRenderComp->SetText(FText::FromString("Press E to Pickup"));

					if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
					{
						FVector CameraLocation = CameraManager->GetCameraLocation();
						FVector ProjectileLocation = GetActorLocation();

						FRotator BillboardRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileLocation, CameraLocation);
						TextRenderComp->SetWorldRotation(BillboardRotation);
					}
				}
			}
		}
	}
}

void AFPSMultiplayerProjectile::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherComponent != nullptr)
	{
		if (AFPSMultiplayerCharacter* Player = Cast<AFPSMultiplayerCharacter>(OtherActor))
		{
			Player->SetCurrentOverlappedProjectile(nullptr);

			if (UTextRenderComponent* TextRenderComp = Cast<UTextRenderComponent>(GetComponentByClass(UTextRenderComponent::StaticClass())))
			{
				TextRenderComp->SetText(FText::FromString(""));
			}
		}
	}
}

void AFPSMultiplayerProjectile::ServerPickUp_Implementation()
{
	if (GetWorld())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionAnimationDelay_TimeHandler))
			GetWorld()->GetTimerManager().ClearTimer(ExplosionAnimationDelay_TimeHandler);

		if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionDelay_TimeHandler))
			GetWorld()->GetTimerManager().ClearTimer(ExplosionDelay_TimeHandler);

		Destroy();
	}
}

bool AFPSMultiplayerProjectile::ServerPickUp_Validate()
{
	return true;
}

void AFPSMultiplayerProjectile::Blink()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	if (GetWorld()->GetTimerManager().IsTimerActive(ExplosionDelay_TimeHandler))
		return;

	if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		ExplosionDynamicMaterial = StaticMeshComp->CreateDynamicMaterialInstance(0, ExploadingMaterialInstance);

		ExplosionSpeedParameterValue = 0;
		GetWorld()->GetTimerManager().SetTimer(ExplosionDelay_TimeHandler, this, &AFPSMultiplayerProjectile::Expload, AnimationExplosionTime, true);
	}
}

void AFPSMultiplayerProjectile::Expload()
{
	if (GetLocalRole() != ROLE_Authority)
		return;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, IgnoreActors, GetOwner(), GetOwner()->GetInstigatorController(), true);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 100, FColor::Red, true, 20);

	bIsExploaded = true;
	OnRep_bIsExploaded();

	SetHidden(true);
	SetLifeSpan(0.05f);
}

void AFPSMultiplayerProjectile::PlayExplosionEffect()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	}
}

void AFPSMultiplayerProjectile::OnRep_ExplosionSpeedParameterValue()
{
	if (ExplosionSpeedParameterValue > 0)
	{
		if (!ExplosionDynamicMaterial)
		{
			if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass())))
				ExplosionDynamicMaterial = StaticMeshComp->CreateDynamicMaterialInstance(0, ExploadingMaterialInstance);
		}

		ExplosionDynamicMaterial->SetScalarParameterValue(ExplosionSpeedParameterName, ExplosionSpeedParameterValue);
	}
}

void AFPSMultiplayerProjectile::OnRep_bIsExploaded()
{
	if (bIsExploaded)
		PlayExplosionEffect();
}

void AFPSMultiplayerProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSMultiplayerProjectile, ExplosionSpeedParameterValue);
	DOREPLIFETIME(AFPSMultiplayerProjectile, bIsExploaded);
}