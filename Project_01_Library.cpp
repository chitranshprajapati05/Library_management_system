
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;

const int FINE_PER_DAY = 5;

class Book {
public:
    int bookID;
    string title, author;
    bool available;

    Book(int id, string t, string a, bool status = true) {
        bookID = id;
        title = t;
        author = a;
        available = status;
    }
};

class Student {
public:
    int studentID;
    string name;

    Student(int id, string n) {
        studentID = id;
        name = n;
    }
};

class IssueRecord {
public:
    int studentID, bookID;
    time_t issueDate, dueDate;

    IssueRecord(int s, int b, int days) {
        studentID = s;
        bookID = b;
        issueDate = time(nullptr);
        dueDate = issueDate + days * 86400;
    }

    IssueRecord(int s, int b, time_t issue, time_t due) {
        studentID = s;
        bookID = b;
        issueDate = issue;
        dueDate = due;
    }
};

class Library {
private:
    vector<Book> books;
    vector<Student> students;
    vector<IssueRecord> records;

    int nextBookID = 101;
    int nextStudentID = 1001;

    // Save all books
    void saveBooks() {
        ofstream file("books.txt");

        for (auto &b : books) {
            file << b.bookID << "|"
                 << b.title << "|"
                 << b.author << "|"
                 << b.available << "\n";
        }

        file.close();
    }

    // Save students
    void saveStudents() {
        ofstream file("students.txt");

        for (auto &s : students) {
            file << s.studentID << "|"
                 << s.name << "\n";
        }

        file.close();
    }

    // Save issued books
    void saveRecords() {
        ofstream file("issued.txt");

        for (auto &r : records) {
            file << r.studentID << "|"
                 << r.bookID << "|"
                 << r.issueDate << "|"
                 << r.dueDate << "\n";
        }

        file.close();
    }

    // Load books
    void loadBooks() {
        ifstream file("books.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string id, title, author, status;

            if (getline(ss, id, '|') &&
                getline(ss, title, '|') &&
                getline(ss, author, '|') &&
                getline(ss, status)) {

                int bookID = stoi(id);

                books.push_back(Book(
                    bookID, title, author, stoi(status)
                ));

                if (bookID >= nextBookID)
                    nextBookID = bookID + 1;
            }
        }

        file.close();
    }

    // Load students
    void loadStudents() {
        ifstream file("students.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string id, name;

            if (getline(ss, id, '|') &&
                getline(ss, name)) {

                int studentID = stoi(id);

                students.push_back(Student(studentID, name));

                if (studentID >= nextStudentID)
                    nextStudentID = studentID + 1;
            }
        }

        file.close();
    }

    // Load issue records
    void loadRecords() {
        ifstream file("issued.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string s, b, issue, due;

            if (getline(ss, s, '|') &&
                getline(ss, b, '|') &&
                getline(ss, issue, '|') &&
                getline(ss, due)) {

                records.push_back(IssueRecord(
                    stoi(s),
                    stoi(b),
                    stoll(issue),
                    stoll(due)
                ));
            }
        }

        file.close();
    }

    void saveAll() {
        saveBooks();
        saveStudents();
        saveRecords();
    }

    Book* findBook(int id) {
        for (auto &b : books) {
            if (b.bookID == id)
                return &b;
        }
        return nullptr;
    }

    Student* findStudent(int id) {
        for (auto &s : students) {
            if (s.studentID == id)
                return &s;
        }
        return nullptr;
    }

public:

    Library() {
        loadBooks();
        loadStudents();
        loadRecords();

        cout << "\nLibrary Data Loaded Successfully!\n";
    }

    // Add book
    void addBook() {
        string title, author;

        cin.ignore();

        cout << "Enter Book Title: ";
        getline(cin, title);

        cout << "Enter Author: ";
        getline(cin, author);

        books.push_back(Book(nextBookID, title, author));

        cout << "\nBook Added! Book ID: " << nextBookID;

        nextBookID++;

        saveAll();
    }

    // Register student
    void registerStudent() {
        string name;

        cin.ignore();

        cout << "Enter Student Name: ";
        getline(cin, name);

        students.push_back(Student(nextStudentID, name));

        cout << "\nRegistration Successful!";
        cout << "\nStudent ID: " << nextStudentID;

        nextStudentID++;

        saveAll();
    }

    // Show available books
    void showBooks() {
        cout << "\n--- AVAILABLE BOOKS ---\n";

        for (auto &b : books) {
            if (b.available) {
                cout << "\nID: " << b.bookID;
                cout << "\nTitle: " << b.title;
                cout << "\nAuthor: " << b.author << "\n";
            }
        }
    }

