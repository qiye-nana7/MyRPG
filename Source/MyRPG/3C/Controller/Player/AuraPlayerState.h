// Learning Aura Project Only

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

/**
 * 
 */
UCLASS()
class MYRPG_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAuraAttributeSet* GetAttributeSet() const{ return AttributeSet; };

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;
};
