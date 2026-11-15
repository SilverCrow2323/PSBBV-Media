#include "psbbv/media/Catalogue.hpp"

#include <cstring>

namespace psbbv::media {
namespace {
bool boundedLength(const char* value, std::size_t capacity, std::size_t& length) noexcept {
    if (value == nullptr) return false;
    length = 0;
    while (length < capacity && value[length] != '\0') ++length;
    return length < capacity;
}
char lowerAscii(char value) noexcept {
    return value >= 'A' && value <= 'Z' ? static_cast<char>(value - 'A' + 'a') : value;
}
int compareTitle(const char* left, const char* right) noexcept {
    for (std::size_t i = 0; i < MediaTitleCapacity; ++i) {
        const char a = lowerAscii(left[i]);
        const char b = lowerAscii(right[i]);
        if (a < b) return -1;
        if (a > b) return 1;
        if (a == '\0') return 0;
    }
    return 0;
}
bool containsInsensitive(const char* text, const char* query) noexcept {
    std::size_t textLength = 0, queryLength = 0;
    if (!boundedLength(text, MediaTitleCapacity, textLength) ||
        !boundedLength(query, MediaTitleCapacity, queryLength) || queryLength == 0) return false;
    if (queryLength > textLength) return false;
    for (std::size_t start = 0; start + queryLength <= textLength; ++start) {
        std::size_t i = 0;
        while (i < queryLength && lowerAscii(text[start + i]) == lowerAscii(query[i])) ++i;
        if (i == queryLength) return true;
    }
    return false;
}
bool matchesKind(MediaKind kind, MediaKindFilter filter) noexcept {
    switch (filter) {
        case MediaKindFilter::All: return true;
        case MediaKindFilter::Movies: return kind == MediaKind::Movie;
        case MediaKindFilter::Episodes: return kind == MediaKind::Episode;
        case MediaKindFilter::Music: return kind == MediaKind::Music;
        case MediaKindFilter::Videos: return kind == MediaKind::Video;
    }
    return false;
}
bool comesBefore(const MediaItem& left, const MediaItem& right, MediaSortOrder sort) noexcept {
    if (sort == MediaSortOrder::TitleAscending) {
        const int titleOrder = compareTitle(left.title, right.title);
        return titleOrder == 0 ? left.id < right.id : titleOrder < 0;
    }
    if (sort == MediaSortOrder::YearDescending) {
        if (left.year != right.year) return left.year > right.year;
        return left.id < right.id;
    }
    return false;
}
}

CatalogueResult Catalogue::upsert(const MediaItem& item) noexcept {
    if (item.id == 0) return {CatalogueError::InvalidId};
    if (item.kind > MediaKind::Video) return {CatalogueError::InvalidKind};
    std::size_t pathLength = 0, titleLength = 0;
    if (!boundedLength(item.path, MediaPathCapacity, pathLength))
        return {item.path[0] == '\0' ? CatalogueError::MissingPath : CatalogueError::PathTooLong};
    if (pathLength == 0) return {CatalogueError::MissingPath};
    if (!boundedLength(item.title, MediaTitleCapacity, titleLength))
        return {CatalogueError::TitleTooLong};
    if (titleLength == 0) return {CatalogueError::MissingTitle};

    for (std::size_t i = 0; i < count_; ++i) {
        if (items_[i].id == item.id) {
            const std::uint32_t savedPosition = items_[i].resumePositionMs;
            const bool savedFavourite = items_[i].favourite;
            items_[i] = item;
            items_[i].resumePositionMs = savedPosition;
            items_[i].favourite = savedFavourite;
            if (items_[i].resumePositionMs > items_[i].durationMs && items_[i].durationMs != 0)
                items_[i].resumePositionMs = items_[i].durationMs;
            return {};
        }
    }
    if (count_ == CatalogueCapacity) return {CatalogueError::Full};
    items_[count_] = item;
    if (items_[count_].resumePositionMs > items_[count_].durationMs && items_[count_].durationMs != 0)
        items_[count_].resumePositionMs = items_[count_].durationMs;
    ++count_;
    return {};
}

CatalogueResult Catalogue::remove(std::uint32_t id) noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
        if (items_[i].id == id) {
            for (std::size_t j = i + 1; j < count_; ++j) items_[j - 1] = items_[j];
            --count_;
            items_[count_] = MediaItem{};
            return {};
        }
    }
    return {CatalogueError::NotFound};
}

const MediaItem* Catalogue::find(std::uint32_t id) const noexcept {
    for (std::size_t i = 0; i < count_; ++i) if (items_[i].id == id) return &items_[i];
    return nullptr;
}

const MediaItem* Catalogue::at(std::size_t index) const noexcept {
    return index < count_ ? &items_[index] : nullptr;
}

std::size_t Catalogue::searchTitle(const char* query, std::uint32_t* output,
                                   std::size_t outputCapacity) const noexcept {
    if (query == nullptr || output == nullptr || outputCapacity == 0) return 0;
    std::size_t matches = 0;
    for (std::size_t i = 0; i < count_ && matches < outputCapacity; ++i) {
        if (containsInsensitive(items_[i].title, query)) output[matches++] = items_[i].id;
    }
    return matches;
}

std::size_t Catalogue::query(const CatalogueQuery& filter, std::uint32_t* output,
                             std::size_t outputCapacity) const noexcept {
    if (output == nullptr || outputCapacity == 0 ||
        filter.kind > MediaKindFilter::Videos ||
        filter.sort > MediaSortOrder::YearDescending) return 0;

    std::uint32_t ordered[CatalogueCapacity]{};
    std::size_t matches = 0;
    for (std::size_t i = 0; i < count_; ++i) {
        const MediaItem& item = items_[i];
        if (!matchesKind(item.kind, filter.kind) || (filter.favouritesOnly && !item.favourite))
            continue;
        std::size_t position = matches;
        if (filter.sort != MediaSortOrder::CatalogueOrder) {
            while (position > 0) {
                const MediaItem* previous = find(ordered[position - 1]);
                if (previous == nullptr || !comesBefore(item, *previous, filter.sort)) break;
                ordered[position] = ordered[position - 1];
                --position;
            }
        }
        ordered[position] = item.id;
        ++matches;
    }

    const std::size_t written = matches < outputCapacity ? matches : outputCapacity;
    for (std::size_t i = 0; i < written; ++i) output[i] = ordered[i];
    return written;
}

CatalogueResult Catalogue::setResumePosition(std::uint32_t id, std::uint32_t positionMs) noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
        if (items_[i].id == id) {
            if (items_[i].durationMs != 0 && positionMs > items_[i].durationMs)
                return {CatalogueError::InvalidPosition};
            items_[i].resumePositionMs = positionMs;
            return {};
        }
    }
    return {CatalogueError::NotFound};
}

CatalogueResult Catalogue::setFavourite(std::uint32_t id, bool favourite) noexcept {
    for (std::size_t i = 0; i < count_; ++i) {
        if (items_[i].id == id) { items_[i].favourite = favourite; return {}; }
    }
    return {CatalogueError::NotFound};
}

} // namespace psbbv::media
