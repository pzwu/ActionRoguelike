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
	// ���ñ�Actorʹ�ñ༭����Ԥ��õ���Ϊ"Projectile"����ײ����Ԥ�裬Projectile���Լ����ӵ�
	// ��������BlockAll,OverlapAll�ȣ��ڱ༭�����õ�Collision��Preset�����Ԥ��
	// �˴����ú�ֻ�Ǳ༭����Ĭ��ʹ��Projectile��ײԤ�裬�ڱ༭���л��ǿ��Ա༭�޸ĳ���������ײԤ��
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
