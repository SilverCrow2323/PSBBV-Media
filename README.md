# PSBBV Media

**SPDW Factory · Play, watch, listen. One coherent media workspace.**

PSBBV Media is the independent media product and Media node for PSBBV, a native PlayStation Vita frontend inspired by the PlayStation Broadband Navigator. It owns media discovery and playback experiences; PSBBV Core owns application navigation, focus, lifecycle, permissions and node registration.

## Scope

- Movies and TV-show libraries, metadata, artwork and resume state.
- IPTV catalogue and EPG presentation, subject to source and regional legality.
- Stream4me feasibility and provider integration behind explicit adapters.
- Playback orchestration, subtitle/audio-track selection and playback status.
- Add-on discovery and management through a reviewed compatibility boundary.
- A measured Kodi compatibility strategy; no assumption that Kodi desktop add-ons run on Vita.

## Project map

| Document | What it covers |
|---|---|
| [Architecture](docs/ARCHITECTURE.md) | Layer boundaries, sources, playback, add-on trust and quality gates |
| [Roadmap](docs/ROADMAP.md) | Staged delivery, from domain model to Vita integration |
| [Node manifest](psbbv-node.json) | Provisional PSBBV identity and declared capabilities |

## Architecture rules

- Implement as a PSBBV built-in node first; do not introduce dynamic loading or Python on Vita.
- Consume the versioned Node API and manifest contract defined by [PSBBV Core](https://github.com/SilverCrow2323/PSBBV).
- Keep provider, catalogue, playback and UI layers separate. Network calls must be asynchronous and cancellable.
- Declare capabilities and permissions explicitly. No add-on receives arbitrary filesystem, process or network access.
- Never bundle copyrighted media, provider credentials, private playlists or DRM circumvention.
- Keep the Vita client within VitaSDK constraints; desktop tooling may be used for catalogue validation and fixture generation.

## Quality gates

Host tests for parsers, scheduling, cache invalidation and provider failures; static checks for path and URL safety; VitaSDK build and VPK inspection; then physical Vita tests for memory, playback, controls, suspend/resume and network recovery. A successful cross-build is not a playback certification.

## Status

Repository foundation and product boundary are in place. The manifest is provisional and still needs automated validation against the canonical Core schema. Product features remain planned until implemented and validated. User-facing UI and documentation are written in English and follow SPDW Factory's lively, clear visual identity.
