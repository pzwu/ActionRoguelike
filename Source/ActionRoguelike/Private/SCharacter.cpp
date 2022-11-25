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
	SpringArmComp->bUsePawnControlRotation = true;  // ���ɱ�(��ͷ)�Ƿ����PawnControlRotation������ͷ�᲻��������ת��
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	 
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	
	// ������������ʱ����ģ�һ��Ϊtrue��һ������Ҫfalse
	GetCharacterMovement()->bOrientRotationToMovement = true; // ����CharacterMovement(Inherited)�� Orient Rotation to Movement ����Ϊ true������ת��ɫ�������Movement���ƶ�����ʹ�ü��̿��Ʒ���ʱ������Ϳ���ת�򣨳���ı䣩�ˣ�ת������Ӧ��ǰ������
	bUseControllerRotationYaw = false; // ��ɫ���򲻸���ControllerRotationYaw������ɫ���򲻸������ת��(��꼴������PlayerController)������Ϊ����ͷ������Ϊ���Ľ���360�㻷�ƣ�
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

	// ��ӳ�䣬��һֱ�������ã����簴��W���»�ÿ֡����MoveForward���ƶ����X�ƶ���ÿ֡����Turn���ƶ����Y�ƶ���ÿ֡����LookUp
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// ����ӳ�䣬ֻ�ᴥ��һ�Σ���Ҫ�ظ���������괥��
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

}

void ASCharacter::MoveForward(float Value)
{
	// ��ͷ�ĳ����ƶ���������������ĳ����ƶ�����Ҫ��ȡController��Yawƫ��������
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), Value);


	// ������������ĳ����ƶ�
	//AddMovementInput(GetActorForwardVector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// Actor��������ϵ����������ϵ
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

	// GetWorldTimerManager()().ClearTimer(TimerHandle_PrimaryAttack);  // ����ɫ����ʱ��Ӧ��ȡ����ʱ����ȡ���ӳٹ�����

}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	if (ensure(ProjectileClass))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		FTransform SpawnTM = FTransform(GetControlRotation()/*��ͷ����ķ���*/, HandLocation/*�ֲ�λ��*/);    //���ֲ�λ�ã���ͷ����ķ��򣬷����ӵ�

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this; // ���뷢���߱��ˣ�����ͼ���ж�Projectile���е��ǲ��Ƿ����߱��ˣ���Ϊ�ӵ����ֲ�spawn����������ײ�����𹥻��߱��ˣ����Ӷ����Է��𹥻��߱��ˡ�

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

