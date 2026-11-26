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

The bounded local catalogue supports typed media records, unique-ID upsert, removal, title search, favourites, bounded resume-position updates, kind/favourite filters and stable title/year sorting. A portable version-1 snapshot codec serializes fields explicitly in little-endian order, bounds records and strings, checks a CRC32 payload checksum, rejects malformed/truncated/unknown-version data, and restores transactionally so a failed load cannot partially overwrite the live catalogue. Host CI covers round-trip and corruption cases. The codec is wrapped by a backend-neutral save/load API that uses caller-owned workspace memory and requires adapters to provide an atomic-replacement write callback. Host tests cover successful save/load, read/write failures and preservation of the previous snapshot/live catalogue on failure. No Vita filesystem adapter or on-device atomic-replacement implementation exists yet. The playback session contract now includes an explicit Completed state. Completing a playing or paused session pins the playhead to the known duration, rejects further seek/pause operations, and permits a fresh resolve cycle. This remains a state contract, not a decoder or playback backend. Filesystem discovery, artwork, TV-show hierarchy, decoder integration and Kodi add-on execution remain unimplemented. The manifest is provisional and is validated against the repository's pinned copy of the Core schema; schema synchronization remains a maintenance requirement. User-facing UI and documentation are written in English and follow SPDW Factory's lively, clear visual identity.
