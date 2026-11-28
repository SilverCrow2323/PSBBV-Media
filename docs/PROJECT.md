# Project: PSBBV Media

## Purpose
Own the media catalogue, source adapters, EPG metadata and playback experience as a built-in PSBBV node. Core owns global navigation, lifecycle, focus and permissions. Stream4me/Kodi compatibility is feasibility work, not an assumption.

## Users and use cases
- PlayStation Vita owners browsing an offline-first console-style media/games workspace.
- Maintainers implementing a built-in node against the versioned PSBBV Core manifest and Node API.
- Testers validating host logic separately from VitaSDK compilation and physical-device behavior.

## Product boundary
This repository owns the media domain only. PSBBV Core owns global navigation, focus, lifecycle, permissions, node registration and compatibility policy. No node may create a second global shell, load arbitrary third-party code, or silently claim a feature is ready.

## Target and architecture
- Native PS Vita / PS TV, VitaSDK and the PSBBV built-in-node model.
- Portable domain model and host fixtures; platform adapters remain isolated.
- Manifest: psbbv-node.json; schema snapshot: schemas/psbbv-node-manifest.schema.json.
- Tests: host contract/logic tests and, where applicable, VitaSDK cross-build gates.

## Dependencies and integration
The node consumes the versioned Node API and manifest contract from https://github.com/SilverCrow2323/PSBBV. The local schema is a pinned compatibility copy and must be reconciled when Core changes its contract. No Python runtime or dynamic third-party loader is assumed on Vita.

## Safety, accessibility and localization
Validate untrusted metadata and paths, bound reads and allocations, keep network operations cancellable, and require explicit confirmation for state-changing operations. Respect controller focus order, readable typography and the Vita's memory budget. UI and technical documentation are English.

## Verified baseline
The node manifest is present and currently declares availability=planned. Host-side implementation and CI coverage are listed in README/docs; a successful host test or cross-build is not physical Vita certification. The staged roadmap is maintained in docs/ROADMAP.md.
