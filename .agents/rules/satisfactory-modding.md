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

## 5. Vehicle Build Gun Zero-Allocation & Pointer Hardening
- **`IsValid()` Pointer Hardening**:
  - Always check `IsValid(Actor)` and `IsValid(Hologram)` rather than raw null checks to guard against garbage-collected or destroying UObjects during high-speed construction or dismantling.
- **Zero-Allocation in `Tick` Loops**:
  - Never call `SetCrosshairState()`, `UnmapKey()`, or query/modify primitive collision responses unconditionally in `Tick()`. Calling HUD setters or delegate dispatchers on every frame causes millions of transient `UObject` allocations, triggering `Maximum number of UObjects exceeded (2162688)`.
  - Always guard with state-checks: `if (HUD->GetCrosshairState() != DesiredState) HUD->SetCrosshairState(DesiredState);`.
- **Hologram Rotation via D-Pad & Scroll**:
  - Forward `Gamepad_DPad_Left` / `Gamepad_DPad_Right` and `MouseScrollUp` / `MouseScrollDown` directly to `BuildState->Scroll(delta)` to enable rotation in vehicles.
- **`SetOverrideEquipment` / `ClearOverrideEquipment` Standard**:
  - Temporary hand equipment in vehicles MUST use `Driver->SetOverrideEquipment(BuildGun)` and `Driver->Server_SetOverrideEquipment(BuildGun)`.
  - On build mode cancel or vehicle exit, call `Driver->ClearOverrideEquipment(BuildGun)` and `Driver->Server_ClearOverrideEquipment(BuildGun)`.
- **B / Circle & LT Cancel**:
  - Map `Gamepad_FaceButton_Right` (B/Circle), `Escape`, and `Gamepad_LeftTrigger` (LT) to `BuildGun->GotoNoneState()` and `Driver->ClearOverrideEquipment(BuildGun)`.
- **Vehicle Clearance Suppression**:
  - During vehicle entry, set `ECC_GameTraceChannel10` (ClearanceDetector) and `ECC_GameTraceChannel4` (Clearance) to `ECR_Ignore` on vehicle primitive components once.

## 6. Resource Scanner & Radar Integration Architecture (Factory Sonar)
- **Adding Custom Scanner Targets to Radial Wheel**:
  - Create a subclass inheriting from `UFGResourceDescriptor` (e.g. `UFGResourceDescriptor_FactorySonar`).
  - Unlock dynamically in `AFGUnlockSubsystem`:
    ```cpp
    AFGUnlockSubsystem* UnlockSubsystem = AFGUnlockSubsystem::Get(World);
    FScannableResourcePair SonarPair(UFGResourceDescriptor_FactorySonar::StaticClass());
    UnlockSubsystem->UnlockScannableResource(SonarPair);
    ```
- **Zero-GC In-World AR Telemetry Rendering**:
  - Custom HUD billboarding should override `UUserWidget::NativePaint` and use direct Slate draw elements (`FSlateDrawElement::MakeBox`, `FSlateDrawElement::MakeText`) with projected screen space coordinates (`PC->ProjectWorldLocationToScreen`), eliminating transient UObject instantiation and maintaining 144+ FPS.
