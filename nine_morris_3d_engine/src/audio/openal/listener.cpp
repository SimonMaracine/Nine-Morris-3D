#include "nine_morris_3d_engine/audio/openal/listener.hpp"

#include <cassert>

#include <AL/al.h>

#include "nine_morris_3d_engine/audio/openal/debug.hpp"

namespace sm {
    struct LookAtAndUp {
        glm::vec3 look_at;
        glm::vec3 up;
    };

    void listener::set_gain(float gain) {
        assert(gain >= 0.0f);

        alListenerf(AL_GAIN, gain);

        openal_debug::check_errors();
    }

    void listener::set_position(glm::vec3 position) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);

        openal_debug::check_errors();
    }

    void listener::set_velocity(glm::vec3 velocity) {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        openal_debug::check_errors();
    }

    void listener::set_look_at_and_up(glm::vec3 look_at, glm::vec3 up) {
        LookAtAndUp look_up_and_up;
        look_up_and_up.look_at = look_at;
        look_up_and_up.up = up;

        alListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(&look_up_and_up));  // Actually safe

        openal_debug::check_errors();
    }

    void listener::set_distance_model(DistanceModel distance_model) {
        ALenum result {};

        switch (distance_model) {
            case DistanceModel::None:
                result = AL_NONE;
                break;
            case DistanceModel::Exponent:
                result = AL_EXPONENT_DISTANCE;
                break;
            case DistanceModel::Inverse:
                result = AL_INVERSE_DISTANCE;
                break;
            case DistanceModel::Linear:
                result = AL_LINEAR_DISTANCE;
                break;
            case DistanceModel::ExponentClamped:
                result = AL_EXPONENT_DISTANCE_CLAMPED;
                break;
            case DistanceModel::InverseClamped:
                result = AL_INVERSE_DISTANCE_CLAMPED;
                break;
            case DistanceModel::LinearClamped:
                result = AL_LINEAR_DISTANCE_CLAMPED;
                break;
        }

        alDistanceModel(result);

        openal_debug::check_errors();
    }

    float listener::get_gain() {
        float gain {};
        alGetListenerf(AL_GAIN, &gain);

        openal_debug::check_errors();

        return gain;
    }

    glm::vec3 listener::get_position() {
        glm::vec3 position {};
        alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);

        openal_debug::check_errors();

        return position;
    }

    glm::vec3 listener::get_velocity() {
        glm::vec3 velocity {};
        alGetListener3f(AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);

        openal_debug::check_errors();

        return velocity;
    }

    glm::vec3 listener::get_look_at() {
        LookAtAndUp look_up_and_up {};
        alGetListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(&look_up_and_up));  // Actually safe

        openal_debug::check_errors();

        return look_up_and_up.look_at;
    }

    glm::vec3 listener::get_up() {
        LookAtAndUp look_up_and_up {};
        alGetListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(&look_up_and_up));  // Actually safe

        openal_debug::check_errors();

        return look_up_and_up.up;
    }

    listener::DistanceModel listener::get_distance_model() {
        const ALenum distance_model {alGetInteger(AL_DISTANCE_MODEL)};

        openal_debug::check_errors();

        switch (distance_model) {
            case AL_NONE:
                return DistanceModel::None;
            case AL_EXPONENT_DISTANCE:
                return DistanceModel::Exponent;
            case AL_INVERSE_DISTANCE:
                return DistanceModel::Inverse;
            case AL_LINEAR_DISTANCE:
                return DistanceModel::Linear;
            case AL_EXPONENT_DISTANCE_CLAMPED:
                return DistanceModel::ExponentClamped;
            case AL_INVERSE_DISTANCE_CLAMPED:
                return DistanceModel::InverseClamped;
            case AL_LINEAR_DISTANCE_CLAMPED:
                return DistanceModel::LinearClamped;
            default:
                assert(false);
                break;
        }

        return {};
    }
}
