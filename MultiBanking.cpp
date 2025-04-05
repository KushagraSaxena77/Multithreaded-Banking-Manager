#include <iostream>
#include <fstream>
#include <cctype>
#include <iomanip>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

using namespace std;

 
shared_mutex accounts_mutex;  
mutex io_mutex;  
atomic<bool> system_running{true};  

class Bank_Account {
private:
    int Money_Deposit;
    char type;
    int acno;
    char name[70];
    mutex account_mutex;  

public:
    void report() const;
    int retMoney_Deposit() const;
    void create_Bank_Account();
    void dep(int);
    int retacno() const;
    void Display_Account() const;
    void Updation();
    char rettype() const;
    void draw(int);
    
     
    bool deposit_thread_safe(int amount);
    bool withdraw_thread_safe(int amount);
    bool get_account_info(int& balance, char& acc_type, string& acc_name) const;
};

 
bool Bank_Account::deposit_thread_safe(int amount) {
    lock_guard<mutex> lock(account_mutex);
    if (amount > 0) {
        Money_Deposit += amount;
        return true;
    }
    return false;
}

 
bool Bank_Account::withdraw_thread_safe(int amount) {
    lock_guard<mutex> lock(account_mutex);
    if (amount > 0 && Money_Deposit >= amount) {
        Money_Deposit -= amount;
        return true;
    }
    return false;
}

 
bool Bank_Account::get_account_info(int& balance, char& acc_type, string& acc_name) const {
     
    balance = Money_Deposit;
    acc_type = type;
    acc_name = name;
    return true;
}

void Bank_Account::Updation() {
    lock_guard<mutex> io_lock(io_mutex);
    
    cout << "\n\tBank_Account No. : " << acno;
    cout << "\n\tUpdation Bank_Account Holder Name : ";
    cin.ignore();
    cin.getline(name, 50);
    cout << "\n\tUpdation Type of Bank_Account : ";
    cin >> type;
    type = toupper(type);
    cout << "\n\tUpdation Balance Total-Money : ";
    cin >> Money_Deposit;
}

void Bank_Account::create_Bank_Account() {
    {
        lock_guard<mutex> io_lock(io_mutex);
        system("CLS");
        cout << "\n\tPlease Enter the Bank_Account No. : ";
        cin >> acno;
        cout << "\n\n\tPlease Enter the Name of the Bank_Account holder : ";
        cin.ignore();
        cin.getline(name, 50);
        cout << "\n\tPlease Enter Type of the Bank_Account (C/S) : ";
        cin >> type;
        type = toupper(type);
        cout << "\n\tPlease Enter The Starting Total-Money : ";
        cin >> Money_Deposit;
    }
    
    cout << "\n\n\tBank_Account Created..";
}

void Bank_Account::Display_Account() const {
    lock_guard<mutex> io_lock(io_mutex);
    
    cout << "\n\tBank_Account No. : " << acno;
    cout << "\n\tBank_Account Holder Name : ";
    cout << name;
    cout << "\n\tType of Bank_Account : " << type;
    cout << "\n\tBalance Total-Money : " << Money_Deposit;
}

int Bank_Account::retacno() const {
    return acno;
}

char Bank_Account::rettype() const {
    return type;
}

void Bank_Account::report() const {
    lock_guard<mutex> io_lock(io_mutex);
    cout << acno << setw(10) << " " << name << setw(10) << " " << type << setw(6) << Money_Deposit << endl;
}

void Bank_Account::dep(int x) {
    lock_guard<mutex> lock(account_mutex);
    Money_Deposit += x;
}

void Bank_Account::draw(int x) {
    lock_guard<mutex> lock(account_mutex);
    Money_Deposit -= x;
}

