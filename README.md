<div align="center">

# 📖 Bank ATM Manager: Project Documentation & Walkthrough

A comprehensive look at the **Bank ATM Management System**, a console-based application written in C++ designed to demonstrate core **Programming Fundamentals**. This document explains the inner workings, data structures, and algorithms used in the project.

</div>

---

## 🏗️ System Architecture

This project is built using procedural C++ (no Object-Oriented Programming). It utilizes parallel arrays to store account properties in memory and text files (`.txt`) for persistent data storage.

### 🧠 Data Structures (In-Memory)
Instead of using classes or structs, the application manages up to **2000 accounts** using parallel global arrays:
- `accountNumbers[2000]`: Stores the unique 5-digit account numbers (e.g., `10001`).
- `pins[2000][6]`: A 2D character array storing the 4-digit PINs (plus null terminators).
- `balances[2000]`: Stores the monetary balance of each account.
- `status[2000]`: Character array tracking if an account is Active (`'A'`), Closed (`'C'`), or Disabled (`'D'`).
- `failedAttempts[2000]`: Tracks how many times an incorrect PIN was entered.
- `locked[2000]`: Boolean flag (0 or 1) indicating if the account is locked due to security breaches.

### 💾 File Persistence (Database Alternative)
The application doesn't use a database like SQL. Instead, it uses `fstream` (File Stream) to write and read text files:
1. **`accounts.txt`**: When the program starts, `loadAccounts()` reads this file line by line, pushing data into the parallel arrays. Every time a change happens (deposit, withdraw, lock), `saveAccounts()` overwrites this file with the updated arrays.
2. **`transactions.txt`**: Every financial action calls `recordTransactions()`. This function opens the file in `ios::app` (append mode) to add a new line containing the Transaction ID, Type (1=Deposit, 2/3=Transfer, 4=Withdraw), Source, Destination, and Amount.

---

## 🔐 Security & Authentication Logic

### 1. The Login Flow
When a user attempts to log in (`customerLogin()`):
1. **Linear Search:** The system searches the `accountNumbers` array to find the index of the provided account.
2. **Status Checks:** It verifies that `status[index] == 'A'` (Active) and `locked[index] == 0`.
3. **PIN Validation:** The user gets 3 attempts (`loginAttempts = 3`). Input is hidden using `_getch()` which prints `*` to the console instead of the actual number.
4. **Lockout Mechanism:** If the user fails 3 times, `failedAttempts[index]` hits the limit, `locked[index]` is set to `1`, and the account is saved. Only an Admin can unlock it.

### 2. PIN Changing
The `changePIN()` function requires the user to input their old PIN, then input the new PIN twice to ensure there are no typos. It strictly validates that the PIN is exactly 4 characters long and contains only numeric digits (`'0'` to `'9'`).

---

## 💸 Core Algorithms

### Transfers (`transfer()`)
Transferring money requires updating two accounts simultaneously:
1. It verifies the destination account exists using a linear search.
2. It checks if the sender has sufficient balance.
3. It subtracts the amount from `balances[senderIndex]` and adds it to `balances[destIndex]`.
4. It records **two** transactions in the history: one `TransferOut` (Type 2) for the sender, and one `TransferIn` (Type 3) for the receiver.
5. It triggers `saveAccounts()` to ensure the transfer is permanent.

### Mini Statement (`miniStatement()`)
To generate a statement, the application:
1. Reads all history from `transactions.txt` into temporary arrays.
2. Loops backwards (from the most recent transaction to the oldest).
3. Filters the results so it only prints transactions where the `Source` or `Destination` matches the logged-in user.
4. Uses `<iomanip>` (e.g., `setw()`) to beautifully format the output table.

---

## 👑 The Admin Panel (God Mode)

The system includes an Admin menu that can be accessed by matching the credentials stored in `admin.txt`. 
The Admin has the power to:
- **Open new accounts**: Automatically assigns the next available account number (e.g., `10005`) and sets initial deposit.
- **Close or Suspend accounts**: Modifies the `status` array.
- **Unlock accounts**: Resets the `locked` flag and `failedAttempts` to `0`.
- **Display & Sort**: Admins can view all accounts. The application uses a **Bubble Sort algorithm** to sort accounts in Ascending or Descending order based on their `balances` before displaying them.

---

## 🚀 How to Run the Project

1. Clone or download this repository.
2. Ensure you have a C++ compiler (like GCC or MSVC).
3. Open `bscs25109 PRO 01.cpp` and compile it:
   ```bash
   g++ "bscs25109 PRO 01.cpp" -o atm_manager
   ```
4. Run the executable:
   ```bash
   ./atm_manager
   ```
5. **Requirement:** Ensure `accounts.txt`, `transactions.txt`, and `admin.txt` are in the same folder as the executable for the file streams to work properly.

---

## 📚 Supporting Documents
For more in-depth diagrams and examples:
- [**Project Report (PDF)**](BSCS25109%20BANK%20ATM%20SYSTEM%20SCREENSHOT.pdf)
- [**Terminal Screenshots (PDF)**](BANK%20ATM%20SYSTEM%20SCREENSHOT.pdf)
