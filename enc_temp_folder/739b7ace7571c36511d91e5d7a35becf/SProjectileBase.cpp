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
	//SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	//SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	//SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// ���ñ�Actorʹ�ñ༭����Ԥ��õ���Ϊ"Projectile"����ײ����Ԥ�裬Projectile���Լ����ӵ�
	// ��������BlockAll,OverlapAll�ȣ��ڱ༭�����õ�Collision��Preset�����Ԥ��
	// �˴����ú�ֻ�Ǳ༭����Ĭ��ʹ��Projectile��ײԤ�裬�ڱ༭���л��ǿ��Ա༭�޸ĳ���������ײԤ��
	SphereComp->SetCollisionProfileName("Projectile");

	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

}

void ASProjectileBase::BeginPlay()
{
	APawn * ProjectileInstigator = GetInstigator();
	if (ensure(ProjectileInstigator))
	{
		SphereComp->IgnoreActorWhenMoving(ProjectileInstigator, true);
	}
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
}

void ASProjectileBase::Explode_Implementation()
{
	if (ensure(!IsPendingKill()))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		Destroy();
	}
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}
