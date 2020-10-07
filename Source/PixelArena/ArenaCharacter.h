// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <map>

#include "PaperCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"

#include "ArenaCharacter.generated.h"

UENUM(BlueprintType)
enum CharacterState
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Attacking UMETA(DisplayName = "Attacking"),
	Ability UMETA(DisplayName = "Ability")
};


UENUM(BlueprintType)
enum Direction {
	North UMETA(DisplayName = "North"),
    East UMETA(DisplayName = "East"),
    South UMETA(DisplayName = "South"),
    West UMETA(DisplayName = "West")
};

UCLASS()
class PIXELARENA_API AArenaCharacter : public APaperCharacter
{
	GENERATED_BODY()
	
	public:
		AArenaCharacter();

		// Properties
		UPROPERTY(EditAnywhere, Category="Arena Character")
		float MoveSpeed = 400;
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State", BlueprintReadOnly)
		TEnumAsByte<CharacterState> CharacterState = Idle;
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State", BlueprintReadOnly)
		TEnumAsByte<Direction> Facing = South;
		UPROPERTY(EditAnywhere, Category="Arena Character|Animations")
		TMap<TEnumAsByte<Direction>, UPaperFlipbook*> IdleAnimations;
		UPROPERTY(EditAnywhere, Category="Arena Character|Animations")
		TMap<TEnumAsByte<Direction>, UPaperFlipbook*> WalkingAnimations;

		// Functions
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void Move();
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void ApplyVelocity(float speed, Direction direction);
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void SetVelocity(float speed, Direction direction);
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void PlayFlipbook(UPaperFlipbook* flipbook, bool loop = false);
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void FinishAttack();
		UFUNCTION(BlueprintCallable, Category="Arena Character")
		void FinishAbility();

		// Events
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void IdleState();
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
	    void WalkingState();
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void AttackState(float AttackTime, bool AttackDown);
	
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void AbilityStart();
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void AbilityState(float AbilityTime, bool AbilityDown);
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void AbilityEnd();
	
		UFUNCTION(BlueprintImplementableEvent, Category="Arena Character")
		void AnimationFinished();

	protected:
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State")
		bool isMoving = false;
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State")
		bool isAttacking = false;
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State")
		bool isAbility = false;

		Direction MoveDirection = South;

		FDateTime attackDownTime = -1;
		bool attackKeyDown = 0;

		FDateTime abilityDownTime = -1;
		bool abilityKeyDown = 0;
		
		UPROPERTY(VisibleAnywhere, Category="Arena Character|State")
		TMap<TEnumAsByte<Direction>, FDateTime> MoveInputMap;
	
		FVector& Velocity = GetCharacterMovement()->Velocity;

		void UpdateMovementInput(Direction direction, bool keyDown);
		DECLARE_DELEGATE_TwoParams(UpdateMovementInputDelegate, Direction, bool);
		void UpdateFacing();

		void UpdateAttackInput(bool active);
		DECLARE_DELEGATE_OneParam(UpdateAttackInputDelegate, bool);

		void UpdateAbilityInput(bool active);
		DECLARE_DELEGATE_OneParam(UpdateAbilityInputDelegate, bool);

	protected:
		// Called every frame
		virtual void Tick( float DeltaSeconds ) override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};