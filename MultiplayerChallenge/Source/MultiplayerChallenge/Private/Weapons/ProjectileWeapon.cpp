// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"

#include "Weapons/Projectile.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	//Need to spawn only on server not on client
	if (!HasAuthority())return;

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform MuzzleTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		if (ProjectileClass)
		{
			FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
			FRotator TargetRotation = ToTarget.Rotation();

			FActorSpawnParameters Params;
			Params.Owner = GetOwner();//Setting the character which shoot as the owner of the bullet
			Params.Instigator = Cast<APawn>(GetOwner());

			GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform.GetLocation(), TargetRotation, Params);

		}

	}
}
