<div align="center">

# 🧭 PSBBV MEDIA — THE ROADMAP

**Build the foundations. Prove playback. Then get ambitious.**

<sub>SPDW FACTORY · PROJECT TRACKER</sub>

</div>

## Status legend

- ✅ Implemented and present in this repository
- 🟡 Started; validation or integration remains
- 🔵 Planned
- 🟠 Requires target-device validation
- ⛔ Blocked by a named dependency

## Phase 0 — Product foundation

- [x] Establish the independent PSBBV Media repository.
- [x] Define Core/Media ownership and the built-in Vita integration strategy.
- [x] Define product areas and platform boundaries.
- [x] Define a measured Kodi add-on compatibility vocabulary.
- [x] Add a provisional node manifest.
- [ ] Add automated manifest validation against the canonical PSBBV schema.
- [ ] Add contribution, security and support policies.

## Phase 1 — Domain foundation

- [ ] Typed models for media items, collections, sources and playback sessions.
- [ ] Provider interfaces with explicit capability declarations.
- [ ] Typed errors and deterministic result handling.
- [ ] Host test harness and fixture catalogue.
- [ ] Safe URL, path and credential handling.

## Phase 2 — Catalogue experience

- [ ] Local movie library and metadata.
- [ ] TV shows, seasons, episodes and resume state.
- [ ] Search, sorting, filters and recently viewed.
- [ ] Artwork cache with bounded storage.
- [ ] Empty, loading, unavailable and error states.

## Phase 3 — Network sources

- [ ] M3U/M3U8 parsing for user-provided IPTV playlists.
- [ ] XMLTV EPG parsing and timezone handling.
- [ ] Channel groups, search and programme timeline.
- [ ] Cancellable provider refresh and network recovery.
- [ ] Stream4me feasibility report based on verifiable official interfaces.

## Phase 4 — Playback proof

- [ ] Define the playback backend interface.
- [ ] Inventory VitaSDK-supported playback options and constraints.
- [ ] Build a minimal Vita playback spike.
- [ ] Measure memory, supported codecs, subtitles and seek behavior on hardware.
- [ ] Select backend only after evidence.
- [ ] Implement desktop adapters independently where useful.

## Phase 5 — Add-on ecosystem

- [ ] Build static add-on metadata inspection.
- [ ] Publish per-add-on compatibility reports.
- [ ] Define package integrity and allowlisting policy.
- [ ] Implement only the API subset that can be tested and safely supported.
- [ ] Keep unsupported add-ons clearly labelled; never imply universal Kodi compatibility.

## Phase 6 — PSBBV integration and release

- [ ] Integrate as a built-in PSBBV node using the versioned Node API.
- [ ] Wire host navigation, permissions, lifecycle and cancellation.
- [ ] Validate Core/Media API compatibility in CI.
- [ ] Test on physical Vita and supported desktop targets.
- [ ] Publish reproducible packages, checksums and release notes.

## Release rule

No feature is marked ready because a screen exists or a build passes. Each feature needs tests, documented platform support and a clear failure path. Vita playback additionally requires physical-device evidence.

---

**SPDW FACTORY** · Progress you can verify.
