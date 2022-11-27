// Fill out your copyright notice in the Description page of Project Settings.


#include "SDushProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ASDushProjectile::ASDushProjectile()
{
	DetonateDelay = 0.2f;
	TeleportDelay = 0.2f;
	
	MovementComp->InitialSpeed = 6000.0f;
}


void ASDushProjectile::BeginPlay()
{
	Super::BeginPlay();

	//GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDushProjectile::Explode, DetonateDelay);

}

void ASDushProjectile::Explode_Implementation()
{
	// �������ײ���µı�ը������ȡ����ʱ��DetonateDelay����Ϊ�˱��ⶨʱ�������ٴδ�����ը
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	// �ͷű�ը��Ч��ʹ�þ�̬������������ϵͳUParticleSystemd(ImpactVFX)
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	// Actor ����ر���ײ
	SetActorEnableCollision(false);

	// �˶���� ֹͣ�˶�
	MovementComp->StopMovementImmediately();

	//����ϵͳ��� �رգ�������Ч�رգ�
	EffectComp->DeactivateSystem();

	// �ӳ�0.2�봫�����
	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDushProjectile::TeleportInstigator, TeleportDelay);

}

void ASDushProjectile::TeleportInstigator() 
{
	AActor* ActorToTeleport = GetInstigator();

	if (ensure(ActorToTeleport))
	{
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
		
	}

	Destroy();

}
