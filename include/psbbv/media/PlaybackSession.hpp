#pragma once
#include <cstdint>
namespace psbbv::media {
enum class PlaybackState : std::uint8_t { Idle, Resolving, Ready, Playing, Paused, Stopped, Error };
enum class PlaybackError : std::uint8_t { None, InvalidTransition, InvalidDuration, InvalidPosition };
class PlaybackSession final {
public:
    PlaybackError beginResolve() noexcept;
    PlaybackError markReady(std::uint32_t durationMs) noexcept;
    PlaybackError play() noexcept;
    PlaybackError pause() noexcept;
    PlaybackError resume() noexcept;
    PlaybackError stop() noexcept;
    PlaybackError fail() noexcept;
    PlaybackError seek(std::uint32_t positionMs) noexcept;
    PlaybackState state() const noexcept { return state_; }
    std::uint32_t duration() const noexcept { return durationMs_; }
    std::uint32_t position() const noexcept { return positionMs_; }
private:
    PlaybackState state_ = PlaybackState::Idle;
    std::uint32_t durationMs_ = 0;
    std::uint32_t positionMs_ = 0;
};
} // namespace psbbv::media
