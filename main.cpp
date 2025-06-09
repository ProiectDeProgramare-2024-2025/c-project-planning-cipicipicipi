#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <algorithm>

using namespace std;

struct Appointment {
    int patient_no;
    string name;
    string reason;
    string date;
    string time;
};

bool isValidName(const string& name) {
    return regex_match(name, regex("^[A-Za-z ]+$"));
}

bool isValidDate(const string& date) {
    if (!regex_match(date, regex("^\\d{2}/\\d{2}/\\d{4}$")))
        return false;

    int day, month, year;
    char sep1, sep2;
    stringstream ss(date);
    ss >> day >> sep1 >> month >> sep2 >> year;

    if (sep1 != '/' || sep2 != '/')
        return false;

    if (month < 1 || month > 12 || day < 1)
        return false;

    int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        daysInMonth[1] = 29;

    if (day > daysInMonth[month - 1])
        return false;

    return true;
}

bool isValidTime(const string& time) {
    if (!regex_match(time, regex("^\\d{2}:\\d{2}$")))
        return false;

    int hour, minute;
    char sep;
    stringstream ss(time);
    ss >> hour >> sep >> minute;

    if (sep != ':' || hour < 0 || hour > 23 || minute < 0 || minute > 59)
        return false;

    return true;
}

vector<Appointment> readAppointments(const string& filename) {
    vector<Appointment> appointments;
    ifstream fin(filename);
    if (!fin) return appointments;

    int n;
    fin >> n;
    fin.ignore();

    for (int i = 0; i < n; ++i) {
        Appointment a;
        fin >> a.patient_no;
        fin.ignore();
        getline(fin, a.name);
        getline(fin, a.reason);
        fin >> a.date >> a.time;
        fin.ignore();
        appointments.push_back(a);
    }
    return appointments;
}

void writeAppointments(const string& filename, const vector<Appointment>& appointments) {
    ofstream fout(filename);
    fout << appointments.size() << "\n";
    for (const auto& a : appointments) {
        fout << a.patient_no << "\n";
        fout << a.name << "\n";
        fout << a.reason << "\n";
        fout << a.date << " " << a.time << "\n";
    }
}

void viewAll(const vector<Appointment>& appointments) {
    if (appointments.empty()) {
        cout << "No appointments found.\n";
        return;
    }
    for (const auto& a : appointments) {
        cout << "Patient No: " << a.patient_no << "\n";
        cout << "Name: " << a.name << "\n";
        cout << "Reason: " << a.reason << "\n";
        cout << "Date: " << a.date << "\n";
        cout << "Time: " << a.time << "\n";
        cout << "--------------------------\n";
    }
}

void addAppointment(vector<Appointment>& appointments) {
    Appointment a;
    a.patient_no = appointments.empty() ? 1 : appointments.back().patient_no + 1;

    cout << "Enter name: ";
    getline(cin >> ws, a.name);
    while (!isValidName(a.name)) {
        cout << "Invalid name. Use letters and spaces only.\nEnter name: ";
        getline(cin >> ws, a.name);
    }

    cout << "Enter reason: ";
    getline(cin >> ws, a.reason);

    cout << "Enter date (DD/MM/YYYY): ";
    cin >> a.date;
    while (!isValidDate(a.date)) {
        cout << "Invalid date format. Use DD/MM/YYYY and make sure the date is valid.\nEnter date (DD/MM/YYYY): ";
        cin >> a.date;
    }

    cout << "Enter time (HH:MM): ";
    cin >> a.time;
    while (!isValidTime(a.time)) {
        cout << "Invalid time format. Use HH:MM in 24-hour format.\nEnter time (HH:MM): ";
        cin >> a.time;
    }

    appointments.push_back(a);
    cout << "Appointment added.\n";
}

void deleteAppointment(vector<Appointment>& appointments) {
    int id;
    cout << "Enter patient number to delete: ";
    cin >> id;
    auto it = remove_if(appointments.begin(), appointments.end(), [id](const Appointment& a) {
        return a.patient_no == id;
    });
    if (it != appointments.end()) {
        appointments.erase(it, appointments.end());
        cout << "Appointment deleted.\n";
    } else {
        cout << "Appointment not found.\n";
    }
}

void searchAppointment(const vector<Appointment>& appointments) {
    int id;
    cout << "Enter patient number to search: ";
    cin >> id;
    bool found = false;
    for (const auto& a : appointments) {
        if (a.patient_no == id) {
            cout << "Patient No: " << a.patient_no << "\n";
            cout << "Name: " << a.name << "\n";
            cout << "Reason: " << a.reason << "\n";
            cout << "Date: " << a.date << "\n";
            cout << "Time: " << a.time << "\n";
            found = true;
            break;
        }
    }
    if (!found) cout << "Appointment not found.\n";
}

int main(int argc, char* argv[]) {
    string filename = "appointments.txt";
    vector<Appointment> appointments = readAppointments(filename);

    if (argc < 2) {
        cout << "Usage: ./DoctorAppointmentManager [view_all | add | delete | search]\n";
        return 1;
    }

    string command = argv[1];

    if (command == "view_all") {
        viewAll(appointments);
    } else if (command == "add") {
        addAppointment(appointments);
        writeAppointments(filename, appointments);
    } else if (command == "delete") {
        deleteAppointment(appointments);
        writeAppointments(filename, appointments);
    } else if (command == "search") {
        searchAppointment(appointments);
    } else {
        cout << "Unknown command.\n";
    }

    return 0;
}
