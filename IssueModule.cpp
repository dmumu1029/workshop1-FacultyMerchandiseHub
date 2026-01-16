// ============================================================================
// ISSUE MODULE IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "IssueModule.h"

// Standard Libraries
#include <iostream>    // Standard I/O (cout, cin)
#include <iomanip>     // Formatting (setw)
#include <string>      // String manipulation
#include <cstdlib>     // System calls
#include <limits>      // Numeric limits
#include "Utils.h"     // Shared Utility Functions

using namespace std;

// ============================================================================
// 2/11 printSuccess
// ============================================================================
static void printSuccess(string msg) 
{
    cout << "\n   ─────────────────────────────────────────────────────\n";
    cout << "    \033[1;32m✔ " << msg << "\033[0m\n"; 
    cout << "   ─────────────────────────────────────────────────────\n";
}

// ============================================================================
// 3/11 printError
// ============================================================================
static void printError(string msg) 
{
    cout << "   \033[1;31m[ERROR] " << msg << "\033[0m\n";
}

// ============================================================================
// 4/11 processReport (Static Helper)
// ============================================================================
static void processReport(MYSQL* conn, string smartID)
{
    // --------------------------------------------------
    // Validate Order ID
    // --------------------------------------------------
    string sql = "SELECT id, expected_date FROM orders WHERE smart_id='" + smartID + "'";
    
    if (mysql_query(conn, sql.c_str())) 
    {
        return;
    }
    
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row = mysql_fetch_row(res);

    // #### Not Found Check ####
    if (!row) 
    { 
        printError("Order Not Found.");
        mysql_free_result(res); 
        system("pause"); 
        return; 
    }
    
    string dbId = row[0]; 
    mysql_free_result(res);

    // --------------------------------------------------
    // Issue Type Selection
    // --------------------------------------------------
    cout << "\n   \033[1;33m[ SELECTED ORDER: " << smartID << " ]\033[0m\n";
    cout << "    1) Refund (Return Money)\n";
    cout << "    2) Redo (Production Error)\n";
    cout << "    0) Cancel\n";
    cout << "   Choice ➜ "; 
    
    int type = Utils::getValidRange(0, 2);

    if (type == 0) 
    {
        return;
    }

    string reason;
    cout << "   Enter Reason (e.g. Defect, Wrong Size) ➜ ";
    getline(cin, reason);

    // --------------------------------------------------
    // Process Resolution
    // --------------------------------------------------
    if (type == 1) 
    {
        // Refund Logic: Update order status -> Log issue
        mysql_query(conn, ("UPDATE orders SET status='Refunded' WHERE id=" + dbId).c_str());
        string q = "INSERT INTO issues (order_id, issue_type, resolution) VALUES (" + dbId + ", 'Complaint', 'Refund: " + reason + "')";
        mysql_query(conn, q.c_str());
        printSuccess("Refund Processed");
    }
    else if (type == 2) 
    {
        // Redo Logic: Update status & Extend Date -> Log issue
        mysql_query(conn, ("UPDATE orders SET status='Redo In Progress', expected_date = DATE_ADD(NOW(), INTERVAL 7 DAY) WHERE id=" + dbId).c_str());
        string q = "INSERT INTO issues (order_id, issue_type, resolution) VALUES (" + dbId + ", 'Defect', 'Redo: " + reason + "')";
        mysql_query(conn, q.c_str());
        
        // Calculate Changes
        mysql_query(conn, ("SELECT DATE(NOW()), DATE(DATE_ADD(NOW(), INTERVAL 3 DAY)), DATE(DATE_ADD(NOW(), INTERVAL 7 DAY))"));
        MYSQL_RES* timeRes = mysql_store_result(conn);
        MYSQL_ROW timeRow = mysql_fetch_row(timeRes);
        
        printSuccess("Redo Scheduled Successfully");
        cout << "\n   New Timeline\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        
        if (timeRow) 
        {
            cout << "   Redo Start     : " << timeRow[0] << "\n";
            cout << "   Production ETA : " << timeRow[1] << "\n";
            cout << "   Delivery ETA   : " << timeRow[2] << "\n";
        }
        
        cout << "   ──────────────────────────────────────────────────────\n";
        mysql_free_result(timeRes);
    }
    
    system("pause");
}

// ============================================================================
// 5/11 IssueModule (Constructor)
// ============================================================================
IssueModule::IssueModule(MYSQL* c) 
{ 
    conn = c; 
}

