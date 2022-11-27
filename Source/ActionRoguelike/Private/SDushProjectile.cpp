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
	// 如果是碰撞导致的爆炸，这里取消定时器DetonateDelay，是为了避免定时器到了再次触发爆炸
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	// 释放爆炸特效，使用静态函数传入粒子系统UParticleSystemd(ImpactVFX)
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	// Actor 自身关闭碰撞
	SetActorEnableCollision(false);

	// 运动组件 停止运动
	MovementComp->StopMovementImmediately();

	//粒子系统组件 关闭（粒子特效关闭）
	EffectComp->DeactivateSystem();

	// 延迟0.2秒传送玩家
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
