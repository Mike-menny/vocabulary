#include "vocabulary.h"
#include <iostream>
#include <limits>
#include <string>

void displayMenu() {
    std::cout << "\nVocabulary Recitation App\n";
    std::cout << "1. Add new word\n";
    std::cout << "2. Recite words\n";
    std::cout << "3. Set daily word count\n";
    std::cout << "4. Exit\n";
    std::cout << "Choose an option: ";
}

int getValidInput() {
    int choice;
    while (true) {
        if (std::cin >> choice) {
            // Input was a number
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (choice >= 1 && choice <= 4) {
                return choice;
            }
        } else {
            // Input was not a number
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "Invalid choice. Please enter a number between 1 and 4: ";
    }
}

int main() {
    VocabularyManager manager;
    int choice;
    
    do {
        displayMenu();
        choice = getValidInput();
        
        switch (choice) {
            case 1: {
                std::string word, meaning;
                std::cout << "Enter word: ";
                std::getline(std::cin, word);
                std::cout << "Enter meaning: ";
                std::getline(std::cin, meaning);
                manager.addWord(word, meaning);
                break;
            }
            case 2:
                manager.reciteWords(); // Will use the stored words_per_day value
                break;
            case 3: {
                int count;
                std::cout << "Enter number of words per day: ";
                while (!(std::cin >> count) ){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Please enter a number: ";
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                manager.setWordsPerDay(count);
                break;
            }
            case 4:
                std::cout << "Goodbye!\n";
                break;
        }
    } while (choice != 4);
    
    return 0;
}