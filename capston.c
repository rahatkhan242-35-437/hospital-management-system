#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Structures
struct Account {
    char accountID[20];
    char password[30];
    char firstName[30];
    char lastName[30];
    char email[50];
    char mobile[15];
    char address[100];
    char dob[12];
    char nid[20];
    char nominee[50];
    char nominee_nid[20];
    char nominee_relation[20];
    char accountType[15];
    float balance;
};

struct Admin {
    char id[20];
    char password[30];
    int isDefaultPassword;
};

struct Loan {
    char accountID[20];
    float amount;
    int status; // 0:pending, 1:approved, -1:rejected
    char purpose[200];
    char rejection_reason[200];
};

// file contains....:(
const char *accountFile = "accounts.txt";
const char *transactionFile = "transactions.txt";
const char *loanFile = "loans.txt";
const char *adminFile = "admin.txt";
const char *supportFile = "support_tickets.txt";
const char *interBankFile = "inter_bank_transfers.txt";

// default admin information....:)
const char *DEFAULT_ADMIN_ID = "admin";
const char *DEFAULT_ADMIN_PASS = "Admin@123";

// function type....:)
void clearScreen();
void pressEnterToContinue();
void getMaskedPassword(char *password);
int isPasswordStrong(const char *password);
void recordTransaction(char *accountID, char *description, float amount);
int getAccount(const char *accountID, struct Account *acc);
void updateUserAccount(struct Account updatedAcc);
void changeAdminPassword(struct Admin *admin, int forceChange);
void adminMenu(struct Admin *admin);
void userMenu(char *accountID);
void forgotPassword();
void displayLogo();

// Validation Functions :)
int isValidEmail(const char *email);
int isValidMobileNumber(const char *mobile);

// Admin Functions......
void displayAllAccountsSummary();
void viewAccountDetails();
void deleteAccount();
void blockAccount();
void viewAdminTransactionHistory();
void reviewLoanRequests();
void generateAccountReport();
void viewBankSummary();
void reviewSupportTickets();

// User Functions....
void checkBalance(char *accountID);
void depositMoney(char *accountID);
void withdrawMoney(char *accountID);
void transferMoney(char *accountID);
void viewUserTransactionHistory(char *accountID);
void applyForLoan(char *accountID);
void checkLoanStatus(char *accountID);
void bankToBankTransfer(char *accountID);
void customerSupport(char *accountID);
void registerUser();

// logo...
void displayLogo() {
    printf("                                                                        \n"
           "                                                                          \n"
           "BBBBBBBBBBBBBBBBB    MMMMMMMM                 MMMMMMMM    SSSSSSSSSSSSSSS \n"
           "B::::::::::::::::B   M:::::::M               M:::::::M   SS:::::::::::::::S\n"
           "B::::::BBBBBB:::::B  M::::::::M             M::::::::M  S:::::SSSSSS::::::S\n"
           "BB:::::B     B:::::B M:::::::::M           M:::::::::M S:::::S     SSSSSSS\n"
           "  B::::B     B:::::B M::::::::::M         M::::::::::M S:::::S            \n"
           "  B::::B     B:::::B M:::::::::::M       M:::::::::::M S:::::S            \n"
           "  B::::BBBBBB:::::B  M:::::::M::::M     M::::M:::::::M  S::::SSSS         \n"
           "  B:::::::::::::BB   M::::::M M::::M   M::::M M::::::M   SS::::::SSSSS    \n"
           "  B::::BBBBBB:::::B  M::::::M  M::::M M::::M  M::::::M     SSS::::::::SS  \n"
           "  B::::B     B:::::B M::::::M   M::::M::::M   M::::::M        SSSSSS::::S \n"
           "  B::::B     B:::::B M::::::M    M:::::M      M::::::M             S:::::S\n"
           "  B::::B     B:::::B M::::::M     MMMMM       M::::::M             S:::::S\n"
           "BB:::::BBBBBB::::::B M::::::M                 M::::::M SSSSSSS    S:::::S\n"
           "B:::::::::::::::::B  M::::::M                 M::::::M  S::::SSSSS:::::S\n"
           "B::::::::::::::::B   M::::::M                 M::::::M   S::::::::::::S \n"
           "BBBBBBBBBBBBBBBBB    MMMMMMMM                 MMMMMMMM    SSSSSSSSSSS   \n"
           "                                                                        \n");
}


