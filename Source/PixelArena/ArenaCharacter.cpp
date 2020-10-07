// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

static FDateTime InputReleaseTime = -1; // The time used to tell weather a key has been released

AArenaCharacter::AArenaCharacter()
{
    // Setup Movement Input Map
    MoveInputMap.Add(North, InputReleaseTime);
    MoveInputMap.Add(East, InputReleaseTime);
    MoveInputMap.Add(South, InputReleaseTime);
    MoveInputMap.Add(West, InputReleaseTime);

    GetSprite()->OnFinishedPlaying.AddDynamic(this, &AArenaCharacter::AnimationFinished);
}

void AArenaCharacter::Move()
{
    switch (MoveDirection)
    {
        case North:
            Velocity = FVector(0, 0, MoveSpeed);
            break;
        case East:
            Velocity = FVector(MoveSpeed, 0, 0);
            break;
        case South:
            Velocity = FVector(0, 0, -MoveSpeed);
            break;
        case West:
            Velocity = FVector(-MoveSpeed, 0, 0);
        default:
            break;
    }
}

void AArenaCharacter::ApplyVelocity(float speed, Direction direction)
{
}

void AArenaCharacter::SetVelocity(float speed, Direction direction)
{
    switch (direction)
    {
    case North:
        Velocity = FVector(0, 0, speed);
        break;
    case East:
        Velocity = FVector(speed, 0, 0);
        break;
    case South:
        Velocity = FVector(0, 0, -speed);
        break;
    case West:
        Velocity = FVector(-speed, 0, 0);
    default:
        break;
    }
}

void AArenaCharacter::PlayFlipbook(UPaperFlipbook* flipbook, bool loop)
{
    GetSprite()->SetLooping(loop);
    GetSprite()->Play();
    GetSprite()->SetFlipbook(flipbook);
}

void AArenaCharacter::FinishAttack()
{
    isAttacking = false;
    attackDownTime = -1;
}

void AArenaCharacter::FinishAbility()
{
    isAbility = false;
    abilityDownTime = -1;
}

void AArenaCharacter::UpdateMovementInput(Direction direction, bool keyDown)
{
    // Update map
    MoveInputMap[direction] = keyDown ? FDateTime::Now() : InputReleaseTime;

    if (MoveInputMap[North] > InputReleaseTime ||
        MoveInputMap[West] > InputReleaseTime ||
        MoveInputMap[South] > InputReleaseTime ||
        MoveInputMap[East] > InputReleaseTime)
    {
        isMoving = true;
    }
    else
    {
        isMoving = false;
    }
}

void AArenaCharacter::UpdateFacing()
{   
    // Find key that's down, that was pressed the most recent
    TPair<TEnumAsByte<Direction>, FDateTime> recent;
    for (const TPair<TEnumAsByte<Direction>, FDateTime>& pair : MoveInputMap)
    {
        if (pair.Value > recent.Value)
            recent = pair;
    }

    // Only update direction if we're actually moving
    if (isMoving)
    {
        // TODO: Replace with Facing, Separate direction no longer needed
        MoveDirection = recent.Key;
        Facing = recent.Key;
    }
}

void AArenaCharacter::UpdateAttackInput(bool active)
{
    attackKeyDown = active;

    if (!isAttacking && active)
    {
        isAttacking = true;
        attackDownTime = FDateTime::Now();
    }
}

void AArenaCharacter::UpdateAbilityInput(bool active)
{
    abilityKeyDown = active;
    
    if (active)
    {
        AbilityStart();
        isAbility = true;
        abilityDownTime = FDateTime::Now();
    }
}


void AArenaCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    switch (CharacterState)
    {
        case Idle:
            UpdateFacing();
            IdleState();
            if (IdleAnimations.Contains(Facing))
            {
                PlayFlipbook(IdleAnimations[Facing], true);
            }
            if (isMoving)
            {
                CharacterState = Walking;
                break;
            }
            if (isAttacking)
            {
                CharacterState = Attacking;
                break;
            }
            if (isAbility)
            {
                CharacterState = Ability;
                break;
            }
            break;
        case Walking:
            UpdateFacing();
            WalkingState();
            if (WalkingAnimations.Contains(Facing))
            {
                PlayFlipbook(WalkingAnimations[Facing], true);
            }
            if (isAttacking)
            {
                CharacterState = Attacking;
                break;
            }
            if (isAbility)
            {
                CharacterState = Ability;
                break;
            }
            if (!isMoving)
            {
                SetVelocity(0, Facing);
                CharacterState = Idle;
                break;
            }
            break;
        case Attacking:
            if (!isAttacking && isMoving)
            {
                CharacterState = Walking;
                break;
            }
            if (!isAttacking)
            {
                CharacterState = Idle;
                break;
            }
            
            AttackState((FDateTime::Now() - attackDownTime).GetTotalMilliseconds(), attackKeyDown);
            break;
        case Ability:
            if (!isAbility && isMoving)
            {
                AbilityEnd();
                CharacterState = Walking;
                break;
            }
            if (!isAbility)
            {
                AbilityEnd();
                CharacterState = Idle;
                break;
            }
            AbilityState((FDateTime::Now() - abilityDownTime).GetTotalMilliseconds(), abilityKeyDown);
            break;
        default:
            break;
    }
}

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Bind Movement Inputs
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("North", IE_Pressed, this, &AArenaCharacter::UpdateMovementInput, North, true);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("North", IE_Released, this, &AArenaCharacter::UpdateMovementInput, North, false);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("East", IE_Pressed, this, &AArenaCharacter::UpdateMovementInput, East, true);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("East", IE_Released, this, &AArenaCharacter::UpdateMovementInput, East, false);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("South", IE_Pressed, this, &AArenaCharacter::UpdateMovementInput, South, true);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("South", IE_Released, this, &AArenaCharacter::UpdateMovementInput, South, false);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("West", IE_Pressed, this, &AArenaCharacter::UpdateMovementInput, West, true);
    PlayerInputComponent->BindAction<UpdateMovementInputDelegate>("West", IE_Released, this, &AArenaCharacter::UpdateMovementInput, West, false);

    // Bind Attack Inputs
    PlayerInputComponent->BindAction<UpdateAttackInputDelegate>("Attack", IE_Pressed, this, &AArenaCharacter::UpdateAttackInput, true);
    PlayerInputComponent->BindAction<UpdateAttackInputDelegate>("Attack", IE_Released, this, &AArenaCharacter::UpdateAttackInput, false);

    // Bind Ability Inputs
    PlayerInputComponent->BindAction<UpdateAttackInputDelegate>("Ability", IE_Pressed, this, &AArenaCharacter::UpdateAbilityInput, true);
    PlayerInputComponent->BindAction<UpdateAttackInputDelegate>("Ability", IE_Released, this, &AArenaCharacter::UpdateAbilityInput, false);
}