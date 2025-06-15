#include "Headers/LeaderboardManager.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

LeaderboardManager::LeaderboardManager(const std::string& filename) : leaderboard_file(filename)
{
    load_leaderboard();
}

void LeaderboardManager::add_score(const ScoreEntry& entry)
{
    high_scores.push_back(entry);
    sort_scores();
    trim_to_max_entries();
    save_leaderboard();
}

void LeaderboardManager::save_leaderboard()
{
    std::ofstream file(leaderboard_file);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open leaderboard file for writing: " << leaderboard_file << std::endl;
        return;
    }
    
    for (const auto& entry : high_scores)
    {
        file << serialize_score_entry(entry) << std::endl;
    }
}

void LeaderboardManager::load_leaderboard()
{
    std::ifstream file(leaderboard_file);
    if (!file.is_open())
    {
        // File doesn't exist yet, which is fine for first run
        return;
    }
    
    high_scores.clear();
    std::string line;
    
    while (std::getline(file, line) && high_scores.size() < MAX_ENTRIES)
    {
        ScoreEntry entry;
        if (parse_leaderboard_line(line, entry))
        {
            high_scores.push_back(entry);
        }
    }
    
    sort_scores();
}

void LeaderboardManager::clear_leaderboard()
{
    high_scores.clear();
    save_leaderboard();
}

bool LeaderboardManager::is_high_score(unsigned long score) const
{
    if (high_scores.size() < MAX_ENTRIES)
    {
        return true; // Always a high score if leaderboard isn't full
    }
    
    return score > high_scores.back().score;
}

size_t LeaderboardManager::get_rank(unsigned long score) const
{
    for (size_t i = 0; i < high_scores.size(); ++i)
    {
        if (score >= high_scores[i].score)
        {
            return i + 1; // 1-based ranking
        }
    }
    
    if (high_scores.size() < MAX_ENTRIES)
    {
        return high_scores.size() + 1;
    }
    
    return 0; // Not in top scores
}

std::string LeaderboardManager::format_score_entry(const ScoreEntry& entry, size_t rank) const
{
    std::ostringstream oss;
    oss << std::setw(2) << rank << ". "
        << std::setw(12) << entry.player_name.substr(0, 12) << " "
        << std::setw(10) << entry.score << " "
        << "Lv" << std::setw(2) << entry.level << " "
        << std::setw(3) << entry.lines_cleared << "L "
        << format_duration(entry.game_duration_seconds);
    return oss.str();
}

std::string LeaderboardManager::format_duration(unsigned int seconds) const
{
    unsigned int minutes = seconds / 60;
    unsigned int secs = seconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setw(2) << secs;
    return oss.str();
}

std::string LeaderboardManager::format_timestamp(const std::chrono::system_clock::time_point& timestamp) const
{
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M");
    return oss.str();
}

void LeaderboardManager::sort_scores()
{
    std::sort(high_scores.begin(), high_scores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score; // Sort by score descending
        });
}

void LeaderboardManager::trim_to_max_entries()
{
    if (high_scores.size() > MAX_ENTRIES)
    {
        high_scores.resize(MAX_ENTRIES);
    }
}

bool LeaderboardManager::parse_leaderboard_line(const std::string& line, ScoreEntry& entry)
{
    std::istringstream iss(line);
    std::string timestamp_str;
    
    // Format: score|lines|level|name|tetrominos|duration|timestamp
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(iss, token, '|'))
    {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 7)
    {
        return false;
    }
    
    try
    {
        entry.score = std::stoul(tokens[0]);
        entry.lines_cleared = std::stoul(tokens[1]);
        entry.level = std::stoul(tokens[2]);
        entry.player_name = tokens[3];
        entry.tetrominos_placed = std::stoul(tokens[4]);
        entry.game_duration_seconds = std::stoul(tokens[5]);
        
        // Parse timestamp (simple epoch time)
        auto timestamp_epoch = std::stoll(tokens[6]);
        entry.timestamp = std::chrono::system_clock::from_time_t(timestamp_epoch);
        
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

std::string LeaderboardManager::serialize_score_entry(const ScoreEntry& entry) const
{
    std::ostringstream oss;
    auto timestamp_epoch = std::chrono::system_clock::to_time_t(entry.timestamp);
    
    oss << entry.score << "|"
        << entry.lines_cleared << "|"
        << entry.level << "|"
        << entry.player_name << "|"
        << entry.tetrominos_placed << "|"
        << entry.game_duration_seconds << "|"
        << timestamp_epoch;
    
    return oss.str();
}
