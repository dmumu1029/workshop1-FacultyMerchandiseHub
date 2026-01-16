#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <sstream>

using namespace std;

namespace Utils {

    // ============================================================================
    // getValidInt
    // Reads a full line, ensures it contains only a valid integer.
    // ============================================================================
    static int getValidInt() {
        string line;
        while (true) {
            getline(cin, line);
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

            if (line.empty()) {
                // If just enter is pressed, we might want to ignore or retry depending on usage.
                // Assuming we want a value:
                cout << " \033[1;31mInput required ➜ \033[0m";
                continue;
            }

            // Check if every character is a digit (allow leading -)
            bool valid = true;
            size_t start = 0;
            if (line[0] == '-') {
                if (line.length() == 1) valid = false; // just "-" is invalid
                start = 1;
            }
            
            for (size_t i = start; i < line.length(); ++i) {
                if (!isdigit(line[i])) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                try {
                    return stoi(line);
                } catch (...) {
                    // Overflow or other conversion error
                    valid = false;
                }
            }

            cout << " \033[1;31mInvalid input (integers only) ➜ \033[0m";
        }
    }

    // ============================================================================
    // getValidRange
    // ============================================================================
    static int getValidRange(int min, int max) {
        while (true) {
            int x = getValidInt();
            if (x >= min && x <= max) {
                return x;
            }
            cout << " \033[1;31mOut of range (" << min << "-" << max << ") ➜ \033[0m";
        }
    }

    // ============================================================================
    // getValidDouble
    // Reads a full line, ensures it is a valid double.
    // ============================================================================
    static double getValidDouble() {
        string line;
        while (true) {
            getline(cin, line);

            // Trim
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

            if (line.empty()) {
                cout << " \033[1;31mInput required ➜ \033[0m";
                continue;
            }

            size_t idx;
            try {
                double val = stod(line, &idx);
                // Check if we consumed the whole string
                if (idx == line.length()) {
                    return val;
                }
            } catch (...) {
                // Conversion failed
            }
            cout << " \033[1;31mInvalid input (number required) ➜ \033[0m";
        }
    }

    // ============================================================================
    // getValidString
    // Ensures non-empty string. Optional max length check.
    // ============================================================================
    static string getValidString(size_t maxLength = 0) {
        string line;
        while (true) {
            getline(cin, line);
            
            // Trim (Internal logic may vary, but generally we want to allow spaces in names/sentences,
            // but not *just* spaces)
            string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
            trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

            if (trimmed.empty()) {
                cout << " \033[1;31mInput required ➜ \033[0m";
                continue;
            }

            if (maxLength > 0 && line.length() > maxLength) {
                 cout << " \033[1;31mInput too long (Max " << maxLength << " chars) ➜ \033[0m";
                 continue;
            }

            return line;
        }
    }

    // ============================================================================
    // getValidYesNo
    // Returns 1 for Yes, 0 for No. Accepts 1, 0, y, n, Y, N.
    // ============================================================================
    static int getValidYesNo() {
         string line;
         while(true) {
             getline(cin, line);
             // Trim
             line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
             line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

             if (line == "1" || line == "y" || line == "Y" || line == "yes" || line == "YES") return 1;
             if (line == "0" || line == "n" || line == "N" || line == "no" || line == "NO") return 0;

             cout << " \033[1;31mInvalid input (1=Yes, 0=No) ➜ \033[0m";
         }
    }
}

#endif // UTILS_H