    // Issue book
    void issueBook() {
        int sid, bid, days;

        cout << "\nStudent ID: ";
        cin >> sid;

        cout << "Book ID: ";
        cin >> bid;

        cout << "Rental Days: ";
        cin >> days;

        if (days <= 0) {
            cout << "\nInvalid rental period!";
            return;
        }

        Student* student = findStudent(sid);
        Book* book = findBook(bid);

        if (!student) {
            cout << "\nStudent Not Found!";
            return;
        }

        if (!book || !book->available) {
            cout << "\nBook Not Available!";
            return;
        }

        // Prevent multiple issue of same book
        book->available = false;

        records.push_back(IssueRecord(sid, bid, days));

        cout << "\nBook Issued Successfully!";

        saveAll();
    }

    // Return book
    void returnBook() {
        int sid, bid;

        cout << "\nStudent ID: ";
        cin >> sid;

        cout << "Book ID: ";
        cin >> bid;

        for (int i = 0; i < records.size(); i++) {

            if (records[i].studentID == sid &&
                records[i].bookID == bid) {

                time_t now = time(nullptr);

                double late = difftime(now, records[i].dueDate);

                int lateDays = 0;

                if (late > 0) {
                    lateDays = (int)((late + 86399) / 86400);
                }

                int fine = lateDays * FINE_PER_DAY;

                cout << "\n--- RETURN DETAILS ---";
                cout << "\nLate Days: " << lateDays;
                cout << "\nFine: Rs. " << fine;

                Book* book = findBook(bid);

                if (book)
                    book->available = true;

                records.erase(records.begin() + i);

                cout << "\nBook Returned Successfully!";

                saveAll();

                return;
            }
        }

        cout << "\nIssue Record Not Found!";
    }

    // Student records
    void showStudentRecords() {
        cout << "\n--- STUDENT RECORDS ---\n";

        for (auto &s : students) {
            cout << "\nStudent ID: " << s.studentID;
            cout << "\nName: " << s.name;

            bool found = false;

            for (auto &r : records) {
                if (r.studentID == s.studentID) {
                    Book* b = findBook(r.bookID);

                    if (b) {
                        cout << "\nBook: " << b->title;
                        cout << "\nBook ID: " << b->bookID;
                        found = true;
                    }
                }
            }

            if (!found)
                cout << "\nNo Book Issued.";

            cout << "\n---------------------\n";
        }
    }

    // Issued book and fine details
    void showIssuedBooks() {
        cout << "\n--- ISSUED BOOKS ---\n";

        for (auto &r : records) {
            cout << "\nStudent ID: " << r.studentID;
            cout << "\nBook ID: " << r.bookID;

            Book* b = findBook(r.bookID);

            if (b)
                cout << "\nBook Name: " << b->title;

            time_t now = time(nullptr);
            double late = difftime(now, r.dueDate);

            int lateDays = 0;

            if (late > 0)
                lateDays = (int)((late + 86399) / 86400);

            cout << "\nDue Date: "
                 << put_time(localtime(&r.dueDate), "%d-%m-%Y");

            cout << "\nLate Days: " << lateDays;
            cout << "\nCurrent Fine: Rs. "
                 << lateDays * FINE_PER_DAY;

            cout << "\n---------------------\n";
        }
    }

    // Search student
    void searchStudent() {
        int id;

        cout << "\nEnter Student ID: ";
        cin >> id;

        Student* s = findStudent(id);

        if (!s) {
            cout << "\nStudent Not Found!";
            return;
        }

        cout << "\nName: " << s->name;
        cout << "\nStudent ID: " << s->studentID;

        bool found = false;

        for (auto &r : records) {
            if (r.studentID == id) {
                Book* b = findBook(r.bookID);

                if (b) {
                    cout << "\nBook: " << b->title;
                    cout << "\nBook ID: " << b->bookID;
                    found = true;
                }
            }
        }

        if (!found)
            cout << "\nNo Book Currently Issued.";
    }

    // Main menu
    void menu() {
        int choice;

        do {
            cout << "\n\n==============================";
            cout << "\n    LIBRARY MANAGEMENT SYSTEM";
            cout << "\n==============================";

            cout << "\n1. Add Book";
            cout << "\n2. Register Student";
            cout << "\n3. Show Available Books";
            cout << "\n4. Issue Book";
            cout << "\n5. Return Book";
            cout << "\n6. Show Student Records";
            cout << "\n7. Show Issued Books & Fine";
            cout << "\n8. Search Student";
            cout << "\n9. Exit";

            cout << "\n\nEnter Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addBook(); break;
                case 2: registerStudent(); break;
                case 3: showBooks(); break;
                case 4: issueBook(); break;
                case 5: returnBook(); break;
                case 6: showStudentRecords(); break;
                case 7: showIssuedBooks(); break;
                case 8: searchStudent(); break;

                case 9:
                    saveAll();
                    cout << "\nData Saved Successfully!";
                    cout << "\nThank You!";
                    break;

                default:
                    cout << "\nInvalid Choice!";
            }

        } while (choice != 9);
    }
};

int main() {
    Library lib;

    lib.menu();

    return 0;
}
