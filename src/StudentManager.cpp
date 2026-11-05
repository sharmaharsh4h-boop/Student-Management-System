#include "StudentManager.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

StudentManager::StudentManager(std::string dataPath) 
    : filePath(std::move(dataPath)) {
    loadFromFile();
}

bool StudentManager::loadFromFile() {
    students.clear();
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        // File doesn't exist yet, which is normal on initial launch
        return false;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines

        Student s;
        if (Student::deserialize(line, s)) {
            students.push_back(s);
        }
    }
    inFile.close();
    return true;
}

bool StudentManager::saveToFile() const {
    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << " [Error] Could not open file for writing: " << filePath << "\n";
        return false;
    }

    for (const auto& student : students) {
        outFile << student.serialize() << "\n";
    }
    outFile.close();
    return true;
}

bool StudentManager::isRollNumberExists(int rollNumber) const {
    return findStudentIndexByRoll(rollNumber) != -1;
}

int StudentManager::findStudentIndexByRoll(int rollNumber) const {
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].getRollNumber() == rollNumber) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void StudentManager::addStudent() {
    Utils::clearScreen();
    Utils::printHeader("ADD NEW STUDENT RECORD");

    int roll;
    while (true) {
        roll = Utils::getValidatedInt("Enter Roll Number (e.g. 101): ", 1, 999999);
        if (isRollNumberExists(roll)) {
            std::cout << " [Error] Roll Number " << roll << " already exists! Duplicate roll numbers are not allowed.\n";
        } else {
            break;
        }
    }

    std::string id = Utils::getValidatedString("Enter Student ID (e.g. STU101): ");
    std::string name = Utils::getValidatedString("Enter Full Name: ");
    int age = Utils::getValidatedInt("Enter Age (15-60): ", 15, 60);
    std::string course = Utils::getValidatedString("Enter Course (e.g. BCA, B.Tech, MCA): ");
    int semester = Utils::getValidatedInt("Enter Semester (1-8): ", 1, 8);
    std::string phone = Utils::getValidatedPhone("Enter Phone Number (10 digits): ");

    std::cout << "\n--- Enter Marks for 5 Subjects (Max: 100 each) ---\n";
    std::array<double, 5> marks;
    for (int i = 0; i < 5; ++i) {
        std::string prompt = "  " + std::to_string(i + 1) + ". " + Student::SUBJECT_NAMES[i] + ": ";
        marks[i] = Utils::getValidatedDouble(prompt, 0.0, 100.0);
    }

    Student newStudent(id, roll, name, age, course, semester, phone, marks);

    students.push_back(newStudent);

    if (saveToFile()) {
        std::cout << "\n========================================\n";
        std::cout << " [SUCCESS] Student added and saved successfully!\n";
        std::cout << "========================================\n";
        newStudent.displayCard();
    } else {
        std::cout << "\n [Warning] Student added to memory, but failed to write to file.\n";
    }

    Utils::pauseScreen();
}

void StudentManager::displayAllStudents() const {
    Utils::clearScreen();
    Utils::printHeader("ALL REGISTERED STUDENTS");

    if (students.empty()) {
        std::cout << "\n [Info] No student records found in the system.\n";
        std::cout << " Use Option 1 to add a new student record.\n";
        Utils::pauseScreen();
        return;
    }

    std::cout << "Total Records: " << students.size() << "\n\n";
    std::cout << std::left 
              << std::setw(8)  << "Roll No"
              << std::setw(12) << "ID"
              << std::setw(22) << "Name"
              << std::setw(12) << "Course"
              << std::setw(6)  << "Sem"
              << std::right 
              << std::setw(8)  << "Total"
              << std::setw(8)  << "Perc(%)"
              << std::setw(8)  << "Grade"
              << std::setw(10) << "Status" << "\n";
    Utils::printDivider('-', 94);

    int passCount = 0;
    for (const auto& s : students) {
        std::cout << std::left 
                  << std::setw(8)  << s.getRollNumber()
                  << std::setw(12) << s.getStudentId()
                  << std::setw(22) << (s.getName().length() > 20 ? s.getName().substr(0, 19) + "." : s.getName())
                  << std::setw(12) << s.getCourse()
                  << std::setw(6)  << s.getSemester()
                  << std::right 
                  << std::setw(8)  << std::fixed << std::setprecision(1) << s.getTotalMarks()
                  << std::setw(8)  << std::fixed << std::setprecision(1) << s.getPercentage()
                  << std::setw(8)  << s.getGrade()
                  << std::setw(10) << s.getStatus() << "\n";
        if (s.getStatus() == "Pass") passCount++;
    }

    Utils::printDivider('=', 94);
    std::cout << " Summary: Total Students: " << students.size() 
              << " | Passed: " << passCount 
              << " | Failed: " << (students.size() - passCount) << "\n";

    Utils::pauseScreen();
}

