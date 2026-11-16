#pragma once

#include "psbbv/media/CatalogueStorage.hpp"

namespace psbbv::media {

using ReadCatalogueSnapshot = bool (*)(void* context, std::uint8_t* output,
                                       std::size_t capacity, std::size_t* bytesRead) noexcept;
using WriteCatalogueSnapshotAtomic = bool (*)(void* context, const std::uint8_t* data,
                                               std::size_t size) noexcept;

struct CatalogueStorageBackend final {
    void* context = nullptr;
    ReadCatalogueSnapshot read = nullptr;
    // The adapter must leave the previous snapshot intact if replacement fails.
    WriteCatalogueSnapshotAtomic writeAtomic = nullptr;
};

enum class PersistenceError : std::uint8_t {
    None, InvalidBackend, BackendReadFailed, BackendWriteFailed, CodecFailed
};
struct PersistenceResult final {
    PersistenceError error = PersistenceError::None;
    StorageError codecError = StorageError::None;
    std::size_t bytes = 0;
    constexpr bool ok() const noexcept { return error == PersistenceError::None; }
};

// Caller owns the bounded workspace; this layer performs no filesystem calls or allocation.
PersistenceResult saveCatalogue(const Catalogue& catalogue,
                                const CatalogueStorageBackend& backend,
                                std::uint8_t* workspace,
                                std::size_t workspaceCapacity) noexcept;
PersistenceResult loadCatalogue(Catalogue& destination,
                                const CatalogueStorageBackend& backend,
                                std::uint8_t* workspace,
                                std::size_t workspaceCapacity) noexcept;

} // namespace psbbv::media
