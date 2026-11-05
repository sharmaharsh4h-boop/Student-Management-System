CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = sms.exe
SOURCES = src/main.cpp src/Student.cpp src/StudentManager.cpp src/Utils.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

single: main_single.cpp
	$(CXX) -std=c++17 main_single.cpp -o sms_single.exe

clean:
	del /f /q sms.exe sms_single.exe 2>nul || rm -f sms.exe sms_single.exe
