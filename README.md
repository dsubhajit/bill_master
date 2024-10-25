
# Hotel Billing Master

This Hotel Billing Software is a complete billing solution for hotels and restaurants, built with Qt C++ for desktop PCs. Designed for ease of use, it enables hotel staff to handle room bookings, restaurant orders, and generate GST-compliant invoices efficiently. The software includes dedicated modules for hotel billing and restaurant billing, ensuring seamless and accurate financial tracking.




## Features

- GST-Compliant Invoicing: Generate invoices with automated GST calculation based on applicable rates, ensuring legal compliance.
- Room Billing: Handle room bookings, check-ins, check-outs, and room service charges.
- Restaurant Billing: Manage restaurant orders with itemized bills and add charges directly to guest accounts.
- User-Friendly Interface: Designed using Qt's modern GUI components, offering an intuitive and responsive interface.
- Database Integration: Save and retrieve customer and transaction data, supporting reliable record-keeping.
- Multi-User Support: Set up role-based access for administrators, receptionists, and restaurant staff.

## Requirements
- Qt: Developed and tested with Qt 5.x/6.x.
- C++: Compiled with a standard C++ compiler (GCC, MSVC, or Clang).
- MySQL/MariaDB : To store and manage data.

## Installation
Clone the repository and build using Qt Creator or compile with your preferred C++ compiler.
```bash
git clone https://github.com/dsubhajit/bill_master.git
cd hotel-billing-software
qmake && make
bill_master.exe
```
## Database Credentials
Add database credentials in dbinfo.xml file.