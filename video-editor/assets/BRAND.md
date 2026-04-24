# NTG Productions — Brand Spec

Matched to the live site at **ntgproductions.org**.

## Palette

| Role            | Hex       | FFmpeg     | Notes                                                                 |
| --------------- | --------- | ---------- | --------------------------------------------------------------------- |
| Orange (accent) | `#F5A03A` | `0xF5A03A` | Keyword highlight. Matches the site's accent and headline emphasis.   |
| White (body)    | `#FFFFFF` | `0xFFFFFF` | Caption body text. Pure white, max contrast for mobile playback.      |
| Near-black (bg) | `#0A0A0A` | `0x0A0A0A` | Intro/outro cards and any solid-frame base.                           |
| Gold (legacy)   | `#D4A84A` | `0xD4A84A` | Old logo tone. Available as `ntg-gold` preset if you want the photo-brand vibe back. |

## Signature pattern

The site splits headlines: **white setup, orange punchline** — e.g. "Tired of Chasing Leads? *We'll Bring Them to You.*"

The caption engine already mirrors this: white body words, orange on keywords. One-to-one match with the web brand.

## Typography

- Geometric sans, bold/black weight, tight letter-spacing. Same feel as the "NTGPRODUCTIONS" wordmark.
- Recommended: **Inter Black**, **General Sans Bold**, or **Plus Jakarta Sans Black** (all free).
- To use: drop the `.ttf` at `assets/fonts/caption.ttf`. The renderer auto-detects it.

## Grade

Punchy — contrast boost, increased saturation. Matches the performance-marketing / sales tone of the site ("No vanity metrics. No manual work. Just revenue."), not cinematic-editorial restraint.

## Logo overlay

- Drop a transparent-background logo PNG at `assets/logo.png`.
- Auto-placed **top-right, 7% of frame width, 85% opacity** — discreet watermark.

## Handles

- IG (personal): `@natethecameraman`
- IG (brand): `@ntgproductions_`
- Web: `ntgproductions.org`

## Presets available

- `ntg` — **default**. Orange accent matched to the live site.
- `ntg-gold` — Gold accent matched to the older logo. Same punchy grade.
- `hormozi-gold`, `hormozi-yellow`, `cinematic`, `warm`, `moody` — generic fallbacks.
