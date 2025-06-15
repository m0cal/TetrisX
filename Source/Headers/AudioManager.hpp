#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>

enum class SoundType
{
    BGM,
    CLEAR,
    DROP,
    FAILURE
};

class AudioManager
{
private:
    std::unordered_map<SoundType, sf::SoundBuffer> sound_buffers;
    std::unordered_map<SoundType, std::unique_ptr<sf::Sound>> sounds;
    sf::Music background_music;
    bool is_muted;
    float sound_volume;
    float music_volume;

public:
    AudioManager();
    ~AudioManager() = default;
    
    bool load_audio_files();
    void play_sound(SoundType type);
    void play_background_music();
    void stop_background_music();
    void set_sound_volume(float volume);
    void set_music_volume(float volume);
    void toggle_mute();
    bool is_background_music_playing() const;
    
private:
    std::string get_sound_file_path(SoundType type) const;
};
