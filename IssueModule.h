// ============================================================================
// ISSUE MODULE HEADER
// ============================================================================
#ifndef ISSUE_MODULE_H
#define ISSUE_MODULE_H

// External Libraries
#include <mysql.h> // MySQL C API for database interaction

class IssueModule
{
public:
    // ============================================================================
    // Constructor
    // ============================================================================
    IssueModule(MYSQL* c);
    
    // ============================================================================
    // Public Operations
    // ============================================================================
    void manageIssues();

private:
    MYSQL* conn; // Database Connection Handler
    
    // ============================================================================
    // Private Helpers
    // ============================================================================
    void createIssue();
    void viewIssues();
    void updateIssue();
    void deleteIssue();
    void searchIssue();
};

#endif
