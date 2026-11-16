#pragma once

#include "psbbv/media/Catalogue.hpp"

namespace psbbv::media {

constexpr std::uint16_t CatalogueStorageVersion = 1;
constexpr std::size_t CatalogueStorageHeaderSize = 16;
constexpr std::size_t CatalogueStorageRecordFixedSize = 20;
constexpr std::size_t CatalogueStorageMaxSize =
    CatalogueStorageHeaderSize + CatalogueCapacity *
    (CatalogueStorageRecordFixedSize + (MediaPathCapacity - 1) + (MediaTitleCapacity - 1));

enum class StorageError : std::uint8_t {
    None, InvalidArgument, BufferTooSmall, InvalidMagic, UnsupportedVersion,
    InvalidLength, ChecksumMismatch, TooManyItems, InvalidRecord, DuplicateId
};

struct StorageResult final {
    StorageError error = StorageError::None;
    std::size_t bytes = 0;
    constexpr bool ok() const noexcept { return error == StorageError::None; }
};

// Encodes a portable little-endian snapshot. No raw structs or heap allocation.
StorageResult encodeCatalogue(const Catalogue& catalogue, std::uint8_t* output,
                              std::size_t outputCapacity) noexcept;

// Decodes transactionally: destination is unchanged on every failure.
StorageResult decodeCatalogue(const std::uint8_t* input, std::size_t inputSize,
                              Catalogue& destination) noexcept;

} // namespace psbbv::media