void getMaskedPassword(char *password) {
    int i = 0;
    char ch;
    while (1) {
        
        if (ch == '\n' || ch == '\r') {
            password[i] = '\0';
            break;
        } else if (ch == '\b' || ch == 127) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint(ch) && i < 29) {
            password[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}

int isPasswordStrong(const char *password) {
    int hasUpper = 0, hasLower = 0, hasSpecial = 0, hasDigit = 0;
    int len = strlen(password);
    if (len < 6) return 0;
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
        else if (ispunct(password[i])) hasSpecial = 1;
    }
    return hasUpper && hasLower && (hasDigit || hasSpecial);
}

void recordTransaction(char *accountID, char *description, float amount) {
    FILE *file = fopen(transactionFile, "a");
    if (file) {
        fprintf(file, "AccountID: %s, Description: %s, Amount: %.2f\n", accountID, description, amount);
        fclose(file);
    }
}


int isValidEmail(const char *email) {
    if (strchr(email, '@') != NULL) {
        return 1;
    }
    return 0;
}

int isValidMobileNumber(const char *mobile) {
    if (strlen(mobile) != 11) {
        return 0;
    }

    for (int i = 0; i < 11; i++) {
        if (!isdigit(mobile[i])) {
            return 0;
        }
    }

    const char *validPrefixes[] = {"017", "019", "018", "016", "015", "013", "014"};
    int numPrefixes = sizeof(validPrefixes) / sizeof(validPrefixes[0]);
    int isValidPrefix = 0;
    for (int i = 0; i < numPrefixes; i++) {
        if (strncmp(mobile, validPrefixes[i], 3) == 0) {
            isValidPrefix = 1;
            break;
        }
    }

    return isValidPrefix;
}


// ==================== FILE I/O FUNCTIONS ====================
int readAccount(FILE *file, struct Account *acc) {
    return fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f\n",
                  acc->accountID, acc->password, acc->firstName, acc->lastName, acc->email,
                  acc->mobile, acc->address, acc->dob, acc->nid, acc->nominee, acc->nominee_nid,
                  acc->nominee_relation, acc->accountType, &acc->balance) == 14;
}

void writeAccount(FILE *file, struct Account acc) {
    fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%.2f\n",
            acc.accountID, acc.password, acc.firstName, acc.lastName, acc.email, acc.mobile,
            acc.address, acc.dob, acc.nid, acc.nominee, acc.nominee_nid, acc.nominee_relation,
            acc.accountType, acc.balance);
}

int readLoan(FILE *file, struct Loan *loan) {
    return fscanf(file, "%[^,],%f,%d,%[^,],%[^\n]\n",
                  loan->accountID, &loan->amount, &loan->status, loan->purpose, loan->rejection_reason) == 5;
}

void writeLoan(FILE *file, struct Loan loan) {
    fprintf(file, "%s,%.2f,%d,%s,%s\n",
            loan.accountID, loan.amount, loan.status, loan.purpose, loan.rejection_reason);
}

