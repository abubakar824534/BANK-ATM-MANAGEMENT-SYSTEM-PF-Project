<div align="center">

# 🏦💳 Bank ATM Manager 💳🏦
### *Project Documentation & Walkthrough — Console Banking in C++*

---

![C++](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=cplusplus&logoColor=white&color=00599C)
![Platform](https://img.shields.io/badge/Platform-Windows-informational?style=for-the-badge&logo=windows&logoColor=white&color=0078D6)
![Type](https://img.shields.io/badge/Type-Console%20App-critical?style=for-the-badge&logoColor=white&color=FF4500)
![Accounts](https://img.shields.io/badge/Capacity-2000%20Accounts-success?style=for-the-badge&logoColor=white&color=28A745)
![Status](https://img.shields.io/badge/Status-Fully%20Functional-brightgreen?style=for-the-badge&color=00C853)

---

```
 █████╗ ████████╗███╗   ███╗    ███╗   ███╗ █████╗ ███╗   ██╗ █████╗  ██████╗ ███████╗██████╗
██╔══██╗╚══██╔══╝████╗ ████║    ████╗ ████║██╔══██╗████╗  ██║██╔══██╗██╔════╝ ██╔════╝██╔══██╗
███████║   ██║   ██╔████╔██║    ██╔████╔██║███████║██╔██╗ ██║███████║██║  ███╗█████╗  ██████╔╝
██╔══██║   ██║   ██║╚██╔╝██║    ██║╚██╔╝██║██╔══██║██║╚██╗██║██╔══██║██║   ██║██╔══╝  ██╔══██╗
██║  ██║   ██║   ██║ ╚═╝ ██║    ██║ ╚═╝ ██║██║  ██║██║ ╚████║██║  ██║╚██████╔╝███████╗██║  ██║
╚═╝  ╚═╝   ╚═╝   ╚═╝     ╚═╝    ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝
```

> ### *"Secure. Fast. Reliable. Your accounts, your control."*

A comprehensive look at the **Bank ATM Management System**, a console-based application written in **C++** designed to demonstrate core **Programming Fundamentals**. This document explains the inner workings, data structures, and algorithms used in the project.

</div>

---

## 🏗️ System Architecture

This project is built using **procedural C++** (no Object-Oriented Programming). It utilizes **parallel arrays** to store account properties in memory and **text files (`.txt`)** for persistent data storage — acting as a lightweight database replacement.

### 🧠 Data Structures — In-Memory Representation

Instead of using classes or structs, the application manages up to **2,000 accounts** using parallel global arrays. Each index `i` across all arrays represents one single account:

| Array | Size | Purpose |
|---|---|---|
| `accountNumbers[2000]` | 2000 ints | Stores unique 5-digit account numbers (e.g., `10001`) |
| `pins[2000][6]` | 2000 × 6 chars | 2D char array storing 4-digit PINs + null terminators |
| `balances[2000]` | 2000 floats | The monetary balance of each account |
| `status[2000]` | 2000 chars | Account state: `'A'` Active, `'C'` Closed, `'D'` Disabled |
| `failedAttempts[2000]` | 2000 ints | Counts incorrect PIN attempts per account |
| `locked[2000]` | 2000 bools | `1` = account locked due to security breach, `0` = unlocked |

> 💡 **Why parallel arrays?** Since this project focuses on *Programming Fundamentals*, using `struct` or `class` is avoided. All six arrays are indexed in sync — `accountNumbers[3]`, `balances[3]`, `status[3]` all refer to the **same account**.

---

### 💾 File Persistence — The Database Alternative

The application doesn't use SQL or any database engine. Instead, it uses C++'s `fstream` (File Stream) library to read and write plain text files as persistent storage:

```
📁 Project Files
├── 📄 accounts.txt       → Master record of all accounts (loaded on startup)
├── 📄 transactions.txt   → Append-only log of every financial action
└── 📄 admin.txt          → Admin credentials (used for God Mode access)
```

**How it works:**

1. **`accounts.txt`** — On startup, `loadAccounts()` reads this file **line by line**, pushing each field into the parallel arrays. Every time any change occurs (deposit, withdrawal, account lock), `saveAccounts()` **overwrites** the entire file with the updated in-memory arrays.

2. **`transactions.txt`** — Every financial action triggers `recordTransactions()`. This function opens the file in `ios::app` (append mode) to add a new log line containing:
   - Transaction ID
   - Type (`1`=Deposit, `2/3`=Transfer Out/In, `4`=Withdrawal)
   - Source account
   - Destination account
   - Amount

---

## 🔐 Security & Authentication Logic

### 1️⃣ The Login Flow

When a user attempts to log in via `customerLogin()`, the following chain of checks occurs:

```
User enters Account Number
        ↓
[1] Linear Search → Find index in accountNumbers[]
        ↓
[2] Status Check  → status[index] == 'A' ? Continue : Reject
        ↓
[3] Lock Check    → locked[index] == 0  ? Continue : "Account Locked"
        ↓
[4] PIN Entry     → Up to 3 attempts, input hidden with _getch() → prints '*'
        ↓
[5] Fail 3 times? → failedAttempts hits limit → locked[index] = 1 → Save!
```

> 🔒 **Security Detail:** Input is hidden using `_getch()` from `<conio.h>`, which captures each keypress and prints `*` to the console instead of the actual digit — just like a real ATM!

---

### 2️⃣ PIN Changing — `changePIN()`

The PIN change function enforces strict validation rules before accepting a new PIN:

| Validation Step | Rule |
|---|---|
| ✅ Old PIN verification | Must match the current PIN on record |
| ✅ New PIN entered twice | Both entries must match exactly (no typos) |
| ✅ Length check | Must be **exactly 4 characters** |
| ✅ Numeric check | Every character must be between `'0'` and `'9'` |

---

## 💸 Core Algorithms

### 🔄 Transfers — `transfer()`

Transferring money between two accounts requires **atomic-style** updates to avoid data corruption. Here's the step-by-step algorithm:

```
Step 1: Linear search → verify destination account exists
Step 2: Check sender balance >= transfer amount
Step 3: balances[senderIndex] -= amount
Step 4: balances[destIndex]  += amount
Step 5: Record TWO transactions:
        → TransferOut (Type 2) for the sender
        → TransferIn  (Type 3) for the receiver
Step 6: Call saveAccounts() → persist both changes to accounts.txt
```

> ⚠️ **Why two transaction records?** One entry isn't enough — both the sender's and receiver's mini-statements must show the transfer independently.

---

### 📋 Mini Statement — `miniStatement()`

Generating a bank statement from a flat text file requires clever filtering. Here's how it works:

```cpp
// Step 1: Read ALL transactions from transactions.txt into temp arrays
// Step 2: Loop BACKWARDS (most recent → oldest)
// Step 3: Filter — only print where Source OR Destination == current user
// Step 4: Use <iomanip> setw() to align columns into a formatted table
```

| Column | Content |
|---|---|
| Transaction ID | Unique identifier for the transaction |
| Type | Deposit / Transfer Out / Transfer In / Withdrawal |
| Source | Account number that initiated the action |
| Destination | Target account (for transfers) |
| Amount | The transaction value |

---

## 👑 The Admin Panel — God Mode

The Admin panel is accessed by matching credentials from `admin.txt`. Once authenticated, the Admin gains **full control** over all accounts in the system:

| Admin Power | What It Does |
|---|---|
| 🆕 **Open New Account** | Auto-assigns the next account number, sets initial deposit |
| 🚫 **Close Account** | Sets `status[index] = 'C'` — account becomes inaccessible |
| ⏸️ **Suspend Account** | Sets `status[index] = 'D'` — temporarily disables access |
| 🔓 **Unlock Account** | Resets `locked[index] = 0` and `failedAttempts[index] = 0` |
| 📊 **View All Accounts** | Displays every account with full details |
| 🔀 **Sort by Balance** | Uses **Bubble Sort** to reorder accounts Ascending or Descending |

### 🔀 Bubble Sort — How Accounts Are Sorted

The admin's "sort by balance" feature uses the classic **Bubble Sort** algorithm across all 6 parallel arrays simultaneously. When two accounts are swapped, **all corresponding values** in every array must be swapped together to maintain synchronization:

```cpp
// For every pass through the array:
// If balances[j] > balances[j+1] → swap ALL 6 arrays at index j and j+1
// This keeps accountNumbers, pins, status, locked, failedAttempts in sync
```

---

## 🚀 How to Run the Project

### Prerequisites
- ✅ **Windows OS** (uses `<conio.h>` and `_getch()` for hidden input)
- ✅ **C++ Compiler** — GCC (MinGW) or MSVC
- ✅ **Supporting files** in the same directory as the executable

### Step-by-Step

```bash
# Step 1: Clone or download this repository
git clone https://github.com/YOUR_USERNAME/bank-atm-manager.git

# Step 2: Navigate into the folder
cd bank-atm-manager

# Step 3: Compile the source file
g++ "bscs25109 PRO 01.cpp" -o atm_manager.exe

# Step 4: Run the program
./atm_manager.exe
```

> ⚠️ **Important:** Make sure `accounts.txt`, `transactions.txt`, and `admin.txt` exist **in the same folder** as the executable. Without them, the file streams will fail to load/save data correctly.

---

## 📁 Project Structure

```
📦 Bank ATM Manager/
├── 🧠 bscs25109 PRO 01.cpp            <- Main source file (all logic here)
├── 📄 accounts.txt                    <- Persistent account storage
├── 📄 transactions.txt                <- Append-only transaction log
├── 📄 admin.txt                       <- Admin credentials file
├── 📋 REPORT of BANK ATM SYSTEM.pdf  <- Full project report
├── 📸 BSCS25109 BANK ATM SYSTEM SCREENSHOT.pdf  <- Terminal screenshots
└── 📖 README 1.md                     <- You are here!
```

---

## 🌐 Concepts Demonstrated

```
✅ Parallel Arrays          (In-memory data management without structs)
✅ File I/O (fstream)       (ofstream / ifstream for persistent storage)
✅ Linear Search            (Finding accounts by account number)
✅ Bubble Sort Algorithm    (Sorting accounts by balance)
✅ Input Validation         (PIN length, numeric check, match check)
✅ Security via _getch()    (Hidden PIN input like a real ATM)
✅ Append-mode File Writing (ios::app for transaction logging)
✅ Loops & Conditionals     (All menu logic and control flow)
✅ Functions (Modular)      (Each feature is its own function)
✅ Global State Management  (Shared arrays across all functions)
```

---

## 📚 Supporting Documents

For more in-depth diagrams, flowcharts, and examples:

| Document | Description |
|---|---|
| 📋 [**Project Report (PDF)**](REPORT%20of%20BANK%20ATM%20SYSTEM.pdf) | Full written report covering design decisions |
| 📸 [**Terminal Screenshots (PDF)**](BSCS25109%20BANK%20ATM%20SYSTEM%20SCREENSHOT.pdf) | Live screenshots of every menu and feature |

---

<div align="center">

---

### ⚡ Built with Passion — Pure Procedural C++ ⚡

```
  💳 [ LOGIN ]  →  [ PIN CHECK ]  →  [ DASHBOARD ]
       ↓                 ↓                  ↓
  Account #         3 Attempts          Deposit 💰
  Lookup            _getch() *          Withdraw 💸
  Linear            Lock on Fail        Transfer 🔄
  Search            Admin Unlock        Statement 📋
```

**🏦 Secure. Fast. Reliable. Built from scratch in C++. 🏆**

---

*© 2025 Abu Bakar — BSCS25109 | Programming Fundamentals Project*

</div>
