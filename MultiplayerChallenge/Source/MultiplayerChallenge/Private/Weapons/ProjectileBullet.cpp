#include "Weapons/ProjectileBullet.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (auto OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (auto OwnerController = OwnerCharacter->GetController())
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}

	//called at end as it will destroy the current object
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