int getAccount(const char *accountID, struct Account *acc) {
    FILE *file = fopen(accountFile, "r");
    if (!file) return 0;
    while (readAccount(file, acc)) {
        if (strcmp(accountID, acc->accountID) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void updateUserAccount(struct Account updatedAcc) {
    FILE *file = fopen(accountFile, "r");
    FILE *temp = fopen("temp.txt", "w");
    struct Account currentAcc;
    if (!file || !temp) return;

    while (readAccount(file, &currentAcc)) {
        if (strcmp(updatedAcc.accountID, currentAcc.accountID) == 0) {
            writeAccount(temp, updatedAcc);
        } else {
            writeAccount(temp, currentAcc);
        }
    }
    fclose(file);
    fclose(temp);
    remove(accountFile);
    rename("temp.txt", accountFile);
}

//ADMIN FUNCTIONS (Sabiha's Part)
void displayAllAccountsSummary() {
    FILE *file = fopen(accountFile, "r");
    struct Account acc;
    if (!file) {
        printf("No accounts file found.\n");
        return;
    }

    printf("\n--- All Registered Accounts ---\n");
    printf("%-15s %-25s %-10s\n", "Account ID", "Name", "Status");
    printf("---------------------------------------------------\n");
    while (readAccount(file, &acc)) {
        char fullName[60];
        sprintf(fullName, "%s %s", acc.firstName, acc.lastName);
        const char* status = (strcmp(acc.password, "BLOCKED") == 0) ? "Blocked" : "Active";
        printf("%-15s %-25s %-10s\n", acc.accountID, fullName, status);
    }
    printf("---------------------------------------------------\n");
    fclose(file);
}

void viewAccountDetails() {
    char accountID[20];
    struct Account acc;

    displayAllAccountsSummary();
    printf("\nEnter Account ID to view details (or type '0' to go back): ");
    scanf("%s", accountID);
    while(getchar() != '\n');
    if(strcmp(accountID, "0") == 0) return;
    if (getAccount(accountID, &acc)) {
        printf("\n--- Account Details for %s ---\n", accountID);
        printf("Full Name        : %s %s\n", acc.firstName, acc.lastName);
        printf("Account Type     : %s\n", acc.accountType);
        printf("Balance          : %.2f\n", acc.balance);
        printf("Email            : %s\n", acc.email);
        printf("Mobile           : %s\n", acc.mobile);
        printf("Address          : %s\n", acc.address);
        printf("Date of Birth    : %s\n", acc.dob);
        printf("NID/Passport     : %s\n", acc.nid);
        printf("Nominee Name     : %s\n", acc.nominee);
        printf("Nominee NID      : %s\n", acc.nominee_nid);
        printf("Nominee Relation : %s\n", acc.nominee_relation);
        printf("Status           : %s\n", strcmp(acc.password, "BLOCKED") == 0 ? "Blocked" : "Active");
        printf("----------------------------------\n");
    } else {
        printf("Account not found.\n");
    }
}

void deleteAccount() {
    char accountID[20];
    displayAllAccountsSummary();

    printf("\nEnter Account ID to delete (or type '0' to go back): ");
    scanf("%s", accountID);
    while(getchar() != '\n');
    if(strcmp(accountID, "0") == 0) return;

    FILE *file = fopen(accountFile, "r");
    FILE *temp = fopen("temp.txt", "w");
    struct Account acc;
    int found = 0;
    if (!file || !temp) {
        printf("Error processing files.\n");
        return;
    }
    while (readAccount(file, &acc)) {
        if (strcmp(accountID, acc.accountID) != 0) {
            writeAccount(temp, acc);
        } else {
            found = 1;
        }
    }
    fclose(file);
    fclose(temp);
    remove(accountFile);
    rename("temp.txt", accountFile);
    printf(found ? "Account deleted successfully.\n" : "Account not found.\n");
}

void blockAccount() {
    char accountID[20];
    struct Account acc;
    displayAllAccountsSummary();

    printf("\nEnter Account ID to block (or type '0' to go back): ");
    scanf("%s", accountID);
    while(getchar() != '\n');
    if(strcmp(accountID, "0") == 0)
        return;
    if (getAccount(accountID, &acc)) {
        strcpy(acc.password, "BLOCKED");
        updateUserAccount(acc);
        printf("Account %s has been blocked.\n", accountID);
    } else {
        printf("Account not found.\n");
    }
}

void viewAdminTransactionHistory() {
    char accountID[20];
    displayAllAccountsSummary();

    printf("\nEnter Account ID to view transactions (or type '0' to go back): ");
    scanf("%s", accountID);
    while(getchar() != '\n');
    if(strcmp(accountID, "0") == 0)
        return;
    FILE *file = fopen(transactionFile, "r");
    char line[200];
    int found = 0;
    if (!file) {
        printf("No transactions recorded yet.\n");
        return;
    }
    printf("\n--- Transaction History for %s ---\n", accountID);
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, accountID)) {
            printf("%s", line);
            found = 1;
        }
    }
    fclose(file);
    if (!found) printf("No transactions found for this account.\n");
}

void reviewLoanRequests() {
    printf("This feature will be implemented in the future.\n");
}

void generateAccountReport() {
    printf("This feature will be implemented in the future.\n");
}

void viewBankSummary() {
    printf("This feature will be implemented in the future.\n");
}

void reviewSupportTickets() {
    printf("This feature will be implemented in the future.\n");
}

