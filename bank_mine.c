#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "account.h"

void process_transaction(account* accounts, int num_accounts, char* transaction) {
    // Parse the transaction request
    char* saveptr;
    char* cmd = strtok_r(transaction, " ", &saveptr);

    if (cmd == NULL) {
        printf("Skipping invalid transaction: %s\n", transaction);
        return;
    }

    char* acc_num = strtok_r(NULL, " ", &saveptr);
    char* password = strtok_r(NULL, " ", &saveptr);
    char* amount_str = strtok_r(NULL, " ", &saveptr);
    if (acc_num == NULL || password == NULL || amount_str == NULL) {
        printf("Invalid transaction format\n");
        return;
    }
    double amount = atof(amount_str);

    // Find the account with the given account number
    for (int i = 0; i < num_accounts; i++) {
        if (strcmp(accounts[i].account_number, acc_num) == 0) {
            // Verify the password
            if (strcmp(accounts[i].password, password) == 0) {
                // Process the transaction based on the command
                if (strcmp(cmd, "T") == 0) {
                    // Transfer funds
                    char* dest_acc = strtok_r(NULL, " ", &saveptr);
                    char* transfer_amount_str = strtok_r(NULL, " ", &saveptr);
                    if (dest_acc == NULL || transfer_amount_str == NULL) {
                        printf("Invalid transfer format\n");
                        return;
                    }
                    double transfer_amount = atof(transfer_amount_str);

                    // Find the destination account
                    for (int j = 0; j < num_accounts; j++) {
                        if (strcmp(accounts[j].account_number, dest_acc) == 0) {
                            // Deduct the amount from the source account and add to the destination account
                            pthread_mutex_lock(&accounts[i].ac_lock);
                            accounts[i].balance -= transfer_amount;
                            pthread_mutex_lock(&accounts[j].ac_lock);
                            accounts[j].balance += transfer_amount;
                            pthread_mutex_unlock(&accounts[j].ac_lock);
                            pthread_mutex_unlock(&accounts[i].ac_lock);
                            break;
                        }
                    }
                }
                else if (strcmp(cmd, "D") == 0) {
                    // Deposit
                    pthread_mutex_lock(&accounts[i].ac_lock);
                    accounts[i].balance += amount;
                    pthread_mutex_unlock(&accounts[i].ac_lock);
                }
                else if (strcmp(cmd, "W") == 0) {
                    // Withdraw
                    pthread_mutex_lock(&accounts[i].ac_lock);
                    accounts[i].balance -= amount;
                    pthread_mutex_unlock(&accounts[i].ac_lock);
                }
                else if (strcmp(cmd, "C") == 0) {
                    // Check balance (do nothing)
                }
                // Update the transaction tracker
                accounts[i].transaction_tracker++;
                break;
            }
        }
    }

}

int main() {
    FILE* input_file = fopen("input-1.txt", "r");
    if (input_file == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    // Read the number of accounts
    int num_accounts;
    char line[100];
    if (fgets(line, sizeof(line), input_file) == NULL || sscanf(line, "%d", &num_accounts) != 1) {
        printf("Error reading number of accounts\n");
        fclose(input_file);
        return 1;
    }

    // Allocate memory for the accounts
    account* accounts = malloc(num_accounts * sizeof(account));
    if (accounts == NULL) {
        printf("Memory allocation failed\n");
        fclose(input_file);
        return 1;
    }

    // Read the account information
    for (int i = 0; i < num_accounts; i++) {
        // Read and process each line
        for (int j = 0; j < 5; j++) { // 5 lines per account: index, account number, password, balance, reward rate
            if (fgets(line, sizeof(line), input_file) == NULL) {
                printf("Error reading data for account %d, line %d\n", i, j);
                free(accounts);
                fclose(input_file);
                return 1;
            }

            // Process the line based on the line number
            switch (j) {
            case 1: // Account number
                strncpy(accounts[i].account_number, line, sizeof(accounts[i].account_number));
                accounts[i].account_number[strcspn(accounts[i].account_number, "\n")] = 0; // Remove newline
                break;
            case 2: // Password
                strncpy(accounts[i].password, line, sizeof(accounts[i].password));
                accounts[i].password[strcspn(accounts[i].password, "\n")] = 0; // Remove newline
                break;
            case 3: // Balance
                if (sscanf(line, "%lf", &accounts[i].balance) != 1) {
                    printf("Error reading balance for account %d\n", i);
                    free(accounts);
                    fclose(input_file);
                    return 1;
                }
                break;
            case 4: // Reward rate
                if (sscanf(line, "%lf", &accounts[i].reward_rate) != 1) {
                    printf("Error reading reward rate for account %d\n", i);
                    free(accounts);
                    fclose(input_file);
                    return 1;
                }
                break;
            default:
                break;
            }
        }
        accounts[i].transaction_tracker = 0;
        pthread_mutex_init(&accounts[i].ac_lock, NULL);
    }

    // Read and process the transactions
    while (fgets(line, sizeof(line), input_file) != NULL) {
        process_transaction(accounts, num_accounts, line);
    }

    fclose(input_file);

    // Output the balances
    for (int i = 0; i < num_accounts; i++) {
        printf("%d balance:\t%.2lf\n", i, accounts[i].balance);
    }
    free(accounts);

    return 0;
}
