// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <SInteractionComponent.h>

DEFINE_LOG_CATEGORY_STATIC(LogASCharacter, Log, All);
// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SrpingArmComp");
	SpringArmComp->bUsePawnControlRotation = true;  // 弹簧臂(镜头)是否跟随PawnControlRotation。即镜头会不会跟着鼠标转动
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	 
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	
	// 下面两个设置时互斥的，一个为true另一个必须要false
	GetCharacterMovement()->bOrientRotationToMovement = true; // 设置CharacterMovement(Inherited)的 Orient Rotation to Movement 设置为 true，即旋转角色朝向跟随Movement（移动），使用键盘控制方向时，人物就可以转向（朝向改变）了，转动到相应的前后左右
	bUseControllerRotationYaw = false; // 角色朝向不跟随ControllerRotationYaw，即角色朝向不跟随鼠标转动(鼠标即控制着PlayerController)。表现为：镜头以人物为中心进行360°环绕，
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 轴映射，会一直触发调用，比如按键W按下会每帧调用MoveForward或移动鼠标X移动会每帧调用Turn，移动鼠标Y移动会每帧调用LookUp
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// 动作映射，只会触发一次，需要重复按键或按鼠标触发
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

}

void ASCharacter::MoveForward(float Value)
{
	// 向镜头的朝向移动，而不是向人物的朝向移动，需要获取Controller的Yaw偏航角向量
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), Value);


	// 下面是向人物的朝向移动
	//AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// Actor自身坐标系，左手坐标系
	// X = Forward (Red Arrow)
	// Y = Right (Green Arrow)
	// Z = Up (Blue Arrow)

	//UKismetMathLibrary::GetRightVector(FRotator InRot);
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::Jump()
{
	UE_LOG(LogASCharacter, Log, L"Jump");
	ACharacter::Jump();
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);


	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

	// GetWorldTimerManager()().ClearTimer(TimerHandle_PrimaryAttack);  // 当角色死亡时，应该取消定时器（取消延迟攻击）

}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	if (ensure(ProjectileClass))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		FTransform SpawnTM = FTransform(GetControlRotation()/*镜头朝向的方向*/, HandLocation/*手部位置*/);    //从手部位置，向镜头朝向的方向，发射子弹

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this; // 传入发起攻者本人，在蓝图中判断Projectile击中的是不是发起攻者本人（因为子弹从手部spawn，会立即碰撞到发起攻击者本人），从而忽略发起攻击者本人。

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::Dash()
{
	
}

