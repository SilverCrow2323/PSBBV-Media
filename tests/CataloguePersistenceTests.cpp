#include "psbbv/media/CataloguePersistence.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {
void require(bool value, const char* message) {
    if (!value) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
}
struct MemoryBackend {
    std::uint8_t data[psbbv::media::CatalogueStorageMaxSize]{};
    std::size_t size = 0;
    bool failRead = false;
    bool failWrite = false;
};
bool readMemory(void* context, std::uint8_t* output, std::size_t capacity,
                std::size_t* bytesRead) noexcept {
    auto& memory = *static_cast<MemoryBackend*>(context);
    if (memory.failRead || memory.size > capacity) return false;
    std::memcpy(output, memory.data, memory.size);
    *bytesRead = memory.size;
    return true;
}
bool writeMemoryAtomic(void* context, const std::uint8_t* data, std::size_t size) noexcept {
    auto& memory = *static_cast<MemoryBackend*>(context);
    if (memory.failWrite || size > sizeof(memory.data)) return false;
    std::uint8_t replacement[psbbv::media::CatalogueStorageMaxSize]{};
    std::memcpy(replacement, data, size);
    std::memcpy(memory.data, replacement, size);
    memory.size = size;
    return true;
}
bool reportOversizedRead(void*, std::uint8_t*, std::size_t capacity,
                         std::size_t* bytesRead) noexcept {
    *bytesRead = capacity + 1;
    return true;
}
}

int main() {
    using namespace psbbv::media;
    MemoryBackend memory;
    CatalogueStorageBackend backend{&memory, readMemory, writeMemoryAtomic};
    Catalogue source;
    MediaItem item{};
    item.id = 7; item.kind = MediaKind::Movie; item.durationMs = 60000;
    item.resumePositionMs = 1234; item.favourite = true;
    std::strcpy(item.path, "ux0:video/test.mp4");
    std::strcpy(item.title, "Test Movie");
    require(source.upsert(item).ok(), "insert source item");
    std::uint8_t workspace[CatalogueStorageMaxSize]{};
    require(saveCatalogue(source, backend, workspace, sizeof(workspace)).ok(),
            "save through atomic backend");
    Catalogue loaded;
    require(loadCatalogue(loaded, backend, workspace, sizeof(workspace)).ok() &&
            loaded.find(7) != nullptr && loaded.find(7)->resumePositionMs == 1234,
            "load through backend restores state");

    memory.failWrite = true;
    const std::size_t previousSize = memory.size;
    require(saveCatalogue(source, backend, workspace, sizeof(workspace)).error ==
            PersistenceError::BackendWriteFailed, "write failure is surfaced");
    require(memory.size == previousSize, "failed atomic write preserves previous snapshot");
    memory.failWrite = false;

    memory.failRead = true;
    require(loadCatalogue(loaded, backend, workspace, sizeof(workspace)).error ==
            PersistenceError::BackendReadFailed, "read failure is surfaced");
    memory.failRead = false;

    CatalogueStorageBackend oversizedBackend{nullptr, reportOversizedRead, nullptr};
    const PersistenceResult oversized = loadCatalogue(
        loaded, oversizedBackend, workspace, sizeof(workspace));
    require(oversized.error == PersistenceError::BackendReadFailed &&
            oversized.codecError == StorageError::InvalidLength,
            "backend cannot report a read larger than the supplied workspace");
    require(loaded.find(7) != nullptr,
            "oversized backend read preserves the live catalogue");

    Catalogue untouched;
    MediaItem keep{};
    keep.id = 99; std::strcpy(keep.path, "ux0:keep.mp4"); std::strcpy(keep.title, "Keep");
    require(untouched.upsert(keep).ok(), "prepare load destination");
    memory.data[0] ^= 1;
    require(loadCatalogue(untouched, backend, workspace, sizeof(workspace)).error ==
            PersistenceError::CodecFailed, "invalid stored snapshot is surfaced");
    require(untouched.find(99) != nullptr, "failed load preserves live catalogue");
    require(saveCatalogue(source, CatalogueStorageBackend{}, workspace, sizeof(workspace)).error ==
            PersistenceError::InvalidBackend, "missing backend callback is rejected");

    std::cout << "PSBBV Media persistence adapter: all checks passed.\n";
    return EXIT_SUCCESS;
}
