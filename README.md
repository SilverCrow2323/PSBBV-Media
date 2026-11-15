# PSBBV Media

**SPDW Factory · Play, watch, listen. One coherent media workspace.**

PSBBV Media is the media node for PSBBV, a native PlayStation Vita frontend inspired by the PlayStation Broadband Navigator. It owns media discovery and playback experiences; PSBBV Core owns application navigation, focus, lifecycle, permissions and node registration.

## Scope

- Movies and TV-show libraries, metadata, artwork and resume state.
- IPTV catalogue and EPG presentation, subject to source and regional legality.
- Stream4me feasibility and provider integration behind explicit adapters.
- Playback orchestration, subtitle/audio-track selection and playback status.
- Add-on discovery and management through a reviewed compatibility boundary.
- A measured Kodi compatibility strategy; no assumption that Kodi desktop add-ons run on Vita.

## Architecture rules

- Implement as a PSBBV built-in node first; do not introduce dynamic loading or Python on Vita.
- Consume the versioned Node API and manifest contract defined by [PSBBV Core](https://github.com/SilverCrow2323/PSBBV).
- Keep provider, catalogue, playback and UI layers separate. Network calls must be asynchronous and cancellable.
- Declare capabilities and permissions explicitly. No add-on receives arbitrary filesystem, process or network access.
- Never bundle copyrighted media, provider credentials, private playlists or DRM circumvention.
- Keep the Vita client within VitaSDK constraints; desktop tooling may be used for catalogue validation and fixture generation.

## Delivery order

1. Domain model and source/provider interfaces with deterministic fixtures.
2. Catalogue browsing, search, metadata and artwork cache.
3. EPG timeline and IPTV channel presentation.
4. Playback adapter spike on Vita hardware before committing to a backend.
5. Subtitle/audio selection, resume state and error recovery.
6. Add-on compatibility report and signed/allowlisted package policy.

## Quality gates

Host tests for parsers, scheduling, cache invalidation and provider failures; static checks for path and URL safety; VitaSDK build and VPK inspection; then physical Vita tests for memory, playback, controls, suspend/resume and network recovery. A successful cross-build is not a playback certification.

## Status

Repository bootstrap and product boundary only. Features above are planned until implemented and validated. User-facing UI and documentation are written in English and follow SPDW Factory's lively, clear visual identity.
