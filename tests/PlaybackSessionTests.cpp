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
    PlaybackSession finished;
    assert(finished.beginResolve() == PlaybackError::None);
    assert(finished.markReady(120000) == PlaybackError::None);
    assert(finished.play() == PlaybackError::None);
    assert(finished.seek(119000) == PlaybackError::None);
    assert(finished.complete() == PlaybackError::None);
    assert(finished.state() == PlaybackState::Completed);
    assert(finished.position() == finished.duration());
    assert(finished.pause() == PlaybackError::InvalidTransition);
    assert(finished.seek(1000) == PlaybackError::InvalidTransition);
    assert(finished.beginResolve() == PlaybackError::None);
    assert(finished.position() == 0 && finished.duration() == 0);
    std::cout << "PSBBV media playback session tests passed\n";
}