// ============================================================================
// 6/11 manageIssues
// ============================================================================
void IssueModule::manageIssues()
{
    int choice;
    do
    {
        system("cls");
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║               ORDER ISSUES & REFUNDS                 ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";
        
        // --------------------------------------------------
        // Display Current List
        // --------------------------------------------------
        viewIssues();

        cout << "\n   [ ACTIONS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) Report New Issue (Select Order)\n";
        cout << "    2) Update Issue Resolution\n";
        cout << "    3) Delete Issue Record\n";
        cout << "    4) Search Issues\n";
        cout << "\n";
        cout << "    0) Back\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        cout << "   Choice ➜ ";
        choice = Utils::getValidRange(0, 3);

        switch(choice)
        {
            case 1: 
            {
                // --------------------------------------------------
                // Order Selection Logic
                // --------------------------------------------------
                system("cls");
                mysql_query(conn, "SELECT smart_id, customer_name, status, total_price FROM orders");
                MYSQL_RES* res = mysql_store_result(conn); 
                MYSQL_ROW row;
                
                int totalRows = mysql_num_rows(res);

                cout << "\n   [ SELECT ORDER TO REPORT (ENTER 0 ONLY TO CANCEL) ]\n";
                cout << "  ┌─────┬──────────────────────┬──────────────────────┬──────────────────────┐\n";
                cout << "  │  No │ ID                   │ CUSTOMER             │ STATUS               │\n";
                cout << "  ├─────┼──────────────────────┼──────────────────────┼──────────────────────┤\n";
                
                int i = 1;
                while ((row = mysql_fetch_row(res))) 
                {
                    cout << "  │ " 
                         << left << setw(3) << i++ << " │ " 
                         << left << setw(20) << row[0] << " │ " 
                         << left << setw(20) << row[1] << " │ " 
                         << left << setw(20) << row[2] << " │\n";
                }
                cout << "  └─────┴──────────────────────┴──────────────────────┴──────────────────────┘\n";
                mysql_free_result(res); 
                
                if (totalRows == 0) 
                {
                    printError("No orders available."); 
                    system("pause");
                } 
                else 
                {
                    // Select Row to Report
                    cout << "\n   Select Row Number to Report Issue (0 to Cancel) ➜ ";
                    int sel = Utils::getValidRange(0, totalRows);
                    if (sel > 0 && sel <= totalRows) 
                    {
                        int offset = sel - 1;
                        string getIDQ = "SELECT smart_id FROM orders LIMIT " + to_string(offset) + ", 1";
                        mysql_query(conn, getIDQ.c_str());
                        MYSQL_RES* resID = mysql_store_result(conn);
                        MYSQL_ROW rowID = mysql_fetch_row(resID);
                        if (rowID) 
                        {
                            processReport(conn, rowID[0]);
                        }
                        mysql_free_result(resID);
                    }
                }
                break;
            }
            case 2: 
                updateIssue(); 
                break;
            case 3: 
                deleteIssue(); 
                break;
            case 4: 
                searchIssue(); 
                break;
            case 0: 
                break;
            default: 
                printError("Option not found."); 
                system("pause");
        }

    } while (choice != 0);
}

// ============================================================================
// 7/11 createIssue
// ============================================================================
void IssueModule::createIssue()
{
    string smartID; 
    cout << "\n   [ INFO: Enter 0 to Cancel ]\n";
    cout << "   Enter Order ID ➜ "; 
    smartID = Utils::getValidString();
    
    // #### Cancel Check ####
    if (smartID == "0") 
    {
        return;
    }
    
    // Delegate to helper
    processReport(conn, smartID);
}

