#include "StudentManager.h"
#include "Utils.h"
#include <iostream>

void printMainMenu() {
    Utils::printDivider('=', 40);
    std::cout << "       STUDENT MANAGEMENT SYSTEM\n";
    Utils::printDivider('=', 40);
    std::cout << "1. Add New Student\n";
    std::cout << "2. Display All Students\n";
    std::cout << "3. Search Student\n";
    std::cout << "4. Update Student Details\n";
    std::cout << "5. Delete Student\n";
    std::cout << "6. Calculate Result\n";
    std::cout << "7. Exit\n";
    Utils::printDivider('=', 40);
    std::cout << "\n";
}

int main() {
    // Initialize student manager with permanent file storage
    StudentManager manager("students.txt");

    bool running = true;
    while (running) {
        Utils::clearScreen();
        printMainMenu();

        int choice = Utils::getValidatedInt("Enter Your Choice: ", 1, 7);

        switch (choice) {
            case 1:
                manager.addStudent();
                break;
            case 2:
                manager.displayAllStudents();
                break;
            case 3:
                manager.searchStudent();
                break;
            case 4:
                manager.updateStudent();
                break;
            case 5:
                manager.deleteStudent();
                break;
            case 6:
                manager.calculateResult();
                break;
            case 7: {
                Utils::clearScreen();
                Utils::printDivider('=', 40);
                std::cout << "  THANK YOU FOR USING THE SYSTEM!\n";
                Utils::printDivider('=', 40);
                std::cout << " All records have been safely saved to 'students.txt'.\n";
                std::cout << " Exiting application... Have a great day!\n\n";
                running = false;
                break;
            }
            default:
                std::cout << " [Error] Invalid choice! Please select between 1 and 7.\n";
                Utils::pauseScreen();
                break;
        }
    }

    return 0;
}