int Bank_Account::retMoney_Deposit() const {
    return Money_Deposit;
}

 
vector<Bank_Account> accounts;

 
void log_transaction(int account_no, const string& transaction_type, int amount) {
    lock_guard<mutex> lock(io_mutex);
    ofstream log_file("transaction_log.txt", ios::app);
    if (log_file) {
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        log_file << "Thread ID: " << this_thread::get_id() 
                << " | Time: " << ctime(&now_time)
                << " | Account: " << account_no 
                << " | Transaction: " << transaction_type 
                << " | Amount: " << amount << endl;
        log_file.close();
    }
}

 
void load_accounts() {
    unique_lock<shared_mutex> lock(accounts_mutex);
    accounts.clear();
    
    Bank_Account acc;
    ifstream inFile;
    inFile.open("account.dat", ios::binary);
    if (!inFile) {
        return;
    }
    
    while (inFile.read(reinterpret_cast<char*>(&acc), sizeof(Bank_Account))) {
        accounts.push_back(acc);
    }
    inFile.close();
}

 
void save_accounts() {
    shared_lock<shared_mutex> lock(accounts_mutex);
    
    ofstream outFile;
    outFile.open("account.dat", ios::binary);
    
    for (const auto& acc : accounts) {
        outFile.write(reinterpret_cast<const char*>(&acc), sizeof(Bank_Account));
    }
    outFile.close();
}

 
void backup_thread_function() {
    while (system_running) {
        this_thread::sleep_for(chrono::minutes(5));  
        save_accounts();
        
        {
            lock_guard<mutex> io_lock(io_mutex);
            cout << "\n[System] Backup completed at: " << chrono::system_clock::now() << endl;
        }
    }
}

 
int find_account(int acno) {
    shared_lock<shared_mutex> lock(accounts_mutex);
    
    for (size_t i = 0; i < accounts.size(); i++) {
        if (accounts[i].retacno() == acno) {
            return i;
        }
    }
    return -1;
}

 
void write_Bank_Account() {
    Bank_Account ac;
    ac.create_Bank_Account();
    
    {
        unique_lock<shared_mutex> lock(accounts_mutex);
        accounts.push_back(ac);
    }
    
    save_accounts();
    log_transaction(ac.retacno(), "Account Creation", ac.retMoney_Deposit());
}

 
void display_sp(int n) {
    int index = find_account(n);
    
    if (index == -1) {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\tAccount number does not exist";
        return;
    }
    
    shared_lock<shared_mutex> lock(accounts_mutex);
    accounts[index].Display_Account();
}

 
void display_all() {
    system("CLS");
    
    {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\n\t\tACCOUNT HOLDER LIST\n\n";
        cout << "==================================================================\n";
        cout << "A/c no.      NAME           Type  Balance\n";
        cout << "==================================================================\n";
    }
    
    {
        shared_lock<shared_mutex> lock(accounts_mutex);
        for (const auto& acc : accounts) {
            acc.report();
        }
    }
}

 
void delete_Bank_Account(int n) {
    int index = find_account(n);
    
    if (index == -1) {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\tAccount number does not exist";
        return;
    }
    
    {
        unique_lock<shared_mutex> lock(accounts_mutex);
        accounts.erase(accounts.begin() + index);
    }
    
    save_accounts();
    
    {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\n\tAccount Deleted..";
    }
    
    log_transaction(n, "Account Deletion", 0);
}

 
void Money_Deposit_withdraw(int n, int option) {
    int amount;
    int index = find_account(n);
    
    if (index == -1) {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\tAccount number does not exist";
        return;
    }
    
    {
        lock_guard<mutex> io_lock(io_mutex);
        accounts[index].Display_Account();
        
        if (option == 1) {
            cout << "\n\n\tTO DEPOSIT AMOUNT";
            cout << "\n\n\tEnter The amount to be deposited: ";
            cin >> amount;
        } else {
            cout << "\n\n\tTO WITHDRAW AMOUNT";
            cout << "\n\n\tEnter The amount to be withdrawn: ";
            cin >> amount;
        }
    }
    
    bool success = false;
    {
        if (option == 1) {
            success = accounts[index].deposit_thread_safe(amount);
            if (success) {
                log_transaction(n, "Deposit", amount);
            }
        } else {
            success = accounts[index].withdraw_thread_safe(amount);
            if (success) {
                log_transaction(n, "Withdrawal", amount);
            } else {
                lock_guard<mutex> io_lock(io_mutex);
                cout << "\n\n\tInsufficient Balance or Invalid Amount";
            }
        }
    }
    
    save_accounts();
    
    if (success) {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\n\tTransaction Successful";
    }
}

 
void Updation_Bank_Account(int n) {
    int index = find_account(n);
    
    if (index == -1) {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\tAccount number does not exist";
        return;
    }
    
    {
        unique_lock<shared_mutex> lock(accounts_mutex);
        accounts[index].Display_Account();
        accounts[index].Updation();
    }
    
    save_accounts();
    
    {
        lock_guard<mutex> io_lock(io_mutex);
        cout << "\n\n\tAccount Updated";
    }
    
    log_transaction(n, "Account Update", 0);
}

 
void monitor_accounts() {
    while (system_running) {
        this_thread::sleep_for(chrono::seconds(30));
        
        int total = 0;
        int count = 0;
        
        {
            shared_lock<shared_mutex> lock(accounts_mutex);
            count = accounts.size();
            for (const auto& acc : accounts) {
                total += acc.retMoney_Deposit();
            }
        }
        
        {
            lock_guard<mutex> io_lock(io_mutex);
            cout << "\n[Monitor] Active accounts: " << count << ", Total balance: $" << total << endl;
        }
    }
}