void StudentManager::searchStudent() const {
    Utils::clearScreen();
    Utils::printHeader("SEARCH STUDENT RECORD");

    if (students.empty()) {
        std::cout << "\n [Info] No student records available to search.\n";
        Utils::pauseScreen();
        return;
    }

    int roll = Utils::getValidatedInt("Enter Student Roll Number to search: ", 1, 999999);
    int idx = findStudentIndexByRoll(roll);

    if (idx != -1) {
        std::cout << "\n [Record Found]\n";
        students[idx].displayCard();
        std::cout << "\nWould you like to view the complete subject marksheet?";
        if (Utils::getConfirmation("")) {
            std::cout << "\n";
            students[idx].displayMarksheet();
        }
    } else {
        std::cout << "\n [Error] No student record found with Roll Number: " << roll << "\n";
    }

    Utils::pauseScreen();
}

void StudentManager::updateStudent() {
    Utils::clearScreen();
    Utils::printHeader("UPDATE STUDENT DETAILS");

    if (students.empty()) {
        std::cout << "\n [Info] No student records available to update.\n";
        Utils::pauseScreen();
        return;
    }

    int roll = Utils::getValidatedInt("Enter Roll Number of the student to update: ", 1, 999999);
    int idx = findStudentIndexByRoll(roll);

    if (idx == -1) {
        std::cout << "\n [Error] Student with Roll Number " << roll << " not found!\n";
        Utils::pauseScreen();
        return;
    }

    Student& s = students[idx];
    std::cout << "\n--- Current Student Record ---\n";
    s.displayCard();

    std::cout << "\nSelect the field you wish to update:\n";
    std::cout << " 1. Full Name\n";
    std::cout << " 2. Age\n";
    std::cout << " 3. Course and Semester\n";
    std::cout << " 4. Phone Number\n";
    std::cout << " 5. Subject Marks (Recalculates Result)\n";
    std::cout << " 6. Update All Fields\n";
    std::cout << " 7. Cancel and Return to Menu\n";

    int choice = Utils::getValidatedInt("\nEnter Choice (1-7): ", 1, 7);

    switch (choice) {
        case 1: {
            std::string newName = Utils::getValidatedString("Enter New Full Name: ");
            s.setName(newName);
            std::cout << " [SUCCESS] Name updated successfully.\n";
            break;
        }
        case 2: {
            int newAge = Utils::getValidatedInt("Enter New Age (15-60): ", 15, 60);
            s.setAge(newAge);
            std::cout << " [SUCCESS] Age updated successfully.\n";
            break;
        }
        case 3: {
            std::string newCourse = Utils::getValidatedString("Enter New Course: ");
            int newSem = Utils::getValidatedInt("Enter New Semester (1-8): ", 1, 8);
            s.setCourse(newCourse);
            s.setSemester(newSem);
            std::cout << " [SUCCESS] Course and Semester updated successfully.\n";
            break;
        }
        case 4: {
            std::string newPhone = Utils::getValidatedPhone("Enter New Phone Number: ");
            s.setPhoneNumber(newPhone);
            std::cout << " [SUCCESS] Phone number updated successfully.\n";
            break;
        }
        case 5: {
            std::cout << "\n--- Enter Updated Marks for 5 Subjects ---\n";
            std::array<double, 5> newMarks;
            for (int i = 0; i < 5; ++i) {
                std::string prompt = "  " + std::to_string(i + 1) + ". " + Student::SUBJECT_NAMES[i] + ": ";
                newMarks[i] = Utils::getValidatedDouble(prompt, 0.0, 100.0);
            }
            s.setMarks(newMarks);
            std::cout << " [SUCCESS] Marks updated and result recalculated.\n";
            break;
        }
        case 6: {
            std::string newName = Utils::getValidatedString("Enter New Full Name: ");
            int newAge = Utils::getValidatedInt("Enter New Age (15-60): ", 15, 60);
            std::string newCourse = Utils::getValidatedString("Enter New Course: ");
            int newSem = Utils::getValidatedInt("Enter New Semester (1-8): ", 1, 8);
            std::string newPhone = Utils::getValidatedPhone("Enter New Phone Number: ");

            std::cout << "\n--- Enter Updated Marks for 5 Subjects ---\n";
            std::array<double, 5> newMarks;
            for (int i = 0; i < 5; ++i) {
                std::string prompt = "  " + std::to_string(i + 1) + ". " + Student::SUBJECT_NAMES[i] + ": ";
                newMarks[i] = Utils::getValidatedDouble(prompt, 0.0, 100.0);
            }

            s.setName(newName);
            s.setAge(newAge);
            s.setCourse(newCourse);
            s.setSemester(newSem);
            s.setPhoneNumber(newPhone);
            s.setMarks(newMarks);
            std::cout << " [SUCCESS] All details updated successfully.\n";
            break;
        }
        case 7:
            std::cout << " [Info] Update operation cancelled.\n";
            Utils::pauseScreen();
            return;
    }

    if (saveToFile()) {
        std::cout << " [SUCCESS] Changes saved permanently to file.\n";
        std::cout << "\n--- Updated Student Record ---\n";
        s.displayCard();
    } else {
        std::cout << " [Warning] Changes made in memory, but could not be saved to disk.\n";
    }

    Utils::pauseScreen();
}

