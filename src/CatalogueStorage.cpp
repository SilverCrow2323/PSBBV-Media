#include "psbbv/media/CatalogueStorage.hpp"

namespace psbbv::media {
namespace {
constexpr std::uint8_t Magic[4] = {'P', 'S', 'M', 'C'};

void put16(std::uint8_t* p, std::uint16_t v) noexcept {
    p[0] = static_cast<std::uint8_t>(v); p[1] = static_cast<std::uint8_t>(v >> 8);
}
void put32(std::uint8_t* p, std::uint32_t v) noexcept {
    p[0] = static_cast<std::uint8_t>(v); p[1] = static_cast<std::uint8_t>(v >> 8);
    p[2] = static_cast<std::uint8_t>(v >> 16); p[3] = static_cast<std::uint8_t>(v >> 24);
}
std::uint16_t get16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0] | (static_cast<std::uint16_t>(p[1]) << 8));
}
std::uint32_t get32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
        (static_cast<std::uint32_t>(p[1]) << 8) |
        (static_cast<std::uint32_t>(p[2]) << 16) |
        (static_cast<std::uint32_t>(p[3]) << 24);
}
std::uint32_t crc32(const std::uint8_t* data, std::size_t size) noexcept {
    std::uint32_t crc = 0xffffffffu;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}
bool boundedLength(const char* value, std::size_t capacity, std::size_t& length) noexcept {
    length = 0;
    if (value == nullptr) return false;
    while (length < capacity && value[length] != '\0') ++length;
    return length < capacity;
}
}

StorageResult encodeCatalogue(const Catalogue& catalogue, std::uint8_t* output,
                              std::size_t outputCapacity) noexcept {
    if (output == nullptr) return {StorageError::InvalidArgument, 0};
    if (catalogue.size() > CatalogueCapacity) return {StorageError::TooManyItems, 0};
    if (outputCapacity < CatalogueStorageHeaderSize) return {StorageError::BufferTooSmall, 0};

    std::size_t cursor = CatalogueStorageHeaderSize;
    for (std::size_t i = 0; i < catalogue.size(); ++i) {
        const MediaItem* item = catalogue.at(i);
        std::size_t pathLength = 0, titleLength = 0;
        if (item == nullptr || item->id == 0 ||
            static_cast<std::uint8_t>(item->kind) > static_cast<std::uint8_t>(MediaKind::Video) ||
            !boundedLength(item->path, MediaPathCapacity, pathLength) || pathLength == 0 ||
            !boundedLength(item->title, MediaTitleCapacity, titleLength) || titleLength == 0 ||
            (item->durationMs != 0 && item->resumePositionMs > item->durationMs))
            return {StorageError::InvalidRecord, 0};

        const std::size_t recordSize = CatalogueStorageRecordFixedSize + pathLength + titleLength;
        if (recordSize > outputCapacity - cursor) return {StorageError::BufferTooSmall, 0};
        put32(output + cursor, item->id); cursor += 4;
        output[cursor++] = static_cast<std::uint8_t>(item->kind);
        put16(output + cursor, item->year); cursor += 2;
        put32(output + cursor, item->durationMs); cursor += 4;
        put32(output + cursor, item->resumePositionMs); cursor += 4;
        output[cursor++] = item->favourite ? 1 : 0;
        put16(output + cursor, static_cast<std::uint16_t>(pathLength)); cursor += 2;
        put16(output + cursor, static_cast<std::uint16_t>(titleLength)); cursor += 2;
        for (std::size_t j = 0; j < pathLength; ++j) output[cursor++] = static_cast<std::uint8_t>(item->path[j]);
        for (std::size_t j = 0; j < titleLength; ++j) output[cursor++] = static_cast<std::uint8_t>(item->title[j]);
    }

    for (unsigned i = 0; i < 4; ++i) output[i] = Magic[i];
    put16(output + 4, CatalogueStorageVersion);
    put16(output + 6, static_cast<std::uint16_t>(catalogue.size()));
    put32(output + 8, static_cast<std::uint32_t>(cursor - CatalogueStorageHeaderSize));
    put32(output + 12, crc32(output + CatalogueStorageHeaderSize,
                             cursor - CatalogueStorageHeaderSize));
    return {StorageError::None, cursor};
}

StorageResult decodeCatalogue(const std::uint8_t* input, std::size_t inputSize,
                              Catalogue& destination) noexcept {
    if (input == nullptr) return {StorageError::InvalidArgument, 0};
    if (inputSize < CatalogueStorageHeaderSize) return {StorageError::InvalidLength, 0};
    for (unsigned i = 0; i < 4; ++i)
        if (input[i] != Magic[i]) return {StorageError::InvalidMagic, 0};
    if (get16(input + 4) != CatalogueStorageVersion)
        return {StorageError::UnsupportedVersion, 0};
    const std::uint16_t count = get16(input + 6);
    const std::uint32_t payloadSize = get32(input + 8);
    if (count > CatalogueCapacity || payloadSize != inputSize - CatalogueStorageHeaderSize)
        return {count > CatalogueCapacity ? StorageError::TooManyItems : StorageError::InvalidLength, 0};
    if (get32(input + 12) != crc32(input + CatalogueStorageHeaderSize, payloadSize))
        return {StorageError::ChecksumMismatch, 0};

    Catalogue decoded;
    std::size_t cursor = CatalogueStorageHeaderSize;
    for (std::size_t i = 0; i < count; ++i) {
        if (inputSize - cursor < CatalogueStorageRecordFixedSize)
            return {StorageError::InvalidLength, 0};
        MediaItem item{};
        item.id = get32(input + cursor); cursor += 4;
        item.kind = static_cast<MediaKind>(input[cursor++]);
        item.year = get16(input + cursor); cursor += 2;
        item.durationMs = get32(input + cursor); cursor += 4;
        item.resumePositionMs = get32(input + cursor); cursor += 4;
        const std::uint8_t favourite = input[cursor++];
        const std::uint16_t pathLength = get16(input + cursor); cursor += 2;
        const std::uint16_t titleLength = get16(input + cursor); cursor += 2;
        if (favourite > 1 || pathLength == 0 || pathLength >= MediaPathCapacity ||
            titleLength == 0 || titleLength >= MediaTitleCapacity ||
            static_cast<std::uint8_t>(item.kind) > static_cast<std::uint8_t>(MediaKind::Video) ||
            (item.durationMs != 0 && item.resumePositionMs > item.durationMs) ||
            static_cast<std::size_t>(pathLength) + titleLength > inputSize - cursor)
            return {StorageError::InvalidRecord, 0};
        for (std::size_t j = 0; j < pathLength; ++j) {
            const std::uint8_t c = input[cursor++];
            if (c == 0) return {StorageError::InvalidRecord, 0};
            item.path[j] = static_cast<char>(c);
        }
        for (std::size_t j = 0; j < titleLength; ++j) {
            const std::uint8_t c = input[cursor++];
            if (c == 0) return {StorageError::InvalidRecord, 0};
            item.title[j] = static_cast<char>(c);
        }
        item.favourite = favourite != 0;
        if (decoded.find(item.id) != nullptr) return {StorageError::DuplicateId, 0};
        if (!decoded.upsert(item).ok()) return {StorageError::InvalidRecord, 0};
    }
    if (cursor != inputSize) return {StorageError::InvalidLength, 0};
    destination = decoded;
    return {StorageError::None, inputSize};
}

} // namespace psbbv::media
