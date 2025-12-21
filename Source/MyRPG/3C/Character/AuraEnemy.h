// Learning Aura Project Only

#pragma once

#include "CoreMinimal.h"
#include "3C/Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
	AAuraEnemy();
public:
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
};
