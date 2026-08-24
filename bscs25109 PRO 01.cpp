/*
acount.txt sample data:
accountNumber  pin  balance  status  failedAttempts  locked  needsApproval
10001         1122  5000     A        0                 0      0
10002         3344  2000     A        0                 0      0
10003         5566  0        A        0                 0      1
10004         7788  15000    D        0                 0      0


transactions.txt sample data:
TransactionID   Type   SourceAccount   DestinationAccount   Amount
1                1        10001         -1                  2000      
2                4        10001         -1                  1000      
3                2        10001         10002               500    
4                3        10002         10001               500    

admin.txt sample data:
GXHX7:65>      ->BSCS25109
*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
using namespace std;


const int maxAccounts = 2000;
const int loginAttempts = 3;

long long accountNumbers[maxAccounts];
char pins[maxAccounts][6];
long long balances[maxAccounts];
char status[maxAccounts];
int failedAttempts[maxAccounts];
int locked[maxAccounts];
int totalAccounts = 0;
int nextTransactionID = 1;
int needsApproval[maxAccounts];


void saveAccounts()
{
    ofstream fout("accounts.txt");
    if(!fout)
    {
        cout << "No accounts file found"<<endl;
        return;
	}
    for (int i = 0; i < totalAccounts; i++)
    {
        fout << accountNumbers[i] << " "<< pins[i] << " "<< balances[i] << " "<< status[i] << " "<< failedAttempts[i] << " " << locked[i] << " "<< needsApproval[i] << endl;
    }
    fout.close();
}


void loadAccounts()
{
    ifstream fin("accounts.txt");

    if (!fin)
    {
        cout << "No accounts file found"<<endl;
		return;
    }

    totalAccounts = 0;
    while (fin >> accountNumbers[totalAccounts] >> pins[totalAccounts] >> balances[totalAccounts]>> status[totalAccounts]>> failedAttempts[totalAccounts]>> locked[totalAccounts]>> needsApproval[totalAccounts]) 
    {
        totalAccounts++;
    }
    fin.close();
}


void recordTransactions(int type, long long source, long long destination, long long amount)
{
    
    ifstream fin("transactions.txt");
    int lastID = 0;
    int tranID;
    int t;
    long long s;
    int d;
    int a;
    while (fin >> tranID >> t >> s >> d >> a)
    {
        lastID = tranID;
    }
    fin.close();

    int nextID = lastID + 1;

    ofstream fout("transactions.txt", ios::app);

    fout << nextID << " " << type << " " << source << " " << destination << " " << amount << endl;
    fout.close();
}


void deposit(int index)
{
    long long amount;
    cout << "Enter amount to deposit: ";
    cin >> amount;

    if (amount <= 0)
    {
        cout << "Invalid amount"<<endl;
        return;
    }

    balances[index] = balances[index]+ amount;

    recordTransactions(1, accountNumbers[index], -1, amount);
    saveAccounts();

    cout << "Deposit successful"<<endl<<endl;
    cout << "New balance : " << balances[index] << endl;
}

 
void withdraw(int index)
{
    long long amount;
    cout << "Enter amount to withdraw: ";
    cin >> amount;
    if (amount <= 0 || amount > balances[index])
    {
        cout << "Invalid or insufficient balance"<<endl;
        return;
    }

    balances[index] = balances[index] - amount;

    recordTransactions( 4, accountNumbers[index], -1, amount);
    saveAccounts();

    cout << "Withdraw successful" << endl<<endl;
    cout << "New balance : " << balances[index] << endl;
}

void transfer(int index)
{
    long long destination;
    long long amount;
    int destIdx = -1;
    cout << "Enter destination account: ";
    cin >> destination;
    cout << "Enter amount to transfer: ";
    cin >> amount;

    
    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == destination)
        {
            destIdx = i;
            break;
        }
    }

    if (destIdx == -1)
    {
        cout << "Destination account does not exist. Check the number and try again." << endl;
        return;
    }
    if (amount <= 0)
    {
        cout << "Invalid transfer amount. Must be greater than zero." << endl;
        return;
    }
    if (amount > balances[index])
    {
        cout << "Insufficient balance. Your current balance is " << balances[index] << endl;
        return;
    }

    balances[index] = balances[index ]- amount;
    balances[destIdx] = balances[destIdx]+amount;

   
    recordTransactions(2, accountNumbers[index], destination, amount);
   
    recordTransactions(3, destination, accountNumbers[index], amount);

    saveAccounts();

    cout << "Transfer successful"<<endl;
}


void checkBalance(int index)
{
    cout << "Current Balance: " << balances[index] << endl;
}

void miniStatement(long long accountNo)
{
    ifstream fin("transactions.txt");
    if (!fin)
    {
        cout << "No transactions found" << endl;
        return;
    }
    
    int ids[1000];
    int types[1000];
    long long sources[1000];
    long long destinations[1000];
    long long amounts[1000];
    int count = 0;


    while (fin >> ids[count] >> types[count] >> sources[count] >> destinations[count] >> amounts[count])
    {
        count++;
        if (count >= 1000)
        {
            break;
        }
    }
    fin.close();


    int k;
    cout << "Enter number of last transactions to view: ";
    cin >> k;
    if (k > count)
    {
        k = count;
    }
    cout << "<=================><o.o><================>" << endl;
    cout << "ID    Type          Amount    Counterparty" << endl;
    cout << "<=================><o.o><================>" << endl;

    int shown = 0;
    for (int i = count - 1; i >= 0 && shown < k; i--)
    {
        if (sources[i] == accountNo || destinations[i] == accountNo)
        {
            cout << setw(3) << ids[i] << " ";

            if (types[i] == 1)
            {
                cout << "Deposit       ";
            }
            else if (types[i] == 2)
            {
                cout << "TransferOut   ";
            }
            else if (types[i] == 3)
            {
                cout << "TransferIn    ";
            }
            else if (types[i] == 4)
            {
                cout << "Withdraw      ";
            }
            else
            {
                cout << "Unknown       ";
            }

            cout << setw(8) << amounts[i] << " ";

            if (types[i] == 2)
            {
                cout << "  " << amounts[i];
            }
            else if (types[i] == 3)
            {
                cout << "  " << sources[i];
            }

            cout << endl;
            shown++;
        }
    }

    if (shown == 0)
    {
        cout << "No transactions found for this account"<<endl;
    }
}
void changePIN(int index)
{
    cout << "Enter current PIN: ";
    char oldPIN[6];
    char ch;
    int i = 0;
    while ((ch = _getch()) != 13 && i < 5)
    { 
        oldPIN[i] = ch;
        cout << '*';
        i++;
    }
    oldPIN[i] = '\0'; 
    cout << endl;

    int match = 1;
    for (int j = 0; j < 6; j++)
    {
        if (pins[index][j] != oldPIN[j])
        {
            match = 0;
          break;
        }
        if (pins[index][j] == '\0' && oldPIN[j] == '\0')
        {
            break;
        }
    }
    if (!match)
    {
        cout << "Incorrect PIN"<<endl;
        { 
            return;
        }
    }

    cout << "Enter new PIN: ";
    char newpin1[6];
    i = 0;
    while ((ch = _getch()) != 13 && i < 4)
    { 
        newpin1[i++] = ch;
        cout << '*';
    }
    newpin1[i] = '\0';
    cout << endl;

    cout << "Please Re enter new PIN again to confirm : ";
    char newpin2[6];
    i = 0;
    while ((ch = _getch()) != 13 && i < 4)
    {
        newpin2[i] = ch;
        cout << '*';
        i++;

    }
    newpin2[i] = '\0';
    cout << endl;


    int len = 0;
    while (newpin1[len] != '\0')
    {
        len++;
    }
    if (len != 4) 
    { 
        cout << "PIN must be exactly four digits"<<endl;
        return;
    }

    for (int j = 0; j < 4; j++)
    {
        if (newpin1[j] < '0' || newpin1[j] > '9')
        { 
            cout << "PIN must contain only digits"<<endl;
            return;
        }
    }
    int same = 1;
    for (int j = 0; j < 6; j++)
    {
        if (newpin1[j] != newpin2[j])
        { 
            same = 0;
            break;
        }
        if (newpin1[j] == '\0' && newpin2[j] == '\0')
        {
            break;
        }
    }
    if (!same) 
    {
        cout << "PIN mismatch please try again after some time"<<endl; 
        { 
            return;
        }
    }
    for (int j = 0; j < 6; j++)
    {
        pins[index][j] = newpin1[j];
    } 
    saveAccounts();

    cout << "PIN changed successfully"<<endl;
}


void customerMenu(int index)
{
    int c;
    do {
        system("cls");
        cout << "<===============================>" << endl;
        cout << "         CUSTOMER MENU           " << endl;
        cout << "<===============================>" << endl;
        cout << "1. Deposit" << endl;
        cout << "2. Withdraw" << endl;
        cout << "3. Check Balance" << endl;
        cout << "4. Transfer" << endl;
        cout << "5. Change PIN" << endl;
        cout << "6. Mini Statement" << endl;
        cout << "7. Logout" << endl<<endl;
        cout << "<------------------------------->" << endl;
        cout << "Enter choice: ";
        cin >> c;
       
        if (c == 1)
        {
            deposit(index);
        }
        else if (c == 2)
        {
            withdraw(index);
        }
        else if (c == 3)
        {
            checkBalance(index);
        }
        else if (c == 4)
        {
            transfer(index);
        }
        else if (c == 5)
        {
            changePIN(index);
        }
        else if (c == 6)
        {
            miniStatement(accountNumbers[index]);
        }

        if (c != 7) 
        {
            cout <<endl<< "Press any key to continue";
            _getch();
        }
    } while (c != 7);
}


void customerLogin()
{
    long long account;
    int index = 0;
    cout << "Enter account number: ";
    cin >> account;
   
    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == account)
        {
            index = i;
        }
    }

    if (index == -1)
    {
        cout << "Account not found"<<endl;
        return; 
    }
    if (status[index] != 'A')
    { 
        cout << "Account not active"<<endl;
        return;
    }
    if (locked[index])
    { 
        cout << "Account locked"<<endl;
        return; 
    }

    int attempts = 0;
    while (attempts < loginAttempts)
    {
        cout << "Enter PIN: ";

        char pin[6]; 
        char ch;
        int i = 0;
        while ((ch = _getch()) != 13 && i < 5) 
        { 
            pin[i] = ch; 
            cout << '*';
            i++;
        }
        pin[i] = '\0';
        cout << endl;

        int found = 1;
        for (int j = 0; j < 6; j++)
        {
            if (pins[index][j] != pin[j])
            { 
                found = 0; 
                break;
            }
            if (pins[index][j] == '\0' && pin[j] == '\0')
            {
                break;
            }
        }

        if (found)
        {
            failedAttempts[index] = 0;
            saveAccounts();

            if (needsApproval[index])
            {
                cout << "Login successful but your account is pending admin approval"<<endl;
                cout << "You cannot perform any transactions until approved"<<endl;
                return;
            }

            cout << "Login successful"<<endl;

            customerMenu(index);
            return;
        }
        else
        {
            attempts++;
            failedAttempts[index]++;
            cout << "Incorrect PIN. Attempts left: " << (3 - attempts) << endl;
        }
    }
    locked[index] = 1;
    saveAccounts();
    cout << "Account locked due to 3 failed attempts."<<endl;
}



void openAccount()
{
    long long account;

    if (totalAccounts == 0)
    {
        account = 10001;
    }
    else
    {
        account = accountNumbers[totalAccounts - 1] + 1;
    }

    cout << "New Account Number assigned: " << account << endl;

    
    char pin[6];
    int pin2[6];
    char ch;
    int i;
    do
    {
        cout << "Enter 4-digit PIN: ";
        i = 0;
        while ((ch = _getch()) != 13 && i < 4)
        {
            if (ch >= '0' && ch <= '9') 
            {
                pin[i] = ch;
                cout << '*';
                i++;
            }
        }
        pin[i] = '\0';
        cout << endl;

        if (i != 4)
        {
            cout << "PIN must be exactly 4 digits, Try again"<<endl;
        }
    } while (i != 4);

    i = 0;
    cout << "Re-enter PIN: ";
    while ((ch = _getch()) != 13 && i < 4)
    {
        if (ch >= '0' && ch <= '9') 
        {
            pin2[i] = ch;
            cout << '*';
            i++;
        }
    }
    pin2[i] = '\0';
    cout << endl;

  
    int same = 1;
    for (int j = 0; j < 5; j++)
    {
        if (pin[j] != pin2[j])
        {
            same = 0;
            break;
        }
        if (pin[j] == '\0' && pin2[j] == '\0')
            break;
    }

    if (!same)
    {
        cout << "PIN mismatch. Account creation cancel"<<endl;
        return;
    }

    for (int j = 0; j < 6; j++)
    {
        pins[totalAccounts][j] = pin[j];
    }

   
    long long deposit;
    do
    {
        cout << "Enter initial deposit : ";
        cin >> deposit;
        if (deposit < 0)
        {
            cout << "Deposit cannot be negative"<<endl;
        }
    } while (deposit < 0);

    balances[totalAccounts] = deposit;

    accountNumbers[totalAccounts] = account;
    status[totalAccounts] = 'A';
    failedAttempts[totalAccounts] = 0;
    locked[totalAccounts] = 0;
    needsApproval[totalAccounts] = 1;

    totalAccounts++;
    saveAccounts();

    cout << "Account created successfully" << endl << endl;
    cout << "New Account Number is: " << account << endl;
}

void closeAccount()
{
    long long account;
    cout << "Enter Account Number to close: ";
    
    cin >> account;
    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == account)
        {
            status[i] = 'C';
            saveAccounts();
            cout << "Account closed" << endl;
            return;
        }
    }
    cout << "Account not found" << endl;
}

void changeAccountStatus()
{
    long long account;
    cout << "Enter Account Number: ";
     cin >> account;
    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == account)
        {
            if (status[i] == 'A')
            {
                status[i] = 'D';
            }
            else if (status[i] == 'D')
            {
                status[i] = 'A';
            }

            saveAccounts();
            cout << "Account status changed" << endl;
            return;
        }
    }
    cout << "Account not found" << endl;
}

void unlockAccount()
{
    long long account;
    cout << "Enter Account Number to unlock: ";
     cin >> account;
    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == account)
        {
            locked[i] = 0;
            failedAttempts[i] = 0;
            saveAccounts();
            cout << "Account unlocked" << endl;
            return;
        }
    }
    cout << "Account not found" << endl;;
}

void searchAccount()
{
    long long account;
    cout << "Enter Account Number: ";
     cin >> account;
    for (int i = 0; i < totalAccounts; i++) 
    {
        if (accountNumbers[i] == account)
        {
            cout << "Account: " << accountNumbers[i] << " | Bal: " << balances[i]<< " | Status: " << status[i] << " | Locked: " << locked[i] << endl;
            return;
        }
    }
    cout << "Not found" << endl;
}
void swapAccounts(int i, int j)
{
    long long tempAcc;
    long long tempBal;
    char tempStatus;
    int tempLocked;


    tempAcc = accountNumbers[i];
    accountNumbers[i] = accountNumbers[j];
    accountNumbers[j] = tempAcc;

    
    tempBal = balances[i];
    balances[i] = balances[j];
    balances[j] = tempBal;

    
    tempStatus = status[i];
    status[i] = status[j];
    status[j] = tempStatus;

    
    tempLocked = locked[i];
    locked[i] = locked[j];
    locked[j] = tempLocked;
}

void displayAllAccounts()
{
    if (totalAccounts == 0)
    {
        cout << "No accounts to display" << endl;
        return;
    }

    int order;
	cout << "<==============================>" << endl;
    cout << "      Display All Accounts      " << endl;
	cout << "<==============================>" << endl;
    cout << " 1. Ascending " << endl;
    cout << " 2. Descending "<<endl;
	cout << "Enter choice: ";
    cin >> order;

    if (totalAccounts <= 1)
    {
        cout << "Nothing to sort. Only " << totalAccounts << " account available." << endl;
        return;
    }
    for (int i = 0; i < totalAccounts - 1; i++)
    {
        for (int j = 0; j < totalAccounts - i - 1; j++)
        {
            bool swapFlag = false;

            if ((order == 1 && balances[j] > balances[j + 1]) || (order == 2 && balances[j] < balances[j + 1]))   
            {
                swapFlag = true;
            }

            if (swapFlag)
            {
                swapAccounts(j, j + 1);
            }

        }
    }

    cout << "AccNo\tBalance\tStatus\tLocked" << endl;
    for (int i = 0; i < totalAccounts; i++)
    {
        cout << accountNumbers[i] << "\t" << balances[i] << "\t" << status[i] << "\t" << locked[i] << endl;
    }
}


void summaryReport()
{
    int active = 0;
    int closed = 0;
    int deactive = 0;
    int lockcount = 0;
    long long totalBalance = 0;
    for (int i = 0; i < totalAccounts; i++)
    {
        if (status[i] == 'A')
        {
            active++;
        }
        if (status[i] == 'C')
        {
            closed++;
        }
        if (status[i] == 'D')
        {
            deactive++;
        }
        if (locked[i] == 1)
        {
            lockcount++;
        }
        totalBalance += balances[i];
    }

    cout << "<==============================>" << endl;
    cout << "         Summary Report         " << endl;
    cout << "<==============================>" << endl;
    cout << "1. Active: " << active << endl;
    cout << "2. Closed: " << closed << endl;
    cout << "3. Deactivated: " << deactive << endl;
    cout << "4 .Locked: " << lockcount << endl;
    cout << "5. Total Balance: " << totalBalance << endl;
}
void approveAccounts()
{
    cout << "<==================================>" << endl;
    cout << "      Accounts Pending Approval     " << endl;
    cout << "<==================================>" << endl;
    bool found = false;
    for (int i = 0; i < totalAccounts; i++)
    {
        if (needsApproval[i])
        {
            cout << "Account: " << accountNumbers[i] << endl;
            cout << "Balance: " << balances[i] << endl;
            found = true;
        }
    }

    if (!found)
    {
        cout << "No accounts pending approval" << endl;
        return;
    }
    long long account;
    cout << "Enter account number to approve : ";
   
    cin >> account;

    

    for (int i = 0; i < totalAccounts; i++)
    {
        if (accountNumbers[i] == account)
        {
            needsApproval[i] = 0;
            saveAccounts();
            cout << "Account " << account << " approved successfully" << endl;
            return;
        }
    }

    cout << "Account not found" << endl;;
}

void managerMenu()
{
    int ch;
    do
    {
        system("cls");
        cout << "<==============================>" << endl;
        cout << "          MANAGER MENU          " << endl;
        cout << "<==============================>" << endl;
        cout << "1. Open New Account" << endl;
        cout << "2. Close Account" << endl;
        cout << "3. Deactivate/Activate" << endl;
        cout << "4. Unlock Account" << endl;
        cout << "5. Search Account" << endl;
        cout << "6. Display All" << endl;
        cout << "7. Summary Report" << endl;
        cout << "8. Approve Accounts" << endl;
        cout << "9. Back" << endl<<endl;
        cout << "<==============================>" << endl<<endl;
        cout << "Enter choice: ";
        cin >> ch;
        

        if (ch == 1)
        {
            openAccount();
        }
        else if (ch == 2)
        {
            closeAccount();
        }
        else if (ch == 3)
        {
            changeAccountStatus();
        }
        else if (ch == 4)
        {
            unlockAccount();
        }
        else if (ch == 5)
        {
            searchAccount();
        }
        else if (ch == 6)
        {
            displayAllAccounts();
        }
        else if (ch == 7)
        {
            summaryReport();
        }
        else if (ch == 8)
        {
            approveAccounts();
        }

        if (ch != 9) 
        {
            cout <<endl<< "Press any key to continue";
            _getch();
        }

    } while (ch != 9);

   
}
int managerLogin()
{
    ifstream fin("admin.txt");

    if (!fin)
    {
        cout << "File not found" << endl;
        return 0;
    }
    char encryptedPass[40];
    char realPass[40];
    char inputPass[40];
    fin >> encryptedPass;
    fin.close();

    int i = 0;
    while (encryptedPass[i] != '\0')
    {
        realPass[i] = encryptedPass[i] - 5;   
        i++;
    }
    realPass[i] = '\0';

    cout << "Enter Manager Password: ";
    char ch;
    int j = 0;
    while ((ch = _getch()) != 13 && j < 39)
    {
        inputPass[j] = ch;
        cout << '*';
        j++;
    }
    inputPass[j] = '\0';
    cout << endl;

    int match = 1;
    for (int j = 0; realPass[j] != '\0' || inputPass[j] != '\0'; j++)
    {
        if (realPass[j] != inputPass[j])
        {
            match = 0;
            break;
        }
    }

    if (match)
    {
        cout << "Manager login successful" << endl;
        return 1;
    }
    else
    {
        cout << "Invalid manager password please try again " << endl;
        return 0;
    }
}


int main() 
{
    loadAccounts();
    
    int ch;
    do 
    {
        Sleep(1500);
        system("cls");
        cout << "<==============================>" << endl;
        cout << "       BANK ATM SYSTEM          " << endl;
        cout << "<==============================>" << endl;
        cout << "1. Customer Login" << endl;
        cout << "2. Manager Login" << endl;
        cout << "3. Exit" << endl << endl;
        cout << "<==============================>" << endl << endl;
        cout << "Enter choice: ";
        cin >> ch;
         
        if (ch == 1)
        {
            customerLogin();
        }
        else if (ch == 2)
        { 
              
            if (managerLogin() == 1)
            {
                managerMenu();
            }
        }

    } while (ch != 3);
    
 
    system("cls");
    cout << "<========================================>" << endl;
    cout << "   Thank you for using BANK ATM SYSTEM    " << endl;
    cout << "          Your money is safe              " << endl;
    cout << "                 /\\_/\\                  " << endl;
    cout << "                ( o.o )                   " << endl;
    cout << "                 > ^ <                    " << endl;
    cout << "       Have a secure and pleasant day     " << endl;
    cout << "<========================================>" << endl;

	return 0;
}
