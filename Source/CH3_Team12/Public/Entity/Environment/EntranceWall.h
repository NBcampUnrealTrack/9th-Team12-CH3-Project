#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EntranceWall.generated.h"

class UMaterialInstanceDynamic;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossRoomEntered);

UCLASS()
class CH3_TEAM12_API AEntranceWall : public AActor
{
	GENERATED_BODY()
	
public:	
	AEntranceWall();

	UPROPERTY(BlueprintAssignable)
	FOnBossRoomEntered OnBossRoomEntered;
	
protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnEnterTriggerBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnExitTriggerBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void ActivateFogWall();
	
private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FogMesh;

	// 보스방 바깥쪽
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* EnterTrigger;

	// 보스방 안쪽
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ExitTrigger;

	UPROPERTY()
	UMaterialInstanceDynamic* FogMid;

	bool bPlayerEntered = false;

	bool bActivated = false;

public:

	UPROPERTY(EditAnywhere, Category="Fog")
	FName OpacityParameter = "Opacity";

	UPROPERTY(EditAnywhere, Category="Fog")
	float ActivatedOpacity = 0.95f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> BossBGM;
};
