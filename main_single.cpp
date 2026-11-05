/**
 * ============================================================================
 * Project Name  : Student Management System (BCA Project)
 * Language      : C++ (Standard C++17)
 * Author        : College Administration Project Team
 * Description   : Complete Console-based Student Management System with 
 *                 File Handling, Result Processing, and Input Validation.
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
#include <cstdlib>

// ============================================================================
// 1. UTILITY FUNCTIONS & INPUT VALIDATORS
// ============================================================================
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
        std::cin.sync();
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void printDivider(char ch = '=', int length = 40) {
        for (int i = 0; i < length; ++i) {
            std::cout << ch;
        }
        std::cout << "\n";
    }

    void printHeader(const std::string& title) {
        printDivider('=', 40);
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

    int getValidatedInt(const std::string& prompt, int minVal = 0, int maxVal = 1000000000) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << " [Error] Value must be between " << minVal << " and " << maxVal << ". Try again.\n";
                }
            } else {
                std::cout << " [Error] Invalid input! Please enter digits only.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    double getValidatedDouble(const std::string& prompt, double minVal = 0.0, double maxVal = 100.0) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << " [Error] Marks must be between " << minVal << " and " << maxVal << ". Try again.\n";
                }
            } else {
                std::cout << " [Error] Invalid input! Please enter a valid number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    std::string getValidatedString(const std::string& prompt, bool allowEmpty = false) {
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
}

// ============================================================================
// 2. STUDENT CLASS DEFINITION
// ============================================================================
class Student {
private:
    std::string studentId;
    int rollNumber;
    std::string name;
    int age;
    std::string course;
    int semester;
    std::string phoneNumber;
    std::array<double, 5> marks; // 5 subjects
    double totalMarks;
    double percentage;
    std::string grade;
    std::string status;          // "Pass" or "Fail"

public:
    inline static const std::array<std::string, 5> SUBJECT_NAMES = {
        "C++ Programming",
        "Data Structures",
        "Database Systems",
        "Operating Systems",
        "Computer Networks"
    };

    Student() 
        : studentId(""), rollNumber(0), name(""), age(0), course(""), 
          semester(1), phoneNumber(""), totalMarks(0.0), percentage(0.0), 
          grade("F"), status("Fail") {
        marks.fill(0.0);
    }

    Student(std::string id, int roll, std::string n, int a, 
            std::string c, int sem, std::string phone, 
            const std::array<double, 5>& m)
        : studentId(std::move(id)), rollNumber(roll), name(std::move(n)), 
          age(a), course(std::move(c)), semester(sem), 
          phoneNumber(std::move(phone)), marks(m), 
          totalMarks(0.0), percentage(0.0), grade("F"), status("Fail") {
        calculateResult();
    }

    void calculateResult() {
        totalMarks = 0.0;
        bool hasFailedSubject = false;

        for (int i = 0; i < 5; ++i) {
            totalMarks += marks[i];
            if (marks[i] < 33.0) {
                hasFailedSubject = true;
            }
        }

        percentage = totalMarks / 5.0;

        // Condition: Must score >= 33 in EVERY subject to pass
        if (hasFailedSubject) {
            status = "Fail";
            grade = "F";
        } else {
            status = "Pass";
            if (percentage >= 90.0) {
                grade = "A+";
            } else if (percentage >= 80.0) {
                grade = "A";
            } else if (percentage >= 70.0) {
                grade = "B";
            } else if (percentage >= 60.0) {
                grade = "C";
            } else if (percentage >= 50.0) {
                grade = "D";
            } else {
                grade = "F";
                status = "Fail";
            }
        }
    }

    // Getters
    std::string getStudentId() const { return studentId; }
    int getRollNumber() const { return rollNumber; }
    std::string getName() const { return name; }
    int getAge() const { return age; }
    std::string getCourse() const { return course; }
    int getSemester() const { return semester; }
    std::string getPhoneNumber() const { return phoneNumber; }
    const std::array<double, 5>& getMarks() const { return marks; }
    double getTotalMarks() const { return totalMarks; }
    double getPercentage() const { return percentage; }
    std::string getGrade() const { return grade; }
    std::string getStatus() const { return status; }

    // Setters
    void setStudentId(const std::string& id) { studentId = id; }
    void setRollNumber(int roll) { rollNumber = roll; }
    void setName(const std::string& n) { name = n; }
    void setAge(int a) { age = a; }
    void setCourse(const std::string& c) { course = c; }
    void setSemester(int sem) { semester = sem; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    void setMarks(const std::array<double, 5>& m) {
        marks = m;
        calculateResult();
    }

    void displayCard() const {
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  Student ID   : " << studentId << "\n";
        std::cout << "  Roll Number  : " << rollNumber << "\n";
        std::cout << "  Full Name    : " << name << "\n";
        std::cout << "  Age          : " << age << "\n";
        std::cout << "  Course       : " << course << " (Sem " << semester << ")\n";
        std::cout << "  Phone Number : " << phoneNumber << "\n";
        std::cout << "  Total Marks  : " << std::fixed << std::setprecision(1) << totalMarks << " / 500\n";
        std::cout << "  Percentage   : " << std::fixed << std::setprecision(2) << percentage << "%\n";
        std::cout << "  Grade        : " << grade << "\n";
        std::cout << "  Status       : [" << status << "]\n";
        std::cout << "--------------------------------------------------------\n";
    }

    void displayMarksheet() const {
        std::cout << "========================================================\n";
        std::cout << "                 OFFICIAL MARKSHEET                     \n";
        std::cout << "========================================================\n";
        std::cout << " Student ID   : " << std::left << std::setw(15) << studentId 
                  << " Roll No   : " << rollNumber << "\n";
        std::cout << " Student Name : " << std::left << std::setw(15) << name 
                  << " Course    : " << course << " (Sem " << semester << ")\n";
        std::cout << " Phone Number : " << std::left << std::setw(15) << phoneNumber 
                  << " Age       : " << age << "\n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << std::left << std::setw(4) << "No." 
                  << std::left << std::setw(28) << "Subject Name" 
                  << std::right << std::setw(10) << "Max Marks" 
                  << std::right << std::setw(12) << "Obtained\n";
        std::cout << "--------------------------------------------------------\n";
        for (int i = 0; i < 5; ++i) {
            std::cout << std::left << std::setw(4) << (i + 1)
                      << std::left << std::setw(28) << SUBJECT_NAMES[i]
                      << std::right << std::setw(10) << "100"
                      << std::right << std::setw(12) << std::fixed << std::setprecision(1) << marks[i]
                      << (marks[i] < 33.0 ? " *" : "") << "\n";
        }
        std::cout << "--------------------------------------------------------\n";
        std::cout << " Total Marks Obtained : " << std::fixed << std::setprecision(1) << totalMarks << " / 500\n";
        std::cout << " Percentage           : " << std::fixed << std::setprecision(2) << percentage << "%\n";
        std::cout << " Final Grade          : " << grade << "\n";
        std::cout << " Academic Result      : " << (status == "Pass" ? "PASSED" : "FAILED (* indicates < 33 marks)") << "\n";
        std::cout << "========================================================\n";
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << studentId << "|"
            << rollNumber << "|"
            << name << "|"
            << age << "|"
            << course << "|"
            << semester << "|"
            << phoneNumber << "|";
        for (int i = 0; i < 5; ++i) {
            oss << marks[i] << (i < 4 ? "," : "");
        }
        oss << "|"
            << totalMarks << "|"
            << percentage << "|"
            << grade << "|"
            << status;
        return oss.str();
    }

    static bool deserialize(const std::string& line, Student& outStudent) {
        if (line.empty()) return false;

        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 8) return false;

        try {
            outStudent.studentId = tokens[0];
            outStudent.rollNumber = std::stoi(tokens[1]);
            outStudent.name = tokens[2];
            outStudent.age = std::stoi(tokens[3]);
            outStudent.course = tokens[4];
            outStudent.semester = std::stoi(tokens[5]);
            outStudent.phoneNumber = tokens[6];

            std::stringstream marksStream(tokens[7]);
            std::string markVal;
            int idx = 0;
            while (std::getline(marksStream, markVal, ',') && idx < 5) {
                outStudent.marks[idx++] = std::stod(markVal);
            }
            while (idx < 5) {
                outStudent.marks[idx++] = 0.0;
            }

            outStudent.calculateResult();
            return true;
        } catch (...) {
            return false;
        }
    }
};

// ============================================================================
// 3. STUDENT MANAGER CLASS DEFINITION
// ============================================================================
class StudentManager {
private:
    std::vector<Student> students;
    std::string filePath;

public:
    explicit StudentManager(std::string dataPath = "students.txt") 
        : filePath(std::move(dataPath)) {
        loadFromFile();
    }

    bool loadFromFile() {
        students.clear();
        std::ifstream inFile(filePath);
        if (!inFile.is_open()) return false;

        std::string line;
        while (std::getline(inFile, line)) {
            line = Utils::trim(line);
            if (line.empty() || line[0] == '#') continue;

            Student s;
            if (Student::deserialize(line, s)) {
                students.push_back(s);
            }
        }
        inFile.close();
        return true;
    }

    bool saveToFile() const {
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

    bool isRollNumberExists(int rollNumber) const {
        return findStudentIndexByRoll(rollNumber) != -1;
    }

    int findStudentIndexByRoll(int rollNumber) const {
        for (size_t i = 0; i < students.size(); ++i) {
            if (students[i].getRollNumber() == rollNumber) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void addStudent() {
        Utils::clearScreen();
        Utils::printHeader("ADD NEW STUDENT RECORD");

        int roll;
        while (true) {
            roll = Utils::getValidatedInt("Enter Roll Number (e.g. 101): ", 1, 999999);
            if (isRollNumberExists(roll)) {
                std::cout << " [Error] Roll Number " << roll << " already exists! Duplicates are not allowed.\n";
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
            std::cout << "\n [Warning] Student added to memory, but file saving failed.\n";
        }

        Utils::pauseScreen();
    }

    void displayAllStudents() const {
        Utils::clearScreen();
        Utils::printHeader("ALL REGISTERED STUDENTS");

        if (students.empty()) {
            std::cout << "\n [Info] No student records found in the system.\n";
            std::cout << " Use Option 1 from main menu to add records.\n";
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

    void searchStudent() const {
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

    void updateStudent() {
        Utils::clearScreen();
        Utils::printHeader("UPDATE STUDENT DETAILS");

        if (students.empty()) {
            std::cout << "\n [Info] No student records available to update.\n";
            Utils::pauseScreen();
            return;
        }

        int roll = Utils::getValidatedInt("Enter Roll Number of student to update: ", 1, 999999);
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

    void deleteStudent() {
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

    void calculateResult() {
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
                case 1: {
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
                    break;
                }
                case 2: {
                    Utils::clearScreen();
                    Utils::printHeader("CLASS MERIT LIST (RANK WISE)");
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
                    break;
                }
                case 3: {
                    Utils::clearScreen();
                    Utils::printHeader("BATCH ACADEMIC PERFORMANCE");

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
                    break;
                }
                case 4:
                    return;
            }
        }
    }
};

// ============================================================================
// 4. MAIN ENTRY POINT & MENU LOOP
// ============================================================================
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
