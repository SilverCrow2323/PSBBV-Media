#include "psbbv/media/Catalogue.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {
void require(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
}
psbbv::media::MediaItem item(std::uint32_t id, const char* title, const char* path,
                             psbbv::media::MediaKind kind = psbbv::media::MediaKind::Movie,
                             std::uint16_t year = 2000) {
    psbbv::media::MediaItem value{};
    value.id = id;
    value.kind = kind;
    value.year = year;
    std::strncpy(value.title, title, sizeof(value.title) - 1);
    std::strncpy(value.path, path, sizeof(value.path) - 1);
    value.durationMs = 100000;
    return value;
}
}

int main() {
    using namespace psbbv::media;
    Catalogue catalogue;
    require(catalogue.upsert(item(1, "The Matrix", "ux0:video/matrix.mkv")).ok(),
            "valid movie is inserted");
    require(catalogue.upsert(item(2, "Blade Runner", "ux0:video/blade.mkv",
                                  MediaKind::Movie, 1982)).ok(),
            "second movie is inserted");
    require(catalogue.upsert(item(3, "Opening Theme", "ux0:music/theme.mp3",
                                  MediaKind::Music, 2020)).ok(),
            "music item is inserted");
    require(catalogue.upsert(item(4, "Episode One", "ux0:video/episode.mp4",
                                  MediaKind::Episode, 2024)).ok(),
            "episode item is inserted");
    require(catalogue.size() == 4, "catalogue counts entries");
    require(catalogue.find(1) != nullptr &&
            std::strcmp(catalogue.find(1)->title, "The Matrix") == 0,
            "lookup returns matching metadata");

    std::uint32_t matches[8]{};
    require(catalogue.searchTitle("matrix", matches, 8) == 1 && matches[0] == 1,
            "title search is case-insensitive");
    require(catalogue.setFavourite(2, true).ok() && catalogue.find(2)->favourite,
            "favourite state is mutable");
    require(catalogue.setResumePosition(1, 45000).ok() &&
            catalogue.find(1)->resumePositionMs == 45000,
            "resume position is stored");
    require(catalogue.setFavourite(1, true).ok(), "first item can be favourited");
    require(catalogue.setResumePosition(1, 100001).error == CatalogueError::InvalidPosition,
            "resume position cannot exceed known duration");

    MediaItem replacement = item(1, "The Matrix Reloaded", "ux0:video/matrix2.mkv");
    require(catalogue.upsert(replacement).ok() && catalogue.size() == 4 &&
            std::strcmp(catalogue.find(1)->title, "The Matrix Reloaded") == 0,
            "upsert replaces matching identity without duplication");
    require(catalogue.find(1)->resumePositionMs == 45000 && catalogue.find(1)->favourite,
            "metadata refresh preserves user resume and favourite state");

    CatalogueQuery movieQuery{};
    movieQuery.kind = MediaKindFilter::Movies;
    movieQuery.sort = MediaSortOrder::TitleAscending;
    require(catalogue.query(movieQuery, matches, 8) == 2 &&
            matches[0] == 2 && matches[1] == 1,
            "movie filter sorts titles case-insensitively");
    CatalogueQuery favouriteQuery{};
    favouriteQuery.favouritesOnly = true;
    favouriteQuery.sort = MediaSortOrder::YearDescending;
    require(catalogue.query(favouriteQuery, matches, 8) == 2 &&
            matches[0] == 1 && matches[1] == 2,
            "favourite filter sorts newest year first");
    CatalogueQuery musicQuery{};
    musicQuery.kind = MediaKindFilter::Music;
    require(catalogue.query(musicQuery, matches, 1) == 1 && matches[0] == 3,
            "kind filter respects bounded output capacity");
    CatalogueQuery invalidQuery{};
    invalidQuery.sort = static_cast<MediaSortOrder>(99);
    require(catalogue.query(invalidQuery, matches, 8) == 0,
            "invalid sort order is rejected");

    require(catalogue.remove(2).ok() && catalogue.size() == 3,
            "remove compacts catalogue");
    require(catalogue.remove(99).error == CatalogueError::NotFound,
            "unknown removal is reported");

    MediaItem invalid{};
    require(catalogue.upsert(invalid).error == CatalogueError::InvalidId,
            "zero identity is rejected");
    invalid.id = 5;
    require(catalogue.upsert(invalid).error == CatalogueError::MissingPath,
            "missing path is rejected");
    require(catalogue.at(3) == nullptr, "out-of-range index is safe");

    std::cout << "PSBBV Media catalogue: all checks passed.\n";
    return EXIT_SUCCESS;
}
