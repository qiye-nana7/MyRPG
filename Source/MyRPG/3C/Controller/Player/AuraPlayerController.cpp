// Learning Aura Project Only

#include "AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraInputContext);

	// Add Input Mapping Context
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraInputContext, 0);

	// Setup Cursor and Input Mode
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);// 注意不要覆盖错Tick()

	CursorTrace();
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!HitResult.bBlockingHit) return;
	
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(HitResult.GetActor());
	/*
	 * Line trace from cursor. There are several sceneries:
	 * 1. ThisActor is nullptr and LastActor is nullptr.
	 *		- Do nothing.
	 * 2. ThisActor is nullptr and LastActor is not nullptr.
	 *		- UnHighlight LastActor.
	 * 3. ThisActor is not nullptr and LastActor is nullptr.
	 *		- Highlight ThisActor.
	 * 4. ThisActor is not nullptr and LastActor is not nullptr and ThisActor != LastActor.
	 *		- UnHighlight LastActor, Highlight ThisActor.
	 * 5. ThisActor is not nullptr and LastActor is not nullptr and ThisActor == LastActor.
	 *		- Do nothing.
	 */
	if (ThisActor != nullptr && LastActor == nullptr)
	{
		ThisActor->HighlightActor();
	} else if (ThisActor == nullptr && LastActor != nullptr)
	{
		LastActor->UnHighlightActor();
	} else if (ThisActor != nullptr && LastActor != nullptr && ThisActor != LastActor)
	{
		ThisActor->HighlightActor();
		LastActor->UnHighlightActor();
	}
}