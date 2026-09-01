# 🏭 Satisfactory 1.0+ Modding Architecture & Guardrails
# Reference file for Satisfactory Modding (SML v3.8+ / UE 5.3+)

## 1. UI & Menu Hooking Architecture
- **Never Rely on Runtime Subsystem Widget Tree Injection for Main Menus**:
  - Satisfactory 1.0 bakes main menu layouts from Blueprint archetypes prior to subsystem startup.
  - **The Standard**: Always create a C++ class inheriting from `UGameInstanceModule` with `bRootModule = true;` and populate `WidgetBlueprintHooks`.
  - **Main Menu Target**: `WidgetClass = "/Game/FactoryGame/Interface/UI/Menu/MainMenu/BP_MainMenuWidget.BP_MainMenuWidget_C"`.
  - **Menu List Container**: `ParentWidgetName = "mMainMenuList"` (type `UVerticalBox*`).
  - **Authentic FrontEnd Buttons**: Always instantiate `/Game/FactoryGame/Interface/UI/Menu/Widget_FrontEnd_Button.Widget_FrontEnd_Button_C` and call `SetTitle()` & `SetIsBigButton(true)` for native Satisfactory typography, hover glow, shaders, and sound effects.

## 2. SML Internal Bug Guards
- **`UWidgetBlueprintHookSlot_Generic` Typo Protection**:
  - SML contains an internal bug in `UWidgetBlueprintHookSlot_Generic::SetupPanelSlot()` where `HorizontalBoxSlot->SetSize()` is invoked inside the `UVerticalBoxSlot` branch.
  - **The Guard**: Always set `HookData->SlotConfiguration = nullptr;` when hooking into `UVerticalBox` containers like `mMainMenuList` to prevent null pointer access violations during SML startup.

## 3. Slate Geometry & Rebuild Lifecycle
- **Slate Evaluation Order**:
  - In Unreal Engine UMG, Slate computes layout and geometry passes during `RebuildWidget()`, *before* `NativeConstruct()` is called.
  - **The Guard**: All dynamic child widget hierarchies (e.g. `WidgetTree->RootWidget`, child boxes, buttons) must be constructed inside an overridden `RebuildWidget()` method before calling `Super::RebuildWidget()`. Never defer root widget assignment to `NativeConstruct()`.

## 4. Multiplayer Session Migration & Direct IP
- **Never Use Raw `open <IP>` or Direct `APlayerController::ClientTravel`**:
  - In Satisfactory 1.0 through 1.2+, Coffee Stain Studios unified multiplayer under the `OnlineIntegration` module (`UCommonSessionSubsystem`).
  - Raw console commands or direct travels bypass the client's `UOnlineIntegrationBackend`, causing a hard crash at offset `0x668` when player actors tick on the client.
  - **The Standard**: Always invoke Satisfactory's official session migration sequence:
    ```cpp
    #include "CommonSessionSubsystem.h"
    #include "SessionCreationSettings.h"
    #include "SessionMigrationSequence.h"

    UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
    FSessionJoinParams JoinParams;
    JoinParams.Player = PlayerController;
    JoinParams.RawAddress = ServerIP; // e.g. "192.168.1.89"

    USessionMigrationSequence* JoinSequence = SessionSubsystem->CreateSessionJoiningSequence(JoinParams);
    if (JoinSequence) {
        JoinSequence->Start();
    }
    ```
  - **Dependency Requirement**: Add `"OnlineIntegration"` to `PublicDependencyModuleNames` in the mod's `.Build.cs`.

## 5. Automated Mod Packaging & Trigger Mapping
- **AutomationTool Command-Line Packaging**:
  - Package plugins directly using `RunUAT.bat` with `-platform=Win64 -server -serverplatform=Win64`:
    ```powershell
    & "K:\SatisfactoryModding\Engine\Engine\Build\BatchFiles\RunUAT.bat" -ScriptsForProject="$project" PackagePlugin -project="$project" -clientconfig=Shipping -serverconfig=Shipping -utf8output -DLCName=VehicleBuildMode -build -platform=Win64 -server -serverplatform=Win64 -nocompileeditor -installed
    ```
- **Unreal Gamepad Trigger Key Constants**:
  - Use `EKeys::Gamepad_RightTrigger` and `EKeys::Gamepad_LeftTrigger` in UE 5.3 C++.
