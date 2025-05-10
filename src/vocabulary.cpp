#include "vocabulary.h"
#include "constants.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

VocabularyManager::VocabularyManager() : words_per_day(10) {
    loadVocabulary();
    loadProgress();
}

void VocabularyManager::addWord(const std::string& word, const std::string& meaning) {
    // Check if word already exists
    auto it = std::find_if(vocabulary_list.begin(), vocabulary_list.end(),
        [&word](const Vocabulary& v) { return v.word == word; });
    
    if (it == vocabulary_list.end()) {
        vocabulary_list.push_back({word, meaning});
        
        // Initialize progress for new word
        word_progress.push_back({word, 0, time(nullptr), false});
        
        saveVocabulary();
        saveProgress();
        std::cout << "Word added successfully!\n";
    } else {
        std::cout << "Word already exists in vocabulary.\n";
    }
}

void VocabularyManager::reciteWords(int words_per_day_param) {
    // Use parameter if provided, otherwise use stored value
    int count_to_review = (words_per_day_param > 0) ? words_per_day_param : words_per_day;
    auto todays_words = getTodaysWords(count_to_review);
    
    if (todays_words.empty()) {
        std::cout << "No words to review today. Good job!\n";
        return;
    }
    
    std::cout << "\nToday's vocabulary review (" << todays_words.size() << " words):\n";
    
    for (const auto& vocab : todays_words) {
        std::cout << "\nWord: " << vocab.word << "\nDo you know the meaning? (y/n): ";
        char answer;
        std::cin >> answer;
        
        if (tolower(answer) == 'y') {
            std::cout << "Meaning: " << vocab.meaning << "\nIs this correct? (y/n): ";
            std::cin >> answer;
            
            if (tolower(answer) == 'y') {
                updateProgress(vocab.word, true);
            } else {
                updateProgress(vocab.word, false);
            }
        } else {
            std::cout << "Meaning: " << vocab.meaning << "\n";
            updateProgress(vocab.word, false);
        }
    }
    
    saveProgress();
}

void VocabularyManager::setWordsPerDay(int count) {
    words_per_day = count;
    std::cout << "Set daily word count to " << words_per_day << "\n";
}

void VocabularyManager::loadVocabulary() {
    std::ifstream file(VOCAB_FILE);
    if (!file.is_open()) return;
    
    vocabulary_list.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter = line.find('|');
        if (delimiter != std::string::npos) {
            vocabulary_list.push_back({
                line.substr(0, delimiter),
                line.substr(delimiter + 1)
            });
        }
    }
}

void VocabularyManager::saveVocabulary() {
    std::ofstream file(VOCAB_FILE);
    if (!file.is_open()) return;
    
    for (const auto& vocab : vocabulary_list) {
        file << vocab.word << "|" << vocab.meaning << "\n";
    }
}

void VocabularyManager::loadProgress() {
    std::ifstream file(PROGRESS_FILE);
    if (!file.is_open()) return;
    
    word_progress.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        WordProgress progress;
        std::string date_str;
        
        if (std::getline(iss, progress.word, '|') &&
            iss >> progress.consecutive_correct &&
            iss.ignore() &&
            std::getline(iss, date_str, '|') &&
            iss >> progress.learned) {
            
            progress.next_review_date = std::stol(date_str);
            word_progress.push_back(progress);
        }
    }
}

void VocabularyManager::saveProgress() {
    std::ofstream file(PROGRESS_FILE);
    if (!file.is_open()) return;
    
    for (const auto& progress : word_progress) {
        file << progress.word << "|"
             << progress.consecutive_correct << "|"
             << progress.next_review_date << "|"
             << progress.learned << "\n";
    }
    
    // Update learned words file
    std::ofstream learned_file(LEARNED_FILE);
    if (learned_file.is_open()) {
        for (const auto& progress : word_progress) {
            if (progress.learned) {
                auto it = std::find_if(vocabulary_list.begin(), vocabulary_list.end(),
                    [&progress](const Vocabulary& v) { return v.word == progress.word; });
                if (it != vocabulary_list.end()) {
                    learned_file << progress.word << "|" << it->meaning << "\n";
                }
            }
        }
    }
}

void VocabularyManager::updateProgress(const std::string& word, bool known) {
    auto it = std::find_if(word_progress.begin(), word_progress.end(),
        [&word](const WordProgress& wp) { return wp.word == word; });
    
    if (it != word_progress.end()) {
        if (known) {
            it->consecutive_correct++;
            if (it->consecutive_correct >= 3) {
                markAsLearned(word);
            } else {
                // Schedule next review in 2 days
                it->next_review_date = time(nullptr) + (2 * 24 * 60 * 60);
            }
        } else {
            it->consecutive_correct = 0;
            // Schedule next review for tomorrow
            it->next_review_date = time(nullptr) + (1 * 24 * 60 * 60);
        }
    }
}

void VocabularyManager::markAsLearned(const std::string& word) {
    auto it = std::find_if(word_progress.begin(), word_progress.end(),
        [&word](const WordProgress& wp) { return wp.word == word; });
    
    if (it != word_progress.end()) {
        it->learned = true;
        std::cout << "Congratulations! You've learned the word: " << word << "\n";
    }
}

bool VocabularyManager::shouldReviewToday(time_t review_date) {
    time_t now = time(nullptr);
    return difftime(review_date, now) <= 0;
}

std::vector<Vocabulary> VocabularyManager::getTodaysWords(int count) {
    std::vector<Vocabulary> result;
    int words_added = 0;
    
    for (const auto& progress : word_progress) {
        if (!progress.learned && shouldReviewToday(progress.next_review_date)) {
            auto it = std::find_if(vocabulary_list.begin(), vocabulary_list.end(),
                [&progress](const Vocabulary& v) { return v.word == progress.word; });
            
            if (it != vocabulary_list.end()) {
                result.push_back(*it);
                words_added++;
                if (words_added >= count) break;
            }
        }
    }
    
    return result;
}