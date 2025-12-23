// Learning Aura Project Only

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuarEffectActor.generated.h"

class USphereComponent;

UCLASS()
class MYRPG_API AAuarEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuarEffectActor();

	UFUNCTION()
	virtual void OnOverlep(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
