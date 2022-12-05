// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSWeapon.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSWeapon::AFPSWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "Muzzle";
	BaseDamage = 20.0f;

	StarterAmmoNumber = 20;
	CurrentAmmoInClip = StarterAmmoNumber;

	SetReplicates(true);
}

void AFPSWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AFPSWeapon::ServerFire_Validate()
{
	return true;
}

void AFPSWeapon::ServerAddAmmo_Implementation()
{
	CurrentAmmoInClip++;
	OnAmmoChanged.Broadcast(this, CurrentAmmoInClip);
}

bool AFPSWeapon::ServerAddAmmo_Validate()
{
	return true;
}

void AFPSWeapon::Fire()
{
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
		return;
	}

	if (GetWorld() && GetOwner())
	{
		AActor* MyOwner = GetOwner();

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (ProjectileClass)
		{
			AActor* Proj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
			Proj->SetOwner(MyOwner);
		}

		CurrentAmmoInClip--;
		OnAmmoChanged.Broadcast(this, CurrentAmmoInClip);
	}
}

bool AFPSWeapon::CanFire()
{
	return CurrentAmmoInClip > 0;
}

void AFPSWeapon::OnRep_CurrentAmmoInClip()
{
	OnAmmoChanged.Broadcast(this, CurrentAmmoInClip);
}

void AFPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSWeapon, CurrentAmmoInClip);
}

