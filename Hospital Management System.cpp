#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
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
    Patient(int pid, string n, int a, string c) : id(pid), name(n), age(a), contact(c), isAdmitted(false) {}

    // Getter methods
    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getContact() const { return contact; }
    bool getAdmissionStatus() const { return isAdmitted; }

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
        medicalHistory.push(record);
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

        while (!temp.empty())
        {
            cout << " - " << temp.top() << endl;
            temp.pop();
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
    // Constructor
    Doctor(int did, string n, Department d) : id(did), name(n), department(d) {}

    // Adds a patient ID to the appointment queue (FIFO)
    void addAppointment(int patientId)
    {
        appointmentQueue.push(patientId);
        cout << "Appointment added for patient " << patientId << " with " << name << endl;
    }

    // Processes the next patient in the queue
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

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }

    // Converts the enum to a human-readable string
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
};

// ========== HOSPITAL CLASS ========== //
class Hospital
{
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    queue<int> emergencyQueue;
    int patientCounter;
    int doctorCounter;

public:
    // ---- Constructor ----
    Hospital() : patientCounter(1), doctorCounter(1) {}

    // ---- registerPatient ----
    int registerPatient(string name, int age, string contact)
    {
        // Validate input
        if (name.empty() || age <= 0 || age > 120 || contact.empty())
        {
            cout << "Error: Invalid patient information." << endl;
            return -1;
        }

        Patient p(patientCounter, name, age, contact);
        patients.push_back(p);
        cout << "Patient registered: " << name
             << " (ID: " << patientCounter << ")" << endl;
        return patientCounter++;
    }

    // ---- addDoctor ----
    int addDoctor(string name, Department dept)
    {
        // Validate input
        if (name.empty())
        {
            cout << "Error: Invalid doctor name." << endl;
            return -1;
        }

        Doctor d(doctorCounter, name, dept);
        doctors.push_back(d);

        // Build department name for the print
        string deptName;
        switch (dept)
        {
        case CARDIOLOGY:
            deptName = "Cardiology";
            break;
        case NEUROLOGY:
            deptName = "Neurology";
            break;
        case ORTHOPEDICS:
            deptName = "Orthopedics";
            break;
        case PEDIATRICS:
            deptName = "Pediatrics";
            break;
        case EMERGENCY:
            deptName = "Emergency";
            break;
        case GENERAL:
            deptName = "General";
            break;
        }

        cout << "Doctor added: " << name
             << " (ID: " << doctorCounter << ") - " << deptName << endl;
        return doctorCounter++;
    }

