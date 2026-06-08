#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

// ========== ENUMERATIONS ========== //
enum Department
{
    CARDIOLOGY,
    NEUROLOGY,
    ORTHOPEDICS,
    PEDIATRICS,
    EMERGENCY,
    GENERAL
};

enum RoomType
{
    GENERAL_WARD,
    ICU,
    PRIVATE_ROOM,
    SEMI_PRIVATE
};

// Helper to convert enum to string for file storage
string departmentToString(Department d)
{
    switch (d)
    {
    case CARDIOLOGY:
        return "0";
    case NEUROLOGY:
        return "1";
    case ORTHOPEDICS:
        return "2";
    case PEDIATRICS:
        return "3";
    case EMERGENCY:
        return "4";
    case GENERAL:
        return "5";
    default:
        return "5";
    }
}

string roomTypeToString(RoomType r)
{
    switch (r)
    {
    case GENERAL_WARD:
        return "0";
    case ICU:
        return "1";
    case PRIVATE_ROOM:
        return "2";
    case SEMI_PRIVATE:
        return "3";
    default:
        return "0";
    }
}

// Helper function to escape special characters in strings for serialization
string escapeString(const string &str)
{
    string result;
    for (char c : str)
    {
        if (c == '|')
        {
            result += "\\x01"; // Escape pipe character
        }
        else if (c == '\\')
        {
            result += "\\\\"; // Escape backslash
        }
        else if (c == ',')
        {
            result += "\\x02"; // Escape comma
        }
        else
        {
            result += c;
        }
    }
    return result;
}

