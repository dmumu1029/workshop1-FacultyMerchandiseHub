// ============================================================================
// ORDER MODULE HEADER
// ============================================================================
#ifndef ORDER_MODULE_H
#define ORDER_MODULE_H

// External Libraries
#include <mysql.h>      // MySQL C API
#include <string>       // String operations
#include <iostream>     // I/O operations

using namespace std;

class OrderModule
{
public:
    // ============================================================================
    // Constructor
    // ============================================================================
    OrderModule(MYSQL* c);
    
    // ============================================================================
    // Public Operations
    // ============================================================================
    void placeOrder();
    void trackOrder();
    
private:
    MYSQL* conn; // Database Connection Handler
    
    // ============================================================================
    // Private Helpers
    // ============================================================================
    void showTable();
    string getTodayDate();
    string calculateDate(int hoursPerUnit, int qty);
    string generateID(string type, int seq);
};

#endif