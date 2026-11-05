#include "Student.h"
#include <sstream>
#include <vector>
#include <algorithm>

const std::array<std::string, 5> Student::SUBJECT_NAMES = {
    "C++ Programming",
    "Data Structures",
    "Database Systems",
    "Operating Systems",
    "Computer Networks"
};

Student::Student() 
    : studentId(""), rollNumber(0), name(""), age(0), course(""), 
      semester(1), phoneNumber(""), totalMarks(0.0), percentage(0.0), 
      grade("F"), status("Fail") {
    marks.fill(0.0);
}

Student::Student(std::string id, int roll, std::string n, int a, 
                 std::string c, int sem, std::string phone, 
                 const std::array<double, 5>& m)
    : studentId(std::move(id)), rollNumber(roll), name(std::move(n)), 
      age(a), course(std::move(c)), semester(sem), 
      phoneNumber(std::move(phone)), marks(m), 
      totalMarks(0.0), percentage(0.0), grade("F"), status("Fail") {
    calculateResult();
}

void Student::calculateResult() {
    totalMarks = 0.0;
    bool hasFailedSubject = false;

    for (int i = 0; i < 5; ++i) {
        totalMarks += marks[i];
        if (marks[i] < 33.0) {
            hasFailedSubject = true;
        }
    }

    percentage = totalMarks / 5.0;

    // Status: Must score >= 33 in every subject to pass
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
std::string Student::getStudentId() const { return studentId; }
int Student::getRollNumber() const { return rollNumber; }
std::string Student::getName() const { return name; }
int Student::getAge() const { return age; }
std::string Student::getCourse() const { return course; }
int Student::getSemester() const { return semester; }
std::string Student::getPhoneNumber() const { return phoneNumber; }
const std::array<double, 5>& Student::getMarks() const { return marks; }
double Student::getTotalMarks() const { return totalMarks; }
double Student::getPercentage() const { return percentage; }
std::string Student::getGrade() const { return grade; }
std::string Student::getStatus() const { return status; }

// Setters
void Student::setStudentId(const std::string& id) { studentId = id; }
void Student::setRollNumber(int roll) { rollNumber = roll; }
void Student::setName(const std::string& n) { name = n; }
void Student::setAge(int a) { age = a; }
void Student::setCourse(const std::string& c) { course = c; }
void Student::setSemester(int sem) { semester = sem; }
void Student::setPhoneNumber(const std::string& phone) { phoneNumber = phone; }

void Student::setMarks(const std::array<double, 5>& m) {
    marks = m;
    calculateResult();
}

void Student::displayCard() const {
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

void Student::displayMarksheet() const {
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

std::string Student::serialize() const {
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

bool Student::deserialize(const std::string& line, Student& outStudent) {
    if (line.empty()) return false;

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    // Expecting at least 11 tokens: id, roll, name, age, course, sem, phone, marks, total, perc, grade, status
    if (tokens.size() < 8) {
        return false;
    }

    try {
        outStudent.studentId = tokens[0];
        outStudent.rollNumber = std::stoi(tokens[1]);
        outStudent.name = tokens[2];
        outStudent.age = std::stoi(tokens[3]);
        outStudent.course = tokens[4];
        outStudent.semester = std::stoi(tokens[5]);
        outStudent.phoneNumber = tokens[6];

        // Parse 5 subject marks separated by commas
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
