// ============================================================================
// ORDER MODULE IMPLEMENTATION
// ============================================================================
// Internal Headers
#include "OrderModule.h"

// Standard Libraries
#include <iostream>    // Standard I/O (cout, cin)
#include <ctime>       // Time functions for dates
#include <iomanip>     // Formatting (setw, setfill)
#include <string>      // String manipulation
#include <sstream>     // String streams for date construction
#include <cstdlib>     // Standard lib (system, atoi)
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
// 4/11 OrderModule (Constructor)
// ============================================================================
OrderModule::OrderModule(MYSQL* c)
{
    conn = c;
}

// ============================================================================
// 5/11 showTable
// ============================================================================
void OrderModule::showTable()
{
    // --------------------------------------------------
    // Fetch Product Inventory
    // --------------------------------------------------
    mysql_query(conn, "SELECT id, name, type, price, stock_quantity, production_hours FROM products");
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\n   AVAILABLE PRODUCTS\n";
    cout << "  ┌─────┬────────────────────┬──────────────┬────────────┬──────────┬────────┐\n";
    cout << "  │ ID  │ PRODUCT NAME       │ TYPE         │ PRICE      │ STOCK    │ HOURS  │\n";
    cout << "  ├─────┼────────────────────┼──────────────┼────────────┼──────────┼────────┤\n";

    // --------------------------------------------------
    // Populate List
    // --------------------------------------------------
    while ((row = mysql_fetch_row(res)))
    {
        string stock = row[4] ? row[4] : "0";
        string hours = row[5] ? row[5] : "0";
        int idNum = atoi(row[0]);

        cout << "  │ " 
             << right << setfill('0') << setw(3) << idNum << setfill(' ') << " │ " 
             << left  << setw(18) << row[1] << " │ " 
             << left  << setw(12) << row[2] << " │ RM " 
             << left  << setw(7) << row[3] << " │ " 
             << left  << setw(8) << stock << " │ " 
             << left  << setw(6) << hours << " │\n";
    }
    cout << "  └─────┴────────────────────┴──────────────┴────────────┴──────────┴────────┘\n";
    mysql_free_result(res);
}

// ============================================================================
// 6/11 getTodayDate
// ============================================================================
string OrderModule::getTodayDate()
{
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    // Format: YYYY-MM-DD
    ss << (1900 + ltm->tm_year) << "-" << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-" << setfill('0') << setw(2) << ltm->tm_mday;
    return ss.str();
}

// ============================================================================
// 7/11 calculateDate
// ============================================================================
string OrderModule::calculateDate(int hoursPerUnit, int qty)
{
    // --------------------------------------------------
    // Calculate Production Time
    // --------------------------------------------------
    time_t now = time(0); 
    long prodSeconds = (long)qty * hoursPerUnit * 3600; 
    
    // Default 1 day minimum if ready stock
    if (prodSeconds == 0)
    {
        prodSeconds = 24 * 3600;
    }
    
    // --------------------------------------------------
    // Add Shipping Time (3 Days)
    // --------------------------------------------------
    long shippingSeconds = 3 * 24 * 3600; 
    time_t future = now + prodSeconds + shippingSeconds; 
    tm* f = localtime(&future);
    
    stringstream ss;
    ss << (1900 + f->tm_year) << "-" << setfill('0') << setw(2) << (1 + f->tm_mon) << "-" << setfill('0') << setw(2) << f->tm_mday << " " 
       << setfill('0') << setw(2) << f->tm_hour << ":" << setfill('0') << setw(2) << f->tm_min << ":" << setfill('0') << setw(2) << f->tm_sec;
    return ss.str();
}

// ============================================================================
// 8/11 getShippingDateDisplay (Local Helper)
// ============================================================================
string getShippingDateDisplay(int hoursPerUnit, int qty)
{
    time_t now = time(0);
    long prodSeconds = (long)qty * hoursPerUnit * 3600;
    
    if (prodSeconds == 0)
    {
        prodSeconds = 24 * 3600;
    }
    
    // Shipping starts after production ends
    time_t shipTime = now + prodSeconds;
    tm* f = localtime(&shipTime);
    stringstream ss;
    ss << (1900 + f->tm_year) << "-" << setfill('0') << setw(2) << (1 + f->tm_mon) << "-" << setfill('0') << setw(2) << f->tm_mday;
    return ss.str();
}

// ============================================================================
// 9/11 generateID
// ============================================================================
string OrderModule::generateID(string type, int seq)
{
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    // Format Example: FAIX-15022025-001
    ss << type << "-" << setfill('0') << setw(2) << ltm->tm_mday << setfill('0') << setw(2) << 1 + ltm->tm_mon 
       << (1900 + ltm->tm_year) << "-" << setfill('0') << setw(3) << seq;
    return ss.str();
}

