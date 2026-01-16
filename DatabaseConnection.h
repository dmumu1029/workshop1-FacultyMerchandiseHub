// ============================================================================
// DATABASE CONNECTION HEADER
// ============================================================================
#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

// External Libraries
#include <mysql.h> // MySQL C API for database interaction

class DatabaseConnection
{
public:
    // ============================================================================
    // Constructor / Destructor
    // ============================================================================
    DatabaseConnection();
    ~DatabaseConnection();
    
    // ============================================================================
    // Connection Methods
    // ============================================================================
    MYSQL* connect();

private:
    MYSQL* conn; // Internal MySQL connection handler
};

#endif