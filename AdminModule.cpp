// ============================================================================
// ADMIN MODULE IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "AdminModule.h"

// Standard Libraries
#include <iostream>    // Standard I/O streams (cout, cin)
#include <cstdlib>     // Standard library (system calls, conversion)
#include <iomanip>     // I/O manipulation (setw)
#include <limits>      // Numeric limits used for input buffer handling
#include "Utils.h"     // Shared Utility Functions

using namespace std;

// ============================================================================
// 2/8 printSuccess
// ============================================================================
static void printSuccess(string msg)
{
    cout << "\n   ─────────────────────────────────────────────────────\n";
    cout << "    \033[1;32m✔ " << msg << "\033[0m\n";
    cout << "   ─────────────────────────────────────────────────────\n";
}

// ============================================================================
// 3/8 AdminModule (Constructor)
// ============================================================================
AdminModule::AdminModule(MYSQL* c)
{
    conn = c;
}

// ============================================================================
// 4/8 login
// ============================================================================
string AdminModule::login()
{
    string u;
    string p;

    // --------------------------------------------------
    // Display Login Screen
    // --------------------------------------------------
    cout << "\n\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                     SYSTEM LOGIN                     ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "   [ INFO: Enter 0 to Exit ]\n\n";
    
    // --------------------------------------------------
    // Input Credential
    // --------------------------------------------------
    cout << "   Username ➜ ";
    u = Utils::getValidString();

    // #### Exit Check ####
    if (u == "0")
    {
        return "EXIT";
    }

    cout << "   Password ➜ ";
    p = Utils::getValidString();

    // --------------------------------------------------
    // Authenticate Against Database
    // --------------------------------------------------
    string query = "SELECT role FROM admins WHERE username='" + u + "' AND password='" + p + "'";
    
    // #### Database Query Check ####
    if (mysql_query(conn, query.c_str()))
    {
        return "";
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    // #### Validation Check ####
    if (row)
    {
        string role = row[0];
        mysql_free_result(res);
        return role;
    }

    mysql_free_result(res);
    return "";
}

// ============================================================================
// 5/8 manageAdmins
// ============================================================================
void AdminModule::manageAdmins()
{
    int choice;

    do
    {
        system("cls");
        
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║                ADMIN ACCOUNT CONTROL                 ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";

        // --------------------------------------------------
        // List Existing Admins
        // --------------------------------------------------
        listAdmins();

        // --------------------------------------------------
        // Display Menu Options
        // --------------------------------------------------
        cout << "\n   [ ACCOUNT ACTIONS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) Add New Admin\n";
        cout << "    2) Delete Admin\n";
        cout << "\n";
        cout << "    0) Back\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        
        cout << "   Choice ➜ ";
        choice = Utils::getValidRange(0, 2);

        // --------------------------------------------------
        // Process Selection
        // --------------------------------------------------
        if (choice == 1)
        {
            addAdmin();
        }

        if (choice == 2)
        {
            deleteAdmin();
        }

    } while (choice != 0);
}

// ============================================================================
// 6/8 addAdmin
// ============================================================================
void AdminModule::addAdmin()
{
    string u;
    string p;
    string r;

    // --------------------------------------------------
    // Input New Details
    // --------------------------------------------------
    cout << "   ┌────────────────────────────────────────────────────┐\n";
    cout << "   │ CREATE NEW ACCOUNT (ENTER 0 ONLY TO CANCEL)        │\n";
    cout << "   └────────────────────────────────────────────────────┘\n";
    cout << "   [ INFO: Enter 0 to Cancel ]\n";
    
    cout << "    Username : ";
    u = Utils::getValidString();
    
    // #### Cancel Check ####
    if (u == "0" || u.empty())
    {
        return;
    }

    // --------------------------------------------------
    // Duplicate Check
    // --------------------------------------------------
    string check = "SELECT id FROM admins WHERE username='" + u + "'";
    mysql_query(conn, check.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    
    // #### Existence Validation ####
    if (mysql_num_rows(res) > 0)
    {
        cout << "    \033[1;31m[ERROR] Username already taken.\033[0m\n";
        mysql_free_result(res);
        system("pause");
        return;
    }
    mysql_free_result(res);
    
    cout << "    Password : ";
    p = Utils::getValidString();

    cout << "    Role (1=Manager, 2=Staff): ";
    int rChoice = Utils::getValidRange(1, 2);
    
    // --------------------------------------------------
    // Role Assignment
    // --------------------------------------------------
    if (rChoice == 1)
    {
        r = "Manager";
    }
    else
    {
        r = "Staff";
    }

    // --------------------------------------------------
    // Insert Into Database
    // --------------------------------------------------
    string query = "INSERT INTO admins (username, password, role) VALUES ('" + u + "', '" + p + "', '" + r + "')";
    
    if (!mysql_query(conn, query.c_str()))
    {
        printSuccess("New User Added");
    }
    else
    {
        cout << "    \033[1;31m[ERROR] Failed.\033[0m\n";
    }

    system("pause");
}

// ============================================================================
// 7/8 listAdmins
// ============================================================================
void AdminModule::listAdmins()
{
    // --------------------------------------------------
    // Fetch Admins
    // --------------------------------------------------
    mysql_query(conn, "SELECT id, username, role FROM admins");
    
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\n";
    cout << "  ┌────┬────────────────────────────┬──────────────┐\n";
    cout << "  │ ID │ USERNAME                   │ ROLE         │\n";
    cout << "  ├────┼────────────────────────────┼──────────────┤\n";

    // --------------------------------------------------
    // Populate Table
    // --------------------------------------------------
    while ((row = mysql_fetch_row(res)))
    {
        cout << "  │ " 
             << right << setw(2)  << row[0] << " │ "
             << left  << setw(26) << row[1] << " │ "
             << left  << setw(12) << row[2] << " │\n";
    }

    cout << "  └────┴────────────────────────────┴──────────────┘\n";

    mysql_free_result(res);
}

// ============================================================================
// 8/8 deleteAdmin
// ============================================================================
void AdminModule::deleteAdmin()
{
    cout << "\n   [ INFO: Enter 0 to Cancel ]\n";
    cout << "   Enter ID to delete ➜ ";
    int id = Utils::getValidInt();

    // --------------------------------------------------
    // Verify Existence
    // --------------------------------------------------
    string check = "SELECT id FROM admins WHERE id=" + to_string(id);
    mysql_query(conn, check.c_str());
    
    MYSQL_RES* res = mysql_store_result(conn);

    // #### Not Found Check ####
    if (mysql_num_rows(res) == 0)
    {
        cout << "   \033[1;31m[ERROR] User ID not found.\033[0m\n";
        mysql_free_result(res);
        system("pause");
        return;
    }

    mysql_free_result(res);

    // --------------------------------------------------
    // Prevent Deleting Last Admin
    // --------------------------------------------------
    mysql_query(conn, "SELECT COUNT(*) FROM admins");
    MYSQL_RES* cRes = mysql_store_result(conn);
    MYSQL_ROW cRow = mysql_fetch_row(cRes);
    int totalAdmins = atoi(cRow[0]);
    mysql_free_result(cRes);

    // #### Min Admin Check ####
    if (totalAdmins <= 1)
    {
        cout << "   \033[1;31m[ERROR] Cannot delete the last administrator.\033[0m\n";
        system("pause");
        return;
    }

    // --------------------------------------------------
    // Execute Deletion
    // --------------------------------------------------
    cout << "   Are you sure? (1=Yes, 0=No) ➜ ";
    int confirm = Utils::getValidRange(0, 1);
    
    // #### Confirmation Check ####
    if (confirm != 1)
    {
        return;
    }

    string query = "DELETE FROM admins WHERE id=" + to_string(id);
    
    if (!mysql_query(conn, query.c_str()))
    {
        printSuccess("User Deleted");
    }
    else
    {
        cout << "   \033[1;31m[ERROR] Failed.\033[0m\n";
    }

    system("pause");
}