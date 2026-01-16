// ============================================================================
// INVENTORY MODULE IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "InventoryModule.h"

// Standard Libraries
#include <iostream>    // Standard I/O (cout, cin)
#include <string>      // String manipulation
#include <iomanip>     // I/O formatting (setw, setfill)
#include <cstdlib>     // Standard library (atoi, system)
#include <limits>      // Numeric limits for validation
#include <ctime>       // Time functions for timestamps
#include "Utils.h"     // Shared Utility Functions

using namespace std;

// ============================================================================
// 2/10 printSuccess
// ============================================================================
static void printSuccess(string msg)
{
    cout << "\n   ─────────────────────────────────────────────────────\n";
    cout << "    \033[1;32m✔ " << msg << "\033[0m\n";
    cout << "   ─────────────────────────────────────────────────────\n";
}

// ============================================================================
// 3/10 printError
// ============================================================================
static void printError(string msg)
{
    cout << "   \033[1;31m[ERROR] " << msg << "\033[0m\n";
}

// ============================================================================
// 4/10 InventoryModule (Constructor)
// ============================================================================
InventoryModule::InventoryModule(MYSQL* c)
{
    conn = c;
}

// ============================================================================
// 5/10 manageInventory
// ============================================================================
void InventoryModule::manageInventory()
{
    int choice;
    do
    {
        system("cls");
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║                 INVENTORY MANAGEMENT                 ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";

        // --------------------------------------------------
        // Display Current Products
        // --------------------------------------------------
        viewProducts();

        // --------------------------------------------------
        // Display Menu
        // --------------------------------------------------
        cout << "\n   [ ACTIONS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) Add New Product\n";
        cout << "    2) Edit / Update Product\n";
        cout << "    3) Delete Product\n";
        cout << "    4) Search Product\n";
        cout << "\n";
        cout << "    0) Back to Main Menu\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        cout << "   Choice ➜ ";
        choice = Utils::getValidRange(0, 4);

        // --------------------------------------------------
        // Process Selection
        // --------------------------------------------------
        switch (choice)
        {
            case 1: 
                addProduct(); 
                break;
            case 2: 
                editProduct(); 
                break;
            case 3: 
                deleteProduct(); 
                break;
            case 4: 
                searchProduct(); 
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
// 6/10 viewProducts
// ============================================================================
void InventoryModule::viewProducts()
{
    // --------------------------------------------------
    // Fetch Products
    // --------------------------------------------------
    if (mysql_query(conn, "SELECT * FROM products"))
    {
        printError(mysql_error(conn)); 
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;
    int total = mysql_num_rows(res);

    time_t now = time(0);
    tm* ltm = localtime(&now);

    // --------------------------------------------------
    // Display Header Stats
    // --------------------------------------------------
    cout << "\n";
    cout << "   TOTAL PRODUCTS : " << total << "\n";
    cout << "   LAST UPDATED   : " << 1900 + ltm->tm_year << "-" 
         << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-" 
         << setw(2) << ltm->tm_mday
         << " " << setw(2) << ltm->tm_hour << ":" << setw(2) << ltm->tm_min << "\n";

    cout << "\n";
    cout << "  ┌─────┬────────────────────┬──────────────┬────────────┬─────────────┬────────┐\n";
    cout << "  │ ID  │ PRODUCT NAME       │ TYPE         │ PRICE      │ STOCK       │ HOURS  │\n";
    cout << "  ├─────┼────────────────────┼──────────────┼────────────┼─────────────┼────────┤\n";

    // --------------------------------------------------
    // Populate List
    // --------------------------------------------------
    while ((row = mysql_fetch_row(res)))
    {
        int idNum = atoi(row[0]);
        int stock = atoi(row[4]);
        string type = row[2];
        string priceStr = "RM " + string(row[3]); 

        cout << "  │ " 
             << right << setfill('0') << setw(3) << idNum << setfill(' ') << " │ "
             << left  << setw(18) << row[1] << " │ "
             << left  << setw(12) << row[2] << " │ "
             << right << setw(10) << priceStr << " │ "
             << right << setw(9) << stock;

        // #### Low Stock Warning Check ####
        if (type == "Ready Stock" && stock < 10)
        {
            cout << " \033[1;31m!\033[0m "; 
        }
        else
        {
            cout << "   "; 
        }

        cout << "│ " << right << setw(6) << row[5] << " │\n";
    }

    cout << "  └─────┴────────────────────┴──────────────┴────────────┴─────────────┴────────┘\n";
    
    // --------------------------------------------------
    // Footer Legend
    // --------------------------------------------------
    cout << "   ┌───────────────────────────────────────────────────────────────────────┐\n";
    cout << "   │ [INFO] HOURS = Production Hours                                       │\n";
    cout << "   │ The time it takes to manufacture ONE quantity of that custom item.    │\n";
    cout << "   └───────────────────────────────────────────────────────────────────────┘\n";
    cout << "    \033[1;31m(!) = Low Stock Warning (< 10 units)\033[0m\n"; 

    mysql_free_result(res);
}

// ============================================================================
// 7/10 addProduct
// ============================================================================
void InventoryModule::addProduct()
{
    string name;
    string type;
    double price;
    int stock;
    int hours;

    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ ADD NEW PRODUCT (ENTER 0 ONLY TO CANCEL)           │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   [ INFO: Enter 0 to Cancel ]\n\n";
    cout << "   Product Name ➜ ";
    name = Utils::getValidString();
    
    // #### Cancel Check ####
    if (name == "0") 
    {
        return;
    }

    // #### Empty Check ####
    if (name.empty() || name.find_first_not_of(' ') == string::npos)
    {
        printError("Product name cannot be empty.");
        system("pause");
        return;
    }

    int tChoice;
    cout << "   Type (1=Ready Stock, 2=Custom) ➜ ";
    tChoice = Utils::getValidRange(1, 2);

    // --------------------------------------------------
    // Type-Specific Inputs
    // --------------------------------------------------
    if (tChoice == 1)
    {
        type = "Ready Stock";
        // NOTE: Production hours are zero for Ready Stock items
        hours = 0;
        
        cout << "   Price (RM)   ➜ "; 
        price = Utils::getValidDouble();

        if (price == 0)
        {
            return;
        }

        cout << "   Init Stock   ➜ "; 
        stock = Utils::getValidInt();
        
        if (stock == 0)
        {
            return;
        }
    }
    else
    {
        type = "Custom";
        
        cout << "   Base Price   ➜ "; 
        price = Utils::getValidDouble();

        if (price == 0)
        {
            return;
        }

        // Custom items don't hold stock of finished goods
        stock = 0; 
        
        cout << "   Prod Hours   ➜ "; 
        hours = Utils::getValidInt();
        
        if (hours == 0)
        {
            return;
        }
    }

    // #### Negative Value Check ####
    if (price < 0 || stock < 0 || hours < 0)
    {
        printError("Values cannot be negative.");
        system("pause"); 
        return;
    }

    // --------------------------------------------------
    // Database Insertion
    // --------------------------------------------------
    string query = "INSERT INTO products (name, type, price, stock_quantity, production_hours) VALUES ('" + name + "', '" + type + "', " + to_string(price) + ", " + to_string(stock) + ", " + to_string(hours) + ")";

    if (!mysql_query(conn, query.c_str()))
    {
        printSuccess("Product Added Successfully");
    }
    else
    {
        printError(mysql_error(conn));
    }

    system("pause");
}

// ============================================================================
// 8/10 editProduct
// ============================================================================
void InventoryModule::editProduct()
{
    int id;
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ EDIT / UPDATE PRODUCT (ENTER 0 ONLY TO CANCEL)     │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   [ INFO: Enter 0 to Cancel ]\n\n";
    cout << "   Enter Product ID ➜ ";
    id = Utils::getValidInt();
    
    // #### Cancel Check ####
    if (id == 0) 
    {
        return;
    }

    // --------------------------------------------------
    // Check Existence
    // --------------------------------------------------
    string checkSql = "SELECT name, type, price, stock_quantity, production_hours FROM products WHERE id=" + to_string(id);
    
    if (mysql_query(conn, checkSql.c_str())) 
    {
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    // #### Not Found Check ####
    if (!row)
    {
        printError("Product ID not found!");
        mysql_free_result(res);
        system("pause");
        return;
    }

    string currentName = row[0];
    string currentType = row[1];
    string currentPrice = row[2];
    string currentStock = row[3];
    string currentHours = row[4];
    mysql_free_result(res);

    // --------------------------------------------------
    // Display Current Info
    // --------------------------------------------------
    cout << "\n   \033[1;33m[ SELECTED: " << currentName << " ]\033[0m\n";
    cout << "   ──────────────────────────────────────────\n";
    cout << "    Name       : " << currentName << "\n";
    cout << "    Type       : " << currentType << "\n";
    cout << "    Price      : RM " << currentPrice << "\n";
    
    if (currentType == "Ready Stock") 
    {
         cout << "    Stock      : " << currentStock << "\n";
    }
    else
    {
         cout << "    Prod.Hours : " << currentHours << "\n";
    }
    cout << "   ──────────────────────────────────────────\n";

    // --------------------------------------------------
    // Field Selection (Dynamic Menu)
    // --------------------------------------------------
    cout << "   What would you like to edit?\n";
    cout << "   ─────────────────────────────\n";
    cout << "    1. Name\n";
    cout << "    2. Type (Ready Stock / Custom)\n";
    cout << "    3. Price\n";
    
    // Dynamic fourth option
    if (currentType == "Ready Stock")
    {
        cout << "    4. Stock Quantity\n";
    }
    else
    {
        cout << "    4. Production Hours\n";
    }

    cout << "\n    0. Cancel\n";
    cout << "   ─────────────────────────────\n";
    cout << "   Choice ➜ ";
    
    int editChoice = Utils::getValidRange(0, 4);
    
    if (editChoice == 0) 
    {
        return;
    }

    string updateQuery = "";
    
    if (editChoice == 1) 
    {
        // WARNING: Changing product name affects historical reports
        cout << "\n   \033[1;33m[WARNING] Changing the product name will affect\n";
        cout << "   how it appears in future reports.\033[0m\n";
        cout << "   Are you sure? (1=Yes, 0=No) ➜ ";
        int confirm = Utils::getValidRange(0, 1);
        if (confirm != 1) 
        {
            return;
        }

        string newName;
        cout << "   Enter New Name ➜ "; 
        newName = Utils::getValidString();
        updateQuery = "UPDATE products SET name='" + newName + "' WHERE id=" + to_string(id);
    }
    else if (editChoice == 2) 
    {
        cout << "   Select New Type:\n   1. Ready Stock\n   2. Custom\n   ➜ ";
        int t = Utils::getValidInt();
        string newType = (t == 1) ? "Ready Stock" : "Custom";
        
        // Reset specific fields when switching type
        if (newType == "Ready Stock")
        {
             // Reset hours to 0 if becoming Ready Stock
             updateQuery = "UPDATE products SET type='Ready Stock', production_hours=0 WHERE id=" + to_string(id);
        }
        else
        {
             // Reset stock to 0 if becoming Custom
             updateQuery = "UPDATE products SET type='Custom', stock_quantity=0 WHERE id=" + to_string(id);
        }
    }
    else if (editChoice == 3) 
    {
        double newPrice;
        cout << "   Enter New Price (RM) ➜ "; 
        newPrice = Utils::getValidDouble();

        if (newPrice < 0) 
        { 
            printError("Negative Price."); 
            system("pause"); 
            return; 
        }
        updateQuery = "UPDATE products SET price=" + to_string(newPrice) + " WHERE id=" + to_string(id);
    }
    else if (editChoice == 4) 
    {
        if (currentType == "Ready Stock")
        {
            // Edit Stock
            int newStock;
            cout << "   Enter New Stock Qty ➜ "; 
            newStock = Utils::getValidInt();
            
            if (newStock < 0) 
            { 
                printError("Negative Stock."); 
                system("pause"); 
                return; 
            }
            updateQuery = "UPDATE products SET stock_quantity=" + to_string(newStock) + " WHERE id=" + to_string(id);
        }
        else
        {
            // Edit Hours
            int newHours;
            cout << "   Enter Production Hours ➜ "; 
            newHours = Utils::getValidInt();
            
            if (newHours < 0) 
            { 
                printError("Negative Hours."); 
                system("pause"); 
                return; 
            }
            updateQuery = "UPDATE products SET production_hours=" + to_string(newHours) + " WHERE id=" + to_string(id);
        }
    }

    // --------------------------------------------------
    // Perform Update
    // --------------------------------------------------
    if (!mysql_query(conn, updateQuery.c_str()))
    {
        printSuccess("Product Updated Successfully");
    }
    else
    {
        printError("Update Failed.");
    }

    system("pause");
}

// ============================================================================
// 9/10 deleteProduct
// ============================================================================
void InventoryModule::deleteProduct()
{
    int id;
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ DELETE PRODUCT (ENTER 0 ONLY TO CANCEL)            │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   [ INFO: Enter 0 to Cancel ]\n\n";
    cout << "   Enter Product ID ➜ ";
    id = Utils::getValidInt();
    
    if (id == 0) 
    {
        return;
    }

    // --------------------------------------------------
    // Check Dependencies
    // --------------------------------------------------
    string checkQ = "SELECT COUNT(*) FROM orders WHERE product_id=" + to_string(id);
    
    if (mysql_query(conn, checkQ.c_str())) 
    {
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    int orderCount = atoi(row[0]);
    mysql_free_result(res);

    // #### Linked Order Check ####
    if (orderCount > 0)
    {
        cout << "\n   \033[1;31m[RESTRICTED] Cannot delete Product ID " << id << ".\n";
        cout << "   Reason: " << orderCount << " orders are linked to this item.\033[0m\n";
        system("pause");
        return;
    }

    cout << "   Are you sure you want to delete ID " << id << "? (1=Yes, 0=No) ➜ ";
    int confirm = Utils::getValidRange(0, 1);
    
    if (confirm != 1) 
    {
        return;
    }

    // --------------------------------------------------
    // Execute Deletion
    // --------------------------------------------------
    string query = "DELETE FROM products WHERE id=" + to_string(id);
    
    if (!mysql_query(conn, query.c_str()))
    {
        printSuccess("Product Deleted");
    }
    else
    {
        printError(mysql_error(conn));
    }

    system("pause");
}

// ============================================================================
// 10/10 searchProduct
// ============================================================================
void InventoryModule::searchProduct()
{
    string keyword;
    cout << "\n";
    cout << "  ┌────────────────────────────────────────────────────┐\n";
    cout << "  │ SEARCH PRODUCT                                     │\n";
    cout << "  └────────────────────────────────────────────────────┘\n";
    cout << "   Enter Keyword ➜ ";
    keyword = Utils::getValidString();

    // --------------------------------------------------
    // Fuzzy Search Query
    // --------------------------------------------------
    string query = "SELECT * FROM products WHERE name LIKE '%" + keyword + "%'";
    
    if (mysql_query(conn, query.c_str())) 
    {
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\n   [ SEARCH RESULTS ]\n";
    cout << "  ┌─────┬────────────────────┬────────────┬──────────┐\n";
    cout << "  │ ID  │ PRODUCT NAME       │ PRICE      │ STOCK    │\n";
    cout << "  ├─────┼────────────────────┼────────────┼──────────┤\n";

    bool found = false;
    while ((row = mysql_fetch_row(res)))
    {
        found = true;
        int idNum = atoi(row[0]);
        cout << "  │ " 
             << right << setfill('0') << setw(3) << idNum << setfill(' ') << " │ "
             << left  << setw(18) << row[1] << " │ RM " 
             << left  << setw(7) << row[3] << " │ " 
             << left  << setw(8) << row[4] << " │\n";
    }

    cout << "  └─────┴────────────────────┴────────────┴──────────┘\n";
    
    // #### Result Check ####
    if (!found) 
    {
        cout << "   \033[1;31mNo products found.\033[0m\n";
    }

    mysql_free_result(res);
    system("pause");
}