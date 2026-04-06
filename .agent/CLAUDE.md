# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is **MyRPG**, an Unreal Engine 5.4 RPG project following the "Learning Aura" course. It uses the **Gameplay Abilities System (GAS)** as its core gameplay framework.

## Build Commands

This is an Unreal Engine project built with the Unreal Build Tool (UBT).

```bash
# Generate Visual Studio project files (run this after adding new source files)
# Right-click .uproject file → "Generate Visual Studio project files"

# Build the project from command line (requires Unreal Engine installation)
# Windows:
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" MyRPGEditor Win64 Development "E:\UEProject\5.4\MyRPG\MyRPG.uproject"

# Build via Visual Studio:
# Open MyRPG.sln, set configuration to "Development Editor" and platform to "Win64"
```

## Project Structure

### Source Code Organization (`Source/MyRPG/`)

```
3C/                          # Character, Controller, Camera
  Character/
    AuraCharacterBase.h/cpp  # Base character class implementing IAbilitySystemInterface
    AuraCharacter.h/cpp      # Player character with ability initialization
    AuraEnemy.h/cpp          # Enemy character
  Controller/
    Player/
      AuraPlayerController   # Handles Enhanced Input, cursor tracing for enemies
      AuraPlayerState        # Owns ASC and AttributeSet for player

AbilitySystem/
  AuraAbilitySystemComponent # Custom ASC (currently minimal)
  AuraAttributeSet           # Health, MaxHealth, Mana, MaxMana attributes with replication

Actor/
  AuraEffectActor            # Base class for gameplay effect actors (potions, etc.)

UI/
  HUD/AuraHUD                # Creates overlay widget and controller
  Widget/AuraUserWidget      # Base widget class with controller getter
  WidgetController/
    AuraWidgetController     # Base controller with FWidgetControllerParams
    OverlayWidgetController  # Binds to attribute changes, broadcasts to UI

Interaction/
  EnemyInterface             # Interface for enemy highlighting on hover
```

### Content Organization (`Content/`)

```
BlueprintScripts/            # Blueprint assets mirroring C++ structure
  3C/Character, Controller
  Actor/EffectActors/        # BP_HealthPotion, etc.
  UI/HUD, Overlay, ProgressBar, WidgetController
Maps/
  StartMap.umap              # Main development map
```

## Architecture Patterns

### Gameplay Abilities System (GAS) Flow

The project follows standard GAS initialization patterns:

**For Player Characters:**
- `AuraPlayerState` owns the `AbilitySystemComponent` and `AttributeSet`
- `AuraCharacter::PossessedBy()` (server) and `OnRep_PlayerState()` (client) call `InitAbilityActorInfo()`
- This binds the character to the ASC owned by PlayerState

**For UI:**
- `AuraHUD::InitOverlay()` creates the overlay widget and controller
- `FWidgetControllerParams` struct passes PC, PS, ASC, AS to the controller
- `OverlayWidgetController::BroadcastInitialValues()` sends initial values to UI
- `OverlayWidgetController::BindCallbacksToDependencies()` binds to attribute change delegates

**For Gameplay Effects:**
- Use `AuraEffectActor::ApplyEffectToTarget()` as the standard pattern:
  ```cpp
  FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
  Context.AddSourceObject(this);
  FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, Level, Context);
  ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
  ```

### Attribute Access Pattern

`AuraAttributeSet` uses the `ATTRIBUTE_ACCESSORS` macro to generate getter/setter/initter:

```cpp
ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health)
// Generates: GetHealth(), SetHealth(), InitHealth()
```

### UI Delegates

`OverlayWidgetController` uses Blueprint-assignable dynamic delegates:

```cpp
UPROPERTY(BlueprintAssignable)
FOnHealthChangedSignature OnHealthChanged;
```

## Key Dependencies

From `MyRPG.Build.cs` and `.uproject`:

- `GameplayAbilities` - Core GAS plugin
- `GameplayTags`, `GameplayTasks` - GAS dependencies
- `EnhancedInput` - Modern input system
- `UMG` - UI system

## Development Notes

- **Always use `MakeEffectContext()` pattern** - Never use `const_cast` to modify attributes directly; always go through GameplayEffects
- **Blueprint/C++ split** - Visual components (mesh, collision) often configured in Blueprints; logic in C++
- **Replication** - All vital attributes (`Health`, `Mana`, etc.) are replicated with `ReplicatedUsing` callbacks
- **Include paths** - Module uses `PublicIncludePaths.Add("MyRPG")` for simplified includes

## Cursor Rules / IDE Settings

- `.vsconfig` specifies required VS2022 components for UE5.4 development
- `.idea/` folder contains JetBrains Rider project settings
