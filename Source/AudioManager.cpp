#include "Headers/AudioManager.hpp"
#include <iostream>

AudioManager::AudioManager() : is_muted(false), sound_volume(70.0f), music_volume(50.0f)
{
    load_audio_files();
}

bool AudioManager::load_audio_files()
{
    // Load sound effects
    if (!sound_buffers[SoundType::CLEAR].loadFromFile("Resources/Audio/clear.mp3"))
    {
        std::cerr << "Failed to load clear.mp3" << std::endl;
        return false;
    }
    
    if (!sound_buffers[SoundType::DROP].loadFromFile("Resources/Audio/drop.mp3"))
    {
        std::cerr << "Failed to load drop.mp3" << std::endl;
        return false;
    }
    
    if (!sound_buffers[SoundType::FAILURE].loadFromFile("Resources/Audio/failure.mp3"))
    {
        std::cerr << "Failed to load failure.mp3" << std::endl;
        return false;
    }
    
    // Create sound objects with sound buffers
    sounds[SoundType::CLEAR] = std::make_unique<sf::Sound>(sound_buffers[SoundType::CLEAR]);
    sounds[SoundType::DROP] = std::make_unique<sf::Sound>(sound_buffers[SoundType::DROP]);
    sounds[SoundType::FAILURE] = std::make_unique<sf::Sound>(sound_buffers[SoundType::FAILURE]);
    
    // Set initial volumes
    sounds[SoundType::CLEAR]->setVolume(sound_volume);
    sounds[SoundType::DROP]->setVolume(sound_volume);
    sounds[SoundType::FAILURE]->setVolume(sound_volume);
    
    // Load background music
    if (!background_music.openFromFile("Resources/Audio/bgm.mp3"))
    {
        std::cerr << "Failed to load bgm.mp3" << std::endl;
        return false;
    }
    
    background_music.setVolume(music_volume);
    background_music.setLooping(true);
    
    return true;
}

void AudioManager::play_sound(SoundType type)
{
    if (is_muted || type == SoundType::BGM)
        return;
        
    auto it = sounds.find(type);
    if (it != sounds.end() && it->second)
    {
        it->second->play();
    }
}

void AudioManager::play_background_music()
{
    if (!is_muted && background_music.getStatus() != sf::SoundSource::Status::Playing)
    {
        background_music.play();
    }
}

void AudioManager::stop_background_music()
{
    background_music.stop();
}

void AudioManager::set_sound_volume(float volume)
{
    sound_volume = std::max(0.0f, std::min(100.0f, volume));
    
    for (auto& pair : sounds)
    {
        if (pair.second)
        {
            pair.second->setVolume(is_muted ? 0.0f : sound_volume);
        }
    }
}

void AudioManager::set_music_volume(float volume)
{
    music_volume = std::max(0.0f, std::min(100.0f, volume));
    background_music.setVolume(is_muted ? 0.0f : music_volume);
}

void AudioManager::toggle_mute()
{
    is_muted = !is_muted;
    
    // Update sound volumes
    for (auto& pair : sounds)
    {
        if (pair.second)
        {
            pair.second->setVolume(is_muted ? 0.0f : sound_volume);
        }
    }
    
    // Update music volume
    background_music.setVolume(is_muted ? 0.0f : music_volume);
    
    // Stop music if muted, resume if unmuted
    if (is_muted)
    {
        background_music.pause();
    }
    else if (background_music.getStatus() == sf::SoundSource::Status::Paused)
    {
        background_music.play();
    }
}

bool AudioManager::is_background_music_playing() const
{
    return background_music.getStatus() == sf::SoundSource::Status::Playing;
}

std::string AudioManager::get_sound_file_path(SoundType type) const
{
    switch (type)
    {
        case SoundType::BGM:
            return "Source/Resources/Audio/bgm.mp3";
        case SoundType::CLEAR:
            return "Source/Resources/Audio/clear.mp3";
        case SoundType::DROP:
            return "Source/Resources/Audio/drop.mp3";
        case SoundType::FAILURE:
            return "Source/Resources/Audio/failure.mp3";
        default:
            return "";
    }
}
