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

		/* ����˵����
		*	����(BeginPlay)���ӳ�0.2������DetonateDelay
		* 	����ʱ����������ײ���µ�����������ASProjectileBaseʵ�ֵģ�����ʱӦȡ����ʱ��DetonateDelay�����ⶨʱ�������ٴ�����
		*	����ʱ������ײλ����������Ч����ȡ����ײ��ֹͣ�ƶ����
		*	����ʱ���ӳ�0.2�룬���Լ����͹�ȥImpact point
		*/ 

		virtual void Explode_Implementation() override;
		void TeleportInstigator();


		float DetonateDelay;
		float TeleportDelay;

		FTimerHandle TimerHandle_DelayedDetonate;

};
