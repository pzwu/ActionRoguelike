// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	// 设置本Actor使用编辑器里预设好的名为"Projectile"的碰撞参数预设，Projectile是自己增加的
	// 其他比如BlockAll,OverlapAll等，在编辑器设置的Collision的Preset中添加预设
	// 此处设置后，只是编辑器里默认使用Projectile碰撞预设，在编辑器中还是可以编辑修改成其他的碰撞预设
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(RootComponent);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0;
	MovementComp->InitialSpeed = 8000;

}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ASProjectileBase::Explode_Implementation()
{
	if (ensure(!IsPendingKill()))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		Destroy();
	}
}

/*
void ASProjectileBase::BeginPlay()
{
	APawn * ProjectileInstigator = GetInstigator();
	if (ensure(ProjectileInstigator))
	{
		SphereComp->IgnoreActorWhenMoving(ProjectileInstigator, true);
	}
}
*/

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
}
