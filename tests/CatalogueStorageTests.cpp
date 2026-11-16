#include "psbbv/media/CatalogueStorage.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {
void require(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
}
psbbv::media::MediaItem makeItem(std::uint32_t id, const char* title, const char* path,
                                 psbbv::media::MediaKind kind, std::uint16_t year,
                                 std::uint32_t resume, bool favourite) {
    psbbv::media::MediaItem item{};
    item.id = id; item.kind = kind; item.year = year;
    item.durationMs = 90000; item.resumePositionMs = resume; item.favourite = favourite;
    std::strncpy(item.title, title, sizeof(item.title) - 1);
    std::strncpy(item.path, path, sizeof(item.path) - 1);
    return item;
}
}

int main() {
    using namespace psbbv::media;
    Catalogue original;
    require(original.upsert(makeItem(11, "The Matrix", "ux0:video/matrix.mkv",
                                     MediaKind::Movie, 1999, 42000, true)).ok(),
            "insert movie");
    require(original.upsert(makeItem(12, "Opening Theme", "ux0:music/theme.mp3",
                                     MediaKind::Music, 2024, 0, false)).ok(),
            "insert music");

    std::uint8_t bytes[CatalogueStorageMaxSize]{};
    const StorageResult encoded = encodeCatalogue(original, bytes, sizeof(bytes));
    require(encoded.ok() && encoded.bytes > CatalogueStorageHeaderSize, "encode snapshot");
    Catalogue restored;
    const StorageResult decoded = decodeCatalogue(bytes, encoded.bytes, restored);
    require(decoded.ok() && restored.size() == 2, "decode snapshot");
    const MediaItem* movie = restored.find(11);
    require(movie != nullptr && std::strcmp(movie->title, "The Matrix") == 0 &&
            std::strcmp(movie->path, "ux0:video/matrix.mkv") == 0 &&
            movie->kind == MediaKind::Movie && movie->year == 1999 &&
            movie->durationMs == 90000 && movie->resumePositionMs == 42000 &&
            movie->favourite, "round trip preserves every field");
    require(restored.find(12) != nullptr && restored.find(12)->kind == MediaKind::Music,
            "round trip preserves second record");

    std::uint8_t tooSmall[8]{};
    require(encodeCatalogue(original, tooSmall, sizeof(tooSmall)).error ==
            StorageError::BufferTooSmall, "small output buffer is rejected");

    Catalogue unchanged;
    require(unchanged.upsert(makeItem(99, "Keep me", "ux0:keep.mp4",
                                      MediaKind::Video, 2020, 0, true)).ok(),
            "prepare transactional destination");
    std::uint8_t damaged[CatalogueStorageMaxSize]{};
    std::memcpy(damaged, bytes, encoded.bytes);
    damaged[encoded.bytes - 1] ^= 0x40;
    require(decodeCatalogue(damaged, encoded.bytes, unchanged).error ==
            StorageError::ChecksumMismatch, "payload corruption is detected");
    require(unchanged.size() == 1 && unchanged.find(99) != nullptr,
            "failed decode leaves destination unchanged");

    require(decodeCatalogue(bytes, encoded.bytes - 1, unchanged).error ==
            StorageError::InvalidLength, "truncated snapshot is rejected");
    std::uint8_t badVersion[CatalogueStorageMaxSize]{};
    std::memcpy(badVersion, bytes, encoded.bytes);
    badVersion[4] = 2; badVersion[5] = 0;
    require(decodeCatalogue(badVersion, encoded.bytes, unchanged).error ==
            StorageError::UnsupportedVersion, "unknown version is rejected");
    std::uint8_t badMagic[CatalogueStorageMaxSize]{};
    std::memcpy(badMagic, bytes, encoded.bytes);
    badMagic[0] = 'X';
    require(decodeCatalogue(badMagic, encoded.bytes, unchanged).error ==
            StorageError::InvalidMagic, "unknown file signature is rejected");
    require(decodeCatalogue(nullptr, 0, unchanged).error == StorageError::InvalidArgument,
            "null input is rejected");
    require(decodeCatalogue(bytes, encoded.bytes, unchanged).ok() &&
            unchanged.find(99) == nullptr && unchanged.find(11) != nullptr,
            "valid decode atomically replaces destination");

    std::cout << "PSBBV Media storage: all checks passed.\n";
    return EXIT_SUCCESS;
}
