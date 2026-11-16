#include "psbbv/media/CataloguePersistence.hpp"

namespace psbbv::media {

PersistenceResult saveCatalogue(const Catalogue& catalogue,
                                const CatalogueStorageBackend& backend,
                                std::uint8_t* workspace,
                                std::size_t workspaceCapacity) noexcept {
    if (backend.writeAtomic == nullptr || workspace == nullptr)
        return {PersistenceError::InvalidBackend, StorageError::InvalidArgument, 0};
    const StorageResult encoded = encodeCatalogue(catalogue, workspace, workspaceCapacity);
    if (!encoded.ok()) return {PersistenceError::CodecFailed, encoded.error, 0};
    if (!backend.writeAtomic(backend.context, workspace, encoded.bytes))
        return {PersistenceError::BackendWriteFailed, StorageError::None, 0};
    return {PersistenceError::None, StorageError::None, encoded.bytes};
}

PersistenceResult loadCatalogue(Catalogue& destination,
                                const CatalogueStorageBackend& backend,
                                std::uint8_t* workspace,
                                std::size_t workspaceCapacity) noexcept {
    if (backend.read == nullptr || workspace == nullptr)
        return {PersistenceError::InvalidBackend, StorageError::InvalidArgument, 0};
    std::size_t bytesRead = 0;
    if (!backend.read(backend.context, workspace, workspaceCapacity, &bytesRead))
        return {PersistenceError::BackendReadFailed, StorageError::None, 0};
    if (bytesRead > workspaceCapacity)
        return {PersistenceError::BackendReadFailed, StorageError::InvalidLength, 0};
    const StorageResult decoded = decodeCatalogue(workspace, bytesRead, destination);
    if (!decoded.ok()) return {PersistenceError::CodecFailed, decoded.error, 0};
    return {PersistenceError::None, StorageError::None, decoded.bytes};
}

} // namespace psbbv::media
