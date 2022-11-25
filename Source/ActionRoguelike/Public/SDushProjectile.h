// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDushProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDushProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	

	public:
		ASDushProjectile();

	protected:
		void BeginPlay();

		/* 功能说明：
		*	发射(BeginPlay)后，延迟0.2秒引爆DetonateDelay
		* 	引爆时，可能是碰撞导致的引爆（父类ASProjectileBase实现的），此时应取消定时器DetonateDelay，避免定时器到了再次引爆
		*	引爆时，在碰撞位置生成引爆效果，取消碰撞，停止移动组件
		*	引爆时，延迟0.2秒，把自己传送过去Impact point
		*/ 

		virtual void Explode_Implementation() override;
		void TeleportInstigator();


		float DetonateDelay;
		float TeleportDelay;

		FTimerHandle TimerHandle_DelayedDetonate;

};
