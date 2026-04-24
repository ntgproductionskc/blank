Superman & Lois — VFX / Niagara Systems
=========================================

NS_HeatVision_Beam
  — Twin volumetric laser beams from eyes
  — Per-surface impact: NS_HeatVision_HitConcrete, NS_HeatVision_HitMetal, NS_HeatVision_HitFlesh
  — Scorched air distortion haze around beam path

NS_FreezeBreath_Cone
  — Ice crystal particle emitter in forward cone (600 unit range)
  — Secondary: NS_FreezeBreath_IceCoating (applies MI_IceDecal to surfaces)
  — NS_FreezeBreath_Shatter (when frozen target is hit with force)

NS_FlightTrail_Normal
  — Wispy cloud vapor condensation trail
  — Visibility scales with altitude and speed
  — Color: white/grey at normal speed

NS_FlightTrail_Boost
  — Intense contrail + plasma glow at sonic speeds
  — Fire-orange leading edge, white body
  — NS_SonicBoom_Shockwave triggered on boost activation

NS_SonicBoom_Ring
  — Expanding ring at 80,000 cm radius over 0.3 seconds
  — Additive distortion + debris scatter from ground

NS_SuperPunch_Impact_Light
  — Wind ripple + dust puff on contact

NS_SuperPunch_Impact_Heavy
  — Shockwave ring + concrete chunk scatter + screen shake trigger

NS_LandingImpact_Light
  — Dust ring, light crack decal

NS_LandingImpact_Heavy
  — Seismic crater: large debris, dust column, shockwave ring
  — Scales with landing speed (driven by SupermanCharacter.PreviousSpeed)

NS_KryptoniteAura
  — Green glowing particles around Kryptonite objects
  — Intensity / radius driven by KryptoniteStrength data attribute

NS_BuildingFire_Large
  — Turbulent fire column + smoke plume
  — Wind-responsive (driven by WeatherSystem.WindStrength)
  — Ember scatter system

NS_Explosion_Standard
  — Fireball + smoke + pressure wave ring + debris
  — 3 size scales: Small / Medium / Large

NS_Explosion_Kryptonian
  — Blue-white energy explosion (for Kryptonian tech detonations)

NS_PortalEffect
  — Inverse World portal: spinning ring, distortion, inverse-world preview inside

NS_XKryptoniteCloud
  — Sickly green mist / gas cloud (environmental hazard)

NS_Bizarro_FreezeVision
  — Ice beam from eyes: white-blue, crystalline particles

NS_Bizarro_FlameBreath
  — Wide-cone orange fire with dark smoke trailing

NS_Doomsday_Stomp
  — Ground shatter ring + dust column on each footstep

NS_InverseSuperman_SolarFlare
  — Massive energy burst (1200 unit sphere) — blinding white-red

NS_BloodSplatter_*
  — Restricted to M/T rating within legal limits (configurable)
  — Dark ichor for non-human enemies

NS_IceShard_Shatter
  — Crystalline debris when frozen object is destroyed

NS_SpeedLines
  — Camera-space streak lines during Super Speed activation

NS_TimeSlowEffect
  — Particle slowdown + blue tint particles when near kryptonite critical
