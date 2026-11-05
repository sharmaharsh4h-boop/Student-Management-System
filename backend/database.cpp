#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void StudentRecord::calculateResult() {
    totalMarks = 0.0;
    bool hasFailedSubject = false;

    for (int i = 0; i < 5; ++i) {
        totalMarks += marks[i];
        if (marks[i] < 33.0) {
            hasFailedSubject = true;
        }
    }

    percentage = totalMarks / 5.0;

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

nlohmann::json StudentRecord::toJson() const {
    return nlohmann::json{
        {"student_id", studentId},
        {"roll_number", rollNumber},
        {"full_name", fullName},
        {"age", age},
        {"course", course},
        {"semester", semester},
        {"phone_number", phoneNumber},
        {"marks", {marks[0], marks[1], marks[2], marks[3], marks[4]}},
        {"total_marks", totalMarks},
        {"percentage", percentage},
        {"grade", grade},
        {"status", status}
    };
}

StudentRecord StudentRecord::fromJson(const nlohmann::json& j) {
    StudentRecord s;
    s.studentId = j.value("student_id", "");
    s.rollNumber = j.value("roll_number", 0);
    s.fullName = j.value("full_name", "");
    s.age = j.value("age", 18);
    s.course = j.value("course", "");
    s.semester = j.value("semester", 1);
    s.phoneNumber = j.value("phone_number", "");

    if (j.contains("marks") && j["marks"].is_array() && j["marks"].size() == 5) {
        for (int i = 0; i < 5; ++i) {
            s.marks[i] = j["marks"][i].get<double>();
        }
    } else {
        s.marks[0] = j.value("mark1", 0.0);
        s.marks[1] = j.value("mark2", 0.0);
        s.marks[2] = j.value("mark3", 0.0);
        s.marks[3] = j.value("mark4", 0.0);
        s.marks[4] = j.value("mark5", 0.0);
    }

    s.calculateResult();
    return s;
}

nlohmann::json DashboardStats::toJson() const {
    return nlohmann::json{
        {"total_students", totalStudents},
        {"passed_students", passedStudents},
        {"failed_students", failedStudents},
        {"class_average", classAverage},
        {"pass_percentage", passPercentage},
        {"topper_name", topperName},
        {"topper_percentage", topperPercentage},
        {"topper_roll", topperRoll}
    };
}

Database::Database(const std::string& path) : dbPath(path) {}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "[Database Error] " << (errMsg ? errMsg : "Unknown error") << std::endl;
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::init() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "[Database Error] Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::string createTable = 
        "CREATE TABLE IF NOT EXISTS students ("
        "  student_id TEXT NOT NULL,"
        "  roll_number INTEGER PRIMARY KEY,"
        "  full_name TEXT NOT NULL,"
        "  age INTEGER NOT NULL,"
        "  course TEXT NOT NULL,"
        "  semester INTEGER NOT NULL,"
        "  phone_number TEXT NOT NULL,"
        "  mark1 REAL NOT NULL,"
        "  mark2 REAL NOT NULL,"
        "  mark3 REAL NOT NULL,"
        "  mark4 REAL NOT NULL,"
        "  mark5 REAL NOT NULL,"
        "  total_marks REAL NOT NULL,"
        "  percentage REAL NOT NULL,"
        "  grade TEXT NOT NULL,"
        "  status TEXT NOT NULL"
        ");";

    if (!executeQuery(createTable)) {
        return false;
    }

    seedInitialDataIfEmpty();
    return true;
}

void Database::seedInitialDataIfEmpty() {
    std::string countQuery = "SELECT COUNT(*) FROM students;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, countQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            if (count > 0) {
                sqlite3_finalize(stmt);
                return; // Already has records
            }
        }
        sqlite3_finalize(stmt);
    }

    // Seed 5 realistic records for immediate testing
    std::vector<StudentRecord> sampleStudents = {
        {"STU101", 101, "Aman Sharma", 20, "BCA", 4, "9876543210", {92.0, 88.0, 95.0, 90.0, 89.0}},
        {"STU102", 102, "Priya Verma", 21, "BCA", 4, "9811223344", {82.0, 79.0, 85.0, 80.0, 84.0}},
        {"STU103", 103, "Rohan Patel", 20, "BCA", 4, "9899001122", {72.0, 70.0, 75.0, 68.0, 74.0}},
        {"STU104", 104, "Sneha Gupta", 19, "BCA", 4, "9711554433", {58.0, 62.0, 54.0, 50.0, 60.0}},
        {"STU105", 105, "Rahul Kumar", 22, "BCA", 4, "9655443322", {65.0, 70.0, 28.0, 55.0, 60.0}} // 28 < 33 -> Fail
    };

    for (auto& s : sampleStudents) {
        s.calculateResult();
        std::string err;
        addStudent(s, err);
    }
}