void changeAdminPassword(struct Admin *admin, int forceChange) {
    char oldPass[30], newPass[30], confirmPass[30];
    if (!forceChange) {
        printf("Enter current password (or type '0' to go back): ");
        getMaskedPassword(oldPass);
        if(strcmp(oldPass, "0") == 0) return;
        if (strcmp(oldPass, admin->password) != 0) {
            printf("Incorrect password!\n");
            return;
        }
    } else {
        printf("This is your first login. You must change your password.\n");
    }

    printf("\nPassword must have at least 6 characters, with uppercase, lowercase, and a number/symbol.\n");
    while (1) {
        printf("Enter new password: ");
        getMaskedPassword(newPass);
        if (isPasswordStrong(newPass)) {
            printf("Password strength: Strong\n");
            printf("Confirm new password: ");
            getMaskedPassword(confirmPass);
            if (strcmp(newPass, confirmPass) == 0) {
                strcpy(admin->password, newPass);
                admin->isDefaultPassword = 0;

                FILE *file = fopen(adminFile, "w");
                fprintf(file, "%s,%s,%d\n", admin->id, admin->password, admin->isDefaultPassword);
                fclose(file);
                printf("Admin password changed successfully!\n");
                break;
            } else {
                printf("Passwords do not match. Please try again.\n");
            }
        } else {
            printf("Password is not strong. Please try again.\n");
        }
    }
}

// USER FUNCTIONS (Rahat's Part)

void checkBalance(char *accountID) {
    struct Account acc;
    if (getAccount(accountID, &acc)) {
        printf("Your current balance is: %.2f\n", acc.balance);
    } else {
        printf("Could not retrieve account information.\n");
    }
}

void depositMoney(char *accountID) {
    float amount;
    printf("Enter amount to deposit (or 0 to go back): ");
    scanf("%f", &amount);
    while(getchar() != '\n');
    if (amount == 0) return;
    if (amount < 0) {
        printf("Invalid deposit amount.\n");
        return;
    }
    struct Account acc;
    if (getAccount(accountID, &acc)) {
        acc.balance += amount;
        updateUserAccount(acc);
        recordTransaction(accountID, "Deposit", amount);
        printf("Deposit successful. New balance: %.2f\n", acc.balance);
    } else {
        printf("Error: Could not find your account.\n");
    }
}

void withdrawMoney(char *accountID) {
    float amount;
    char password[30];
    struct Account acc;
    if (!getAccount(accountID, &acc)) {
        printf("Error: Could not find your account.\n");
        return;
    }

    printf("Enter amount to withdraw (or 0 to go back): ");
    scanf("%f", &amount);
    while(getchar() != '\n');
    if (amount == 0)
        return;
    if (amount < 0) {
        printf("Invalid withdrawal amount.\n");
    return;
    }

    printf("Enter your password to confirm: ");
    getMaskedPassword(password);
    if (strcmp(password, acc.password) != 0) {
        printf("Incorrect password! Withdrawal cancelled.\n");
        return;
    }

    if (acc.balance >= amount) {
        acc.balance -= amount;
        updateUserAccount(acc);
        recordTransaction(accountID, "Withdrawal", -amount);
        printf("Withdrawal successful. New balance: %.2f\n", acc.balance);
    } else {
        printf("Insufficient funds.\n");
    }
}

void transferMoney(char *accountID) {
    char targetID[20], password[30];
    float amount;
    struct Account senderAcc, receiverAcc;
    printf("Enter target Account ID (or type '0' to go back): ");
    scanf("%s", targetID);
    while(getchar() != '\n');
    if (strcmp(targetID, "0") == 0)
        return;

    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    while(getchar() != '\n');
    if (amount <= 0) {
        printf("Invalid transfer amount.\n");
        return;
    }
    if (strcmp(accountID, targetID) == 0) {
        printf("Cannot transfer money to the same account.\n");
        return;
    }

    if (getAccount(accountID, &senderAcc) && getAccount(targetID, &receiverAcc)) {
        printf("Enter your password to confirm transfer: ");
        getMaskedPassword(password);

        if(strcmp(password, senderAcc.password) != 0){
            printf("Incorrect password! Transfer cancelled.\n");
            return;
        }

        if (senderAcc.balance >= amount) {
            senderAcc.balance -= amount;
            receiverAcc.balance += amount;

            updateUserAccount(senderAcc);
            updateUserAccount(receiverAcc);

            recordTransaction(accountID, "Transfer Sent", -amount);
            recordTransaction(targetID, "Transfer Received", amount);
            printf("Transfer successful.\n");
        } else {
            printf("Error: Insufficient funds.\n");
        }
    } else {
        printf("Error: Sender or Target account does not exist.\n");
    }
}