void StudentManager::deleteStudent() {
    Utils::clearScreen();
    Utils::printHeader("DELETE STUDENT RECORD");

    if (students.empty()) {
        std::cout << "\n [Info] No student records available to delete.\n";
        Utils::pauseScreen();
        return;
    }

    int roll = Utils::getValidatedInt("Enter Roll Number of the student to delete: ", 1, 999999);
    int idx = findStudentIndexByRoll(roll);

    if (idx == -1) {
        std::cout << "\n [Error] Student with Roll Number " << roll << " not found!\n";
        Utils::pauseScreen();
        return;
    }

    std::cout << "\nStudent Record to be deleted:\n";
    students[idx].displayCard();

    if (Utils::getConfirmation("\nAre you sure you want to permanently delete this student record?")) {
        students.erase(students.begin() + idx);
        if (saveToFile()) {
            std::cout << "\n========================================\n";
            std::cout << " [SUCCESS] Student record deleted successfully!\n";
            std::cout << "========================================\n";
        } else {
            std::cout << " [Warning] Record removed from memory but file update failed.\n";
        }
    } else {
        std::cout << "\n [Info] Deletion cancelled by user.\n";
    }

    Utils::pauseScreen();
}

void StudentManager::calculateResult() {
    while (true) {
        Utils::clearScreen();
        Utils::printHeader("CALCULATE & VIEW RESULTS");

        if (students.empty()) {
            std::cout << "\n [Info] No student records available to calculate results.\n";
            Utils::pauseScreen();
            return;
        }

        std::cout << "1. View Individual Marksheet & Result\n";
        std::cout << "2. View Class Merit List (Ranked by Percentage)\n";
        std::cout << "3. View Batch Academic Performance Summary\n";
        std::cout << "4. Return to Main Menu\n";
        Utils::printDivider('-', 40);

        int choice = Utils::getValidatedInt("Enter Your Choice: ", 1, 4);

        switch (choice) {
            case 1:
                displayIndividualResult();
                break;
            case 2:
                displayMeritList();
                break;
            case 3:
                displayBatchSummary();
                break;
            case 4:
                return;
        }
    }
}

void StudentManager::displayIndividualResult() const {
    Utils::clearScreen();
    Utils::printHeader("INDIVIDUAL STUDENT RESULT");

    int roll = Utils::getValidatedInt("Enter Roll Number: ", 1, 999999);
    int idx = findStudentIndexByRoll(roll);

    if (idx != -1) {
        students[idx].displayMarksheet();
    } else {
        std::cout << "\n [Error] Student with Roll Number " << roll << " not found!\n";
    }

    Utils::pauseScreen();
}

