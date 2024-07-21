#include "nine_morris_3d_engine/audio/music_track.hpp"

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/source.hpp"
#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

namespace sm {
    MusicTrack::MusicTrack(std::shared_ptr<SoundData> data) {
        setup(data);

        LOG_DEBUG("Loaded music track");
    }

    void MusicTrack::setup(std::shared_ptr<SoundData> data) {
        source = std::make_shared<AlSource>();
        buffer = std::make_shared<AlBuffer>(data);

        source->set_rolloff_factor(0.0f);
        source->set_looping(true);

        if (data->get_channels() != 2) {
            LOG_WARNING("Music track is not stereo");
        }
    }
}
