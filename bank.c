#include <stdio.h>
#include <stdlib.h>
#i`nclude "account.h"
#include <pthread.h>
#include <string.h>

// Function Prototypes
void readAccounts(const char* filename, account** accounts, int* numAccounts);
void processTransactions(const char* filename);
void writeOutput(const char* filename);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <account_file> <transaction_file>\n", argv[0]);
        return 1;
    }
    account *accounts;
    int numAccounts;
    readAccounts(argv[1], &accounts, &numAccounts);
    processTransactions(argv[2]);
    writeOutput("final_output.txt");

    return 0;
}

void readAccounts(const char* filename, account** accounts, int* numAccounts) {
    FILE *file = fopen(filename, "r");
    char line[BUFSIZ];

    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    if (fgets(line, BUFSIZ, file) == NULL) {
        perror("Error reading file");
        fclose(file);
        exit(1);
    }

    *numAccounts = atoi(line);
    *accounts = malloc(sizeof(account) * (*numAccounts));

    for (int i = 0; i < *numAccounts; i++) {
        account *currentAccount = &((*accounts)[i]);

        // Initialize the mutex for each account
        pthread_mutex_init(&(currentAccount->ac_lock), NULL);

        // Read index (not used in this version)
        if (fgets(line, BUFSIZ, file) == NULL) break;

        // Read account number
        if (fgets(currentAccount->account_number, 17, file) == NULL) break;
        currentAccount->account_number[strcspn(currentAccount->account_number, "\n")] = 0;

        // Read password
        if (fgets(currentAccount->password, 9, file) == NULL) break;
        currentAccount->password[strcspn(currentAccount->password, "\n")] = 0;

        // Read balance
        if (fgets(line, BUFSIZ, file) == NULL) break;
        currentAccount->balance = atof(line);

        // Read reward rate
        if (fgets(line, BUFSIZ, file) == NULL) break;
        currentAccount->reward_rate = atof(line);

        // Set initial transaction tracker to 0
        currentAccount->transaction_tracter = 0;
	 printf("Account #%d: Number: %s, Password: %s, Balance: %.2f, Reward Rate: %.2f\n",
               i + 1,
               currentAccount->account_number,
               currentAccount->password,
               currentAccount->balance,
               currentAccount->reward_rate);
    }

    fclose(file);
}

/*
 typedef struct {
    char type;          // 'T', 'C', 'D', 'W'
    char sourceAccount[17];
    char password[9];
    char destAccount[17]; // Used only for 'T'
    double amount;        // Used for 'T', 'D', 'W'
} transaction;

void processTransactions(FILE* file, account* accounts, int numAccounts) {
    char line[BUFSIZ];
    transaction trans;

    while (fgets(line, BUFSIZ, file) != NULL) {
        sscanf(line, "%c %s %s %s %lf", &trans.type, trans.sourceAccount, trans.password, trans.destAccount, &trans.amount);

        switch (trans.type) {
            case 'T':
                // Handle Transfer
                printf("Transfer: Source: %s, Dest: %s, Amount: %.2f\n", trans.sourceAccount, trans.destAccount, trans.amount);
                // Add logic to transfer funds
                break;
            case 'C':
                // Handle Check balance
                printf("Check Balance: Account: %s\n", trans.sourceAccount);
                // Add logic to check balance
                break;
            case 'D':
                // Handle Deposit
                printf("Deposit: Account: %s, Amount: %.2f\n", trans.sourceAccount, trans.amount);
                // Add logic to deposit
                break;
            case 'W':
                // Handle Withdraw
                printf("Withdraw: Account: %s, Amount: %.2f\n", trans.sourceAccount, trans.amount);
                // Add logic to withdraw
                break;
            default:
                printf("Unknown transaction type\n");
                break;
        }
    }
}
*/

void processTransactions(const char* filename) {
    // TODO: Implement this function
    // Open the transaction file
    // For each transaction, update the account details accordingly
    // Update transaction_tracter for each account involved
}

void writeOutput(const char* filename) {
    // TODO: Implement this function
    // Open the output file
    // Write the final state of each account to the file
}

