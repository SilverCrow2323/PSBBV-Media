#include "psbbv/media/PlaybackSession.hpp"
namespace psbbv::media {
PlaybackError PlaybackSession::beginResolve() noexcept {
    if (state_ != PlaybackState::Idle && state_ != PlaybackState::Stopped && state_ != PlaybackState::Error)
        return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Resolving; durationMs_ = 0; positionMs_ = 0;
    return PlaybackError::None;
}
PlaybackError PlaybackSession::markReady(std::uint32_t durationMs) noexcept {
    if (state_ != PlaybackState::Resolving) return PlaybackError::InvalidTransition;
    if (durationMs == 0) return PlaybackError::InvalidDuration;
    durationMs_ = durationMs; positionMs_ = 0; state_ = PlaybackState::Ready;
    return PlaybackError::None;
}
PlaybackError PlaybackSession::play() noexcept {
    if (state_ != PlaybackState::Ready) return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Playing; return PlaybackError::None;
}
PlaybackError PlaybackSession::pause() noexcept {
    if (state_ != PlaybackState::Playing) return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Paused; return PlaybackError::None;
}
PlaybackError PlaybackSession::resume() noexcept {
    if (state_ != PlaybackState::Paused) return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Playing; return PlaybackError::None;
}
PlaybackError PlaybackSession::stop() noexcept {
    if (state_ != PlaybackState::Ready && state_ != PlaybackState::Playing && state_ != PlaybackState::Paused)
        return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Stopped; return PlaybackError::None;
}
PlaybackError PlaybackSession::fail() noexcept {
    if (state_ == PlaybackState::Idle || state_ == PlaybackState::Stopped || state_ == PlaybackState::Error)
        return PlaybackError::InvalidTransition;
    state_ = PlaybackState::Error; return PlaybackError::None;
}
PlaybackError PlaybackSession::seek(std::uint32_t positionMs) noexcept {
    if (state_ != PlaybackState::Playing && state_ != PlaybackState::Paused)
        return PlaybackError::InvalidTransition;
    if (positionMs > durationMs_) return PlaybackError::InvalidPosition;
    positionMs_ = positionMs; return PlaybackError::None;
}
} // namespace psbbv::media
