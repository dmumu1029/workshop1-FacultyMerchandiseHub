// ============================================================================
// INVENTORY MODULE HEADER
// ============================================================================
#ifndef INVENTORY_MODULE_H
#define INVENTORY_MODULE_H

// External Libraries
#include <mysql.h> // MySQL C API

class InventoryModule
{
public:
    // ============================================================================
    // Constructor
    // ============================================================================
    InventoryModule(MYSQL* c);
    
    // ============================================================================
    // Public Operations
    // ============================================================================
    void manageInventory();

private:
    MYSQL* conn; // Database Connection Helper

    // ============================================================================
    // Private Helpers
    // ============================================================================
    void viewProducts();
    void addProduct();
    void editProduct();
    void deleteProduct();
    void searchProduct();
};

#endif