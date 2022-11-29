// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UFPSHealthComponent::UFPSHealthComponent()
{
	MaxHealth = 100;
}

// Called when the game starts
void UFPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetHealthComp();
}

void UFPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
		return;

	UE_LOG(LogTemp, Log, TEXT("Current Health %f"), Health);

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UFPSHealthComponent::AddHealth(float Amount)
{
	Health += Amount;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}

void UFPSHealthComponent::ResetHealthComp()
{
	if (AActor* MyOwner = GetOwner())
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UFPSHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = MaxHealth;
}

void UFPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFPSHealthComponent, Health);
}