string unescapeString(const string &str)
{
    string result;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '\\' && i + 3 < str.length())
        {
            if (str.substr(i, 4) == "\\x01")
            {
                result += '|';
                i += 3;
            }
            else if (str.substr(i, 4) == "\\x02")
            {
                result += ',';
                i += 3;
            }
            else
            {
                result += str[i];
            }
        }
        else if (str[i] == '\\' && i + 1 < str.length() && str[i + 1] == '\\')
        {
            result += '\\';
            i += 1;
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

// ========== PATIENT CLASS ========== //
class Patient
{
private:
    int id;
    string name;
    int age;
    string contact;
    stack<string> medicalHistory;
    queue<string> testQueue;
    bool isAdmitted;
    RoomType roomType;

public:
    // Main constructor - initializes roomType to GENERAL_WARD
    Patient(int pid, string n, int a, string c)
        : id(pid), name(n), age(a), contact(c), isAdmitted(false), roomType(GENERAL_WARD) {}

    // Default constructor for deserialization - also initializes roomType
    Patient() : id(0), name(""), age(0), contact(""), isAdmitted(false), roomType(GENERAL_WARD) {}

    // Getter methods
    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getContact() const { return contact; }
    bool getAdmissionStatus() const { return isAdmitted; }
    RoomType getRoomType() const { return roomType; }

    string getRoomTypeName() const
    {
        switch (roomType)
        {
        case GENERAL_WARD:
            return "General Ward";
        case ICU:
            return "Intensive Care Unit (ICU)";
        case PRIVATE_ROOM:
            return "Private Room";
        case SEMI_PRIVATE:
            return "Semi-Private Room";
        default:
            return "Unknown";
        }
    }

    void admitPatient(RoomType type)
    {
        if (isAdmitted)
        {
            cout << "Patient [" << name << "] is already admitted" << endl;
            return;
        }
        else
        {
            isAdmitted = true;
            roomType = type;
            cout << "Patient [" << name << "] admitted to [" << getRoomTypeName() << "]" << endl;
        }
    }

    void dischargePatient()
    {
        if (!isAdmitted)
        {
            cout << "Patient [" << name << "] is not currently admitted." << endl;
            return;
        }
        else
        {
            isAdmitted = false;
            cout << "Patient [" << name << "] has been discharged." << endl;
        }
    }

    void addMedicalRecord(string record)
    {
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        string timestamp = ctime(&time);
        timestamp.pop_back(); // Remove newline

        string recordWithTimestamp = "[" + timestamp + "] " + record;
        medicalHistory.push(recordWithTimestamp);
        cout << "Medical record added for [" << name << "]: " << record << endl;
    }

    void requestTest(string testName)
    {
        testQueue.push(testName);
        cout << "Test requested for [" << name << "]: " << testName << endl;
    }

    string performTest()
    {
        if (testQueue.empty())
        {
            cout << "No pending tests for [" << name << "]." << endl;
            return "";
        }
        else
        {
            string testName = testQueue.front();
            testQueue.pop();
            cout << "Performing test for [" << name << "]: " << testName << endl;
            return testName;
        }
    }

    void displayHistory()
    {
        cout << "Medical History for [" << name << "]:" << endl;

        if (medicalHistory.empty())
        {
            cout << " No medical history." << endl;
            return;
        }

        stack<string> temp = medicalHistory;
        vector<string> records;

        while (!temp.empty())
        {
            records.push_back(temp.top());
            temp.pop();
        }

        // Display in chronological order (oldest first)
        for (auto it = records.rbegin(); it != records.rend(); ++it)
        {
            cout << " - " << *it << endl;
        }
    }

    // Serialization for file I/O - now includes medicalHistory and testQueue
    string serialize() const
    {
        stringstream ss;

        // Basic info
        ss << id << "," << escapeString(name) << "," << age << ","
           << escapeString(contact) << "," << (isAdmitted ? "1" : "0")
           << "," << roomTypeToString(roomType);

        // Serialize medicalHistory stack (preserving order - oldest first for storage)
        ss << ",MEDICAL_HISTORY:";
        stack<string> tempHistory = medicalHistory;
        vector<string> historyVec;
        while (!tempHistory.empty())
        {
            historyVec.push_back(escapeString(tempHistory.top()));
            tempHistory.pop();
        }
        // Store from oldest to newest (reverse of stack order)
        for (int i = historyVec.size() - 1; i >= 0; i--)
        {
            if (i != historyVec.size() - 1)
                ss << "|";
            ss << historyVec[i];
        }

        // Serialize testQueue (FIFO order)
        ss << ",TEST_QUEUE:";
        queue<string> tempQueue = testQueue;
        bool first = true;
        while (!tempQueue.empty())
        {
            if (!first)
                ss << "|";
            ss << escapeString(tempQueue.front());
            tempQueue.pop();
            first = false;
        }

        return ss.str();
    }

    void deserialize(const string &data)
    {
        stringstream ss(data);
        string token;

        // Parse basic info
        getline(ss, token, ',');
        id = stoi(token);
        getline(ss, name, ',');
        name = unescapeString(name);
        getline(ss, token, ',');
        age = stoi(token);
        getline(ss, contact, ',');
        contact = unescapeString(contact);
        getline(ss, token, ',');
        isAdmitted = (token == "1");
        getline(ss, token, ',');
        roomType = static_cast<RoomType>(stoi(token));

        // Parse medical history
        getline(ss, token, ',');
        if (token.find("MEDICAL_HISTORY:") == 0)
        {
            string historyData = token.substr(16); // Remove "MEDICAL_HISTORY:"
            stringstream historyStream(historyData);
            string record;
            vector<string> historyVec;
            while (getline(historyStream, record, '|'))
            {
                if (!record.empty())
                {
                    historyVec.push_back(unescapeString(record));
                }
            }
            // Push records in original order (oldest first) to maintain chronology
            for (const string &rec : historyVec)
            {
                medicalHistory.push(rec);
            }
        }

        // Parse test queue
        getline(ss, token, ',');
        if (token.find("TEST_QUEUE:") == 0)
        {
            string testData = token.substr(11); // Remove "TEST_QUEUE:"
            stringstream testStream(testData);
            string test;
            while (getline(testStream, test, '|'))
            {
                if (!test.empty())
                {
                    testQueue.push(unescapeString(test));
                }
            }
        }
    }
};

// ========== DOCTOR CLASS ========== //
class Doctor
{
private:
    int id;
    string name;
    Department department;
    queue<int> appointmentQueue;

public:
    Doctor(int did, string n, Department d) : id(did), name(n), department(d) {}

    // Default constructor for deserialization
    Doctor() : id(0), name(""), department(GENERAL) {}

    void addAppointment(int patientId)
    {
        appointmentQueue.push(patientId);
        cout << "Appointment added for patient " << patientId << " with " << name << endl;
    }

    int seePatient()
    {
        if (appointmentQueue.empty())
        {
            cout << "No appointments for " << name << "." << endl;
            return -1;
        }
        else
        {
            int patientId = appointmentQueue.front();
            appointmentQueue.pop();
            cout << name << " is seeing patient " << patientId << endl;
            return patientId;
        }
    }

    int getId() const { return id; }
    string getName() const { return name; }
    Department getDepartmentEnum() const { return department; }

    string getDepartment() const
    {
        switch (department)
        {
        case CARDIOLOGY:
            return "Cardiology";
        case NEUROLOGY:
            return "Neurology";
        case ORTHOPEDICS:
            return "Orthopedics";
        case PEDIATRICS:
            return "Pediatrics";
        case EMERGENCY:
            return "Emergency";
        case GENERAL:
            return "General Practice";
        default:
            return "Unknown";
        }
    }

    int getAppointmentCount() const
    {
        return appointmentQueue.size();
    }

    // Serialization - now escapes name (Bug #2 fixed)
    string serialize() const
    {
        stringstream ss;
        ss << id << "," << escapeString(name) << "," << departmentToString(department);
        return ss.str();
    }

    void deserialize(const string &data)
    {
        stringstream ss(data);
        string token;
        getline(ss, token, ',');
        id = stoi(token);
        getline(ss, name, ',');
        name = unescapeString(name);
        getline(ss, token, ',');
        department = static_cast<Department>(stoi(token));
    }
};

// ========== HOSPITAL CLASS ========== //
class Hospital
{
private:
    map<int, unique_ptr<Patient>> patients;
    map<int, unique_ptr<Doctor>> doctors;
    queue<int> emergencyQueue;
    int patientCounter;
    int doctorCounter;
    const string DATA_FILE = "hospital_data.txt";

    void saveToFile()
    {
        ofstream outFile(DATA_FILE);
        if (!outFile.is_open())
        {
            cerr << "Warning: Could not open file for writing." << endl;
            return;
        }

        // Save patient counter
        outFile << "PATIENT_COUNTER:" << patientCounter << endl;

        // Save all patients
        outFile << "PATIENTS:" << endl;
        for (const auto &[id, patient] : patients)
        {
            outFile << patient->serialize() << endl;
        }
        outFile << "END_PATIENTS" << endl;

        // Save doctor counter
        outFile << "DOCTOR_COUNTER:" << doctorCounter << endl;

        // Save all doctors
        outFile << "DOCTORS:" << endl;
        for (const auto &[id, doctor] : doctors)
        {
            outFile << doctor->serialize() << endl;
        }
        outFile << "END_DOCTORS" << endl;

        // Save emergency queue
        outFile << "EMERGENCY_QUEUE:" << endl;
        queue<int> tempQueue = emergencyQueue;
        vector<int> emergencyItems;
        while (!tempQueue.empty())
        {
            emergencyItems.push_back(tempQueue.front());
            tempQueue.pop();
        }
        for (int item : emergencyItems)
        {
            outFile << item << endl;
        }
        outFile << "END_EMERGENCY" << endl;

        outFile.close();
        cout << "Data saved successfully to " << DATA_FILE << endl;
    }

    void loadFromFile()
    {
        ifstream inFile(DATA_FILE);
        if (!inFile.is_open())
        {
            cout << "No existing data file found. Starting fresh." << endl;
            return;
        }

        string line;
        string section = "";

        while (getline(inFile, line))
        {
            if (line.empty())
                continue;

            if (line.find("PATIENT_COUNTER:") == 0)
            {
                patientCounter = stoi(line.substr(16)); // "PATIENT_COUNTER:" is 16 chars
                continue;
            }
            else if (line.find("DOCTOR_COUNTER:") == 0)
            {
                doctorCounter = stoi(line.substr(15)); // "DOCTOR_COUNTER:" is 15 chars
                continue;
            }
            else if (line == "PATIENTS:")
            {
                section = "PATIENTS";
                continue;
            }
            else if (line == "DOCTORS:")
            {
                section = "DOCTORS";
                continue;
            }
            else if (line == "EMERGENCY_QUEUE:")
            {
                section = "EMERGENCY";
                continue;
            }
            else if (line == "END_PATIENTS" || line == "END_DOCTORS" || line == "END_EMERGENCY")
            {
                section = "";
                continue;
            }

            if (section == "PATIENTS")
            {
                auto patient = make_unique<Patient>();
                patient->deserialize(line);
                patients[patient->getId()] = move(patient);
            }
            else if (section == "DOCTORS")
            {
                auto doctor = make_unique<Doctor>();
                doctor->deserialize(line);
                doctors[doctor->getId()] = move(doctor);
            }
            else if (section == "EMERGENCY")
            {
                emergencyQueue.push(stoi(line));
            }
        }

        inFile.close();
        if (patients.size() > 0 || doctors.size() > 0)
        {
            cout << "Data loaded successfully from " << DATA_FILE << endl;
            cout << "Loaded " << patients.size() << " patients and " << doctors.size() << " doctors." << endl;
        }
    }

    bool isPatientIdExists(int id) const
    {
        return patients.find(id) != patients.end();
    }

    bool isDoctorIdExists(int id) const
    {
        return doctors.find(id) != doctors.end();
    }

public:
    Hospital() : patientCounter(1), doctorCounter(1)
    {
        loadFromFile();
    }

    ~Hospital()
    {
        saveToFile();
    }

    int registerPatient(string name, int age, string contact)
    {
        if (name.empty() || age <= 0 || age > 120 || contact.empty())
        {
            throw invalid_argument("Error: Invalid patient information.");
        }

        int newId = patientCounter;
        auto patient = make_unique<Patient>(newId, name, age, contact);
        patients[newId] = move(patient);
        patientCounter++;

        cout << "Patient registered: " << name
             << " (ID: " << newId << ")" << endl;
        return newId;
    }

    int addDoctor(string name, Department dept)
    {
        if (name.empty())
        {
            throw invalid_argument("Error: Invalid doctor name.");
        }

        int newId = doctorCounter;
        auto doctor = make_unique<Doctor>(newId, name, dept);
        doctors[newId] = move(doctor);
        doctorCounter++;

        cout << "Doctor added: " << name
             << " (ID: " << newId << ") - "
             << doctors[newId]->getDepartment() << endl;
        return newId;
    }

    void admitPatient(int patientId, RoomType type)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            it->second->admitPatient(type);
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void addEmergency(int patientId)
    {
        if (!isPatientIdExists(patientId))
        {
            throw runtime_error("Error: Patient ID " + to_string(patientId) + " not found.");
        }

        emergencyQueue.push(patientId);
        cout << "Emergency added for patient " << patientId << endl;
    }

    int handleEmergency()
    {
        if (emergencyQueue.empty())
        {
            cout << "No emergency cases." << endl;
            return -1;
        }
        int patientId = emergencyQueue.front();
        emergencyQueue.pop();
        cout << "Handling emergency for patient " << patientId << endl;
        return patientId;
    }

    void bookAppointment(int doctorId, int patientId)
    {
        if (!isDoctorIdExists(doctorId))
        {
            throw runtime_error("Error: Doctor ID " + to_string(doctorId) + " not found.");
        }

        if (!isPatientIdExists(patientId))
        {
            throw runtime_error("Error: Patient ID " + to_string(patientId) + " not found.");
        }

        doctors[doctorId]->addAppointment(patientId);
    }

    void displayPatientInfo(int patientId)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            cout << "\n=== Patient Information ===" << endl
                 << "ID: " << it->second->getId() << endl
                 << "Name: " << it->second->getName() << endl
                 << "Age: " << it->second->getAge() << endl
                 << "Contact: " << it->second->getContact() << endl
                 << "Admission Status: "
                 << (it->second->getAdmissionStatus() ? "Admitted" : "Not Admitted") << endl;
            if (it->second->getAdmissionStatus())
            {
                cout << "Room Type: " << it->second->getRoomTypeName() << endl;
            }
            cout << "==========================\n"
                 << endl;
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void displayDoctorInfo(int doctorId)
    {
        auto it = doctors.find(doctorId);
        if (it != doctors.end())
        {
            cout << "\n=== Doctor Information ===" << endl;
            cout << "ID: " << it->second->getId() << endl;
            cout << "Name: " << it->second->getName() << endl;
            cout << "Department: " << it->second->getDepartment() << endl;
            cout << "Pending Appointments: "
                 << it->second->getAppointmentCount()
                 << endl;
            cout << "==========================\n"
                 << endl;
        }
        else
        {
            throw runtime_error("Doctor not found.");
        }
    }

    void dischargePatient(int patientId)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            it->second->dischargePatient();
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void addMedicalRecord(int patientId, string record)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            it->second->addMedicalRecord(record);
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void requestTest(int patientId, string testName)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            it->second->requestTest(testName);
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void displayPatientHistory(int patientId)
    {
        auto it = patients.find(patientId);
        if (it != patients.end())
        {
            it->second->displayHistory();
        }
        else
        {
            throw runtime_error("Patient not found.");
        }
    }

    void listAllPatients()
    {
        cout << "\n=== All Registered Patients ===" << endl;
        if (patients.empty())
        {
            cout << "No patients registered." << endl;
        }
        else
        {
            for (const auto &[id, patient] : patients)
            {
                cout << "ID: " << patient->getId()
                     << ", Name: " << patient->getName()
                     << ", Age: " << patient->getAge() << endl;
            }
            cout << "Total: " << patients.size() << " patients" << endl;
        }
        cout << endl;
    }

    void listAllDoctors()
    {
        cout << "\n=== All Registered Doctors ===" << endl;
        if (doctors.empty())
        {
            cout << "No doctors registered." << endl;
        }
        else
        {
            for (const auto &[id, doctor] : doctors)
            {
                cout << "ID: " << doctor->getId()
                     << ", Name: " << doctor->getName()
                     << ", Dept: " << doctor->getDepartment() << endl;
            }
            cout << "Total: " << doctors.size() << " doctors" << endl;
        }
        cout << endl;
    }
};

// ========== HELPER FUNCTIONS ========== //

Department getDepartmentFromInput(int choice)
{
    switch (choice)
    {
    case 1:
        return CARDIOLOGY;
    case 2:
        return NEUROLOGY;
    case 3:
        return ORTHOPEDICS;
    case 4:
        return PEDIATRICS;
    case 5:
        return EMERGENCY;
    case 6:
        return GENERAL;
    default:
        throw out_of_range("Invalid department choice");
    }
}

RoomType getRoomTypeFromInput(int choice)
{
    switch (choice)
    {
    case 1:
        return GENERAL_WARD;
    case 2:
        return ICU;
    case 3:
        return PRIVATE_ROOM;
    case 4:
        return SEMI_PRIVATE;
    default:
        throw out_of_range("Invalid room type choice");
    }
}

void clearInputStream()
{
    cin.clear();
    cin.ignore(10000, '\n');
}

// ========== MAIN PROGRAM ========== //
int main()
{
    try
    {
        Hospital hospital;
        bool run = true;

        while (run)
        {
            int choice;
            cout << "========== HOSPITAL MANAGEMENT SYSTEM ==========" << endl;
            cout << "1. Register new patient" << endl;
            cout << "2. Add new doctor" << endl;
            cout << "3. Admit Patient" << endl;
            cout << "4. Book appointment" << endl;
            cout << "5. Display Patient Info" << endl;
            cout << "6. Display Doctor info" << endl;
            cout << "7. Add Emergency Case" << endl;
            cout << "8. Handle Emergency" << endl;
            cout << "9. Discharge Patient" << endl;
            cout << "10. Add Medical Record" << endl;
            cout << "11. Request Test" << endl;
            cout << "12. Display Patient History" << endl;
            cout << "13. List All Patients" << endl;
            cout << "14. List All Doctors" << endl;
            cout << "15. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail())
            {
                clearInputStream();
                cout << "Invalid input. Please enter a number." << endl;
                continue;
            }

            try
            {
                if (choice == 1)
                {
                    string patientName, contact;
                    int patientAge;

                    cout << "Please enter patient name: ";
                    clearInputStream();
                    getline(cin, patientName);

                    cout << "Please enter patient age: ";
                    cin >> patientAge;
                    // Note: No clearInputStream() here because we need the newline for the next getline

                    cout << "Please enter patient contact: ";
                    clearInputStream(); // Clear the newline from age input
                    getline(cin, contact);

                    hospital.registerPatient(patientName, patientAge, contact);
                }
                else if (choice == 2)
                {
                    int option;
                    string doctorName;

                    cout << "Please enter doctor name: ";
                    clearInputStream();
                    getline(cin, doctorName);

                    cout << "Select department:\n";
                    cout << "1. Cardiology\n2. Neurology\n3. Orthopedics\n4. Pediatrics\n5. Emergency\n6. General\n";
                    cout << "Enter choice: ";
                    cin >> option;

                    hospital.addDoctor(doctorName, getDepartmentFromInput(option));
                }
                else if (choice == 3)
                {
                    int id, option;
                    cout << "Please enter patient id: ";
                    cin >> id;
                    cout << "Select room type:\n";
                    cout << "1. General Ward\n2. ICU\n3. Private Room\n4. Semi-Private\n";
                    cout << "Enter choice: ";
                    cin >> option;

                    hospital.admitPatient(id, getRoomTypeFromInput(option));
                }
                else if (choice == 4)
                {
                    int doctorId, patientId;
                    cout << "Please enter doctor id: ";
                    cin >> doctorId;
                    cout << "Please enter patient id: ";
                    cin >> patientId;

                    hospital.bookAppointment(doctorId, patientId);
                }
                else if (choice == 5)
                {
                    int id;
                    cout << "Please enter patient id: ";
                    cin >> id;
                    hospital.displayPatientInfo(id);
                }
                else if (choice == 6)
                {
                    int id;
                    cout << "Please enter doctor id: ";
                    cin >> id;
                    hospital.displayDoctorInfo(id);
                }
                else if (choice == 7)
                {
                    int id;
                    cout << "Please enter patient id for emergency: ";
                    cin >> id;
                    hospital.addEmergency(id);
                }
                else if (choice == 8)
                {
                    hospital.handleEmergency();
                }
                else if (choice == 9)
                {
                    int id;
                    cout << "Please enter patient id to discharge: ";
                    cin >> id;
                    hospital.dischargePatient(id);
                }
                else if (choice == 10)
                {
                    int id;
                    string record;
                    cout << "Please enter patient id: ";
                    cin >> id;
                    cout << "Please enter medical record: ";
                    clearInputStream();
                    getline(cin, record);
                    hospital.addMedicalRecord(id, record);
                }
                else if (choice == 11)
                {
                    int id;
                    string testName;
                    cout << "Please enter patient id: ";
                    cin >> id;
                    cout << "Please enter test name: ";
                    clearInputStream();
                    getline(cin, testName);
                    hospital.requestTest(id, testName);
                }
                else if (choice == 12)
                {
                    int id;
                    cout << "Please enter patient id: ";
                    cin >> id;
                    hospital.displayPatientHistory(id);
                }
                else if (choice == 13)
                {
                    hospital.listAllPatients();
                }
                else if (choice == 14)
                {
                    hospital.listAllDoctors();
                }
                else if (choice == 15)
                {
                    cout << "Exiting system. Goodbye!" << endl;
                    run = false;
                }
                else
                {
                    cout << "Invalid choice. Please try again." << endl;
                }
            }
            catch (const exception &e)
            {
                cerr << "Error: " << e.what() << endl;
                clearInputStream();
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
