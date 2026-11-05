# 15 College Viva-Voce Questions & Answers
### Project: Student Management System in C++

---

### Q1: What are the main Object-Oriented Programming (OOP) concepts used in this project?
**Answer:**
The project implements four core OOP pillars:
1. **Encapsulation**: Student attributes (`rollNumber`, `marks`, `name`) are kept `private` within the `Student` class. Access and modification are controlled via `public` getters, setters, and member functions.
2. **Abstraction**: Users interact with simple interfaces such as `addStudent()`, `searchStudent()`, and `calculateResult()`, while internal complexity (file serialization, array parsing, stream clearance) is hidden.
3. **Modularity / Classes & Objects**: The system divides responsibility into domain objects (`Student`), controller/service logic (`StudentManager`), and helper utilities (`Utils`).
4. **Constructors**: Both default and parameterized constructors initialize student objects into valid states automatically.

---

### Q2: Why did you use `std::vector` instead of a static raw array for storing students?
**Answer:**
A static array has a fixed size determined at compile time (e.g. `Student arr[100]`), which causes memory waste if fewer students exist or overflow if student count exceeds the limit. `std::vector` is a dynamic array from the C++ Standard Template Library (STL) that automatically resizes as records are added or deleted, managing memory efficiently at runtime.

---

### Q3: How is persistent storage implemented in this application?
**Answer:**
Persistent storage is implemented using C++ file streams (`<fstream>`):
- `std::ifstream` loads existing records line-by-line from `students.txt` into memory when the program initializes.
- `std::ofstream` writes the in-memory `std::vector<Student>` back to `students.txt` using pipe-delimited serialization (`serialize()`) whenever a student is added, updated, or deleted.
This ensures student data persists between program sessions.

---

### Q4: How do you prevent duplicate Roll Numbers in the system?
**Answer:**
Before a new student is appended or modified, the member function `StudentManager::isRollNumberExists(int rollNumber)` searches through the loaded records. If a matching roll number is detected, an error notification is displayed and the user is re-prompted to provide a distinct roll number.

---

### Q5: What happens if a user enters a string or character when a number is expected? How did you handle it?
**Answer:**
In basic C++ programs, entering characters into `cin >> intVar` sets the error flag `cin.fail()` and causes an infinite input loop or crash.
In this project, `Utils::getValidatedInt()` and `Utils::getValidatedDouble()` prevent crashes by:
1. Checking if the extraction succeeded (`if (std::cin >> value)`).
2. If it fails, calling `std::cin.clear()` to reset error flags.
3. Invoking `std::cin.ignore(numeric_limits<streamsize>::max(), '\n')` to discard invalid characters from the input stream before re-prompting the user.

---

### Q6: What is the pass/fail condition and grading criteria in your system?
**Answer:**
- **Pass/Fail Rule**: A student must score at least **33 marks in each of the 5 subjects**. Even if a student's percentage is high (e.g., 65%), if they score $< 33$ in any single subject, their status is set to **Fail** and their final grade is set to **F**.
- **Grading Scale (for students passing all subjects)**:
  - $\ge 90\% \implies \text{A+}$
  - $\ge 80\% \implies \text{A}$
  - $\ge 70\% \implies \text{B}$
  - $\ge 60\% \implies \text{C}$
  - $\ge 50\% \implies \text{D}$
  - $< 50\% \implies \text{F}$

---

### Q7: Why did you use `std::getline()` instead of `std::cin >>` for student names?
**Answer:**
`std::cin >> name` extracts characters only until the first whitespace (space or newline), which truncates multi-word names like `"Aman Sharma"` to just `"Aman"`. `std::getline(std::cin, name)` reads the entire line including spaces up to the newline character.

---

### Q8: What is the purpose of `std::ios::trunc` when opening files for writing?
**Answer:**
`std::ios::trunc` (truncate mode) truncates an existing file to zero bytes upon opening. This is essential when updating or deleting records: the updated in-memory vector is written cleanly without retaining old or duplicate data at the end of the file.

---

### Q9: What are `const` member functions and why are they used (e.g., `displayCard() const`)?
**Answer:**
A `const` member function guarantees that it will not alter any member variables of the calling object. For example, `getRollNumber() const` and `displayCard() const` inspect and display data without modifying state, improving code safety and compiler optimization.

---

### Q10: How does record deletion work in your project?
**Answer:**
1. The user supplies the Roll Number.
2. `findStudentIndexByRoll()` locates the index of the student in the vector.
3. The student's details are displayed for verification.
4. The user is prompted for confirmation (`Y/N`).
5. Upon confirmation, `students.erase(students.begin() + index)` removes the element.
6. `saveToFile()` immediately rewrites the updated records to `students.txt`.

---

### Q11: What is the time complexity of searching a student by Roll Number in this implementation?
**Answer:**
Searching is implemented via linear search (`O(N)` time complexity), where $N$ is the total number of enrolled students. For larger datasets, this can be optimized to `O(log N)` using binary search on a sorted vector or `O(1)` average time complexity using `std::unordered_map<int, Student>`.

---

### Q12: Why is the project split into `.h` header and `.cpp` implementation files?
**Answer:**
Separating declarations (`.h`) from implementations (`.cpp`) achieves:
- **Separation of Concerns**: Readers and compilers can inspect class interfaces without parsing implementation details.
- **Faster Compilation**: Changes in implementation do not require recompiling unmodified dependent translation units.
- **Clean Architecture**: Standard industry practice for scalable C++ codebases.

---

### Q13: What is the difference between `private` and `public` access specifiers?
**Answer:**
- `private`: Members can only be accessed or modified from within member functions of the same class. This guards sensitive data like `marks` or `rollNumber` from unintended external mutation.
- `public`: Members and functions can be invoked from any part of the program (e.g., `manager.addStudent()`).

---

### Q14: How does the Class Merit List calculation work?
**Answer:**
`StudentManager::displayMeritList()` makes a temporary copy of the student vector and sorts it in descending order of percentage using `std::sort` with a custom lambda comparator:
```cpp
std::sort(sortedList.begin(), sortedList.end(), [](const Student& a, const Student& b) {
    return a.getPercentage() > b.getPercentage();
});
```
It then prints ranks sequentially from 1 to $N$.

---

### Q15: How can this system be enhanced in the future?
**Answer:**
1. **Database Integration**: Replace flat-file storage with SQLite or MySQL via ODBC/connector.
2. **Graphical User Interface (GUI)**: Implement a desktop UI using Qt or wxWidgets.
3. **Role-Based Authentication**: Introduce login portals for Administrators, Teachers, and Students with specific access rights.
4. **Attendance & Fee Tracking**: Expand the schema to record attendance percentages and semester fee status.
