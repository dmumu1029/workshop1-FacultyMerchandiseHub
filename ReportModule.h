// ============================================================================
// REPORT MODULE HEADER
// ============================================================================
#ifndef REPORT_MODULE_H
#define REPORT_MODULE_H

// External Libraries
#include <mysql.h>      // MySQL C API
#include <string>       // String manipulation
#include <iostream>     // I/O operations

using namespace std;

class ReportModule
{
public:
    // ============================================================================
    // Constructor
    // ============================================================================
    ReportModule(MYSQL* c);
    
    // ============================================================================
    // Public Operations
    // ============================================================================
    void generateReport();

private:
    MYSQL* conn; // Database Connection Handler

    // ============================================================================
    // Reporting Menus (Periodic)
    // ============================================================================
    void reportDaily();
    void reportWeekly();
    void reportMonthly();
    void reportYearly();
    void reportViewAll();
    
    // ============================================================================
    // Sub-menus
    // ============================================================================
    void menuSalesTrends();
    void menuOrderAnalysis();
    void viewProductReports();

    // ============================================================================
    // Visual & Analysis
    // ============================================================================
    void showTrend(string type);
    void showHighLowOrders(bool high);

    // ============================================================================
    // Export Operations
    // ============================================================================
    void exportToCSV();

    // ============================================================================
    // Data Stats Helpers
    // ============================================================================
    string getItemStats(string dateCondition, bool best);
    string getGlobalBestItem();
    string getGlobalWorstItem();
    string getTopProduct(string dateCondition);
    string getLowProduct(string dateCondition);
    
    // ============================================================================
    // Graph Helpers
    // ============================================================================
    void printBlockGraph(double value, double maxVal);
};

#endif