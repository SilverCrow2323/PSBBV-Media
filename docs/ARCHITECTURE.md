<div align="center">

# 🎬 PSBBV MEDIA — THE MEDIA WORKSPACE

**A proper media experience, not a folder browser wearing a fancy hat.**

<sub>SPDW FACTORY · PRODUCT / ARCHITECTURE</sub>

</div>

## Product boundary

PSBBV Media is an independent product repository and the Media node of PSBBV. The separation is intentional:

- **PSBBV Core** owns the Vita application lifecycle, Home navigation, focus, input, node registration, permissions and shared UI primitives.
- **PSBBV Media** owns media-domain models, catalogues, providers, playback orchestration, metadata, artwork, subtitles, audio-track selection and its workspace presentation.
- **Platform adapters** translate the domain contracts into capabilities available on Vita, Windows, Linux and other explicitly supported targets.

A separate repository does not mean a separately installable Vita plugin today. The first Vita integration is expected to be compiled into PSBBV Core. Dynamic loading is not part of the current Vita implementation.

## Product areas

| Area | Responsibility | First milestone |
|---|---|---|
| Movies | Library, metadata, artwork, resume state | Local catalogue |
| TV Shows | Series, seasons, episodes and progress | Local catalogue |
| IPTV | User-provided playlists, channel groups and EPG | Parser + fixture data |
| Stream4me | Provider adapter and account-aware discovery | Feasibility research |
| Playback | Backend selection, state, errors and controls | Platform spike |
| Subtitles | Track discovery, selection and preferences | Playback-dependent |
| Audio | Track selection and language preferences | Playback-dependent |
| Add-ons | Discovery, review, installation and lifecycle | Safe catalogue first |
| Kodi bridge | Measured compatibility for selected add-ons | Compatibility inventory |

## Layered architecture

1. **Presentation** — channel screens, library grids, details, search, EPG and playback overlays. It consumes view models and never performs network or filesystem work directly.
2. **Application services** — use cases such as scan library, refresh metadata, resolve a stream, schedule EPG refresh and start playback.
3. **Domain** — media items, sources, collections, playback sessions, tracks, provider capabilities and typed failures. This layer must be deterministic and host-testable.
4. **Infrastructure** — filesystem adapters, HTTP clients, metadata providers, artwork cache, playlist parsers and platform playback backends.
5. **PSBBV adapter** — translates the Core Node API into Media services. It must not fork the host main loop or bypass host permissions.

All slow operations are asynchronous and cancellable. Results return to the UI through host-approved state updates. No worker draws directly or mutates presentation state.

## Source and trust model

A source is an explicit user-configured endpoint or local location. Each source declares its type, origin, supported operations and required permissions. Credentials belong in an OS-protected or host-provided secret store when available; never in manifests, logs, fixtures or repository files.

IPTV support is for lawful, user-supplied playlists and EPG data. The project will not ship copyrighted channel lists, provider credentials or instructions to bypass access controls or DRM.

Stream4me remains a research item until its official interfaces, authentication model, terms and target-platform availability are verified. No undocumented endpoint or credential workaround is assumed.

## Playback strategy

Playback is an adapter contract, not a promise that every format works everywhere. Each backend must report supported containers, codecs, subtitles, audio tracks, seek behavior, memory constraints and failure reasons.

The Vita target gets a dedicated feasibility spike before a backend is selected. Desktop playback capability must not be used as evidence of Vita playback capability. A successful build is not a playback test.

## Add-on compatibility policy

Kodi compatibility is evaluated per add-on, not declared globally. The initial report uses four outcomes:

- **Verified** — required APIs and dependencies pass tests on the named platform.
- **Partial** — a documented subset works; missing APIs are listed.
- **Metadata-only** — catalogue information can be read, but execution is unsupported.
- **Unsupported** — incompatible runtime, native dependency, API or platform requirement.

No arbitrary add-on code runs with unrestricted filesystem, process or network access. Compatibility work begins with static inspection and API inventory. A Python runtime, Kodi module emulation or process sandbox is not considered available until separately implemented and tested.

## Quality gates

- Deterministic host tests for parsers, domain rules, source failures and EPG scheduling.
- Fixture-based tests; no live provider account is required in CI.
- Path traversal, URL scheme, redirect and credential-redaction tests.
- Cancellation and stale-result tests for asynchronous work.
- Platform-specific build and package checks.
- Physical Vita validation for memory, playback, controls, suspend/resume and network recovery.

**Status:** architecture baseline only. Product areas above remain planned until code and evidence exist.

---

**SPDW FACTORY** · PSBBV Media Engineering
