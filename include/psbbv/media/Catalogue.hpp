#pragma once

#include <cstddef>
#include <cstdint>

namespace psbbv::media {

constexpr std::size_t CatalogueCapacity = 128;
constexpr std::size_t MediaPathCapacity = 256;
constexpr std::size_t MediaTitleCapacity = 128;

enum class MediaKind : std::uint8_t { Movie, Episode, Music, Video };
enum class MediaKindFilter : std::uint8_t { All, Movies, Episodes, Music, Videos };
enum class MediaSortOrder : std::uint8_t { CatalogueOrder, TitleAscending, YearDescending };

struct CatalogueQuery final {
    MediaKindFilter kind = MediaKindFilter::All;
    bool favouritesOnly = false;
    MediaSortOrder sort = MediaSortOrder::CatalogueOrder;
};

enum class CatalogueError : std::uint8_t {
    None, InvalidId, InvalidKind, MissingPath, PathTooLong,
    MissingTitle, TitleTooLong, Full, NotFound, InvalidPosition
};

struct MediaItem final {
    std::uint32_t id = 0;
    MediaKind kind = MediaKind::Movie;
    char path[MediaPathCapacity]{};
    char title[MediaTitleCapacity]{};
    std::uint16_t year = 0;
    std::uint32_t durationMs = 0;
    std::uint32_t resumePositionMs = 0;
    bool favourite = false;
};

struct CatalogueResult final {
    CatalogueError error = CatalogueError::None;
    constexpr bool ok() const noexcept { return error == CatalogueError::None; }
};

class Catalogue final {
public:
    CatalogueResult upsert(const MediaItem& item) noexcept;
    CatalogueResult remove(std::uint32_t id) noexcept;
    const MediaItem* find(std::uint32_t id) const noexcept;
    const MediaItem* at(std::size_t index) const noexcept;
    std::size_t size() const noexcept { return count_; }
    std::size_t searchTitle(const char* query, std::uint32_t* output,
                            std::size_t outputCapacity) const noexcept;
    std::size_t query(const CatalogueQuery& filter, std::uint32_t* output,
                      std::size_t outputCapacity) const noexcept;
    CatalogueResult setResumePosition(std::uint32_t id, std::uint32_t positionMs) noexcept;
    CatalogueResult setFavourite(std::uint32_t id, bool favourite) noexcept;

private:
    MediaItem items_[CatalogueCapacity]{};
    std::size_t count_ = 0;
};

} // namespace psbbv::media
