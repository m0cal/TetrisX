#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "ScoreSystem.hpp"

class LeaderboardManager
{
private:
    std::vector<ScoreEntry> high_scores;
    std::string leaderboard_file;
    static const size_t MAX_ENTRIES = 10;

public:
    LeaderboardManager(const std::string& filename = "leaderboard.txt");
    
    // Leaderboard management
    void add_score(const ScoreEntry& entry);
    void save_leaderboard();
    void load_leaderboard();
    void clear_leaderboard();
    
    // Getters
    const std::vector<ScoreEntry>& get_high_scores() const { return high_scores; }
    bool is_high_score(unsigned long score) const;
    size_t get_rank(unsigned long score) const; // Returns rank (1-based), 0 if not in top
    
    // Utility methods
    std::string format_score_entry(const ScoreEntry& entry, size_t rank) const;
    std::string format_duration(unsigned int seconds) const;
    std::string format_timestamp(const std::chrono::system_clock::time_point& timestamp) const;
    
private:
    void sort_scores();
    void trim_to_max_entries();
    bool parse_leaderboard_line(const std::string& line, ScoreEntry& entry);
    std::string serialize_score_entry(const ScoreEntry& entry) const;
};
