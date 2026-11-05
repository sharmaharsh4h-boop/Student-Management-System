#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Student.h"
#include <vector>
#include <string>

class StudentManager {
private:
    std::vector<Student> students;
    std::string filePath;

public:
    explicit StudentManager(std::string dataPath = "students.txt");

    // File handling
    bool loadFromFile();
    bool saveToFile() const;

    // Helper checks
    bool isRollNumberExists(int rollNumber) const;
    int findStudentIndexByRoll(int rollNumber) const;

    // Menu operations
    void addStudent();
    void displayAllStudents() const;
    void searchStudent() const;
    void updateStudent();
    void deleteStudent();
    void calculateResult();

    // Statistics & Reports
    void displayMeritList() const;
    void displayIndividualResult() const;
    void displayBatchSummary() const;

    // Total count getter
    size_t getStudentCount() const { return students.size(); }
};

#endif // STUDENT_MANAGER_H
