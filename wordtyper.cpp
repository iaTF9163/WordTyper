#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <cstring>

#define randomWord globalWordList[rand() % globalWordList.size()]

std::vector<std::string> globalBannedList = {"\t", "  ", ".", "/", "\\", "$", "%", "&", "{", "}", "(", ")", "[", "]", "DO_NOT_INCLUDE"};
std::vector<std::string> globalWordList = {"hello", "i love you!"};
float globalScore = 0;

int readWordList (std::string filePath, char delimeter, int maxCharLimit)
{
    if (filePath.empty()) {
        std::cout << "[!] Warning: You're playing without a wordlist. Feel free to set one using '--wordlist' <3" << std::endl;
        return 1;
    }

    std::ifstream file;
    file.open(filePath);
    if (!file.is_open()) {
        std::cout << "[!] Cannot open file " << filePath << ". Therefore not able to get a wordlist :(" << std::endl;
        return 1;
    }

    std::string word;
    while (std::getline(file, word, delimeter)) {
        int clear = 1;
        for (int i = 0; i < globalBannedList.size(); i++) {
            if (word.find(globalBannedList[i]) != std::string::npos) {
                std::cout << "[!] Word '" << word << "' was not able to join the list because it contained '"
                << globalBannedList[i] << "' which is a banned character." << std::endl;
                clear = 0;
                break;
            } else if (word.length() > maxCharLimit) {
                std::cout << "[!] Word '" << word << "' was not able to join the list because it was longer than the max limit."
                << std::endl;
                clear = 0;
                break;
            }
        }

        if (clear)
            globalWordList.push_back(word);
    }

    file.close();

    if (globalWordList.size() <= 2) {
        std::cout << "[!] The wordlist file was read, but cannot capture any words :(" << std::endl;
        return 1;
    }

    std::cout << "[*] Read the wordlist (" << filePath << "). " << globalWordList.size() << " word(s) was captured." << std::endl;

    return 0;
}

void updateBannedList (std::string filePath)
{
    globalBannedList.clear();

    std::ifstream file;
    file.open(filePath);
    if (!file.is_open())
        return;

    std::string bannedWord;
    while (std::getline(file, bannedWord))
        globalBannedList.push_back(bannedWord);

    file.close();
}

void argumentParser (int argc, const char* argv[])
{
    std::string wordlistFilePath = "wordlist.txt";
    char delimeter = '\n';
    int maxCharLimit = 12;
    std::string customBannedList = "";

    for (int i = 0; i < argc - 1; i++) {
        if (strncmp(argv[i + 1], "--", 2) == 0)
            continue;

        if (strcmp(argv[i], "--wordlist") == 0)
            wordlistFilePath = argv[i + 1];
        else if (strcmp(argv[i], "--delimeter") == 0)
            delimeter = argv[i + 1][0];
        else if (strcmp(argv[i], "--maxcharlimit") == 0)
            maxCharLimit = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "--banned") == 0) {
            customBannedList = argv[i + 1];
            updateBannedList(customBannedList);
        }
    }

    std::cout << "Wordlist File: " << wordlistFilePath << std::endl;
    std::cout << "Max Character Limit: " << maxCharLimit << std::endl;
    std::cout << "Wordlist Delimeter (as an integer for visibility): " << int(delimeter) << std::endl;
    std::cout << "Custom banned list: " << ((customBannedList.empty()) ? "No custom banned list." : customBannedList) << std::endl;

    readWordList(wordlistFilePath, delimeter, maxCharLimit);

    sleep(3);
}

int gameLoop ()
{
    std::string currentWord;
    char pressedKey;
    int currentScore;
    time_t startTime, endTime;

    while (1) {
        startTime = time(NULL);

        currentWord = randomWord;
        currentScore = currentWord.length();
        std::cout << "\r\x1b[K" << currentWord << "\r\t\t\t\t\t\t\t\t\t\t\tScore: " << globalScore << "\r";
        fflush(stdout);

        for (int i = 0; i < currentWord.length(); i++) {
            read(0, &pressedKey, 1);

            if (pressedKey == 127) {
                i -= 2;
                std::cout << "\x1b[1D" << std::flush;
                continue;
            } else if (pressedKey == 3) {
                std::cout << "\r\x1b[KGood Game! Hope you enjoyed :) Your Score: " << globalScore << "\r" << std::endl;
                return 0;
            }

            if (pressedKey < 32 || pressedKey > 126) {
                std::cout << "\r\x1b[K[!] A non-standart key (" << int(pressedKey) << ") was pressed. Skipping..." << std::endl;
                break;
            }

            if (pressedKey != currentWord[i]) {
                std::cout << "\x1b[31m";
                currentScore--;
            } else
                std::cout << "\x1b[32m";

            std::cout << currentWord[i] << "\x1b[0m" << std::flush;
        }

        endTime = time(NULL);

        globalScore += 0.0001 / (endTime - startTime + 0.0001) * currentScore;
    }
}

int main (int argc, const char *argv[])
{
    argumentParser(argc, argv);

    srand(time(NULL));
    system("stty raw && stty -echo");

    gameLoop();

    system("stty -raw && stty echo");

    return 0;
}
