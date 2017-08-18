#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

class Employee
{
public:
    Employee(const string &first_name, const string &second_name,
        int salary, int experience);
    Employee(const string &non_parsed_employee);

    void print() const;

    const string& getFirstName() { return first_name_; }
    const string& getSecondName(){ return second_name_; }
    int getSalary() const { return salary_; }
    int getExperience() const { return experience_; }
    string getEmployeeAsLine();

private:
    void parseEmployeeLine(const string &non_parsed_employee);
    string first_name_;
    string second_name_;
    int salary_;
    int experience_;
};

Employee::Employee(const string &first_name, const string &second_name,
                   int salary, int experience) :
    first_name_(first_name),
    second_name_(second_name),
    salary_(salary),
    experience_(experience)
{
}

Employee::Employee(const string &non_parsed_employee)
{
    parseEmployeeLine(non_parsed_employee);
}

//TODO Handle wrong file structure
void Employee::parseEmployeeLine(const string &non_parsed_employee)
{
    size_t position_end_of_first_name
            = non_parsed_employee.find(':');
    size_t position_end_of_second_name
            = non_parsed_employee.find(':', position_end_of_first_name + 1);
    size_t salary_end = non_parsed_employee.find(':', position_end_of_second_name + 1);

    first_name_ = non_parsed_employee.substr(0, position_end_of_first_name);
    second_name_ = non_parsed_employee.substr(position_end_of_first_name + 1,
                               position_end_of_second_name - position_end_of_first_name - 1);
    salary_ = stoi(non_parsed_employee.substr(position_end_of_second_name + 1,
                               salary_end - position_end_of_second_name - 1));
    experience_ = stoi(non_parsed_employee.substr(salary_end + 1));
}

void Employee::print() const
{
    cout << first_name_ << "\t" << second_name_ << "\t"
         << salary_ << "\t" << experience_ << endl;
}

string Employee::getEmployeeAsLine()
{
    ostringstream oss;
    oss << this->getFirstName() << ":" << this->getSecondName() << ":"
        << this->getSalary() << ":" << this->getExperience() << endl;
    return oss.str();
}

class EmployeeCollection : public vector<shared_ptr<Employee>>
{
public:
    void printEmployees() const;
    void printEmployeesSortedBySalaryAsc();
    void printEmployeesSortedBySalaryDesc();
    void printEmployeesSortedByExperienceAsc();
    void printEmployeesSortedByExperienceDesc();
private:
    multimap<int, int> salary_mapper_;
    multimap<int, int> experience_mapper_;
    void rebuildMapperForSalary();
    void rebuildMapperForExperience();
};

void EmployeeCollection::printEmployees() const
{
    for(vector<shared_ptr<Employee>>::const_iterator it = this->cbegin(); it != this->cend(); ++it)
    {
        cout << distance(this->cbegin(), it) << ". ";
        (*it)->print();
    }
}

void EmployeeCollection::printEmployeesSortedBySalaryAsc()
{
    rebuildMapperForSalary();

    for(multimap<int, int>::const_iterator it = salary_mapper_.cbegin(); it != salary_mapper_.cend(); ++it)
    {
        (*this->at(it->second)).print();
    }
}

void EmployeeCollection::printEmployeesSortedBySalaryDesc()
{
    rebuildMapperForSalary();

    for(multimap<int, int>::reverse_iterator it = salary_mapper_.rbegin(); it != salary_mapper_.rend(); ++it)
    {
        (*this->at(it->second)).print();
    }
}

void EmployeeCollection::printEmployeesSortedByExperienceAsc()
{
    rebuildMapperForExperience();

    for(multimap<int, int>::const_iterator it = experience_mapper_.cbegin(); it != experience_mapper_.cend(); ++it) {
        (*this->at(it->second)).print();
    }
}

void EmployeeCollection::printEmployeesSortedByExperienceDesc()
{
    rebuildMapperForExperience();

    for(multimap<int, int>::reverse_iterator it = experience_mapper_.rbegin(); it != experience_mapper_.rend(); ++it) {
        (*this->at(it->second)).print();
    }
}