bool Database::rollNumberExists(int rollNumber, int excludeRoll) {
    std::string sql = "SELECT 1 FROM students WHERE roll_number = ? AND roll_number != ?;";
    sqlite3_stmt* stmt = nullptr;
    bool exists = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, rollNumber);
        sqlite3_bind_int(stmt, 2, excludeRoll);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = true;
        }
        sqlite3_finalize(stmt);
    }
    return exists;
}

std::vector<StudentRecord> Database::getAllStudents(const std::string& search) {
    std::vector<StudentRecord> list;
    std::string sql = "SELECT student_id, roll_number, full_name, age, course, semester, phone_number, "
                      "mark1, mark2, mark3, mark4, mark5, total_marks, percentage, grade, status "
                      "FROM students ";

    if (!search.empty()) {
        sql += "WHERE full_name LIKE ? OR CAST(roll_number AS TEXT) LIKE ? OR course LIKE ? ";
    }
    sql += "ORDER BY roll_number ASC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (!search.empty()) {
            std::string wildcard = "%" + search + "%";
            sqlite3_bind_text(stmt, 1, wildcard.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, wildcard.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, wildcard.c_str(), -1, SQLITE_TRANSIENT);
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            StudentRecord s;
            s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            s.rollNumber = sqlite3_column_int(stmt, 1);
            s.fullName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            s.age = sqlite3_column_int(stmt, 3);
            s.course = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            s.semester = sqlite3_column_int(stmt, 5);
            s.phoneNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            s.marks[0] = sqlite3_column_double(stmt, 7);
            s.marks[1] = sqlite3_column_double(stmt, 8);
            s.marks[2] = sqlite3_column_double(stmt, 9);
            s.marks[3] = sqlite3_column_double(stmt, 10);
            s.marks[4] = sqlite3_column_double(stmt, 11);
            s.totalMarks = sqlite3_column_double(stmt, 12);
            s.percentage = sqlite3_column_double(stmt, 13);
            s.grade = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
            s.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
            list.push_back(s);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

std::optional<StudentRecord> Database::getStudentByRoll(int rollNumber) {
    std::string sql = "SELECT student_id, roll_number, full_name, age, course, semester, phone_number, "
                      "mark1, mark2, mark3, mark4, mark5, total_marks, percentage, grade, status "
                      "FROM students WHERE roll_number = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, rollNumber);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            StudentRecord s;
            s.studentId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            s.rollNumber = sqlite3_column_int(stmt, 1);
            s.fullName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            s.age = sqlite3_column_int(stmt, 3);
            s.course = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            s.semester = sqlite3_column_int(stmt, 5);
            s.phoneNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            s.marks[0] = sqlite3_column_double(stmt, 7);
            s.marks[1] = sqlite3_column_double(stmt, 8);
            s.marks[2] = sqlite3_column_double(stmt, 9);
            s.marks[3] = sqlite3_column_double(stmt, 10);
            s.marks[4] = sqlite3_column_double(stmt, 11);
            s.totalMarks = sqlite3_column_double(stmt, 12);
            s.percentage = sqlite3_column_double(stmt, 13);
            s.grade = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
            s.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
            sqlite3_finalize(stmt);
            return s;
        }
        sqlite3_finalize(stmt);
    }
    return std::nullopt;
}

