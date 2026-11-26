#include "psbbv/media/PlaybackSession.hpp"
#include <cassert>
#include <iostream>
using namespace psbbv::media;
int main() {
    PlaybackSession session;
    assert(session.play() == PlaybackError::InvalidTransition);
    assert(session.beginResolve() == PlaybackError::None);
    assert(session.markReady(0) == PlaybackError::InvalidDuration);
    assert(session.markReady(240000) == PlaybackError::None);
    assert(session.play() == PlaybackError::None);
    assert(session.seek(90000) == PlaybackError::None);
    assert(session.seek(240001) == PlaybackError::InvalidPosition);
    assert(session.pause() == PlaybackError::None);
    assert(session.seek(45000) == PlaybackError::None);
    assert(session.resume() == PlaybackError::None);
    assert(session.stop() == PlaybackError::None);
    assert(session.beginResolve() == PlaybackError::None);
    assert(session.fail() == PlaybackError::None);
    assert(session.state() == PlaybackState::Error);
    std::cout << "PSBBV media playback session tests passed\n";
}
