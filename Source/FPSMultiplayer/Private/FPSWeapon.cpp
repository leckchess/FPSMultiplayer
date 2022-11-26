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

	//SetReplicates(true);

}

void AFPSWeapon::Fire()
{
	if (GetWorld() && GetOwner() && CurrentAmmoInClip > 0)
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

void AFPSWeapon::AddAmmo()
{
	CurrentAmmoInClip++;
	OnAmmoChanged.Broadcast(this, CurrentAmmoInClip);
}

//bool AFPSWeapon::ServerFire_Validate()
//{
//	return true;
//}
//
//void AFPSWeapon::ServerFire_Implementation()
//{
//	Fire();
//}

//void AFPSWeapon::PlayFireEffects(FVector SmokeTrailEndPoint)
//{
//	if (MuzzleEffect)
//		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
//
//	if (SmokeEffect)
//	{
//		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
//		if (UParticleSystemComponent* SmokeTrail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeEffect, MuzzleLocation))
//		{
//			SmokeTrail->SetVectorParameter(SmokeEffectTargetName, SmokeTrailEndPoint);
//		}
//	}
//
//	if (APawn* MyOwner = Cast<APawn>(GetOwner()))
//	{
//		if (APlayerController* PlayerConroller = Cast<APlayerController>(MyOwner->GetController()))
//		{
//			PlayerConroller->ClientStartCameraShake(FireCameraShake);
//		}
//	}
//}
//
//void AFPSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
//{
//	UParticleSystem* SelectedEffect = nullptr;
//
//	switch (SurfaceType)
//	{
//	case SurfaceType1:
//		SelectedEffect = FleshImpactEffect;
//		break;
//
//	case SurfaceType2:
//		SelectedEffect = FleshImpactEffect;
//		break;
//
//	default:
//		SelectedEffect = DefaultImpactEffect;
//		break;
//	}
//
//
//	if (SelectedEffect)
//	{
//		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
//		FVector ShotDirection = ImpactPoint - MuzzleLocation;
//		ShotDirection.Normalize();
//
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
//	}
//}

//void AFPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME_CONDITION(AFPSWeapon, HitScanTrace, COND_SkipOwner);
//}
//
//void AFPSWeapon::OnRip_HitScanTrace()
//{
//	PlayFireEffects(HitScanTrace.TraceTo);
//	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
//}