// ============================================================================
// 8/11 viewIssues
// ============================================================================
void IssueModule::viewIssues()
{
    // --------------------------------------------------
    // Fetch Issues
    // --------------------------------------------------
    string sql = "SELECT issues.id, orders.smart_id, issues.issue_type, issues.resolution, issues.log_date FROM issues JOIN orders ON issues.order_id = orders.id";
    mysql_query(conn, sql.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row;

    cout << "\n   [ ISSUE HISTORY ]\n";
    cout << "  ┌─────┬──────────────────────┬─────────────────┬────────────────────────────────────────┐\n";
    cout << "  │ ID  │ ORDER ID             │ TYPE            │ DETAILS & RESOLUTION                   │\n";
    cout << "  ├─────┼──────────────────────┼─────────────────┼────────────────────────────────────────┤\n";
    
    int rows = 0;
    while ((row = mysql_fetch_row(res))) 
    {
        rows++;
        cout << "  │ " 
             << left << setw(3) << row[0] << " │ " 
             << left << setw(20) << row[1] << " │ " 
             << left << setw(15) << row[2] << " │ " 
             << left << setw(38) << row[3] << " │\n";
    }
    if (rows == 0)
    {
         cout << "  │ " << setw(86) << "No issues found." << " │\n";
    }
    cout << "  └─────┴──────────────────────┴─────────────────┴────────────────────────────────────────┘\n";
    mysql_free_result(res); 
}

// ============================================================================
// 9/11 updateIssue
// ============================================================================
void IssueModule::updateIssue()
{
    int id;
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ UPDATE ISSUE RESOLUTION (ENTER 0 ONLY TO CANCEL)   │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   Enter Issue ID (from History) ➜ "; 
    id = Utils::getValidInt();

    // --------------------------------------------------
    // Validate ID
    // --------------------------------------------------
    string check = "SELECT id FROM issues WHERE id=" + to_string(id);
    mysql_query(conn, check.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    
    // #### Not Found Check ####
    if (mysql_num_rows(res) == 0) 
    {
        printError("Issue ID not found.");
        mysql_free_result(res); 
        system("pause"); 
        return;
    }
    mysql_free_result(res);

    string newRes;
    cout << "   Enter New Resolution (e.g. Solved, Closed) ➜ "; 
    getline(cin, newRes);

    // --------------------------------------------------
    // Perform Update
    // --------------------------------------------------
    string query = "UPDATE issues SET resolution='" + newRes + "' WHERE id=" + to_string(id);
    
    if (!mysql_query(conn, query.c_str())) 
    {
        printSuccess("Issue Updated");
    }
    else 
    {
        printError("Failed to update.");
    }
    system("pause");
}

// ============================================================================
// 10/11 deleteIssue
// ============================================================================
void IssueModule::deleteIssue()
{
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ DELETE ISSUE RECORD (ENTER 0 ONLY TO CANCEL)       │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   Enter Issue ID ➜ "; 
    int id = Utils::getValidInt();

    // --------------------------------------------------
    // Validate ID
    // --------------------------------------------------
    string check = "SELECT id FROM issues WHERE id=" + to_string(id);
    mysql_query(conn, check.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    
    // #### Not Found Check ####
    if (mysql_num_rows(res) == 0) 
    {
        printError("Issue ID not found.");
        mysql_free_result(res); 
        system("pause"); 
        return;
    }
    mysql_free_result(res);

    cout << "   Are you sure? (1=Yes, 0=No) ➜ ";
    int confirm = Utils::getValidRange(0, 1);
    if (confirm != 1) 
    {
        return;
    }

    // --------------------------------------------------
    // Perform Deletion
    // --------------------------------------------------
    string query = "DELETE FROM issues WHERE id=" + to_string(id);
    
    if (!mysql_query(conn, query.c_str())) 
    {
        printSuccess("Record Deleted");
    }
    else 
    {
        printError("Failed to delete.");
    }
    system("pause");
}

// ============================================================================
// 11/11 searchIssue
// ============================================================================
void IssueModule::searchIssue()
{
    string keyword;
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ SEARCH ISSUES                                      │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   Enter Keyword ➜ "; 
    keyword = Utils::getValidString();

    // --------------------------------------------------
    // Search Query
    // --------------------------------------------------
    string query = "SELECT issues.id, orders.smart_id, issues.issue_type, issues.resolution "
                   "FROM issues JOIN orders ON issues.order_id = orders.id "
                   "WHERE issues.issue_type LIKE '%" + keyword + "%' OR issues.resolution LIKE '%" + keyword + "%'";
    
    if (mysql_query(conn, query.c_str())) 
    {
        return;
    }
    
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\n   [ SEARCH RESULTS ]\n";
    cout << "  ┌─────┬──────────────────────┬─────────────────┬────────────────────────────────────────┐\n";
    cout << "  │ ID  │ ORDER ID             │ TYPE            │ DETAILS & RESOLUTION                   │\n";
    cout << "  ├─────┼──────────────────────┼─────────────────┼────────────────────────────────────────┤\n";
    
    bool found = false;
    while ((row = mysql_fetch_row(res))) 
    {
        found = true;
        cout << "  │ " 
             << left << setw(3) << row[0] << " │ " 
             << left << setw(20) << row[1] << " │ " 
             << left << setw(15) << row[2] << " │ " 
             << left << setw(38) << row[3] << " │\n";
    }
    cout << "  └─────┴──────────────────────┴─────────────────┴────────────────────────────────────────┘\n";
    
    // #### Result Check ####
    if (!found) 
    {
        cout << "   \033[1;31mNo issues found matching '" << keyword << "'.\033[0m\n";
    }
    
    mysql_free_result(res); 
    system("pause");
}