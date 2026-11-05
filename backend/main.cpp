#include "httplib.h"
#include "json.hpp"
#include "database.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using json = nlohmann::json;

int main() {
    std::string dbFile = "database/students.db";
    Database db(dbFile);

    if (!db.init()) {
        std::cerr << "[Fatal Error] Failed to initialize SQLite database at " << dbFile << std::endl;
        return 1;
    }

    httplib::Server svr;

    // CORS & Common headers
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    // Handle OPTIONS for CORS preflight
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // -------------------------------------------------------------
    // API: Dashboard Statistics
    // -------------------------------------------------------------
    svr.Get("/api/dashboard", [&db](const httplib::Request&, httplib::Response& res) {
        DashboardStats stats = db.getDashboardStats();
        res.set_content(stats.toJson().dump(), "application/json");
    });

    // -------------------------------------------------------------
    // API: Get All Students (Optional Search Query)
    // -------------------------------------------------------------
    svr.Get("/api/students", [&db](const httplib::Request& req, httplib::Response& res) {
        std::string search = "";
        if (req.has_param("search")) {
            search = req.get_param_value("search");
        }
        auto list = db.getAllStudents(search);
        json arr = json::array();
        for (const auto& s : list) {
            arr.push_back(s.toJson());
        }
        res.set_content(arr.dump(), "application/json");
    });

    // -------------------------------------------------------------
    // API: Get Single Student by Roll Number
    // -------------------------------------------------------------
    svr.Get(R"(/api/students/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int roll = std::stoi(req.matches[1]);
        auto studentOpt = db.getStudentByRoll(roll);
        if (studentOpt.has_value()) {
            res.set_content(studentOpt->toJson().dump(), "application/json");
        } else {
            res.status = 404;
            json err = {{"error", "Student with Roll Number " + std::to_string(roll) + " not found"}};
            res.set_content(err.dump(), "application/json");
        }
    });

    // -------------------------------------------------------------
    // API: Add New Student
    // -------------------------------------------------------------
    svr.Post("/api/students", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            StudentRecord s = StudentRecord::fromJson(body);

            // Validation
            if (s.rollNumber <= 0) {
                res.status = 400;
                res.set_content(json{{"error", "Roll Number must be a positive integer"}}.dump(), "application/json");
                return;
            }
            if (s.fullName.empty()) {
                res.status = 400;
                res.set_content(json{{"error", "Student Name cannot be empty"}}.dump(), "application/json");
                return;
            }
            if (s.age < 15 || s.age > 70) {
                res.status = 400;
                res.set_content(json{{"error", "Age must be between 15 and 70"}}.dump(), "application/json");
                return;
            }
            for (int i = 0; i < 5; ++i) {
                if (s.marks[i] < 0.0 || s.marks[i] > 100.0) {
                    res.status = 400;
                    res.set_content(json{{"error", "Subject marks must be between 0 and 100"}}.dump(), "application/json");
                    return;
                }
            }

            std::string errMsg;
            if (db.addStudent(s, errMsg)) {
                res.status = 201;
                json resp = {
                    {"success", true},
                    {"message", "Student record created successfully"},
                    {"student", s.toJson()}
                };
                res.set_content(resp.dump(), "application/json");
            } else {
                res.status = 400;
                res.set_content(json{{"error", errMsg}}.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", std::string("Malformed JSON body: ") + e.what()}}.dump(), "application/json");
        }
    });

    // -------------------------------------------------------------
    // API: Update Existing Student
    // -------------------------------------------------------------
    svr.Put(R"(/api/students/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        try {
            int originalRoll = std::stoi(req.matches[1]);
            json body = json::parse(req.body);
            StudentRecord s = StudentRecord::fromJson(body);

            // Validation
            if (s.fullName.empty()) {
                res.status = 400;
                res.set_content(json{{"error", "Student Name cannot be empty"}}.dump(), "application/json");
                return;
            }
            if (s.age < 15 || s.age > 70) {
                res.status = 400;
                res.set_content(json{{"error", "Age must be between 15 and 70"}}.dump(), "application/json");
                return;
            }
            for (int i = 0; i < 5; ++i) {
                if (s.marks[i] < 0.0 || s.marks[i] > 100.0) {
                    res.status = 400;
                    res.set_content(json{{"error", "Subject marks must be between 0 and 100"}}.dump(), "application/json");
                    return;
                }
            }

            std::string errMsg;
            if (db.updateStudent(originalRoll, s, errMsg)) {
                res.status = 200;
                json resp = {
                    {"success", true},
                    {"message", "Student record updated successfully"},
                    {"student", s.toJson()}
                };
                res.set_content(resp.dump(), "application/json");
            } else {
                res.status = 400;
                res.set_content(json{{"error", errMsg}}.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", std::string("Malformed JSON body: ") + e.what()}}.dump(), "application/json");
        }
    });

    // -------------------------------------------------------------
    // API: Delete Student
    // -------------------------------------------------------------
    svr.Delete(R"(/api/students/(\d+))", [&db](const httplib::Request& req, httplib::Response& res) {
        int roll = std::stoi(req.matches[1]);
        std::string errMsg;
        if (db.deleteStudent(roll, errMsg)) {
            res.status = 200;
            json resp = {{"success", true}, {"message", "Student record deleted successfully"}};
            res.set_content(resp.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(json{{"error", errMsg}}.dump(), "application/json");
        }
    });

    // -------------------------------------------------------------
    // Serve Static Frontend Files
    // -------------------------------------------------------------
    svr.set_mount_point("/", "./frontend");

    // Direct root handler fallback
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::ifstream file("frontend/index.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found: frontend/index.html</h1>", "text/html");
        }
    });

    std::cout << "========================================================\n";
    std::cout << "       STUDENT MANAGEMENT SYSTEM - WEB SERVER           \n";
    std::cout << "========================================================\n";
    std::cout << " [Status] Server is running successfully!\n";
    std::cout << " [Local URL] http://localhost:8080\n";
    std::cout << " [Database]  " << dbFile << "\n";
    std::cout << " Press Ctrl + C in this terminal window to stop the server.\n";
    std::cout << "========================================================\n\n";

    if (!svr.listen("0.0.0.0", 8080)) {
        std::cerr << "[Fatal Error] Could not start server on port 8080. Check if another program is using it." << std::endl;
        return 1;
    }

    return 0;
}
