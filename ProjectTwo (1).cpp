/*
 * CS 300 Project Two: ABCU Advising Assistance Program
 * Loads course data from a CSV file, lists courses in alphanumeric order,
 * and displays a selected course with its prerequisites.
 */
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

string trim(const string& value) {
    const string whitespace = " \t\r\n";
    size_t start = value.find_first_not_of(whitespace);
    if (start == string::npos) return "";
    size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

string toUpper(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return value;
}

// Split a comma-separated row into fields.
vector<string> splitCSVLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream stream(line);
    while (getline(stream, field, ',')) fields.push_back(trim(field));
    if (!line.empty() && line.back() == ',') fields.push_back("");
    return fields;
}

bool loadCourses(const string& fileName, map<string, Course>& courses) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file '" << fileName << "'.\n"
             << "Check the file name and make sure the CSV is in the correct folder.\n";
        return false;
    }

    map<string, Course> loadedCourses;
    string line;
    size_t lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty()) continue;

        vector<string> fields = splitCSVLine(line);
        if (fields.size() < 2) {
            cerr << "Warning: Skipping malformed row " << lineNumber << ".\n";
            continue;
        }

        string number = toUpper(trim(fields[0]));
        string title = trim(fields[1]);

        // Ignore a header row if one is present.
        if (number == "COURSE NUMBER" || number == "COURSE" || number == "COURSE ID")
            continue;

        if (number.empty() || title.empty()) {
            cerr << "Warning: Skipping row " << lineNumber
                 << " because its course number or title is empty.\n";
            continue;
        }

        Course course;
        course.courseNumber = number;
        course.courseTitle = title;

        for (size_t i = 2; i < fields.size(); ++i) {
            string prerequisite = toUpper(trim(fields[i]));
            if (!prerequisite.empty()) course.prerequisites.push_back(prerequisite);
        }

        loadedCourses[number] = course;
    }

    inputFile.close();
    if (loadedCourses.empty()) {
        cerr << "Error: No valid course records were found in the file.\n";
        return false;
    }

    courses = loadedCourses;
    cout << "Successfully loaded " << courses.size() << " courses.\n";
    return true;
}

void printCourseList(const map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "Please load course data first (menu option 1).\n";
        return;
    }

    cout << "\nHere is a sample schedule:\n\n";
    for (const auto& entry : courses)
        cout << entry.second.courseNumber << ", " << entry.second.courseTitle << '\n';
    cout << '\n';
}

void printCourseInformation(const map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "Please load course data first (menu option 1).\n";
        return;
    }

    cout << "What course do you want to know about? ";
    string courseNumber;
    if (!getline(cin, courseNumber)) return;
    courseNumber = toUpper(trim(courseNumber));

    auto courseIt = courses.find(courseNumber);
    if (courseIt == courses.end()) {
        cout << "Course " << courseNumber
             << " not found. Please check the course number and try again.\n";
        return;
    }

    const Course& course = courseIt->second;
    cout << course.courseNumber << ", " << course.courseTitle << '\n';
    cout << "Prerequisites: ";

    if (course.prerequisites.empty()) {
        cout << "None";
    } else {
        bool first = true;
        for (const string& prerequisiteNumber : course.prerequisites) {
            if (!first) cout << ", ";
            first = false;
            auto prerequisiteIt = courses.find(prerequisiteNumber);
            if (prerequisiteIt != courses.end()) {
                cout << prerequisiteIt->second.courseNumber << " ("
                     << prerequisiteIt->second.courseTitle << ')';
            } else {
                cout << prerequisiteNumber << " (title not found in loaded data)";
            }
        }
    }
    cout << "\n\n";
}

void displayMenu() {
    cout << "\nWelcome to the course planner.\n\n"
         << "  1. Load Data Structure.\n"
         << "  2. Print Course List.\n"
         << "  3. Print Course.\n"
         << "  9. Exit\n\n"
         << "What would you like to do? ";
}

int main() {
    map<string, Course> courses;
    string choiceText;
    bool running = true;

    while (running) {
        displayMenu();
        if (!getline(cin, choiceText)) {
            cout << "\nInput ended. Exiting program.\n";
            break;
        }

        choiceText = trim(choiceText);
        if (choiceText == "1") {
            cout << "Enter the file name: ";
            string fileName;
            if (!getline(cin, fileName)) break;
            fileName = trim(fileName);
            if (fileName.empty()) cout << "Error: File name cannot be empty.\n";
            else loadCourses(fileName, courses);
        } else if (choiceText == "2") {
            printCourseList(courses);
        } else if (choiceText == "3") {
            printCourseInformation(courses);
        } else if (choiceText == "9") {
            cout << "Thank you for using the course planner!\n";
            running = false;
        } else {
            cout << choiceText
                 << " is not a valid option. Please choose 1, 2, 3, or 9.\n";
        }
    }
    return 0;
}
