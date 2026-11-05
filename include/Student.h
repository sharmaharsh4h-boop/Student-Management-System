#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <array>
#include <iostream>
#include <iomanip>

class Student {
private:
    std::string studentId;
    int rollNumber;
    std::string name;
    int age;
    std::string course;
    int semester;
    std::string phoneNumber;
    std::array<double, 5> marks; // Marks for 5 subjects
    double totalMarks;
    double percentage;
    std::string grade;
    std::string status;          // "Pass" or "Fail"

public:
    // Subject names for clear reporting
    static const std::array<std::string, 5> SUBJECT_NAMES;

    // Constructors
    Student();
    Student(std::string id, int roll, std::string n, int a, 
            std::string c, int sem, std::string phone, 
            const std::array<double, 5>& m);

    // Core calculation logic
    void calculateResult();

    // Getters
    std::string getStudentId() const;
    int getRollNumber() const;
    std::string getName() const;
    int getAge() const;
    std::string getCourse() const;
    int getSemester() const;
    std::string getPhoneNumber() const;
    const std::array<double, 5>& getMarks() const;
    double getTotalMarks() const;
    double getPercentage() const;
    std::string getGrade() const;
    std::string getStatus() const;

    // Setters
    void setStudentId(const std::string& id);
    void setRollNumber(int roll);
    void setName(const std::string& n);
    void setAge(int a);
    void setCourse(const std::string& c);
    void setSemester(int sem);
    void setPhoneNumber(const std::string& phone);
    void setMarks(const std::array<double, 5>& m);

    // Display helpers
    void displayCard() const;
    void displayMarksheet() const;

    // Serialization for file handling
    std::string serialize() const;
    static bool deserialize(const std::string& line, Student& outStudent);
};

#endif // STUDENT_H
