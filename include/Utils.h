#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    void clearScreen();
    void pauseScreen();
    void printHeader(const std::string& title);
    void printDivider(char ch = '=', int length = 40);

    // Robust input validation functions
    int getValidatedInt(const std::string& prompt, int minVal = 0, int maxVal = 1000000000);
    double getValidatedDouble(const std::string& prompt, double minVal = 0.0, double maxVal = 100.0);
    std::string getValidatedString(const std::string& prompt, bool allowEmpty = false);
    std::string getValidatedPhone(const std::string& prompt);
    bool getConfirmation(const std::string& prompt);
    std::string trim(const std::string& str);
}

#endif // UTILS_H