void EmployeeCollection::rebuildMapperForSalary()
{
    salary_mapper_.clear();
    for(vector<shared_ptr<Employee>>::const_iterator it = this->cbegin(); it != this->cend(); ++it) {
        salary_mapper_.insert(pair<int,int>((*it)->getSalary(), distance(this->cbegin(), it)));
    }
}

void EmployeeCollection::rebuildMapperForExperience()
{
    experience_mapper_.clear();
    for(vector<shared_ptr<Employee>>::const_iterator it = this->cbegin(); it != this->cend(); ++it) {
        experience_mapper_.insert(pair<int,int>((*it)->getExperience(), distance(this->cbegin(), it)));
    }
}

class FileManager
{
public:
    static bool loadFromFile(const string& filename, EmployeeCollection &target);
    static bool loadToFile(const string& filename, EmployeeCollection &source);
};

bool FileManager::loadFromFile(const string &filename, EmployeeCollection &load_target)
{
    ifstream fs(filename);
    if (!fs.is_open())
        return false;

    for (string line; getline(fs, line);) {
        shared_ptr<Employee> pEmployee(new Employee(line));
        load_target.push_back(pEmployee);
    }

    fs.close();
    return true;
}

bool FileManager::loadToFile(const string &filename, EmployeeCollection &source)
{
    ofstream fs(filename);
    if (!fs.is_open())
        return false;

    for(vector<shared_ptr<Employee>>::iterator it = source.begin(); it != source.end(); ++it) {
        fs << (*it)->getEmployeeAsLine();
    }
    fs.close();
    return true;
}

void showHelper()
{
    cout << "help                 - show this instruction" << endl;
    cout << "show                 - show all employees" << endl;
    cout << "show_salary_asc      - show all employees ordered by salary field by asc" << endl;
    cout << "show_salary_desc     - show all employees ordered by salary field by desc" << endl;
    cout << "show_experience_asc  - show all employees ordered by experience field by asc" << endl;
    cout << "show_experience_desc - show all employees ordered by experience field by desc" << endl;
    cout << "add                  - add new employee" << endl;
    cout << "delete               - delete employee" << endl;
    cout << "save                 - save changes in file" << endl;
    cout << endl;
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        cout << "You need pass the file name as an argument" << endl;
        return 1;
    }

    if(argc != 2) {
        cout << "One argument only" << endl;
        return 1;
    }

    EmployeeCollection employee_collection;
    bool is_ok = FileManager::loadFromFile(argv[1], employee_collection);
    if (!is_ok) {
        cout << "Can't load file " << argv[1] << endl;
    }

    showHelper();
    string command;
    do {
        cout << "Enter your command: ";
        getline(cin, command);

        if (command.compare("show") == 0) {
            employee_collection.printEmployees();
        }

        if (command.compare("show_salary_asc") == 0) {
            employee_collection.printEmployeesSortedBySalaryAsc();
        }

        if (command.compare("show_salary_desc") == 0) {
            employee_collection.printEmployeesSortedBySalaryDesc();
        }

        if (command.compare("show_experience_asc") == 0) {
            employee_collection.printEmployeesSortedByExperienceAsc();
        }

        if (command.compare("show_experience_desc") == 0) {
            employee_collection.printEmployeesSortedByExperienceDesc();
        }

        if (command.compare("add") == 0) {
            string first_name;
            string second_name;
            string salary;
            string experience;
            cout << "Enter first name: ";
            getline(cin, first_name);
            cout << "Enter second name: ";
            getline(cin, second_name);
            cout << "Enter salary: ";
            getline(cin, salary);
            cout << "Enter experience: ";
            getline(cin, experience);
            shared_ptr<Employee> pEmployee(new Employee(first_name, second_name, stoi(salary), stoi(experience)));
            employee_collection.push_back(pEmployee);
        }

        //TODO Handle wrong line number
        if (command.compare("delete") == 0) {
            string line;
            employee_collection.printEmployees();
            cout << "Enter line number for deleting: ";
            getline(cin, line);
            employee_collection.erase(employee_collection.begin() + stoi(line));
        }

        if (command.compare("save") == 0) {
            bool is_ok = FileManager::loadToFile(argv[1], employee_collection);
            if (!is_ok) {
                cout << "Can't save file " << argv[1] << endl;
            }
        }

    } while (command.compare("exit") != 0);

    return 0;
}