void viewUserTransactionHistory(char *accountID) {
    FILE *file = fopen(transactionFile, "r");
    char line[200];
    int found = 0;
    if (!file) {
        printf("No transactions recorded yet.\n");
        return;
    }

    printf("\n--- Your Transaction History ---\n");
    while (fgets(line, sizeof(line), file)) {
        char search_str[30];
        sprintf(search_str, "AccountID: %s", accountID);

        if (strstr(line, search_str)) {
            printf("%s", line);
            found = 1;
        }
    }
    fclose(file);
    if (!found) {
        printf("No transactions found for your account.\n");
    }
    printf("--------------------------------\n");
}


void applyForLoan(char *accountID) {
    struct Loan newLoan;

    strcpy(newLoan.accountID, accountID);
    printf("Enter loan amount (or 0 to go back): ");
    scanf("%f", &newLoan.amount);
    while(getchar() != '\n');
    if(newLoan.amount == 0)
        return;
    if (newLoan.amount <= 0) {
        printf("Invalid loan amount.\n");
        return;
    }

    printf("Enter the purpose of the loan (e.g., business, education): ");
    fgets(newLoan.purpose, sizeof(newLoan.purpose), stdin);
    newLoan.purpose[strcspn(newLoan.purpose, "\n")] = 0;

    newLoan.status = 0;
    strcpy(newLoan.rejection_reason, "N/A");

    FILE *file = fopen(loanFile, "a");
    if (file) {
        writeLoan(file, newLoan);
        fclose(file);
        printf("Loan request of %.2f sent for approval.\n", newLoan.amount);
    }
}

void checkLoanStatus(char *accountID) {
    FILE *file = fopen(loanFile, "r");
    struct Loan currentLoan;
    int found = 0;

    if (!file) {
        printf("You have not applied for any loans yet.\n");
        return;
    }

    printf("\n--- Your Loan Application Status ---\n");
    while (readLoan(file, &currentLoan)) {
        if (strcmp(accountID, currentLoan.accountID) == 0) {
            found = 1;
            printf("----------------------------------\n");
            printf("Loan Amount: %.2f\n", currentLoan.amount);
            printf("Purpose: %s\n", currentLoan.purpose);
            printf("Status: ");
            switch(currentLoan.status){
                case 0: printf("Pending\n");
                break;
                case 1: printf("Approved\n");
                break;
                case -1:
                    printf("Rejected\n");
                    printf("Reason: %s\n", currentLoan.rejection_reason);
                    break;
            }
        }
    }
    fclose(file);
    if (!found) {
        printf("You have not applied for any loans yet.\n");
    }
    printf("------------------------------------\n");
}


void bankToBankTransfer(char *accountID) {
    char targetBank[50], targetAccHolder[50], targetAcc[20], password[30];
    float amount;
    printf("Enter target bank name (or type '0' to go back): ");
    fgets(targetBank, sizeof(targetBank), stdin);
    targetBank[strcspn(targetBank, "\n")] = 0;
    if (strcmp(targetBank, "0") == 0)
        return;


    printf("Enter target account holder's name: ");
    fgets(targetAccHolder, sizeof(targetAccHolder), stdin);
    targetAccHolder[strcspn(targetAccHolder, "\n")] = 0;

    printf("Enter target account number: ");
    fgets(targetAcc, sizeof(targetAcc), stdin);
    targetAcc[strcspn(targetAcc, "\n")] = 0;
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    while(getchar()!='\n');
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }

    struct Account senderAcc;
    if (getAccount(accountID, &senderAcc)) {
        printf("Enter your password to confirm: ");
        getMaskedPassword(password);
        if(strcmp(password, senderAcc.password) != 0){
            printf("Incorrect password! Transfer cancelled.\n");
            return;
        }

        if (senderAcc.balance >= amount) {
            senderAcc.balance -= amount;
            updateUserAccount(senderAcc);

            FILE* file = fopen(interBankFile, "a");
            if(file) {
                fprintf(file, "From_Acc:%s, To_Bank:%s, To_Holder:%s, To_Acc:%s, Amount:%.2f\n",
                        accountID, targetBank, targetAccHolder, targetAcc, amount);
                fclose(file);
            }
            recordTransaction(accountID, "Inter-Bank Transfer", -amount);
            printf("Inter-bank transfer request of %.2f sent.\n", amount);
        } else {
            printf("Insufficient funds for this transfer.\n");
        }
    }
}

