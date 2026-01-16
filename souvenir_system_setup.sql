-- ================================================================
-- MASTER SETUP SCRIPT FOR FAIX MERCHANDISE HUB
-- ================================================================

-- 1. SETUP DATABASE
-- ----------------------------------------------------------------
CREATE DATABASE IF NOT EXISTS `souvenir_system`;
USE `souvenir_system`;

-- 2. RESET TABLES (Drop in correct order to avoid Foreign Key errors)
-- ----------------------------------------------------------------
SET FOREIGN_KEY_CHECKS = 0;
DROP TABLE IF EXISTS `issues`;
DROP TABLE IF EXISTS `orders`;
DROP TABLE IF EXISTS `products`;
DROP TABLE IF EXISTS `admins`;
SET FOREIGN_KEY_CHECKS = 1;

-- 3. CREATE TABLES
-- ----------------------------------------------------------------

-- Table: admins
CREATE TABLE `admins` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password` varchar(50) NOT NULL,
  `role` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table: products
CREATE TABLE `products` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  `type` varchar(20) NOT NULL,
  `price` decimal(10,2) NOT NULL,
  `stock_quantity` int(11) DEFAULT 0,
  `production_hours` int(11) DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table: orders
CREATE TABLE `orders` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `smart_id` varchar(50) NOT NULL,
  `product_id` int(11) NOT NULL,
  `customer_name` varchar(100) NOT NULL,
  `address` varchar(255) DEFAULT NULL,
  `quantity` int(11) NOT NULL,
  `total_price` decimal(10,2) NOT NULL,
  `order_date` timestamp NOT NULL DEFAULT current_timestamp(),
  `expected_date` datetime DEFAULT NULL,
  `status` varchar(20) DEFAULT 'Pending',
  `cust_size` varchar(50) DEFAULT NULL,
  `cust_color` varchar(50) DEFAULT NULL,
  `cust_text` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `smart_id` (`smart_id`),
  KEY `product_id` (`product_id`),
  CONSTRAINT `orders_ibfk_1` FOREIGN KEY (`product_id`) REFERENCES `products` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table: issues
CREATE TABLE `issues` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `order_id` int(11) NOT NULL,
  `issue_type` varchar(50) DEFAULT NULL,
  `resolution` varchar(50) DEFAULT NULL,
  `log_date` timestamp NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`id`),
  KEY `order_id` (`order_id`),
  CONSTRAINT `issues_ibfk_1` FOREIGN KEY (`order_id`) REFERENCES `orders` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 4. INSERT BASE DATA
-- ----------------------------------------------------------------

-- Admins
INSERT INTO `admins` (`id`, `username`, `password`, `role`) VALUES
(1, 'admin', '123', 'Manager'),
(2, 'staff', '123', 'Staff'),
(3, 'dhivya', '123', 'Manager');

-- Products
INSERT INTO `products` (`id`, `name`, `type`, `price`, `stock_quantity`, `production_hours`) VALUES
(1, 'FAIX Lanyard', 'Ready Stock', 15.00, 43, 0),
(2, 'FAIX T-Shirt', 'Ready Stock', 35.00, 5, 0),
(3, 'FAIX Cap', 'Ready Stock', 29.00, 20, 0),
(4, 'Phone Case', 'Custom', 25.00, 0, 24),
(5, 'T-Shirt', 'Custom', 40.00, 0, 48);

-- 5. GENERATE HISTORY (Nov 2024 - Jan 2026)
-- ----------------------------------------------------------------
DROP PROCEDURE IF EXISTS GenerateFullHistory;

DELIMITER $$
CREATE PROCEDURE GenerateFullHistory()
BEGIN
    DECLARE curr_date DATE DEFAULT '2024-11-01';
    DECLARE end_date DATE DEFAULT '2026-01-16';
    DECLARE daily_orders INT;
    DECLARE i INT;
    DECLARE rand_prod_id INT;
    DECLARE rand_qty INT;
    DECLARE unit_price DECIMAL(10,2);
    DECLARE final_price DECIMAL(10,2);
    DECLARE status_str VARCHAR(20);
    DECLARE smart_id_str VARCHAR(50);
    DECLARE cust_name VARCHAR(100);
    DECLARE cust_addr VARCHAR(100);
    DECLARE prod_prefix VARCHAR(10);
    DECLARE issue_chance INT;
    DECLARE inserted_order_id INT;
    
    WHILE curr_date <= end_date DO
        IF (FLOOR(RAND() * 10) < 7) THEN
            SET daily_orders = FLOOR(1 + RAND() * 2);
            SET i = 1;
            WHILE i <= daily_orders DO
                SET rand_prod_id = FLOOR(1 + RAND() * 5);
                IF rand_prod_id = 1 THEN SET unit_price = 15.00; SET prod_prefix = 'FAI';
                ELSEIF rand_prod_id = 2 THEN SET unit_price = 35.00; SET prod_prefix = 'TSH';
                ELSEIF rand_prod_id = 3 THEN SET unit_price = 29.00; SET prod_prefix = 'CAP';
                ELSEIF rand_prod_id = 4 THEN SET unit_price = 25.00; SET prod_prefix = 'CAS';
                ELSE SET unit_price = 40.00; SET prod_prefix = 'TSH'; END IF;

                IF (RAND() < 0.8) THEN SET rand_qty = FLOOR(1 + RAND() * 5);
                ELSE SET rand_qty = FLOOR(10 + RAND() * 5); END IF;
                
                SET final_price = unit_price * rand_qty;
                IF rand_qty >= 10 THEN SET final_price = final_price * 0.90; END IF;

                IF YEAR(curr_date) < 2026 THEN
                    SET issue_chance = FLOOR(RAND() * 100);
                    IF issue_chance < 90 THEN SET status_str = 'Completed';
                    ELSEIF issue_chance < 95 THEN SET status_str = 'Cancelled';
                    ELSE SET status_str = 'Refunded'; END IF;
                ELSE
                    SET issue_chance = FLOOR(RAND() * 100);
                    IF issue_chance < 20 THEN SET status_str = 'Pending';
                    ELSEIF issue_chance < 40 THEN SET status_str = 'Processing';
                    ELSEIF issue_chance < 60 THEN SET status_str = 'In Production';
                    ELSEIF issue_chance < 80 THEN SET status_str = 'Shipped';
                    ELSE SET status_str = 'Completed'; END IF;
                END IF;

                SET cust_name = ELT(FLOOR(1 + RAND() * 25), 'Ali', 'Abu', 'Siti', 'Chong', 'Muthu', 'Sarah', 'David', 'Mei Ling', 'Raju', 'Faizal', 'Ahmad', 'Jessica', 'Tan', 'Kumar', 'Nurul', 'Haziq', 'Wei Hong', 'Priya', 'Daniel', 'Farhana', 'Lim', 'Azlan', 'Grace', 'Kavitha', 'Zaid');
                SET cust_addr = ELT(FLOOR(1 + RAND() * 14), 'Kolej Tuah', 'Kolej Jebat', 'Kolej Kasturi', 'Library', 'FAIX Office', 'Kolej Lekir', 'Kolej Aminuddin', 'FTMK Lab', 'Kafe FAIX', 'Durian Tunggal', 'Melaka Baru', 'Ayer Keroh', 'Bukit Beruang', 'Taman Tasik');
                SET smart_id_str = CONCAT(prod_prefix, '-', DATE_FORMAT(curr_date, '%d%m%Y'), '-', LPAD(i, 3, '0'));

                INSERT INTO `orders` (`smart_id`, `product_id`, `customer_name`, `address`, `quantity`, `total_price`, `order_date`, `expected_date`, `status`, `cust_size`, `cust_color`, `cust_text`) 
                VALUES (smart_id_str, rand_prod_id, cust_name, cust_addr, rand_qty, final_price, CONCAT(curr_date, ' ', '10:00:00'), DATE_ADD(curr_date, INTERVAL 7 DAY), status_str, 'L', 'Black', 'N/A');

                SET inserted_order_id = LAST_INSERT_ID();

                IF (status_str = 'Refunded') THEN
                     INSERT INTO `issues` (`order_id`, `issue_type`, `resolution`, `log_date`) VALUES (inserted_order_id, 'Defect', 'Refund', DATE_ADD(curr_date, INTERVAL 1 DAY));
                ELSEIF (status_str = 'Processing' OR status_str = 'Completed') AND (RAND() < 0.1) THEN
                     INSERT INTO `issues` (`order_id`, `issue_type`, `resolution`, `log_date`) VALUES (inserted_order_id, 'Printing Error', 'Redo', DATE_ADD(curr_date, INTERVAL 1 DAY));
                     IF YEAR(curr_date) = 2026 THEN UPDATE `orders` SET `status` = 'Redo In Progress' WHERE `id` = inserted_order_id; END IF;
                END IF;
                SET i = i + 1;
            END WHILE;
        END IF;
        SET curr_date = DATE_ADD(curr_date, INTERVAL 1 DAY);
    END WHILE;
END$$
DELIMITER ;

CALL GenerateFullHistory();
DROP PROCEDURE GenerateFullHistory;

-- 6. RESTORE STOCK LEVELS
-- ----------------------------------------------------------------
UPDATE products SET stock_quantity = 43 WHERE id = 1;
UPDATE products SET stock_quantity = 5 WHERE id = 2;
UPDATE products SET stock_quantity = 20 WHERE id = 3;
UPDATE products SET stock_quantity = 0 WHERE id = 4;
UPDATE products SET stock_quantity = 0 WHERE id = 5;