bool Database::addStudent(StudentRecord& s, std::string& errorMsg) {
    if (rollNumberExists(s.rollNumber)) {
        errorMsg = "Roll Number " + std::to_string(s.rollNumber) + " already exists!";
        return false;
    }

    s.calculateResult();

    std::string sql = "INSERT INTO students (student_id, roll_number, full_name, age, course, semester, "
                      "phone_number, mark1, mark2, mark3, mark4, mark5, total_marks, percentage, grade, status) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        errorMsg = "Failed to prepare insert statement: " + std::string(sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, s.studentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, s.rollNumber);
    sqlite3_bind_text(stmt, 3, s.fullName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, s.age);
    sqlite3_bind_text(stmt, 5, s.course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, s.semester);
    sqlite3_bind_text(stmt, 7, s.phoneNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 8, s.marks[0]);
    sqlite3_bind_double(stmt, 9, s.marks[1]);
    sqlite3_bind_double(stmt, 10, s.marks[2]);
    sqlite3_bind_double(stmt, 11, s.marks[3]);
    sqlite3_bind_double(stmt, 12, s.marks[4]);
    sqlite3_bind_double(stmt, 13, s.totalMarks);
    sqlite3_bind_double(stmt, 14, s.percentage);
    sqlite3_bind_text(stmt, 15, s.grade.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 16, s.status.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        errorMsg = "Failed to insert record: " + std::string(sqlite3_errmsg(db));
        return false;
    }
    return true;
}

bool Database::updateStudent(int originalRoll, StudentRecord& s, std::string& errorMsg) {
    if (s.rollNumber != originalRoll && rollNumberExists(s.rollNumber, originalRoll)) {
        errorMsg = "Roll Number " + std::to_string(s.rollNumber) + " is already assigned to another student!";
        return false;
    }

    s.calculateResult();

    std::string sql = "UPDATE students SET student_id = ?, roll_number = ?, full_name = ?, age = ?, "
                      "course = ?, semester = ?, phone_number = ?, mark1 = ?, mark2 = ?, mark3 = ?, "
                      "mark4 = ?, mark5 = ?, total_marks = ?, percentage = ?, grade = ?, status = ? "
                      "WHERE roll_number = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        errorMsg = "Failed to prepare update statement: " + std::string(sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, s.studentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, s.rollNumber);
    sqlite3_bind_text(stmt, 3, s.fullName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, s.age);
    sqlite3_bind_text(stmt, 5, s.course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, s.semester);
    sqlite3_bind_text(stmt, 7, s.phoneNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 8, s.marks[0]);
    sqlite3_bind_double(stmt, 9, s.marks[1]);
    sqlite3_bind_double(stmt, 10, s.marks[2]);
    sqlite3_bind_double(stmt, 11, s.marks[3]);
    sqlite3_bind_double(stmt, 12, s.marks[4]);
    sqlite3_bind_double(stmt, 13, s.totalMarks);
    sqlite3_bind_double(stmt, 14, s.percentage);
    sqlite3_bind_text(stmt, 15, s.grade.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 16, s.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 17, originalRoll);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        errorMsg = "Failed to update record: " + std::string(sqlite3_errmsg(db));
        return false;
    }
    return true;
}

bool Database::deleteStudent(int rollNumber, std::string& errorMsg) {
    std::string sql = "DELETE FROM students WHERE roll_number = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        errorMsg = "Failed to prepare delete statement: " + std::string(sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_int(stmt, 1, rollNumber);
    int rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE || changes == 0) {
        errorMsg = "Record with Roll Number " + std::to_string(rollNumber) + " not found.";
        return false;
    }
    return true;
}

DashboardStats Database::getDashboardStats() {
    DashboardStats stats;
    auto students = getAllStudents();
    stats.totalStudents = static_cast<int>(students.size());

    if (stats.totalStudents == 0) return stats;

    double sumPercentages = 0.0;
    double highestPerc = -1.0;

    for (const auto& s : students) {
        if (s.status == "Pass") {
            stats.passedStudents++;
        } else {
            stats.failedStudents++;
        }
        sumPercentages += s.percentage;

        if (s.percentage > highestPerc) {
            highestPerc = s.percentage;
            stats.topperName = s.fullName;
            stats.topperPercentage = s.percentage;
            stats.topperRoll = s.rollNumber;
        }
    }

    stats.classAverage = sumPercentages / stats.totalStudents;
    stats.passPercentage = (static_cast<double>(stats.passedStudents) / stats.totalStudents) * 100.0;

    return stats;
}
