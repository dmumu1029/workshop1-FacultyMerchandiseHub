// ============================================================================
// REPORT MODULE IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "ReportModule.h"

// Standard Libraries
#include <iostream>    // Standard I/O (cout, cin)
#include <iomanip>     // Formatting (setw)
#include <string>      // String manipulation
#include <cstdlib>     // System calls
#include <limits>      // Numeric limits
#include <ctime>       // Time functions
#include "Utils.h"     // Shared Utility Functions
#include <fstream>     // File I/O for CSV Export

using namespace std;

// ============================================================================
// 2/23 printSuccess
// ============================================================================
static void printSuccess(string msg) 
{
    cout << "\n   ─────────────────────────────────────────────────────\n";
    cout << "    \033[1;32m✔ " << msg << "\033[0m\n"; 
    cout << "   ─────────────────────────────────────────────────────\n";
}

// ============================================================================
// 3/23 printError
// ============================================================================
static void printError(string msg) 
{
    cout << "   \033[1;31m[ERROR] " << msg << "\033[0m\n";
}

// ============================================================================
// Helper: saveQueryToCSV
// ============================================================================
static void saveQueryToCSV(MYSQL* conn, string query, string filename) {
    ofstream myFile;
    myFile.open(filename + ".csv");

    if (mysql_query(conn, query.c_str())) {
        cout << " [ERROR] Export failed: " << mysql_error(conn) << endl;
        return;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;
    int num_fields = mysql_num_fields(res);

    MYSQL_FIELD* field;
    while((field = mysql_fetch_field(res))) {
        myFile << field->name << ",";
    }
    myFile << "\n";

    while ((row = mysql_fetch_row(res))) {
        for(int i = 0; i < num_fields; i++) {
            if (row[i]) 
                myFile << "\"" << row[i] << "\","; 
            else 
                myFile << "\"\",";
        }
        myFile << "\n";
    }

    myFile.close();
    cout << "\n    \033[1;32m[SUCCESS] Data exported to " << filename << ".csv\033[0m\n";
    system("pause");
}

// ============================================================================
// 4/23 ReportModule (Constructor)
// ============================================================================
ReportModule::ReportModule(MYSQL* c) 
{ 
    conn = c; 
}

// ============================================================================
// 5/23 generateReport (Main Menu)
// ============================================================================
void ReportModule::generateReport()
{
    int choice;
    do
    {
        system("cls");
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║            EXECUTIVE REPORTING SUITE                 ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";
        
        cout << "\n   [ VISUAL INSIGHTS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) Sales Trend Analytics (Graphs)\n";
        cout << "    2) High & Low Value Orders\n";

        cout << "\n   [ DATA LEDGERS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    3) Export Full Data (.csv)\n";
        cout << "    4) View Detailed Product Reports (Tables)\n";
        
        cout << "\n";
        cout << "    0) Back to Main Menu\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        cout << "   Choice ➜ ";
    choice = Utils::getValidRange(0, 4);

        // --------------------------------------------------
        // Navigation Logic
        // --------------------------------------------------
        if (choice == 1) menuSalesTrends();
        else if (choice == 2) menuOrderAnalysis();
        else if (choice == 3) exportToCSV();
        else if (choice == 4) viewProductReports();

    } while (choice != 0);
}

// ============================================================================
// 6/23 menuSalesTrends
// ============================================================================
void ReportModule::menuSalesTrends()
{
    int choice; 
    system("cls");
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                ANALYTICS FILTER                      ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "   1) Yearly Trend\n";
    cout << "   2) Monthly Trend\n";
    cout << "   3) Daily Trend\n";
    cout << "   0) Cancel\n";
    cout << "  ──────────────────────────────────────────────────────\n";
    cout << "   Choice ➜ "; 
    
    choice = Utils::getValidRange(0, 3);

    if (choice == 1) showTrend("YEAR"); 
    if (choice == 2) showTrend("MONTH"); 
    if (choice == 3) showTrend("DAY");
}

// ============================================================================
// 7/23 menuOrderAnalysis
// ============================================================================
void ReportModule::menuOrderAnalysis()
{
    int choice; 
    system("cls");
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                  VALUE ANALYSIS                      ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "   1) Top 5 Highest Value Orders\n";
    cout << "   2) Top 5 Lowest Value Orders\n";
    cout << "   0) Cancel\n";
    cout << "  ──────────────────────────────────────────────────────\n";
    cout << "   Choice ➜ "; 
    
    choice = Utils::getValidRange(0, 2);

    if (choice == 1) showHighLowOrders(true); 
    if (choice == 2) showHighLowOrders(false);
}

// ============================================================================
// 8/23 viewProductReports
// ============================================================================
void ReportModule::viewProductReports()
{
    int choice;
    do {
        system("cls");
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║              DETAILED PRODUCT REPORTS                ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";
        
        cout << "\n   [ PERIODIC REPORTS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) Daily Report  (Select Month/Year)\n";
        cout << "    2) Weekly Report (Select Month/Year)\n";
        cout << "    3) Monthly Report (Select Year)\n";
        cout << "    4) Yearly Report (All Years)\n";

        cout << "\n   [ MASTER HISTORY ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    5) View All (Every Day History)\n"; 
        
        cout << "\n";
        cout << "    0) Back\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        cout << "   Choice ➜ ";
        choice = Utils::getValidRange(0, 5);

        if (choice == 1) reportDaily();
        if (choice == 2) reportWeekly();
        if (choice == 3) reportMonthly();
        if (choice == 4) reportYearly();
        if (choice == 5) reportViewAll();

    } while (choice != 0);
}

// ============================================================================
// 9/23 getItemStats
// ============================================================================
string ReportModule::getItemStats(string dateCondition, bool best)
{
    string sortOrder = best ? "DESC" : "ASC";
    // --------------------------------------------------
    // Aggregation Query
    // --------------------------------------------------
    string q = "SELECT p.name, SUM(o.quantity) as total_qty "
               "FROM orders o JOIN products p ON o.product_id = p.id "
               "WHERE " + dateCondition + " AND o.status = 'Completed' "
               "GROUP BY p.name ORDER BY total_qty " + sortOrder + " LIMIT 1";
    
    if (mysql_query(conn, q.c_str())) return "-";
    
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    string result = "-";
    
    if (row) result = string(row[0]) + " (" + string(row[1]) + ")";
    
    mysql_free_result(res);
    return result;
}

// ============================================================================
// 10/23 printReportRow (Table Formatter Helper)
// ============================================================================
void printReportRow(string c1, double sales, string c3, string c4)
{
    cout << "  │ " << left << setw(10) << c1 << " "
         << "│ RM " << right << setw(9) << sales << " "
         << "│ " << left << setw(24) << c3 << " "
         << "│ " << left << setw(24) << c4 << " │\n";
}

// ============================================================================
// 11/23 reportDaily
// ============================================================================
void ReportModule::reportDaily()
{
    int m, y;
    cout << "\n   [ INFO: Enter 0 to Cancel ] (ENTER 0 ONLY TO CANCEL)\n";
    cout << "   Enter Year (e.g. 2025) ➜ "; y = Utils::getValidInt(); if (y == 0) return;
    cout << "   Enter Month (1-12)     ➜ "; m = Utils::getValidRange(1, 12); if (m == 0) return;

    cout << "\n   \033[1;33m[ REPORT: DAILY SALES FOR " << m << "/" << y << " ]\033[0m\n";

    // --------------------------------------------------
    // Query Daily Sales
    // --------------------------------------------------
    string dateFilter = "MONTH(order_date) = " + to_string(m) + " AND YEAR(order_date) = " + to_string(y);
    string q = "SELECT DATE(order_date) as d, SUM(total_price) FROM orders "
               "WHERE " + dateFilter + " AND status = 'Completed' GROUP BY d ORDER BY d ASC";
    
    if (mysql_query(conn, q.c_str())) { printError(mysql_error(conn)); return; }
    MYSQL_RES* res = mysql_store_result(conn);
    
    // #### No Data Check ####
    if (mysql_num_rows(res) == 0) { 
        printError("No data found for this period."); mysql_free_result(res); system("pause"); return; 
    }

    MYSQL_ROW row;
    cout << "\n";
    cout << "  ┌────────────┬──────────────┬──────────────────────────┬──────────────────────────┐\n";
    cout << "  │ DATE       │ SALES (RM)   │ BEST ITEM (Qty)          │ WORST ITEM (Qty)         │\n";
    cout << "  ├────────────┼──────────────┼──────────────────────────┼──────────────────────────┤\n";

    while ((row = mysql_fetch_row(res)))
    {
        string dateStr = row[0]; 
        double sales = stod(row[1]);
        string dayCond = "DATE(order_date) = '" + dateStr + "'";
        
        // Fetch Item Stats for Day
        string best = getItemStats(dayCond, true);
        string worst = getItemStats(dayCond, false);
        if (best == worst) worst = "-";

        printReportRow(dateStr, sales, best, worst);
    }
    cout << "  └────────────┴──────────────┴──────────────────────────┴──────────────────────────┘\n";
    mysql_free_result(res);
    
    cout << "\n   [ OPTIONS ]\n";
    cout << "    1) Export this report to Excel (.csv)\n";
    cout << "    0) Return to Menu\n";
    cout << "   Select ➜ ";
    
    int choice = Utils::getValidRange(0, 1);

    if (choice == 1) {
        saveQueryToCSV(conn, q, "Daily_Sales_Report_" + to_string(y) + "_" + to_string(m)); 
    }
}

// ============================================================================
// 12/23 reportWeekly
// ============================================================================
void ReportModule::reportWeekly()
{
    int m, y;
    cout << "\n   [ INFO: Enter 0 to Cancel ]\n";
    cout << "   Enter Year (e.g. 2025) ➜ "; y = Utils::getValidInt(); if (y == 0) return;
    cout << "   Enter Month (1-12)     ➜ "; m = Utils::getValidRange(1, 12); if (m == 0) return;

    cout << "\n   \033[1;33m[ REPORT: WEEKLY SALES FOR " << m << "/" << y << " ]\033[0m\n";

    // --------------------------------------------------
    // Calculate Weeks
    // --------------------------------------------------
    string dateFilter = "MONTH(order_date) = " + to_string(m) + " AND YEAR(order_date) = " + to_string(y);
    string q = "SELECT FLOOR((DAY(order_date)-1)/7)+1 as week_num, SUM(total_price) FROM orders "
               "WHERE " + dateFilter + " AND status = 'Completed' GROUP BY week_num ORDER BY week_num ASC";

    if (mysql_query(conn, q.c_str())) { printError(mysql_error(conn)); return; }
    MYSQL_RES* res = mysql_store_result(conn);
    
    // #### No Data Check ####
    if (mysql_num_rows(res) == 0) { 
        printError("No data found."); mysql_free_result(res); system("pause"); return; 
    }

    MYSQL_ROW row;
    cout << "\n";
    cout << "  ┌────────────┬──────────────┬──────────────────────────┬──────────────────────────┐\n";
    cout << "  │ PERIOD     │ SALES (RM)   │ BEST ITEM (Qty)          │ WORST ITEM (Qty)         │\n";
    cout << "  ├────────────┼──────────────┼──────────────────────────┼──────────────────────────┤\n";

    while ((row = mysql_fetch_row(res)))
    {
        string weekNum = row[0];
        double sales = stod(row[1]);
        string weekCond = "FLOOR((DAY(order_date)-1)/7)+1 = " + weekNum + " AND " + dateFilter;
        
        // Fetch Item Stats for Week
        string best = getItemStats(weekCond, true);
        string worst = getItemStats(weekCond, false);
        if (best == worst) worst = "-";

        printReportRow("Week " + weekNum, sales, best, worst);
    }
    cout << "  └────────────┴──────────────┴──────────────────────────┴──────────────────────────┘\n";
    mysql_free_result(res);
    
    cout << "\n   [ OPTIONS ]\n";
    cout << "    1) Export this report to Excel (.csv)\n";
    cout << "    0) Return to Menu\n";
    cout << "   Select ➜ ";
    
    int choice = Utils::getValidRange(0, 1);

    if (choice == 1) {
        saveQueryToCSV(conn, q, "Weekly_Sales_Report_" + to_string(y) + "_" + to_string(m)); 
    }
}

// ============================================================================
// 13/23 reportMonthly
// ============================================================================
void ReportModule::reportMonthly()
{
    int y;
    cout << "\n   [ INFO: Enter 0 to Cancel ]\n";
    cout << "   Enter Year (e.g. 2025) ➜ "; y = Utils::getValidInt(); if (y == 0) return;

    cout << "\n   \033[1;33m[ REPORT: MONTHLY SALES FOR " << y << " ]\033[0m\n";
    
    string q = "SELECT MONTH(order_date) as m, SUM(total_price) FROM orders "
               "WHERE YEAR(order_date) = " + to_string(y) + " AND status = 'Completed' GROUP BY m ORDER BY m ASC";

    mysql_query(conn, q.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    
    cout << "\n";
    cout << "  ┌────────────┬──────────────┬──────────────────────────┬──────────────────────────┐\n";
    cout << "  │ MONTH      │ SALES (RM)   │ BEST ITEM (Qty)          │ WORST ITEM (Qty)         │\n";
    cout << "  ├────────────┼──────────────┼──────────────────────────┼──────────────────────────┤\n";

    if (mysql_num_rows(res) == 0) { 
        cout << "  │ " << left << setw(86) << "No Sales Recorded for this Year" << " │\n"; 
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        string monthNum = row[0];
        double sales = stod(row[1]);
        string monthNames[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        string mName = monthNames[stoi(monthNum)];

        string monthCond = "MONTH(order_date) = " + monthNum + " AND YEAR(order_date) = " + to_string(y);
        
        // Fetch Item Stats for Month
        string best = getItemStats(monthCond, true);
        string worst = getItemStats(monthCond, false);
        if (best == worst) worst = "-";

        printReportRow(mName, sales, best, worst);
    }
    cout << "  └────────────┴──────────────┴──────────────────────────┴──────────────────────────┘\n";
    mysql_free_result(res);
    
    cout << "\n   [ OPTIONS ]\n";
    cout << "    1) Export this report to Excel (.csv)\n";
    cout << "    0) Return to Menu\n";
    cout << "   Select ➜ ";
    
    int choice = Utils::getValidRange(0, 1);

    if (choice == 1) {
        saveQueryToCSV(conn, q, "Monthly_Sales_Report_" + to_string(y)); 
    }
}

// ============================================================================
// 14/23 reportYearly
// ============================================================================
void ReportModule::reportYearly()
{
    cout << "\n   \033[1;33m[ REPORT: YEARLY SUMMARY ]\033[0m\n";

    string q = "SELECT YEAR(order_date) as y, SUM(total_price) FROM orders WHERE status = 'Completed' GROUP BY y ORDER BY y DESC";
    mysql_query(conn, q.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\n";
    cout << "  ┌────────────┬──────────────┬──────────────────────────┬──────────────────────────┐\n";
    cout << "  │ YEAR       │ SALES (RM)   │ BEST ITEM (Qty)          │ WORST ITEM (Qty)         │\n";
    cout << "  ├────────────┼──────────────┼──────────────────────────┼──────────────────────────┤\n";

    while ((row = mysql_fetch_row(res)))
    {
        string yearStr = row[0];
        double sales = stod(row[1]);
        string yearCond = "YEAR(order_date) = " + yearStr;
        
        string best = getItemStats(yearCond, true);
        string worst = getItemStats(yearCond, false);
        if (best == worst) worst = "-";

        printReportRow(yearStr, sales, best, worst);
    }
    cout << "  └────────────┴──────────────┴──────────────────────────┴──────────────────────────┘\n";
    mysql_free_result(res);
    
    cout << "\n   [ OPTIONS ]\n";
    cout << "    1) Export this report to Excel (.csv)\n";
    cout << "    0) Return to Menu\n";
    cout << "   Select ➜ ";
    
    int choice = Utils::getValidRange(0, 1);

    if (choice == 1) {
        saveQueryToCSV(conn, q, "Yearly_Sales_Report"); 
    }
}

// ============================================================================
// 15/23 reportViewAll
// ============================================================================
void ReportModule::reportViewAll()
{
    cout << "\n   \033[1;33m[ REPORT: ALL-TIME DAILY LEDGER ]\033[0m\n";

    string q = "SELECT DATE(order_date) as d, SUM(total_price) FROM orders "
               "WHERE status = 'Completed' GROUP BY d ORDER BY d DESC";
    
    if (mysql_query(conn, q.c_str())) { printError(mysql_error(conn)); return; }
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if (mysql_num_rows(res) == 0) { 
        printError("No data found."); mysql_free_result(res); system("pause"); return; 
    }

    cout << "\n";
    cout << "  ┌────────────┬──────────────┬──────────────────────────┬──────────────────────────┐\n";
    cout << "  │ DATE       │ SALES (RM)   │ BEST ITEM (Qty)          │ WORST ITEM (Qty)         │\n";
    cout << "  ├────────────┼──────────────┼──────────────────────────┼──────────────────────────┤\n";

    while ((row = mysql_fetch_row(res)))
    {
        string dateStr = row[0];
        double sales = stod(row[1]);
        string dateFilter = "DATE(order_date) = '" + dateStr + "'";
        
        string best = getItemStats(dateFilter, true);
        string worst = getItemStats(dateFilter, false);
        if (best == worst) worst = "-";

        printReportRow(dateStr, sales, best, worst);
    }
    cout << "  └────────────┴──────────────┴──────────────────────────┴──────────────────────────┘\n";
    mysql_free_result(res);
    system("pause");
}

// ============================================================================
// 16/23 showTrend
// ============================================================================
void ReportModule::showTrend(string type)
{
    system("cls");
    string dateFormat, limit, header;
    if (type == "YEAR")       { header = "YEARLY PERFORMANCE (Trend)"; dateFormat = "%Y"; limit = "5"; } 
    else if (type == "MONTH") { header = "MONTHLY PERFORMANCE (Trend)"; dateFormat = "%Y-%m"; limit = "12"; } 
    else                      { header = "DAILY PERFORMANCE (Trend)"; dateFormat = "%Y-%m-%d"; limit = "30"; }

    cout << "\n  ==================================================================================================\n";
    cout << "    " << header << "\n";
    cout << "  ==================================================================================================\n";

    string query = "SELECT DATE_FORMAT(order_date, '" + dateFormat + "') as Period, SUM(total_price) as Rev FROM orders WHERE status = 'Completed' GROUP BY Period ORDER BY Period DESC LIMIT " + limit;
    mysql_query(conn, query.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row;

    // --------------------------------------------------
    // Determine Scale Max Result
    // --------------------------------------------------
    double maxRev = 1.0; 
    while ((row = mysql_fetch_row(res))) {
        double r = stod(row[1]); 
        if (r > maxRev) maxRev = r; 
    }
    mysql_data_seek(res, 0); 

    // Header Graph Width = 29 dashes.
    cout << "  ┌────────────┬─────────────────────────────┬──────────────┬──────────────────────┬──────────────────────┐\n";
    cout << "  │ PERIOD     │ REVENUE GRAPH               │ REVENUE      │ BEST ITEM            │ WORST ITEM           │\n";
    cout << "  ├────────────┼─────────────────────────────┼──────────────┼──────────────────────┼──────────────────────┤\n";

    while ((row = mysql_fetch_row(res)))
    {
        string period = row[0];
        double rev = stod(row[1]);

        string condition = "DATE_FORMAT(o.order_date, '" + dateFormat + "') = '" + period + "'";
        string bestItem = getTopProduct(condition); 
        string worstItem = getLowProduct(condition);
        if (bestItem == worstItem) worstItem = "-";
        
        // Truncate strings to prevent table misalignment
        if (bestItem.length() > 20) bestItem = bestItem.substr(0, 17) + "...";
        if (worstItem.length() > 20) worstItem = worstItem.substr(0, 17) + "...";

        cout << "  │ " << left << setw(10) << period << " ";
        
        // GRAPH COLUMN (Width 29 in Header)
        // We print "  " + blockbar + "  " to align. Blockbar is variable length but max 25 chars.
        cout << "│  "; 
        printBlockGraph(rev, maxRev); // Prints appropriate number of blocks
        cout << "  ";

        cout << "│ RM " << right << setw(9) << rev << " "
             << "│ " << left << setw(20) << bestItem << " "
             << "│ " << left << setw(20) << worstItem << " │\n";
    }
    cout << "  └────────────┴─────────────────────────────┴──────────────┴──────────────────────┴──────────────────────┘\n";
    
    // --------------------------------------------------
    // Global Stats Footer
    // --------------------------------------------------
    string globalBest = getGlobalBestItem(); 
    string globalWorst = getGlobalWorstItem();
    if (globalBest == globalWorst) globalWorst = "-";
    
    cout << "   [INSIGHT] Best Selling: " << globalBest << "\n";
    cout << "   [INSIGHT] Low Selling : " << globalWorst << "\n"; 
    cout << "  ==================================================================================================\n";
    system("pause");
}

// ============================================================================
// 17/23 showHighLowOrders
// ============================================================================
void ReportModule::showHighLowOrders(bool high)
{
    system("cls");
    string order = high ? "DESC" : "ASC"; 
    string title = high ? "TOP 5 HIGH VALUE ORDERS" : "TOP 5 LOW VALUE ORDERS";
    
    cout << "\n  =======================================================\n"; 
    cout << "    " << title << "\n"; 
    cout << "  =======================================================\n";
    
    // --------------------------------------------------
    // Query Top/Bottom 5
    // --------------------------------------------------
    string query = "SELECT smart_id, customer_name, total_price, status, order_date FROM orders WHERE status = 'Completed' ORDER BY total_price " + order + " LIMIT 5";
    mysql_query(conn, query.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row;

    cout << "  ┌────┬────────────────────┬─────────────────┬──────────────┬─────────────────────┐\n";
    cout << "  │  # │ ORDER ID           │ CUSTOMER        │ VALUE        │ DATE                │\n";
    cout << "  ├────┼────────────────────┼─────────────────┼──────────────┼─────────────────────┤\n";
    
    int i = 1;
    while ((row = mysql_fetch_row(res))) {
        cout << "  │ " << right << setw(2) << i++ << " "
             << "│ " << left << setw(18) << row[0] << " "
             << "│ " << left << setw(15) << row[1] << " "
             << "│ RM " << right << setw(9) << row[2] << " "
             << "│ " << left << setw(19) << row[4] << " │\n";
    }
    cout << "  └────┴────────────────────┴─────────────────┴──────────────┴─────────────────────┘\n";
    mysql_free_result(res); 
    system("pause");
}

// ============================================================================
// 18/23 printBlockGraph
// ============================================================================
void ReportModule::printBlockGraph(double value, double maxVal) 
{
    // Ensure graph never exceeds 25 chars of visible blocks
    int bars = (int)((value / maxVal) * 25); 
    if (bars == 0 && value > 0) bars = 1; 
    if (bars > 25) bars = 25; // Safety cap

    // Render Blocks
    for (int i = 0; i < bars; i++) cout << "\xE2\x96\xA0"; 
    // Fill remaining space with whitespace to maintain alignment
    for (int i = bars; i < 25; i++) cout << " ";
}

// ============================================================================
// 19/23 getTopProduct
// ============================================================================
string ReportModule::getTopProduct(string dateCondition) {
    string q = "SELECT p.name FROM orders o JOIN products p ON o.product_id = p.id WHERE " + dateCondition + " AND o.status = 'Completed' GROUP BY p.name ORDER BY SUM(o.quantity) DESC LIMIT 1";
    mysql_query(conn, q.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row = mysql_fetch_row(res);
    string name = "-"; 
    if (row) name = row[0]; 
    mysql_free_result(res); 
    return name;
}

// ============================================================================
// 20/23 getLowProduct
// ============================================================================
string ReportModule::getLowProduct(string dateCondition) {
    string q = "SELECT p.name FROM orders o JOIN products p ON o.product_id = p.id WHERE " + dateCondition + " AND o.status = 'Completed' GROUP BY p.name ORDER BY SUM(o.quantity) ASC LIMIT 1";
    mysql_query(conn, q.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row = mysql_fetch_row(res);
    string name = "-"; 
    if (row) name = row[0]; 
    mysql_free_result(res); 
    return name;
}

// ============================================================================
// 21/23 getGlobalBestItem
// ============================================================================
string ReportModule::getGlobalBestItem() {
    string q = "SELECT p.name FROM orders o JOIN products p ON o.product_id = p.id WHERE o.status = 'Completed' GROUP BY p.name ORDER BY SUM(o.quantity) DESC LIMIT 1";
    mysql_query(conn, q.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row = mysql_fetch_row(res);
    string name = "-"; 
    if(row) name = row[0]; 
    mysql_free_result(res); 
    return name;
}

// ============================================================================
// 22/23 getGlobalWorstItem
// ============================================================================
string ReportModule::getGlobalWorstItem() {
    string q = "SELECT p.name FROM orders o JOIN products p ON o.product_id = p.id WHERE o.status = 'Completed' GROUP BY p.name ORDER BY SUM(o.quantity) ASC LIMIT 1";
    mysql_query(conn, q.c_str()); 
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row = mysql_fetch_row(res);
    string name = "-"; 
    if(row) name = row[0]; 
    mysql_free_result(res); 
    return name;
}

// ============================================================================
// 23/23 exportToCSV
// ============================================================================
void ReportModule::exportToCSV() {
    cout << "\n   ┌────────────────────────────────────────────────────┐\n";
    cout << "   │  GENERATING CSV FILE...                            │\n";
    cout << "   └────────────────────────────────────────────────────┘\n";
    
    ofstream myFile; 
    myFile.open("FAIX_Sales_Report.csv"); 
    
    // --------------------------------------------------
    // Write CSV Header
    // --------------------------------------------------
    myFile << "Date,Order ID,Customer,Product,Qty,Total (RM),Status\n";

    string query = "SELECT o.order_date, o.smart_id, o.customer_name, p.name, o.quantity, o.total_price, o.status FROM orders o JOIN products p ON o.product_id = p.id ORDER BY o.order_date DESC";
    
    if (mysql_query(conn, query.c_str())) 
    { 
        cout << "   \033[1;31m[ERROR] Database Error.\033[0m\n"; 
        return; 
    }
    
    MYSQL_RES* res = mysql_store_result(conn); 
    MYSQL_ROW row;
    
    // --------------------------------------------------
    // Write Rows
    // --------------------------------------------------
    while ((row = mysql_fetch_row(res))) 
    {
        myFile << row[0] << "," << row[1] << ",\"" << row[2] << "\"," << row[3] << "," << row[4] << "," << row[5] << "," << row[6] << "\n";
    }
    
    mysql_free_result(res); 
    myFile.close();
    printSuccess("Export completed successfully\n    \033[1;32m✔ File saved as: FAIX_Sales_Report.csv"); 
    system("pause");
}