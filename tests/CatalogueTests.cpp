#include "psbbv/media/Catalogue.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {
void require(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
}
psbbv::media::MediaItem item(std::uint32_t id, const char* title, const char* path) {
    psbbv::media::MediaItem value{};
    value.id = id;
    value.kind = psbbv::media::MediaKind::Movie;
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
    require(catalogue.upsert(item(2, "Blade Runner", "ux0:video/blade.mkv")).ok(),
            "second movie is inserted");
    require(catalogue.size() == 2, "catalogue counts entries");
    require(catalogue.find(1) != nullptr &&
            std::strcmp(catalogue.find(1)->title, "The Matrix") == 0,
            "lookup returns matching metadata");

    std::uint32_t matches[4]{};
    require(catalogue.searchTitle("matrix", matches, 4) == 1 && matches[0] == 1,
            "title search is case-insensitive");
    require(catalogue.setFavourite(2, true).ok() && catalogue.find(2)->favourite,
            "favourite state is mutable");
    require(catalogue.setResumePosition(1, 45000).ok() &&
            catalogue.find(1)->resumePositionMs == 45000,
            "resume position is stored");
    require(catalogue.setResumePosition(1, 100001).error == CatalogueError::InvalidPosition,
            "resume position cannot exceed known duration");

    MediaItem replacement = item(1, "The Matrix Reloaded", "ux0:video/matrix2.mkv");
    require(catalogue.upsert(replacement).ok() && catalogue.size() == 2 &&
            std::strcmp(catalogue.find(1)->title, "The Matrix Reloaded") == 0,
            "upsert replaces matching identity without duplication");
    require(catalogue.remove(2).ok() && catalogue.size() == 1,
            "remove compacts catalogue");
    require(catalogue.remove(99).error == CatalogueError::NotFound,
            "unknown removal is reported");

    MediaItem invalid{};
    require(catalogue.upsert(invalid).error == CatalogueError::InvalidId,
            "zero identity is rejected");
    invalid.id = 3;
    require(catalogue.upsert(invalid).error == CatalogueError::MissingPath,
            "missing path is rejected");
    require(catalogue.at(1) == nullptr, "out-of-range index is safe");

    std::cout << "PSBBV Media catalogue: all checks passed.\n";
    return EXIT_SUCCESS;
}
