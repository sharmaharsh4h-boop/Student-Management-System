#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <array>
#include <optional>
#include "json.hpp"
#include "sqlite3.h"

struct StudentRecord {
    std::string studentId;
    int rollNumber;
    std::string fullName;
    int age;
    std::string course;
    int semester;
    std::string phoneNumber;
    std::array<double, 5> marks;
    double totalMarks;
    double percentage;
    std::string grade;
    std::string status; // "Pass" or "Fail"

    nlohmann::json toJson() const;
    static StudentRecord fromJson(const nlohmann::json& j);
    void calculateResult();
};

struct DashboardStats {
    int totalStudents = 0;
    int passedStudents = 0;
    int failedStudents = 0;
    double classAverage = 0.0;
    double passPercentage = 0.0;
    std::string topperName = "N/A";
    double topperPercentage = 0.0;
    int topperRoll = 0;

    nlohmann::json toJson() const;
};

class Database {
private:
    sqlite3* db = nullptr;
    std::string dbPath;

    bool executeQuery(const std::string& query);
    void seedInitialDataIfEmpty();

public:
    explicit Database(const std::string& path);
    ~Database();

    bool init();

    // CRUD operations
    std::vector<StudentRecord> getAllStudents(const std::string& search = "");
    std::optional<StudentRecord> getStudentByRoll(int rollNumber);
    bool addStudent(StudentRecord& student, std::string& errorMsg);
    bool updateStudent(int originalRoll, StudentRecord& student, std::string& errorMsg);
    bool deleteStudent(int rollNumber, std::string& errorMsg);
    bool rollNumberExists(int rollNumber, int excludeRoll = -1);

    DashboardStats getDashboardStats();
};

#endif // DATABASE_H