// ============================================================================
// 10/11 placeOrder
// ============================================================================
void OrderModule::placeOrder()
{
    system("cls"); 
    int id, qty;
    string custName;
    string addr;
    string size = "N/A";
    string color = "N/A";
    string text = "N/A";
    
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                    PLACE NEW ORDER                   ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "   [ INFO: Enter 0 to Cancel selection ]\n\n";
    showTable(); 
    
    cout << "   Select Item ID ➜ "; 
    id = Utils::getValidInt();
    
    // #### Cancel Check ####
    if (id == 0)
    {
        return;
    } 

    // --------------------------------------------------
    // Retrieve Product Details
    // --------------------------------------------------
    string sql = "SELECT name, type, price, production_hours, stock_quantity FROM products WHERE id=" + to_string(id);
    mysql_query(conn, sql.c_str());
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    // #### Invalid ID Check ####
    if (!row)
    {
        printError("Invalid ID.");
        mysql_free_result(res);
        system("pause");
        return;
    }
    
    string pName = row[0];
    string pType = row[1];
    double price = stod(row[2]);
    int prodHours = stoi(row[3]);
    int currentStock = stoi(row[4]); 
    mysql_free_result(res);

    cout << "\n   \033[1;33m✔ Selected Item : " << pName << "\033[0m\n";
    cout << "   \033[1;33m✔ Category      : " << pType << "\033[0m\n";

    // --------------------------------------------------
    // Customization Logic
    // --------------------------------------------------
    if (pName == "Phone Case")
    {
        // --------------------------------------------------
        // Phone Case Options
        // --------------------------------------------------
        cout << "\n   ┌────────────────────────────────────────────────────┐\n";
        cout << "   │ PHONE MODEL SELECTION                              │\n";
        cout << "   └────────────────────────────────────────────────────┘\n";
        cout << "    1) iPhone 17\n    2) Samsung S25\n    3) Redmi Note 12\n";
        cout << "   Select Model (0 to Cancel) ➜ "; 
        int m = Utils::getValidRange(0, 3);
        
        if (m == 1) 
        { 
            size = "iPhone 17"; 
        }
        else if (m == 2) 
        { 
            size = "Samsung S25"; 
        }
        else if (m == 3) 
        { 
            size = "Redmi Note 12"; 
        }
        else 
        { 
            return; 
        }

        cout << "\n   ┌────────────────────────────────────────────────────┐\n";
        cout << "   │ COLOR SELECTION                                    │\n";
        cout << "   └────────────────────────────────────────────────────┘\n";
        cout << "    1) Black\n    2) White\n    3) Transparent\n";
        cout << "   Select Color ➜ "; 
        int c = Utils::getValidRange(1, 3);
        
        if (c == 1) 
        { 
            color="Black"; 
        }
        else if (c == 2) 
        { 
            color="White"; 
        }
        else if (c == 3) 
        { 
            color="Transparent"; 
        }
        else 
        { 
            return; 
        }

        cout << "\n   ┌────────────────────────────────────────────────────┐\n";
        cout << "   │ CUSTOM TEXT (Max 10 Characters)                    │\n";
        cout << "   └────────────────────────────────────────────────────┘\n";
        cout << "   Enter Text (0 to Cancel) ➜ "; 
        text = Utils::getValidString(10);
    }
    else if (pName.find("T-Shirt") != string::npos) 
    {
        // --------------------------------------------------
        // T-Shirt Options
        // --------------------------------------------------
        cout << "\n   ┌────────────────────────────────────────────────────┐\n";
        cout << "   │ SIZE SELECTION                                     │\n";
        cout << "   └────────────────────────────────────────────────────┘\n";
        cout << "    1) XS  2) S  3) M  4) L  5) XL\n";
        cout << "   Select Size ➜ "; 
        int s = Utils::getValidRange(1, 5);
        
        if (s == 1) 
        { 
            size = "XS"; 
        }
        else if (s == 2) 
        { 
            size = "S"; 
        }
        else if (s == 3) 
        { 
            size = "M"; 
        }
        else if (s == 4) 
        { 
            size = "L"; 
        }
        else if (s == 5) 
        { 
            size = "XL"; 
        }
        else 
        { 
            return; 
        }

        if (pType == "Custom") 
        {
            cout << "\n   ┌────────────────────────────────────────────────────┐\n";
            cout << "   │ COLOR SELECTION                                    │\n";
            cout << "   └────────────────────────────────────────────────────┘\n";
            cout << "    1) Black\n    2) White\n    3) Navy Blue\n";
            cout << "   Select Color ➜ "; 
            int c = Utils::getValidRange(1, 3);
            
            if (c == 1) 
            { 
                color = "Black"; 
            }
            else if (c == 2) 
            { 
                color = "White"; 
            }
            else if (c == 3) 
            { 
                color = "Navy Blue"; 
            }
            else 
            { 
                return; 
            }
            
            cout << "   Enter Text ➜ "; 
            text = Utils::getValidString(15);
        }
        else
        {
            color = "Official Black";
        }
    }
    else
    {
        // cin.ignore(); // Removed ignore
    }

    // --------------------------------------------------
    // Customer Information
    // --------------------------------------------------
    cout << "\n   ┌────────────────────────────────────────────────────┐\n";
    cout << "   │ PLACE NEW ORDER (ENTER 0 ONLY TO CANCEL)           │\n";
    cout << "   └────────────────────────────────────────────────────┘\n";
    cout << "   Quantity (0 = Cancel) ➜ "; 
    qty = Utils::getValidInt();
    
    // #### Quantity Check ####
    if (qty <= 0)
    {
        return;
    } 

    // #### Stock Availability Check ####
    if (pType == "Ready Stock" && qty > currentStock) 
    {
        printError("Insufficient Stock! Available: " + to_string(currentStock));
        system("pause"); 
        return;
    }

    // cin.ignore(); // Removed
    cout << "   Customer Name         ➜ "; 
    custName = Utils::getValidString();
    
    if (custName.empty())
    {
        custName="Walk-in";
    }
    
    cout << "   Delivery Address      ➜ "; 
    addr = Utils::getValidString();

    // --------------------------------------------------
    // Calculate Totals
    // --------------------------------------------------
    double subtotal = price * qty;
    double discount = 0.0;
    string discountMsg = "No Discount";

    if (qty >= 10)
    {
        discount = subtotal * 0.10; 
        discountMsg = "10% Bulk Discount";
    }

    double finalTotal = subtotal - discount;

    string sqlArrivalDate = calculateDate(prodHours, qty); 
    string displayShipDate = getShippingDateDisplay(prodHours, qty); 
    string displayToday = getTodayDate(); 

    // --------------------------------------------------
    // Generate Order ID
    // --------------------------------------------------
    mysql_query(conn, "SELECT COUNT(*) FROM orders WHERE DATE(order_date) = CURDATE()");
    MYSQL_RES* rS = mysql_store_result(conn); 
    MYSQL_ROW rR = mysql_fetch_row(rS);
    int seq = stoi(rR[0]) + 1; 
    mysql_free_result(rS);

    string code = "FAI"; 
    
    if (pName == "Phone Case") 
    { 
        code = "CAS"; 
    }
    else if (pName.find("T-Shirt") != string::npos) 
    { 
        code = "TSH"; 
    }
    else if (pName.find("Cap") != string::npos) 
    { 
        code = "CAP"; 
    }
    
    string finalID = generateID(code, seq);

    // --------------------------------------------------
    // Show Receipt
    // --------------------------------------------------
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════════════════╗\n";
    cout << "  ║                 OFFICIAL DIGITAL RECEIPT             ║\n";
    cout << "  ╚══════════════════════════════════════════════════════╝\n";
    cout << "   Order ID     : \033[1;33m" << finalID << "\033[0m\n"; 
    cout << "   Customer     : " << custName << "\n";
    cout << "   Item         : " << pName << "\n";
    cout << "   Quantity     : " << qty << "\n";
    cout << "   Unit Price   : RM " << fixed << setprecision(2) << price << "\n";
    cout << "   ------------------------------------------------------\n";
    cout << "   Subtotal     : RM " << subtotal << "\n";
    cout << "   Discount     : -RM " << discount << " (" << discountMsg << ")\n";
    cout << "   ------------------------------------------------------\n";
    cout << "   GRAND TOTAL  : RM " << finalTotal << "\n"; 
    cout << "\n   LOGISTICS INFO\n";
    cout << "   ──────────────────────────────────────────────────────\n";
    cout << "   Delivery To  : " << addr << "\n";
    cout << "\n   TIMELINE\n";
    cout << "   ──────────────────────────────────────────────────────\n";
    cout << "   Order Date   : " << displayToday << "\n";
    cout << "   Shipping Est : " << displayShipDate << "\n"; 
    cout << "   Arrival Est  : " << sqlArrivalDate.substr(0,10) << "\n";
    cout << "\n";
    
    
    cout << "\n  ────────────────────────────────────────────────────────\n";
    cout << "   Confirm Order? (1 = Yes | 0 = No) ➜ "; 
    int confirm = Utils::getValidRange(0, 1);
    
    // --------------------------------------------------
    // Commit to Database
    // --------------------------------------------------
    if (confirm == 1)
    {
        stringstream ss;
        ss << "INSERT INTO orders (smart_id, product_id, customer_name, address, quantity, total_price, expected_date, cust_size, cust_color, cust_text) VALUES ('"
           << finalID << "', " << id << ", '" << custName << "', '" << addr << "', " << qty << ", " << finalTotal << ", '" << sqlArrivalDate << "', '" << size << "', '" << color << "', '" << text << "')";
        
        if (!mysql_query(conn, ss.str().c_str()))
        {
            cout << "\n   ────────────────────────────────────────────────────────\n";
            cout << "\n";
            cout << "    \033[1;32m✔ Order confirmed\033[0m\n";
            cout << "    \033[1;32m✔ Order ID: " << finalID << "\033[0m\n";
            
            if (qty >= 10)
            {
                cout << "    \033[1;32m✔ Bulk Discount Applied!\033[0m\n";
            }
            cout << "    \033[1;32m✔ Processing will begin shortly\033[0m\n";
            cout << "\n   ────────────────────────────────────────────────────────\n";
            
            // --------------------------------------------------
            // Deduct Stock (If Applicable)
            // --------------------------------------------------
            if (pType == "Ready Stock")
            {
                mysql_query(conn, ("UPDATE products SET stock_quantity = stock_quantity - " + to_string(qty) + " WHERE id=" + to_string(id)).c_str());
            }
        }
    }
    system("pause"); 
}

