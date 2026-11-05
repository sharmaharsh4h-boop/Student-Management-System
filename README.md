# Student Management System (Localhost Web Application)

A modern full-stack **Student Management System** built with **C++17**, **SQLite3**, and a responsive **HTML5/CSS3/JavaScript** frontend, running locally at **`http://localhost:8080`**.

---

## 1. Technology Stack

- **Backend Language**: C++17
- **HTTP Server**: `cpp-httplib` (single-header C++ HTTP library)
- **JSON Parser**: `nlohmann/json` (single-header modern C++ JSON)
- **Database**: `SQLite3` (self-contained, serverless relational database)
- **Frontend**: HTML5, Modern CSS3 (Grid & Flexbox), Vanilla JavaScript (Fetch API)
- **Networking Linker**: `-lws2_32` (Windows Winsock library)
- **Host**: Localhost (`http://localhost:8080`)
- **Code Editor**: VS Code

---

## 2. Project Directory Structure

```text
SMS/
├── backend/
│   ├── main.cpp          # HTTP server, routing & REST API controllers
│   ├── database.cpp      # SQLite3 CRUD logic, result calculation & prepared statements
│   ├── database.h        # Data structures (StudentRecord, DashboardStats) & declarations
│   ├── httplib.h         # cpp-httplib single-header library
│   ├── json.hpp          # nlohmann/json single-header library
│   ├── sqlite3.h         # SQLite3 official C interface header
│   ├── sqlite3.c         # SQLite3 C amalgamation source
│   └── sqlite3.o         # Precompiled object file for fast incremental builds
├── frontend/
│   ├── index.html        # Modern dashboard, sidebar, modals & forms
│   ├── style.css         # Responsive styling, color themes, badges & print styles
│   └── script.js         # Fetch API client, live preview calculator & DOM rendering
├── database/
│   └── students.db       # Persistent SQLite3 database file (auto-created)
├── server.exe            # Compiled C++ backend application binary
├── run.bat               # 1-Click compiler & browser launcher
└── README.md             # Complete project documentation
```

---

## 3. SQLite3 Database Schema

Table name: `students` in `database/students.db`

```sql
CREATE TABLE IF NOT EXISTS students (
    student_id TEXT NOT NULL,
    roll_number INTEGER PRIMARY KEY,
    full_name TEXT NOT NULL,
    age INTEGER NOT NULL,
    course TEXT NOT NULL,
    semester INTEGER NOT NULL,
    phone_number TEXT NOT NULL,
    mark1 REAL NOT NULL,
    mark2 REAL NOT NULL,
    mark3 REAL NOT NULL,
    mark4 REAL NOT NULL,
    mark5 REAL NOT NULL,
    total_marks REAL NOT NULL,
    percentage REAL NOT NULL,
    grade TEXT NOT NULL,
    status TEXT NOT NULL
);
```

---

## 4. Academic Grading & Result Criteria

- **Max Marks per Subject**: 100 (Total 500 across 5 subjects)
- **Percentage Formula**: $\text{Percentage} = \frac{\text{Total Marks}}{5.0}$
- **Pass / Fail Condition**: Student **must score at least 33 marks in each of the 5 subjects**. If any subject score is $< 33$, `status` is set to **`Fail`** and `grade` is set to **`F`**.
- **Grading Scale (for students passing all 5 subjects)**:
  - $90\% - 100\% \implies \mathbf{A+}$
  - $80\% - 89.99\% \implies \mathbf{A}$
  - $70\% - 79.99\% \implies \mathbf{B}$
  - $60\% - 69.99\% \implies \mathbf{C}$
  - $50\% - 59.99\% \implies \mathbf{D}$
  - Below $50\% \implies \mathbf{F}$

---

## 5. REST API Endpoints

| Method | Endpoint | Description | Payload / Response |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/dashboard` | Returns total, passed, failed, class average, and topper stats | JSON object |
| `GET` | `/api/students` | Returns all students (supports query `?search=term`) | Array of JSON objects |
| `GET` | `/api/students/:roll`| Returns a single student by Roll Number | Single JSON object |
| `POST`| `/api/students` | Adds a new student (validates fields and checks duplicate roll) | JSON payload $\rightarrow$ 201 Created |
| `PUT` | `/api/students/:roll`| Updates student information and recalculates results | JSON payload $\rightarrow$ 200 OK |
| `DELETE`| `/api/students/:roll`| Deletes a student record permanently | 200 OK |
| `GET` | `/` | Serves the web frontend dashboard (`index.html`) | HTML |

---

## 6. How to Run the Project

### Option A: 1-Click Launch (Recommended)
Double-click [`run.bat`](file:///C:/SMS/run.bat) in File Explorer or run in Command Prompt:
```cmd
cd C:\SMS
run.bat
```
This script automatically:
1. Detects MinGW at `C:\tools\mingw64\bin` and configures `PATH`.
2. Compiles `server.exe` if not already present.
3. Automatically opens your default web browser to **`http://localhost:8080`**.
4. Keeps the C++ server running.

---

### Option B: Manual Command Line Compilation
From Command Prompt or PowerShell:
```cmd
set PATH=C:\tools\mingw64\bin;%PATH%
cd C:\SMS

:: 1. Compile sqlite3 object (first time only)
gcc -O2 -c backend\sqlite3.c -o backend\sqlite3.o

:: 2. Compile C++ server
g++ -std=c++17 -Ibackend backend\main.cpp backend\database.cpp backend\sqlite3.o -lws2_32 -o server.exe

:: 3. Run server
server.exe
```
Then open your browser and visit: **`http://localhost:8080`**

---

## 7. How to Open and Work in VS Code

1. Launch **VS Code**.
2. Go to **File** $\rightarrow$ **Open Folder...** and select **`C:\SMS`**.
3. Open the built-in terminal in VS Code (`Ctrl + ~`).
4. Type:
   ```cmd
   .\run.bat
   ```
5. The application will start and your browser will navigate to `http://localhost:8080`.