void customerSupport(char *accountID) {
    char problem[200];
    printf("Please describe your problem (max 200 chars) or type 'back' to return:\n");
    fgets(problem, sizeof(problem), stdin);
    problem[strcspn(problem, "\n")] = 0;
    if (strcmp(problem, "back") == 0)
        return;
    FILE *file = fopen(supportFile, "a");
    if (file) {
        fprintf(file, "%s,Open,%s\n", accountID, problem);
        fclose(file);
        printf("Your support ticket has been created. Admin will review it shortly.\n");
    } else {
        printf("Could not create support ticket. Please try again later.\n");
    }
}

// MENU
void adminMenu(struct Admin *admin) {
    int choice;
    while (1) {
        clearScreen();
        printf("\n======= Admin Menu =======\n");
        printf("1. View Account Details\n");
        printf("2. Delete Account\n");
        printf("3. Block Account\n");
        printf("4. View Transaction History\n");
        printf("5. Review Loan Requests\n");
        printf("6. Generate Account Report\n");
        printf("7. View Bank Summary\n");
        printf("8. Review Support Tickets\n");
        printf("9. Change My Password\n");
        printf("10. Logout\n");
        printf("----------------------------\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        clearScreen();
        switch (choice) {
            case 1: viewAccountDetails();
            break;
            case 2: deleteAccount();
            break;
            case 3: blockAccount();
            break;
            case 4: viewAdminTransactionHistory();
            break;
            case 5: reviewLoanRequests();
            break;
            case 6: generateAccountReport();
            break;
            case 7: viewBankSummary();
            break;
            case 8: reviewSupportTickets();
            break;
            case 9: changeAdminPassword(admin, 0);
            break;
            case 10: {
                char confirm;
                printf("Are you sure you want to logout? (y/n): ");
                scanf(" %c", &confirm);
                while(getchar()!='\n');
                if(confirm == 'y' || confirm == 'Y'){
                    printf("Logging out from Admin panel...\n");
                    return;
                }
                break;
            }
            default: printf("Invalid choice. Please try again.\n");
        }
        pressEnterToContinue();
    }
}

void userMenu(char *accountID) {
    int choice;
    while (1) {
        clearScreen();
        printf("\n======= User Menu (Logged in as %s) =======\n", accountID);
        printf("1. Check Balance\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Transfer (Same Bank)\n");
        printf("5. Bank to Bank Transfer\n");
        printf("6. View Transaction History\n");
        printf("7. Apply for Loan\n");
        printf("8. Check Loan Status\n");
        printf("9. Customer Support\n");
        printf("10. Logout\n");
        printf("--------------------------\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        clearScreen();
        switch (choice) {
            case 1: checkBalance(accountID);
            break;
            case 2: depositMoney(accountID);
            break;
            case 3: withdrawMoney(accountID);
            break;
            case 4: transferMoney(accountID);
            break;
            case 5: bankToBankTransfer(accountID);
            break;
            case 6: viewUserTransactionHistory(accountID);
            break;
            case 7: applyForLoan(accountID);
            break;
            case 8: checkLoanStatus(accountID);
            break;
            case 9: customerSupport(accountID);
            break;
            case 10: {
                char confirm;
                printf("Are you sure you want to logout? (y/n): ");
                scanf(" %c", &confirm);
                while(getchar()!='\n');
                if(confirm == 'y' || confirm == 'Y'){
                    printf("Logging out...\n");
                    return;
                }
                break;
            }
            default: printf("Invalid choice. Please try again.\n");
        }
        pressEnterToContinue();
    }
}

void registerUser() {
    struct Account acc, checkAcc;
    char confirmPass[30];
    while(1) {
        printf("Enter a new Account ID (or type '0' to go back): ");
        fgets(acc.accountID, sizeof(acc.accountID), stdin);
        acc.accountID[strcspn(acc.accountID, "\n")] = 0;

        if (strcmp(acc.accountID, "0") == 0)
            return;
        if (getAccount(acc.accountID, &checkAcc)) {
            printf("This Account ID is already taken. Please try a different one.\n");
        } else {
            break;
        }
    }

    printf("Enter First Name: ");
    fgets(acc.firstName, sizeof(acc.firstName), stdin);
    acc.firstName[strcspn(acc.firstName, "\n")] = 0;

    printf("Enter Last Name: ");
    fgets(acc.lastName, sizeof(acc.lastName), stdin);
    acc.lastName[strcspn(acc.lastName, "\n")] = 0;

    while (1) {
        printf("Enter Email: ");
        fgets(acc.email, sizeof(acc.email), stdin);
        acc.email[strcspn(acc.email, "\n")] = 0;
        if (isValidEmail(acc.email)) {
            break;
        } else {
            printf("Invalid email format. Please include '@' and try again.\n");
        }
    }

    while (1) {
        printf("Enter Mobile Number (e.g., 01712345678): ");
        fgets(acc.mobile, sizeof(acc.mobile), stdin);
        acc.mobile[strcspn(acc.mobile, "\n")] = 0;
        if (isValidMobileNumber(acc.mobile)) {
            break;
        } else {
            printf("Invalid mobile number. It must be 11 digits and start with a valid prefix (017, 019, etc.).\n");
        }
    }

    printf("Enter Address: ");
    fgets(acc.address, sizeof(acc.address), stdin);
    acc.address[strcspn(acc.address, "\n")] = 0;

    printf("Enter Date of Birth (YYYY-MM-DD): ");
    fgets(acc.dob, sizeof(acc.dob), stdin);
    acc.dob[strcspn(acc.dob, "\n")] = 0;

    printf("Enter NID/Passport Number: ");
    fgets(acc.nid, sizeof(acc.nid), stdin);
    acc.nid[strcspn(acc.nid, "\n")] = 0;

    printf("Enter Nominee Name: ");
    fgets(acc.nominee, sizeof(acc.nominee), stdin);
    acc.nominee[strcspn(acc.nominee, "\n")] = 0;

    printf("Enter Nominee's NID: ");
    fgets(acc.nominee_nid, sizeof(acc.nominee_nid), stdin);
    acc.nominee_nid[strcspn(acc.nominee_nid, "\n")] = 0;

    printf("Enter Relationship with Nominee (e.g., Father, Mother): ");
    fgets(acc.nominee_relation, sizeof(acc.nominee_relation), stdin);
    acc.nominee_relation[strcspn(acc.nominee_relation, "\n")] = 0;

    printf("Enter Account Type (Savings/Current): ");
    fgets(acc.accountType, sizeof(acc.accountType), stdin);
    acc.accountType[strcspn(acc.accountType, "\n")] = 0;

    printf("\nPassword must have at least 6 characters, with uppercase, lowercase, and a number/symbol.\n");
    while (1) {
        printf("Create a password: ");
        getMaskedPassword(acc.password);
        if (isPasswordStrong(acc.password)) {
            printf("Password strength: Strong\n");
            printf("Confirm password: ");
            getMaskedPassword(confirmPass);
            if (strcmp(acc.password, confirmPass) == 0)
                break;
            else printf("Passwords do not match. Please try again.\n\n");
        } else {
            printf("Password is not strong. Please try again.\n\n");
        }
    }
    acc.balance = 0;
    FILE *file = fopen(accountFile, "a");
    if (file) {
        writeAccount(file, acc);
        fclose(file);
        printf("User registration successful! Please login.\n");
    } else {
        printf("Error: Could not open accounts file for writing.\n");
    }
}

void adminLogin() {
    struct Admin admin;
    char id[20], password[30];
    FILE *file = fopen(adminFile, "r");
    if (!file) {
        file = fopen(adminFile, "w");
        if (!file) {
            printf("CRITICAL ERROR: Cannot create admin file.\n");
            return;
        }
        fprintf(file, "%s,%s,1\n", DEFAULT_ADMIN_ID, DEFAULT_ADMIN_PASS);
        fclose(file);
        printf("Default admin account has been created.\nPlease select Admin Login again to continue.\n");
        return;
    }

    fscanf(file, "%[^,],%[^,],%d\n", admin.id, admin.password, &admin.isDefaultPassword);
    fclose(file);

    printf("Enter Admin ID (or type '0' to go back): ");
    scanf("%s", id);
    while (getchar() != '\n');
    if(strcmp(id, "0") == 0)
        return;
    printf("Enter Password: ");
    getMaskedPassword(password);

    if (strcmp(id, admin.id) == 0 && strcmp(password, admin.password) == 0) {
        printf("Admin login successful!\n");
        pressEnterToContinue();
        if (admin.isDefaultPassword) {
            clearScreen();
            changeAdminPassword(&admin, 1);
            printf("Please login again with your new password.\n");
        } else {
            adminMenu(&admin);
        }
    } else {
        printf("Invalid admin credentials!\n");
    }
}

void userLogin(char *loggedInAccountID) {
    int choice;
    while(1) {
        clearScreen();
        printf("\n--- User Login Portal ---\n");
        printf("1. Login with credentials\n");
        printf("2. Forgot Password\n");
        printf("3. Back to Main Menu\n");
        printf("--------------------------\n");
        printf("Enter your choice: ");

        if(scanf("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        clearScreen();
        if (choice == 1) {
            char id[20], password[30];
            struct Account acc;

            printf("Enter Account ID: ");
            fgets(id, sizeof(id), stdin);
            id[strcspn(id, "\n")] = 0;

            printf("Enter Password: ");
            getMaskedPassword(password);
            if (getAccount(id, &acc)) {
                if (strcmp(acc.password, "BLOCKED") == 0) {
                    printf("This account is blocked. Please contact admin.\n");
                    pressEnterToContinue();
                    continue;
                }
                if (strcmp(password, acc.password) == 0) {
                    strcpy(loggedInAccountID, id);
                    printf("Login successful!\n");
                    pressEnterToContinue();
                    userMenu(loggedInAccountID);
                    return;
                }
            }
            printf("Invalid Account ID or Password.\n");
            pressEnterToContinue();

        } else if (choice == 2) {
            forgotPassword();
            pressEnterToContinue();
        } else if (choice == 3) {
            return;
        } else {
            printf("Invalid choice. Please try again.\n");
            pressEnterToContinue();
        }
    }
}

void forgotPassword() {
    char id[20], nid[20];
    struct Account acc;
    printf("--- Password Reset ---\n");
    printf("Enter your Account ID (or type '0' to go back): ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = 0;
    if(strcmp(id, "0") == 0)
        return;

    printf("Enter your NID/Passport number to verify: ");
    fgets(nid, sizeof(nid), stdin);
    nid[strcspn(nid, "\n")] = 0;

    if (getAccount(id, &acc) && strcmp(nid, acc.nid) == 0) {
        printf("Account verified successfully.\n");
        char newPass[30], confirmPass[30];

        printf("\nPassword must have at least 6 characters, with uppercase, lowercase, and a number/symbol.\n");
        while (1) {
            printf("Enter new password: ");
            getMaskedPassword(newPass);
            if (isPasswordStrong(newPass)) {
                printf("Confirm new password: ");
                getMaskedPassword(confirmPass);
                if (strcmp(newPass, confirmPass) == 0) {
                    strcpy(acc.password, newPass);
                    updateUserAccount(acc);
                    printf("Password has been reset successfully. Please login.\n");
                    break;
                } else {
                    printf("Passwords do not match. Please try again.\n");
                }
            } else {
                printf("Password is not strong. Please try again.\n");
            }
        }
    } else {
        printf("Account ID or NID is incorrect. Could not verify.\n");
    }
}

// ==================== MAIN FUNCTION (Apon's Part) ====================
int main() {
    int choice;
    char loggedInAccountID[20];
    while (1) {
        clearScreen();
        displayLogo();

        printf("\n===== Welcome to our Banking Management System =====\n");
        printf("\n1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Register New User Account\n");
        printf("4. Exit\n");
        printf("----------------------\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        clearScreen();
        switch (choice) {
            case 1: adminLogin();
            break;
            case 2: userLogin(loggedInAccountID);
            break;
            case 3: registerUser();
            break;
            case 4: printf("Thank you for using our banking system. Goodbye!\n");
            return 0;
            default: printf("Invalid choice. Please try again.\n");
        }
        pressEnterToContinue();
    }
    return 0;
}
