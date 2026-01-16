// ============================================================================
// ADMIN MODULE HEADER
// ============================================================================
#ifndef ADMIN_MODULE_H
#define ADMIN_MODULE_H

// External Libraries
#include <mysql.h>      // MySQL C API for database interaction
#include <string>       // String manipulation
#include <iostream>     // Standard I/O

using namespace std;

class AdminModule
{
public:
    // ============================================================================
    // Constructor
    // ============================================================================
    AdminModule(MYSQL* c);
    
    // ============================================================================
    // Public Operations
    // ============================================================================
    string login();
    void manageAdmins();

private:
    MYSQL* conn; // Internal MySQL connection handler
    
    // ============================================================================
    // Private Helpers
    // ============================================================================
    void addAdmin();
    void listAdmins();
    void deleteAdmin();
};

#endif