void StudentManager::displayMeritList() const {
    Utils::clearScreen();
    Utils::printHeader("CLASS MERIT LIST (RANK WISE)");

    if (students.empty()) {
        std::cout << " [Info] No records available.\n";
        Utils::pauseScreen();
        return;
    }

    // Create a copy to sort by percentage descending
    std::vector<Student> sortedList = students;
    std::sort(sortedList.begin(), sortedList.end(), [](const Student& a, const Student& b) {
        return a.getPercentage() > b.getPercentage();
    });

    std::cout << std::left 
              << std::setw(6)  << "Rank"
              << std::setw(8)  << "Roll"
              << std::setw(22) << "Name"
              << std::setw(12) << "Course"
              << std::right
              << std::setw(8)  << "Total"
              << std::setw(10) << "Perc(%)"
              << std::setw(8)  << "Grade"
              << std::setw(10) << "Status" << "\n";
    Utils::printDivider('-', 84);

    for (size_t i = 0; i < sortedList.size(); ++i) {
        const auto& s = sortedList[i];
        std::cout << std::left 
                  << std::setw(6)  << (i + 1)
                  << std::setw(8)  << s.getRollNumber()
                  << std::setw(22) << (s.getName().length() > 20 ? s.getName().substr(0, 19) + "." : s.getName())
                  << std::setw(12) << s.getCourse()
                  << std::right
                  << std::setw(8)  << std::fixed << std::setprecision(1) << s.getTotalMarks()
                  << std::setw(10) << std::fixed << std::setprecision(2) << s.getPercentage()
                  << std::setw(8)  << s.getGrade()
                  << std::setw(10) << s.getStatus() << "\n";
    }
    Utils::printDivider('=', 84);

    Utils::pauseScreen();
}

void StudentManager::displayBatchSummary() const {
    Utils::clearScreen();
    Utils::printHeader("BATCH ACADEMIC PERFORMANCE");

    if (students.empty()) {
        std::cout << " [Info] No records available.\n";
        Utils::pauseScreen();
        return;
    }

    int totalStudents = static_cast<int>(students.size());
    int passedCount = 0;
    int failedCount = 0;
    double classTotalMarks = 0.0;
    double highestPerc = -1.0;
    double lowestPerc = 999.0;
    std::string topperName = "";
    int topperRoll = 0;

    std::array<double, 5> subjectTotals = {0.0, 0.0, 0.0, 0.0, 0.0};

    for (const auto& s : students) {
        if (s.getStatus() == "Pass") {
            passedCount++;
        } else {
            failedCount++;
        }

        classTotalMarks += s.getTotalMarks();

        if (s.getPercentage() > highestPerc) {
            highestPerc = s.getPercentage();
            topperName = s.getName();
            topperRoll = s.getRollNumber();
        }

        if (s.getPercentage() < lowestPerc) {
            lowestPerc = s.getPercentage();
        }

        const auto& m = s.getMarks();
        for (int i = 0; i < 5; ++i) {
            subjectTotals[i] += m[i];
        }
    }

    double classAveragePerc = (classTotalMarks / (totalStudents * 500.0)) * 100.0;
    double passPercentage = (static_cast<double>(passedCount) / totalStudents) * 100.0;

    std::cout << " Total Enrolled Students   : " << totalStudents << "\n";
    std::cout << " Passed Students           : " << passedCount << "\n";
    std::cout << " Failed Students           : " << failedCount << "\n";
    std::cout << " Overall Pass Percentage   : " << std::fixed << std::setprecision(2) << passPercentage << "%\n";
    std::cout << " Overall Batch Average     : " << std::fixed << std::setprecision(2) << classAveragePerc << "%\n";
    std::cout << " Class Topper              : " << topperName << " (Roll " << topperRoll << ") - " 
              << std::fixed << std::setprecision(2) << highestPerc << "%\n";
    std::cout << " Lowest Percentage         : " << std::fixed << std::setprecision(2) << lowestPerc << "%\n";

    std::cout << "\n--- Subject-Wise Class Average Marks (Out of 100) ---\n";
    for (int i = 0; i < 5; ++i) {
        double subAvg = subjectTotals[i] / totalStudents;
        std::cout << "  " << std::left << std::setw(25) << Student::SUBJECT_NAMES[i] 
                  << " : " << std::fixed << std::setprecision(2) << subAvg << " / 100\n";
    }

    Utils::printDivider('=', 50);
    Utils::pauseScreen();
}