int main() {
     
    load_accounts();
    
     
    thread backup_thread(backup_thread_function);
    thread monitor_thread(monitor_accounts);
    
    char ch;
    int num;
    
    do {
        {
            lock_guard<mutex> io_lock(io_mutex);
            system("CLS");
            cout << "\n\n\t\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            cout << "\t\tBANK MANAGEMENT SYSTEM";
            cout << "\n\t\t!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            cout << "\t\t    ::MAIN MENU::\n";
            cout << "\n\t\t1. NEW Bank_Account";
            cout << "\n\t\t2. Money_Deposit Total-Money";
            cout << "\n\t\t3. WITHDRAW Total-Money";
            cout << "\n\t\t4. BALANCE ENQUIRY";
            cout << "\n\t\t5. ALL Bank_Account HOLDER LIST";
            cout << "\n\t\t6. CLOSE AN Bank_Account";
            cout << "\n\t\t7. Updation AN Bank_Account";
            cout << "\n\t\t8. EXIT";
            cout << "\n\n\t\tSelect Your Option (1-8): ";
            cin >> ch;
        }
        
        switch (ch) {
            case '1':
                write_Bank_Account();
                break;
            case '2':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tPlease Enter The Bank_Account No. : ";
                    cin >> num;
                }
                Money_Deposit_withdraw(num, 1);
                break;
            case '3':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tPlease Enter The Bank_Account No. : ";
                    cin >> num;
                }
                Money_Deposit_withdraw(num, 2);
                break;
            case '4':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tPlease Enter The Bank_Account No. : ";
                    cin >> num;
                }
                display_sp(num);
                break;
            case '5':
                display_all();
                break;
            case '6':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tPlease Enter The Bank_Account No. : ";
                    cin >> num;
                }
                delete_Bank_Account(num);
                break;
            case '7':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tPlease Enter The Bank_Account No. : ";
                    cin >> num;
                }
                Updation_Bank_Account(num);
                break;
            case '8':
                {
                    lock_guard<mutex> io_lock(io_mutex);
                    system("CLS");
                    cout << "\n\n\tThank you project by::\n\tKushagra Saxena\n\tYash Pokalwar\n\tAnushka Baskota\n\tNamit Kaul";
                }
                system_running = false;
                break;
            default:
                cout << "\a";
        }
        
        {
            lock_guard<mutex> io_lock(io_mutex);
            cin.ignore();
            cin.get();
        }
    } while (ch != '8');
    
     
    backup_thread.join();
    monitor_thread.join();
    
    return 0;
}