# Multithreaded Bank Management System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Concurrency](https://img.shields.io/badge/Concurrency-Enabled-green.svg)](https://en.cppreference.com/w/cpp/thread)

A robust, concurrent bank management system demonstrating modern C++ multithreading capabilities with thread-safe operations, automatic backups, and transaction monitoring.

![Bank Management System](https://raw.githubusercontent.com/username/multithreaded-bank-management/main/docs/images/banner.png)

##  Features

- **Fully Thread-Safe Operations**: Secure account transactions with fine-grained locking mechanisms
- **Concurrent User Support**: Multiple users can access the system simultaneously
- **Automatic Background Services**:
  - Periodic backup threads for data persistence
  - Real-time account monitoring
  - Transaction logging with thread identification
- **Modern Memory Management**: In-memory data structures with periodic disk syncing
- **Comprehensive Account Operations**:
  - Create new accounts
  - Deposit/withdraw funds
  - Balance inquiries
  - Account updates
  - Account closure

##  Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 19.14+)
- CMake 3.10+ (optional, for easier building)
- Git (for cloning the repository)

### Installation

#### Option 1: Using CMake

```bash
# Clone the repository
git clone https://github.com/username/multithreaded-bank-management.git
cd multithreaded-bank-management

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Run the application
./bank_management
```

#### Option 2: Direct Compilation

```bash
# Clone the repository
git clone https://github.com/username/multithreaded-bank-management.git
cd multithreaded-bank-management

# Compile with C++17 support
g++ -std=c++17 -pthread main.cpp -o bank_management

# Run the application
./bank_management
```

## 🔍 System Architecture

### Concurrency Model

This system uses a hybrid concurrency approach:
- **Reader-Writer Locks**: Optimize for concurrent reads of account data
- **Mutex Guards**: Protect critical sections like I/O operations
- **Per-Account Locks**: Enable parallel operations on different accounts
- **Atomic Variables**: Ensure thread-safe state changes

### Data Flow

```
┌───────────────┐     ┌─────────────────┐     ┌────────────────┐
│  User Input   │────▶│  Thread-Safe    │────▶│  In-Memory     │
│  Interface    │     │  Operations     │     │  Data Store    │
└───────────────┘     └─────────────────┘     └────────────────┘
                             │                        │
                             │                        │
                             ▼                        ▼
                      ┌─────────────────┐     ┌────────────────┐
                      │  Background     │     │  Persistent    │
                      │  Services       │────▶│  Storage       │
                      └─────────────────┘     └────────────────┘
```

## 💡 Usage Guide

### Main Menu

The system provides the following options through an interactive menu:

1. **Create New Account**: Set up accounts with initial deposits
2. **Deposit Funds**: Add money to existing accounts
3. **Withdraw Funds**: Remove money with balance validation
4. **Balance Inquiry**: Check current account status
5. **Account Holder List**: View all accounts in the system
6. **Close Account**: Remove accounts from the system
7. **Update Account**: Modify account details
8. **Exit**: Safely terminate the application

### Example Operations

#### Creating a New Account

```
Please Enter the Bank_Account No.: 1001
Please Enter the Name of the Bank_Account holder: John Doe
Please Enter Type of the Bank_Account (C/S): S
Please Enter The Starting Total-Money: 5000

Bank_Account Created..
```

#### Making a Deposit

```
Please Enter The Bank_Account No.: 1001

Bank_Account No.: 1001
Bank_Account Holder Name: John Doe
Type of Bank_Account: S
Balance Total-Money: 5000

TO DEPOSIT AMOUNT
Enter The amount to be deposited: 2500

Transaction Successful
```

## 🛠️ Technical Implementation Details

### Thread-Safe Account Operations

```cpp
// Thread-safe deposit operation
bool Bank_Account::deposit_thread_safe(int amount) {
    lock_guard<mutex> lock(account_mutex);
    if (amount > 0) {
        Money_Deposit += amount;
        return true;
    }
    return false;
}
```

### Background Services

```cpp
// Thread function for periodic data backup
void backup_thread_function() {
    while (system_running) {
        this_thread::sleep_for(chrono::minutes(5)); // Backup every 5 minutes
        save_accounts();
        
        {
            lock_guard<mutex> io_lock(io_mutex);
            cout << "\n[System] Backup completed at: " << chrono::system_clock::now() << endl;
        }
    }
}
```

##  Synchronization Mechanisms

| Operation | Lock Type | Scope | Purpose |
|-----------|-----------|-------|---------|
| Account Read | Shared Lock | Global | Allow concurrent reads |
| Account Write | Unique Lock | Global | Prevent concurrent modification |
| I/O Operations | Mutex | Global | Prevent garbled output |
| Single Account Op | Mutex | Per Account | Enable parallel account operations |

##  Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

##  License

This project is licensed under the MIT License - see the LICENSE file for details.

##  Authors

- **Kushagra Saxena**

##  Acknowledgments

- C++ Standards Committee for the excellent threading library
- Modern C++ concurrency patterns
- All contributors and testers who helped improve this system
