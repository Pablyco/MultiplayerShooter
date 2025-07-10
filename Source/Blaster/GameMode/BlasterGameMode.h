// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and	begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedCharacter,
	                              class ABlasterPlayerController* VictimController,
	                              ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedCharacter, AController* EliminatedController);

	//The time before to fight/start the match
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	//The time of the match.
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	//The time after the match, usually used for showing the winner of the match.
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	// Stores the world time (in seconds) at which the level began playing.
	// Used as a reference point to calculate match phase countdowns.
	float LevelStartingTime = 0.f;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	
private:
	float CountdownTime = 0.f;

public:
	
	//Countdown Time getter for "handle HUD's things in Blaster Player Controller"
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	
};
