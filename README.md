# Souvenir Shop Management System

A C++ based management system for handling inventory, orders, issues, and reports for a souvenir shop. This project uses a MySQL database for data persistence.

## Project Structure

*   **`SouvenirSystem.exe`**: The main executable. Run this to start the application immediately.
*   **`runcode.bat`**: A helper script to compile and run the code. Useful if you make changes to the source.
*   **`.cpp` / `.h` files**: The source code for different modules (Admin, Inventory, Order, Issue, Report).
*   **`souvenir_system_setup.sql`**: The SQL script to set up the database.
*   **`include/` & `libmysql.*`**: Dependencies for connecting C++ to MySQL.

## Prerequisites

To run this system, you need:

1.  **Windows OS** (The executable is built for Windows).
2.  **MySQL Server**: You must have a local MySQL server running.
    *   Default connection assumes: `localhost`, User: `root`, Password: `` (empty), Database: `souvenir_system`.
    *   *Note: If your MySQL password is different, you may need to recompile the code with the correct credentials in `DatabaseConnection.cpp`.*

## Setup Instructions

1.  **Database Setup**:
    *   Open your MySQL Workbench or command line.
    *   Import/Run the `souvenir_system_setup.sql` script to create the database and tables.

2.  **Running the App**:
    *   Double-click **`SouvenirSystem.exe`**.

3.  **Compiling (Optional)**:
    *   If you want to modify the code, you can use the included `runcode.bat` script.
    *   Ensure you have a C++ compiler (like MinGW `g++`) installed and added to your system PATH.
    *   Run `runcode.bat` in a terminal.