// ============================================================================
// 11/11 trackOrder
// ============================================================================
void OrderModule::trackOrder()
{
    int choice;
    string targetID = "";
    do
    {
        system("cls");
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║                 TRACK ORDER DETAILS                  ║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";

        // --------------------------------------------------
        // Retrieve Active Orders
        // --------------------------------------------------
        mysql_query(conn, "SELECT o.smart_id, o.customer_name, o.status, o.total_price, p.name FROM orders o JOIN products p ON o.product_id = p.id");
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        int totalRows = mysql_num_rows(res);

        cout << "\n   [ ACTIVE ORDERS ]\n";
        cout << "  ┌─────┬──────────────────────┬──────────────────────┬──────────────────────┬──────────────────────┬────────────┐\n";
        cout << "  │  No │ ORDER ID             │ ITEM NAME            │ CUSTOMER             │ STATUS               │ TOTAL (RM) │\n";
        cout << "  ├─────┼──────────────────────┼──────────────────────┼──────────────────────┼──────────────────────┼────────────┤\n";
        
        if (totalRows > 0)
        {
            int i = 1;
            while ((row = mysql_fetch_row(res))) 
            {
                cout << "  │ " 
                     << left << setw(3) << i++ << " │ " 
                     << left << setw(20) << row[0] << " │ " 
                     << left << setw(20) << row[4] << " │ " 
                     << left << setw(20) << row[1] << " │ " 
                     << left << setw(20) << row[2] << " │ " 
                     << left << setw(10) << row[3] << " │\n"; 
            }
        }
        else
        {
             cout << "  │ " << setw(108) << "No active orders found." << " │\n";
        }
        cout << "  └─────┴──────────────────────┴──────────────────────┴──────────────────────┴──────────────────────┴────────────┘\n";
        mysql_free_result(res);

        // --------------------------------------------------
        // Selection Menu
        // --------------------------------------------------
        cout << "\n   [ ACTIONS ]\n";
        cout << "   ──────────────────────────────────────────────────────\n";
        cout << "    1) View Detail (Select Row #)\n";
        cout << "    2) Search by ID (e.g. FAIX-...)\n";
        cout << "\n";
        cout << "    0) Back\n";
        cout << "  ────────────────────────────────────────────────────────\n";
        cout << "   Choice ➜ "; 
        
        choice = Utils::getValidRange(0, 2);
        
        if (choice == 0)
        {
            break;
        }

        targetID = "";

        // --------------------------------------------------
        // Search Input Logic
        // --------------------------------------------------
        if (choice == 1)
        {
             if (totalRows == 0)
             {
                 printError("No orders to select.");
                 system("pause");
                 continue;
             }
             cout << "\n   Enter Row # ➜ ";
             int sel = Utils::getValidRange(1, totalRows);
             if (sel < 1 || sel > totalRows)
             {
                 printError("Invalid Row.");
                 system("pause");
                 continue;
             }
             
             // Retrieve actual ID using LIMIT offset
             int offset = sel - 1;
             string getIDQ = "SELECT smart_id FROM orders LIMIT " + to_string(offset) + ", 1";
             mysql_query(conn, getIDQ.c_str());
             MYSQL_RES* resID = mysql_store_result(conn);
             MYSQL_ROW rowID = mysql_fetch_row(resID);
             if (rowID)
             {
                 targetID = rowID[0];
             }
             mysql_free_result(resID);
        }
        else if (choice == 2)
        {
             cout << "   Enter Order ID ➜ ";
             targetID = Utils::getValidString();
        }
        else
        {
             printError("Invalid Option.");
             system("pause");
             continue;
        }

        if (targetID == "")
        {
            continue;
        }

        // --------------------------------------------------
        // Fetch Details
        // --------------------------------------------------
        string sql = "SELECT o.smart_id, o.status, o.customer_name, p.name, o.quantity, p.price, o.total_price, o.cust_size, o.cust_color, o.cust_text, o.order_date, o.expected_date, DATE_SUB(o.expected_date, INTERVAL 3 DAY) FROM orders o JOIN products p ON o.product_id = p.id WHERE o.smart_id='" + targetID + "'";
        mysql_query(conn, sql.c_str()); 
        res = mysql_store_result(conn); 
        row = mysql_fetch_row(res);

        // #### Found Check ####
        if (row)
        {
            cout << "\n";
            cout << "  ╔══════════════════════════════════════════════════════╗\n";
            cout << "  ║                    ORDER DETAILS                     ║\n";
            cout << "  ╚══════════════════════════════════════════════════════╝\n";
            cout << "   Order ID    : \033[1;33m" << row[0] << "\033[0m\n"; 
            cout << "   Status      : " << row[1] << "\n";
            cout << "   Customer    : " << row[2] << "\n";
            cout << "   ──────────────────────────────────────────────────────\n";
            cout << "   Item        : " << row[3] << "\n";
            cout << "   Quantity    : " << row[4] << "\n";
            cout << "   Specs       : " << row[7] << " | " << row[8] << " | " << row[9] << "\n";
            cout << "   Total       : RM " << row[6] << "\n";
            cout << "   ──────────────────────────────────────────────────────\n";
            string dOrder = row[10]; 
            string dArriv = row[11];
            if (dOrder.length() >= 10) 
            {
                cout << "   Ordered On  : " << dOrder.substr(0,10) << "\n";
            }
            
            string dShip = row[12];
            if (dArriv.length() >= 10) 
            {
                 cout << "   Shipping Est: " << (dShip.length() >= 10 ? dShip.substr(0,10) : dShip) << "\n"; 
                 cout << "   Arrival Est : " << dArriv.substr(0,10) << "\n";
            }
            cout << "  ========================================================\n";
            
            // --------------------------------------------------
            // Detail Actions
            // --------------------------------------------------
            cout << "\n   OPTIONS\n";
            cout << "   ──────────────────────────────────────────────────────\n";
            cout << "    1) Update Status\n";
            cout << "    2) Delete Order\n";
            cout << "\n";
            cout << "    0) Back\n";
            cout << "  ────────────────────────────────────────────────────────\n";
            cout << "   Choice ➜ ";
            int ch = Utils::getValidRange(0, 2);
            
            if (ch == 1) 
            {
                 cout << "\n   SELECT NEW STATUS\n   ──────────────────\n    1) Processing\n    2) In Production\n    3) Shipped\n    4) Completed\n    5) Cancelled\n   Select ➜ ";
                 int s = Utils::getValidRange(1, 5); 
                 string newS = "";
                 
                 if (s == 1) newS = "Processing"; 
                 else if (s == 2) newS = "In Production"; 
                 else if (s == 3) newS = "Shipped"; 
                 else if (s == 4) newS = "Completed"; 
                 else if (s == 5) newS = "Cancelled"; 
                 
                 if (newS != "") 
                 {
                     mysql_query(conn, ("UPDATE orders SET status='" + newS + "' WHERE smart_id='" + targetID + "'").c_str());
                     printSuccess("Status Updated");
                 }
            } 
            else if (ch == 2) 
            {
                cout << "   Delete permanently? (1=Yes) ➜ "; 
                int confirm = Utils::getValidRange(0, 1);
                if (confirm == 1) 
                {
                    mysql_query(conn, ("DELETE FROM orders WHERE smart_id='" + targetID + "'").c_str());
                    printSuccess("Order Deleted");
                }
            }
        }
        else 
        { 
            printError("Order Not Found."); 
        }
        mysql_free_result(res); 
        system("pause"); 
    } while(choice != 0);
}