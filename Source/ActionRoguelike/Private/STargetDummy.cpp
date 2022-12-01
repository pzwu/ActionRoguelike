// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"



ASTargetDummy::ASTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

}


