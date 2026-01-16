// ============================================================================
// DATABASE CONNECTION IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "DatabaseConnection.h"

// Standard Libraries
#include <iostream>        // Console output for error logging (cout, cerr)
#include <cstdlib>         // Standard library definitions (NULL, types)

using namespace std;

// ============================================================================
// 1/3 DatabaseConnection (Constructor)
// ============================================================================
DatabaseConnection::DatabaseConnection()
{
    // --------------------------------------------------
    // Initialize MySQL Handler
    // --------------------------------------------------
    conn = mysql_init(0);
}

// ============================================================================
// 2/3 ~DatabaseConnection (Destructor)
// ============================================================================
DatabaseConnection::~DatabaseConnection()
{
    // --------------------------------------------------
    // Cleanup Resources
    // --------------------------------------------------
    if (conn)
    {
        mysql_close(conn);
    }
}

// ============================================================================
// 3/3 connect
// ============================================================================
MYSQL* DatabaseConnection::connect()
{
    // --------------------------------------------------
    // Establish Database Connection
    // --------------------------------------------------
    // #### Connection Check ####
    if (!mysql_real_connect(conn, "127.0.0.1", "root", "", "souvenir_system", 3306, NULL, 0))
    {
        // #### Error Logging ####
        cerr << "\033[1;31m   [CRITICAL] DB Connection Failed: " << mysql_error(conn) << "\033[0m" << endl;
        return nullptr;
    }
    return conn;
}