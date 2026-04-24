Superman & Lois — Material Architecture
=========================================

All materials use Nanite-compatible workflows.
Physically-Based Rendering (PBR) throughout.
Lumen-ready: emissives properly set for GI contribution.

MASTER MATERIALS
────────────────
M_Master_Character
  Inputs: BaseColor, Normal, Metallic, Roughness, Emissive, SubsurfaceColor
  Features: SSS for skin, cloth sheen for fabric, Nanite tessellation support

M_Master_Building
  Inputs: Albedo, Normal, AO, Roughness, Metallic, Emissive, WindOffset (procedural)
  Features: Nanite, triplanar mapping, weathering mask, rain wetness (driven by WeatherSystem)

M_Master_Road
  Features: Nanite displacement, rain puddle reflection mask, tire mark decals,
            blood/oil/debris decal layers, emissive lane markers at night

M_Master_Vegetation
  Features: Nanite tessellation, wind animation, subsurface translucency, PCG instancing

M_Master_Glass
  Features: Raytraced refraction, Lumen screen-space reflections,
            breakable via GeometryCollection (switches to M_Broken_Glass)

M_Master_Sky
  Used with UE5 SkyAtmosphere + VolumetricClouds for full atmospheric scattering

CHARACTER MATERIALS
────────────────────
MI_Superman_Suit
  - Deep blue base, metallic S-shield, emissive cape edge at boost speed
  - Driven by gameplay: KryptoniteExposure parameter greens the material slightly

MI_Superman_Cape
  - Cloth simulation material, dual-sided
  - Speed parameter drives red saturation boost at sonic speed

MI_Bizarro_Suit
  - Inverted color palette of MI_Superman_Suit
  - Cracked-face normal map, frost overlay parameter

MI_Doomsday_Skin
  - Bone-spike geometry, dark grey-green base
  - Pulsating red emissive veins driven by health (more emissive at low HP)

MI_LoisLane_*
  - MetaHuman-quality skin: Alembic hair, SSS skin, detailed eye shaders

POST PROCESS MATERIALS
───────────────────────
PP_XRayVision
  - Full-scene X-ray: renders character skeletons through walls
  - Blue-tinted, pulsing edge detection, depth-blurred background

PP_SuperHearing
  - Desaturates world to 20% saturation
  - Overlays audio-reactive waveform circles around sound sources
  - Distance rings for hearing radius visualization

PP_KryptoniteProximity
  - Green vignette, chromatic aberration increase, slight screen shake
  - Intensity driven by Superman's KryptoniteExposure stat

PP_SonicBoomShockwave
  - Radial lens distortion burst (one-frame trigger)
  - Motion blur spike outward from center

PP_InverseWorld
  - Hue shift: warm → red/purple
  - Sky appears inverted (black sun, white stars in day)
  - Subtle film grain increase, desaturated ground

PP_NightMetropolis
  - Neon bloom emphasis, god rays from streetlights
  - Reflection intensity boost for wet roads
  - Slight blue-teal grade for night atmosphere

VFX MATERIALS (used by Niagara systems)
────────────────────────────────────────
M_HeatVision_Beam          — Additive emissive beam, red-orange gradient
M_FreezeBreath_Cone        — Translucent ice particles + frost spray
M_FlightTrail              — Wispy vapor trail, scales with speed
M_SonicBoom_Ring           — Expanding shockwave ring, refraction distortion
M_Explosion_FireBall       — Physically-based fireball + smoke
M_Explosion_Debris         — Tumbling concrete/metal debris particles
M_BloodDecal               — Wet decal, fades over time
M_ScorchMark               — Heat vision burn mark decal
M_IceDecal                 — Freeze breath ice coating decal
M_CraterDecal              — Landing impact crater decal

WORLD MATERIALS
────────────────
MI_Metropolis_ConcreteFacade_*  — 6 weathering variants, Nanite
MI_Metropolis_Glass_Tower       — Curtain wall glass, reflective
MI_Metropolis_Asphalt           — 3 condition variants (new/worn/cracked)
MI_Smallville_Dirt              — Parallax displacement, tire tracks
MI_KentFarm_Wood                — Aged wood siding, peeling paint mask
MI_InverseWorld_Ground          — Dark, cracked earth, emissive fault lines
MI_Water_Ocean                  — UE5 Water plugin, full wave simulation
MI_Water_River                  — Flowing velocity, shore foam
