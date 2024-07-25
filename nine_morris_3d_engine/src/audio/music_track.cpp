#include "nine_morris_3d_engine/audio/music_track.hpp"

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

namespace sm {
    MusicTrack::MusicTrack(std::shared_ptr<SoundData> data)
        : buffer(std::make_unique<AlBuffer>(data)) {
        if (data->get_channels() != 2) {
            LOG_WARNING("Music track is not stereo");
        }

        LOG_DEBUG("Loaded music track");
    }
}
