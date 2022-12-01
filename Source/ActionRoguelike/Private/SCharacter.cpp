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
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
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
	if (ensure(MagicProjectileClass))
	{
		SpawnProjectile(MagicProjectileClass);
	}
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_BlackHole, this, &ASCharacter::BlackHoleAttack_TimeElapsed, 0.2f);
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_TimeElapsed, 0.2f);
}

void ASCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	// 使用球体进行扫描路径上的碰撞查询
	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	// 碰撞查询时忽略玩家
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 碰撞查询的ObjectType
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	// 射线起点(镜头的位置)
	FVector TraceStart = CameraComp->GetComponentLocation();

	// 根据起点和视线方向，确定射线终点（也即Crosshair所在的位置）     (视线的远端，如果miss了，仍会稍微朝crosshair调整)
	// 向量相减：TraceEnd - TraceStart = 从start朝向End的向量，即ControlRotation.Vector()，就是视线（摄像机）看过去的方向，5000单位是一个arbitrary(随意的)距离，Unreal中单位是cm，5000cm，即50米
	FVector TraceEnd = TraceStart + (GetControlRotation().Vector() * 5000);

	// 从起点到终点的碰撞查询
	FHitResult Hit;
	if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
	{
		// 如果有碰撞，将碰撞点作为结束点，如果无碰撞，则还是原视线的终点TraceEnd作为结束点
		TraceEnd = Hit.ImpactPoint;
	}
	
	//从手到TraceEnd的Rotation
	// 确定子弹的Transform(子弹的Rotation 和 Location，Location为手部位置，Rotation为手部指向crosshair的方向)

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FRotator ProjRotatation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
	// FTransform SpawnTM = FTransform(GetControlRotation()/*镜头朝向的方向*/, HandLocation/*手部位置*/);    //从手部位置，向镜头朝向的方向，发射子弹，则子弹与Crosshair有偏差，因为Crosshair（TraceEnd），是从镜头位置，向镜头朝向的方向计算出来的
	FTransform SpawnTM = FTransform(ProjRotatation, HandLocation/*手部位置*/);    //从手部位置，向ProjRotatation方向，即手部朝向Crosshair（TraceEnd）的方向，发射子弹，目标就是Crosshair（TraceEnd）

	// 生成子弹，携带FActorSpawnParameters参数，指定碰撞时是否生成或调整为位置，指定Instigator等。
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this; // 传入发起攻者本人，在蓝图中判断Projectile击中的是不是发起攻者本人（因为子弹从手部spawn，会立即碰撞到发起攻击者本人），从而忽略发起攻击者本人。

	GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
}


