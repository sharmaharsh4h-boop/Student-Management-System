#include "Utils.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>

namespace Utils {

void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pauseScreen() {
    std::cout << "\nPress Enter to continue...";
    // Clear any leftover characters in the buffer before waiting
    std::cin.sync();
    std::string dummy;
    std::getline(std::cin, dummy);
}

void printDivider(char ch, int length) {
    for (int i = 0; i < length; ++i) {
        std::cout << ch;
    }
    std::cout << "\n";
}

void printHeader(const std::string& title) {
    printDivider('=', 40);
    // Center title in 40 characters
    int spaces = (40 - static_cast<int>(title.length())) / 2;
    if (spaces < 0) spaces = 0;
    for (int i = 0; i < spaces; ++i) std::cout << ' ';
    std::cout << title << "\n";
    printDivider('=', 40);
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

int getValidatedInt(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                // Clear remaining input up to newline
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << " [Error] Value must be between " << minVal << " and " << maxVal << ". Please try again.\n";
            }
        } else {
            std::cout << " [Error] Invalid number! Please enter digits only.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double getValidatedDouble(const std::string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << " [Error] Marks must be between " << minVal << " and " << maxVal << ". Please try again.\n";
            }
        } else {
            std::cout << " [Error] Invalid number! Please enter a valid decimal number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getValidatedString(const std::string& prompt, bool allowEmpty) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        input = trim(input);
        if (!input.empty() || allowEmpty) {
            return input;
        }
        std::cout << " [Error] This field cannot be left blank. Please try again.\n";
    }
}

std::string getValidatedPhone(const std::string& prompt) {
    std::string phone;
    while (true) {
        phone = getValidatedString(prompt, false);
        // Ensure phone contains only digits and is between 10 and 15 digits
        bool allDigits = !phone.empty() && std::all_of(phone.begin(), phone.end(), [](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
        });

        if (allDigits && phone.length() >= 10 && phone.length() <= 15) {
            return phone;
        }
        std::cout << " [Error] Please enter a valid phone number (10 to 15 digits, numbers only).\n";
    }
}

bool getConfirmation(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (Y/N): ";
        std::string choice;
        std::getline(std::cin, choice);
        choice = trim(choice);
        if (choice == "Y" || choice == "y" || choice == "YES" || choice == "yes") {
            return true;
        } else if (choice == "N" || choice == "n" || choice == "NO" || choice == "no") {
            return false;
        }
        std::cout << " [Error] Please enter Y or N.\n";
    }
}

} // namespace Utils
