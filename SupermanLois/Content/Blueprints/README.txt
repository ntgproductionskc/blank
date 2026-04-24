Superman & Lois — Blueprint Hierarchy
=====================================

The following Blueprint classes are expected in the Content Browser
(parent C++ class shown in parentheses):

CORE
────
BP_SupermanLoisGameMode         (ASupermanLoisGameMode)
BP_SupermanLoisGameState        (ASupermanLoisGameState)
BP_SupermanLoisPlayerController (ASupermanLoisPlayerController)

CHARACTER
─────────
BP_Superman                     (ASupermanCharacter)
  └── Mesh: SK_Superman (MetaHuman-quality rig)
  └── Anim: ABP_Superman (Control Rig + Full Body IK)
  └── Cape: SK_Cape with cloth simulation enabled

ENEMIES
───────
BP_Bizarro                      (ABizarroCharacter)
BP_Doomsday                     (ADoomsdayCharacter)
BP_MorganEdge                   (AMorganEdgeCharacter)
BP_InverseSuperman              (AInverseSupermanCharacter)
BP_MetaHumanEnemy               (ABaseVillain)
BP_XKryptoniteSoldier           (ABaseVillain)

EMERGENCY RESPONDERS
────────────────────
BP_PoliceOfficer                (APoliceOfficerAI)
  └── Variants: Sergeant, Swat, Detective
BP_Firefighter                  (AFirefighterAI)
  └── Variants: FF_Hose, FF_Rescue, FF_Captain
BP_Paramedic                    (AParamedicAI)
  └── Variants: EMT, Paramedic, Doctor

COMPANIONS
──────────
BP_LoisLane                     (ALoisLaneCharacter)
BP_JordanKent                   (ACharacter — inherit base villain powers)
BP_JonathanKent                 (ACharacter)

CIVILIANS
─────────
BP_Civilian_M                   (ACivilianCharacter) — male variants
BP_Civilian_F                   (ACivilianCharacter) — female variants
BP_Civilian_Business            (ACivilianCharacter)
BP_Civilian_Tourist             (ACivilianCharacter)

VEHICLES
────────
BP_PoliceCar                    (AChaosWheeledVehiclePawn)
BP_Ambulance                    (AChaosWheeledVehiclePawn)
BP_FireTruck                    (AChaosWheeledVehiclePawn)
BP_Helicopter_Police            (APawn)
BP_CivilianCar_*                (AChaosWheeledVehiclePawn)

UI
──
WBP_SupermanHUD                 (USupermanHUD)
WBP_EmergencyAlert              (UEmergencyAlertWidget)
WBP_MissionTracker              (UMissionWidget)
WBP_WorldMap                    (UUserWidget)
WBP_PauseMenu                   (UUserWidget)
WBP_MissionJournal              (UUserWidget)

INPUT
─────
IMC_Default     — ground movement + UI
IMC_Flight      — flight controls
IMC_Combat      — combat actions + powers
IA_* assets for each input action defined in SupermanLoisPlayerController.h

POST PROCESS
────────────
PP_Metropolis_Day       — warm, saturated, Lumen GI
PP_Metropolis_Night     — neon reflections, god rays
PP_XRayVision           — blue skeletal overlay, walls transparent
PP_SuperHearing         — desaturated + sound wave pulse
PP_KryptoniteExposure   — green vignette, chromatic abberation increase
PP_SonicBoom            — radial blur, lens distortion burst
PP_InverseWorld         — red/purple color grade, inverted sky

LEVELS
──────
L_Metropolis_Main       — open world, World Partition enabled
  ├── L_Metropolis_Downtown
  ├── L_Metropolis_Waterfront
  ├── L_Metropolis_Industrial
  ├── L_Metropolis_Suburbs
  └── L_DailyPlanet_Interior
L_Smallville_Main       — Kent farm + town, seamlessly connected
  ├── L_KentFarm
  ├── L_SmallvilleHigh
  └── L_SmallvilleDowntown
L_InverseWorld          — mirror Metropolis with inverted palette
L_Story_*               — scripted mission levels (streamed in)
