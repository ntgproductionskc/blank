Superman & Lois — Audio Architecture
======================================

ENGINE: Unreal Engine 5 MetaSounds + Resonance Audio (3D spatial)

MUSIC
──────
MUS_MainTheme              — Orchestral John Williams-inspired Superman theme (strings + brass)
MUS_Metropolis_Day         — Ambient city life, gentle orchestral
MUS_Metropolis_Night       — Jazz-influenced, noir Metropolis
MUS_Smallville_Peaceful    — Rural Americana, fiddle + acoustic guitar
MUS_Smallville_Danger      — Tense strings when threat enters Smallville
MUS_Combat_Standard        — Percussive action cue, builds with combo count
MUS_Combat_Boss_Bizarro    — Discordant, inverted motifs (mirrors Superman's theme)
MUS_Combat_Boss_Doomsday   — Deep bass ostinato, relentless percussion
MUS_Combat_Boss_Inverse    — Dark orchestral counterpart to main theme
MUS_Flight_Normal          — Sweeping strings, sense of freedom
MUS_Flight_Boost           — Driving brass + percussion for sonic boom speed
MUS_Emergency_High         — Urgent brass stabs, full ensemble
MUS_Emergency_Critical     — Full emergency orchestra, alarm motifs
MUS_MissionSuccess         — Heroic resolution cadence
MUS_MissionFail            — Somber brass fall
MUS_InverseWorld           — Reversed/glitched main theme, eerie atmosphere
MUS_Cutscene_*             — Per-cutscene adaptive music

SFX — SUPERMAN POWERS
──────────────────────
SFX_HeatVision_Start       — Rising electrical crackle
SFX_HeatVision_Loop        — Sustained beam (MetaSound synth)
SFX_HeatVision_Hit         — Sizzle + impact per surface type
SFX_FreezeBreath_Start     — Ice crystallization build
SFX_FreezeBreath_Loop      — Sustained cryo-wind
SFX_FreezeBreath_Freeze    — Crystalline shatter on target freeze
SFX_FlyStart               — Whoosh + cape snap
SFX_FlyLoop                — Wind rush (scales with speed)
SFX_FlyBoost               — Sonic boom precursor warning
SFX_SonicBoom              — Concussive boom (positional, felt)
SFX_Land_Light             — Soft impact, boot on concrete
SFX_Land_Heavy             — Shockwave rumble + ground crack
SFX_SuperPunch_Light       — Whoosh + meaty impact
SFX_SuperPunch_Heavy       — Deep concussive boom
SFX_Grab                   — Grip lock SFX
SFX_Throw                  — Projectile whoosh
SFX_XRayVision_Toggle      — Subtle frequency sweep
SFX_SuperHearing_Toggle    — World goes muffled → hyper-sensitive

SFX — EMERGENCIES
──────────────────
SFX_FireAlarm_Building     — Building alarm loop
SFX_Siren_Police           — Police unit siren (Doppler enabled)
SFX_Siren_Ambulance        — AMB siren with frequency variation
SFX_Siren_FireTruck        — Fire engine air horn
SFX_Radio_Crackle          — Static squelch
SFX_Radio_Dispatch_*       — Various dispatch lines (VO)
SFX_Explosion_Small        — Car / gas explosion
SFX_Explosion_Large        — Building / industrial
SFX_GlassBreak_*           — Multiple glass types (window, storefront)
SFX_ConcreteCrack          — Structural damage
SFX_MetalCreak             — Bridge / structure stress

SFX — VILLAINS
──────────────
SFX_Bizarro_FreezeVision   — Ice crackling beam
SFX_Bizarro_FlameBreath    — Roaring flame cone
SFX_Bizarro_Roar           — Rage vocalization
SFX_Doomsday_Stomp         — Seismic footsteps
SFX_Doomsday_Slam          — Ground pound shockwave
SFX_Doomsday_Charge        — Rumbling ground crack during charge
SFX_MorganEdge_Blast       — Kryptonian energy discharge
SFX_InverseSuperman_*      — Inverted/reversed Superman power sounds

AMBIENT
────────
AMB_Metropolis_Downtown    — Traffic, crowd, pigeons, HVAC
AMB_Metropolis_Night       — Distant sirens, jazz bars, rain
AMB_Smallville_Farm        — Crickets, wind through wheat, barn animals
AMB_Smallville_Storm       — Thunder, rain on tin roof
AMB_InverseWorld           — Reversed city sounds, eerie static

VOICE OVER LINES
────────────────
VO_Police_Superman_*       — Officers greeting/commenting to Superman (10+ lines)
VO_Police_Radio_*          — Dispatch radio chatter (20+ lines)
VO_Civilian_Panic_*        — Civilians fleeing (15+ lines)
VO_Civilian_Awe_*          — "It's Superman!" reaction lines (15+ lines)
VO_Lois_Danger_*           — Lois calling for help (10+ lines)
VO_Lois_Rescued_*          — Lois thank-you lines (8+ lines)
VO_Jordan_Training_*       — Jordan during training missions (10+ lines)
VO_Superman_*              — Clark contextual remarks (optional)

METASOUND PATCHES
─────────────────
MS_FlightEngine            — Adaptive: speed → pitch/volume of wind
MS_CombatImpact            — Procedural impact layering (material + force)
MS_FireLoop                — Turbulent crackling fire (randomized layers)
MS_RadioFilter             — Band-pass + distortion for radio VO
MS_KryptoniteHum           — Dissonant oscillator for Kryptonite proximity
MS_SonicBoom               — Layered explosion + concussive wave
MS_CrowdSystem             — Distance-aware crowd simulation
