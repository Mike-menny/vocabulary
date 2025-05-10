#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <string>
#include <vector>
#include <ctime>

struct WordProgress {
    std::string word;
    int consecutive_correct;
    time_t next_review_date;
    bool learned;
};

struct Vocabulary {
    std::string word;
    std::string meaning;
};

class VocabularyManager {
public:
    VocabularyManager();
    
    void addWord(const std::string& word, const std::string& meaning);
    void reciteWords(int words_per_day = -1);
    void setWordsPerDay(int count);
    void saveProgress();
    void loadProgress();
    
private:
    std::vector<Vocabulary> vocabulary_list;
    std::vector<WordProgress> word_progress;
    int words_per_day;
    
    void loadVocabulary();
    void saveVocabulary();
    void updateProgress(const std::string& word, bool known);
    void markAsLearned(const std::string& word);
    bool shouldReviewToday(time_t review_date);
    std::vector<Vocabulary> getTodaysWords(int count);  // Added count parameter here
};

#endif