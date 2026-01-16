// ============================================================================
// MAIN APPLICATION ENTRY POINT
// ============================================================================
// Internal Libraries
#include <iostream>        // Standard I/O operations (cout, cin)
#include <cstdlib>         // Standard library for system calls (system("cls"))
#include <iomanip>         // Input/Output manipulation (setw, setprecision)
#include <limits>          // Numeric limits (used in input validation)
#include <windows.h>       // Windows API for console CP setting

// Module Headers
#include "DatabaseConnection.h" // Manages MySQL database connection
#include "AdminModule.h"        // Handles admin authentication and management
#include "InventoryModule.h"    // Manages product inventory operations
#include "OrderModule.h"        // Handles new orders and tracking
#include "IssueModule.h"        // Manages customer issues and refunds
#include "ReportModule.h"       // Generates sales and financial reports
#include "Utils.h"              // Shared Utility Functions

using namespace std;

// ============================================================================
// 2/5 printSuccess
// ============================================================================
static void printSuccess(string msg)
{
    cout << "\n   ─────────────────────────────────────────────────────\n";
    cout << "    \033[1;32m✔ " << msg << "\033[0m\n";
    cout << "   ─────────────────────────────────────────────────────\n";
}

// ============================================================================
// 3/5 printError
// ============================================================================
static void printError(string msg)
{
    cout << "   \033[1;31m[ERROR] " << msg << "\033[0m\n";
}

// ============================================================================
// 4/5 showWelcomeScreen
// ============================================================================
bool showWelcomeScreen()
{
    // --------------------------------------------------
    // Display Welcome Banner
    // --------------------------------------------------
    system("cls");
    cout << "\n\n\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                      ║\n";
    cout << "  ║         FAIX FACULTY MERCHANDISE HUB SYSTEM          ║\n";
    cout << "  ║                                                      ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                      ║\n";
    cout << "  ║     Welcome to the Faculty Souvenir Management       ║\n";
    cout << "  ║              Digital System v3.0                     ║\n";
    cout << "  ║                                                      ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    
    // --------------------------------------------------
    // Display Entry Options
    // --------------------------------------------------
    cout << "\n";
    cout << "   1) ENTER SYSTEM\n";
    cout << "   0) EXIT APPLICATION\n";
    cout << "\n";
    cout << "  ────────────────────────────────────────────────────────\n";
    cout << "   Enter Choice ➜ ";
    
    int choice = Utils::getValidRange(0, 1);
    return (choice == 1);
}

// ============================================================================
// 5/5 Main Loop
// ============================================================================
int main()
{
    // --------------------------------------------------
    // System Initialization
    // --------------------------------------------------
    SetConsoleOutputCP(65001); 

    if (!showWelcomeScreen()) 
    {
        cout << "\n  Goodbye!\n";
        return 0;
    }

    // --------------------------------------------------
    // Database Initialization
    // --------------------------------------------------
    DatabaseConnection db;
    // #### Database Error Check ####
    MYSQL* conn = db.connect();
    
    if (!conn) 
    { 
        return 1; 
    }

    AdminModule admin(conn);

    // --------------------------------------------------
    // Main Application Loop
    // --------------------------------------------------
    while (true)
    {
        system("cls"); 
        
        // --------------------------------------------------
        // Authentication Phase
        // --------------------------------------------------
        string userRole = admin.login(); 

        // #### Exit Check ####
        if (userRole == "EXIT") 
        {
            cout << "  System Shutdown. Goodbye.\n"; 
            break; 
        }

        // #### Login Validation ####
        if (userRole == "") 
        {
            printError("Invalid Credentials. Try again.");
            system("pause"); 
            continue; 
        }

        int choice;
        do
        {
            // --------------------------------------------------
            // Main Dashboard Menu
            // --------------------------------------------------
            system("cls");
            cout << "\n";
            cout << "  ╔══════════════════════════════════════════════════════╗\n";
            cout << "  ║                 FAIX MERCHANDISE HUB                 ║\n";
            cout << "  ╚══════════════════════════════════════════════════════╝\n";
            cout << "\n";
            cout << "   User Role : " << userRole << "\n";
            cout << "   Status    : \033[1;32mLogged In\033[0m\n"; 
            cout << "  ────────────────────────────────────────────────────────\n";
            cout << "\n   [ GENERAL OPERATIONS ]\n";
            cout << "   ──────────────────────────────────────────────────────\n";
            cout << "    1) Inventory Management\n";
            cout << "    2) Place New Order\n";
            cout << "    3) Track / Update Order\n";
            cout << "    4) Order Issues & Refunds\n";
            cout << "\n   [ ADMINISTRATION ]\n";
            cout << "   ──────────────────────────────────────────────────────\n";
            
            // #### Permission Check ####
            if (userRole == "Manager")
            {
                cout << "    5) Financial Reports\n";
                cout << "    6) Manage Admin Accounts\n";
            }
            else
            {
                cout << "    5) \033[1;30mFinancial Reports (LOCKED)\033[0m\n"; 
                cout << "    6) \033[1;30mManage Admin Accounts (LOCKED)\033[0m\n";
            }

            cout << "\n";
            cout << "    0) Logout\n";
            cout << "  ────────────────────────────────────────────────────────\n";
            cout << "   Choice ➜ ";
            
            choice = Utils::getValidRange(0, 6);

            // --------------------------------------------------
            // Menu Logic
            // --------------------------------------------------
            switch(choice)
            {
                case 1: 
                { 
                    InventoryModule inv(conn); 
                    inv.manageInventory(); 
                    break; 
                }
                case 2: 
                { 
                    OrderModule ord(conn); 
                    ord.placeOrder(); 
                    break; 
                }
                case 3: 
                { 
                    OrderModule ord(conn); 
                    ord.trackOrder(); 
                    break; 
                }
                case 4: 
                { 
                    IssueModule iss(conn); 
                    iss.manageIssues(); 
                    break; 
                }
                case 5: 
                { 
                    // #### Role Authorization Check ####
                    if (userRole == "Manager") 
                    { 
                        ReportModule rep(conn); 
                        rep.generateReport(); 
                    }
                    else 
                    { 
                        printError("Staff cannot view reports!"); 
                        system("pause"); 
                    }
                    break; 
                }
                case 6: 
                { 
                    // #### Role Authorization Check ####
                    if (userRole == "Manager") 
                    { 
                        admin.manageAdmins(); 
                    }
                    else 
                    { 
                        printError("Staff cannot manage accounts!"); 
                        system("pause"); 
                    }
                    break; 
                }
                case 0: 
                {
                    cout << "  Logging out...\n"; 
                    break;
                }
                default: 
                {
                    printError("Invalid Choice."); 
                    system("pause"); 
                }
            }

        } while (choice != 0); 
    } 

    return 0;
}