    // ---- admitPatient ----
    void admitPatient(int patientId, RoomType type)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patients[i].admitPatient(type);
                return;
            }
        }
        cout << "Patient not found." << endl;
    }

    // ---- addEmergency ----
    void addEmergency(int patientId)
    {
        // Check if patient exists
        bool patientExists = false;
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patientExists = true;
                break;
            }
        }

        if (!patientExists)
        {
            cout << "Error: Patient ID " << patientId << " not found." << endl;
            return;
        }

        emergencyQueue.push(patientId);
        cout << "Emergency added for patient " << patientId << endl;
    }

    // ---- handleEmergency ----
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

    // ---- bookAppointment ----
    void bookAppointment(int doctorId, int patientId)
    {
        // Find doctor
        int doctorIndex = -1;
        for (int i = 0; i < (int)doctors.size(); i++)
        {
            if (doctors[i].getId() == doctorId)
            {
                doctorIndex = i;
                break;
            }
        }

        if (doctorIndex == -1)
        {
            cout << "Error: Doctor ID " << doctorId << " not found." << endl;
            return;
        }

        // Find patient
        bool patientFound = false;
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patientFound = true;
                break;
            }
        }

        if (!patientFound)
        {
            cout << "Error: Patient ID " << patientId << " not found." << endl;
            return;
        }

        // Book appointment
        doctors[doctorIndex].addAppointment(patientId);
    }

    // ---- displayPatientInfo ----
    void displayPatientInfo(int patientId)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                cout << "=== Patient Information ===" << endl
                     << "ID: " << patients[i].getId() << endl
                     << "Name: " << patients[i].getName() << endl
                     << "Age: " << patients[i].getAge() << endl
                     << "Contact: " << patients[i].getContact() << endl
                     << "Admission Status: "
                     << (patients[i].getAdmissionStatus() ? "Admitted" : "Not Admitted") << endl;
                if (patients[i].getAdmissionStatus())
                {
                    cout << "Room Type: " << patients[i].getRoomTypeName() << endl;
                }
                cout << "==========================" << endl;
                return;
            }
        }
        cout << "Patient not found." << endl;
    }

    // ---- displayDoctorInfo ----
    void displayDoctorInfo(int doctorId)
    {
        for (int i = 0; i < (int)doctors.size(); i++)
        {
            if (doctors[i].getId() == doctorId)
            {
                cout << "=== Doctor Information ===" << endl;
                cout << "ID: " << doctors[i].getId() << endl;
                cout << "Name: " << doctors[i].getName() << endl;
                cout << "Department: " << doctors[i].getDepartment() << endl;
                cout << "Pending Appointments: "
                     << doctors[i].getAppointmentCount()
                     << endl;
                cout << "==========================" << endl;
                return;
            }
        }
        cout << "Doctor not found." << endl;
    }

    // ---- Additional utility methods ----
    void dischargePatient(int patientId)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patients[i].dischargePatient();
                return;
            }
        }
        cout << "Patient not found." << endl;
    }

    void addMedicalRecord(int patientId, string record)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patients[i].addMedicalRecord(record);
                return;
            }
        }
        cout << "Patient not found." << endl;
    }

    void requestTest(int patientId, string testName)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patients[i].requestTest(testName);
                return;
            }
        }
        cout << "Patient not found." << endl;
    }

    void displayPatientHistory(int patientId)
    {
        for (int i = 0; i < (int)patients.size(); i++)
        {
            if (patients[i].getId() == patientId)
            {
                patients[i].displayHistory();
                return;
            }
        }
        cout << "Patient not found." << endl;
    }
};

// ========== HELPER FUNCTIONS ========== //

// Function to convert input to enum value
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
        cout << "Invalid choice, defaulting to GENERAL.\n";
        return GENERAL;
    }
}

// Function to convert input to enum value
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
        cout << "Invalid choice, defaulting to GENERAL_WARD.\n";
        return GENERAL_WARD;
    }
}

// ========== MAIN PROGRAM ========== //
int main()
{
    Hospital hospital;
    bool run = true;

    while (run)
    {
        int choice;
        cout << endl;
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
        cout << "13. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        // Input validation
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        if (choice == 1)
        {
            string patientName, contact;
            int patientAge;

            cout << "Please enter patient name: ";
            cin >> patientName;

            cout << "Please enter patient age: ";
            cin >> patientAge;

            cout << "Please enter patient contact: ";
            cin >> contact;

            hospital.registerPatient(patientName, patientAge, contact);
        }
        else if (choice == 2)
        {
            int option;
            string doctorName;

            cout << "Please enter doctor name: ";
            cin >> doctorName;

            cout << "Select department:\n";
            cout << "1. Cardiology\n2. Neurology\n3. Orthopedics\n4. Pediatrics\n5. Emergency\n6. General\n";
            cout << "Enter choice: ";
            cin >> option;

            hospital.addDoctor(doctorName, getDepartmentFromInput(option));
        }
        else if (choice == 3)
        {
            int id;
            int option;

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
            int id1, id2;

            cout << "Please enter doctor id: ";
            cin >> id1;

            cout << "Please enter patient id: ";
            cin >> id2;

            hospital.bookAppointment(id1, id2);
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
            cin.ignore(); // Clear the newline character
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
            cin >> testName;
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
            cout << "Exiting system. Goodbye!" << endl;
            run = false;
        }
        else
        {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}