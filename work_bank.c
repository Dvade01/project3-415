#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_ACCOUNTS 100

typedef struct {
    int num_token;
    char** command_list;
} command_line;

typedef struct {
    char account_number[17];
    char password[9];
    double balance;
    double reward_rate;
    double transaction_tracker;
} account;

// Function to tokenize a string based on a delimiter
command_line str_filler(const char* str, const char* delimiter) {
    command_line tokens;
    tokens.num_token = 0;
    tokens.command_list = NULL;

    // Create a non-const copy of the input string
    char* str_copy = strdup(str);

    char* token = strtok(str_copy, delimiter);
    while (token != NULL) {
        tokens.command_list = realloc(tokens.command_list, (tokens.num_token + 1) * sizeof(char*));
        tokens.command_list[tokens.num_token] = strdup(token);
        tokens.num_token++;

        token = strtok(NULL, delimiter);
    }

    free(str_copy);  // Free the copied string

    return tokens;
}

// Function to free memory allocated for command_line structure
void free_command_line(command_line* tokens) {
    for (int i = 0; i < tokens->num_token; i++) {
        free(tokens->command_list[i]);
    }
    free(tokens->command_list);
    tokens->num_token = 0;
}

int num_accounts;

// Function to find an account by its account number
account* find_account_by_number(account* accounts, const char* account_number)
{
    for (int i = 0; i < num_accounts; i++)
    {
        if (strcmp(accounts[i].account_number, account_number) == 0)
        {
            return &accounts[i]; // Return a pointer to the found account
        }
    }

    return NULL; // Account not found
}

// Function to process a transaction for a single account
void process_transaction(account* accounts, account* acc, command_line transaction_tokens) {
    char action;
    char src_account[17];
    char password[9];
    char dest_account[17];
    double transfer_amount;

    action = transaction_tokens.command_list[0][0];
    strcpy(src_account, transaction_tokens.command_list[1]);
    strcpy(password, transaction_tokens.command_list[2]);

    // Validate password
    if (strcmp(password, acc->password) != 0)
    {
        // Invalid password, handle accordingly
        return;
    }

    // Implement logic for processing different transaction types
    switch (action)
    {
    case 'T':
        // Transfer funds
        // Find the destination account
        strcpy(dest_account, transaction_tokens.command_list[3]);
        account* dest_acc = find_account_by_number(accounts, dest_account);

        // Validate destination account
        if (dest_acc == NULL)
        {
            // Handle invalid destination account
            break;
        }

        // Get transfer amount
        transfer_amount = atof(transaction_tokens.command_list[4]);

        // Withdraw from source account
        acc->balance -= transfer_amount;
        acc->transaction_tracker += transfer_amount;  // Increment the transaction tracker

        // Deposit to destination account
        dest_acc->balance += transfer_amount;
        break;

    case 'C':
        // Check balance
        // Print or store the account balance
        //printf("%s balance: %.2f\n", acc->account_number, acc->balance);
        break;

    case 'D':
        // Get transfer amount
        transfer_amount = atof(transaction_tokens.command_list[3]);

        // Deposit
        acc->balance += transfer_amount;
        acc->transaction_tracker += transfer_amount;  // Increment the transaction tracker
        break;

    case 'W':

        // Get transfer amount
        transfer_amount = atof(transaction_tokens.command_list[3]);

        // Withdraw
        acc->balance -= transfer_amount;
        acc->transaction_tracker += transfer_amount;  // Increment the transaction tracker
        break;

    default:
        // Handle invalid transaction type
        break;
    }
}

// Function to update the balance for all accounts
void update_balance(account* accounts)
{
    // Update balances based on reward rate and transaction tracker
    for (int i = 0; i < num_accounts; i++)
    {
        // Calculate the additional balance based on reward rate and transaction tracker
        double additional_balance = accounts[i].reward_rate * accounts[i].transaction_tracker;

        // Update the account balance
        accounts[i].balance += additional_balance;

        // Reset the transaction tracker for the next round
        accounts[i].transaction_tracker = 0.0;
    }
}

int main(int argc, char* argv[])
{
    // Check command line arguments and open the input file
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        exit(1);
    }

    FILE* input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Read the total number of accounts
    char line[MAX_LINE];
    fgets(line, MAX_LINE, input_file);

    // Initialize account array
    num_accounts = atoi(line);
    account accounts[num_accounts];

    // Get and set account information
    for (int i = 0; i < num_accounts; i++)
    {
        fgets(line, MAX_LINE, input_file);

        // Use string parser to tokenize the account information
        command_line index_line = str_filler(line, " ");

        // Check if current line tells us which account index it is (if it is an index)
        if (index_line.num_token < 2 || strcmp(index_line.command_list[0], "index") != 0)
        {
            // Free memory allocated for index_line
            free_command_line(&index_line);
            fclose(input_file);
            return EXIT_FAILURE;
        }

        // Number of given accounts exceeds number of accounts given
        if (atoi(index_line.command_list[1]) > num_accounts)
        {
            // Free memory allocated for index_line
            free_command_line(&index_line);
            break;
        }

        // Free memory allocated for index_line
        free_command_line(&index_line);

        // Duplicate the account number line into the account info string
        fgets(line, MAX_LINE, input_file);
        char* account_info = strdup(line);

        // Concatenate the rest of the info for the account to be created
        for (int j = 1; j < 4; j++)
        {
            fgets(line, MAX_LINE, input_file);

            // Ensure that account_info has enough space for the new line
            size_t current_length = strlen(account_info);
            size_t new_line_length = strlen(line);

            // Resize the account_info buffer
            account_info = (char*)realloc(account_info, (current_length + new_line_length + 1) * sizeof(char));

            // Check if realloc succeeded
            if (account_info == NULL) {
                // Handle memory allocation failure
                fclose(input_file);
                free(account_info);
                return EXIT_FAILURE;
            }

            // Concatenate the new line to account_info
            strcat(account_info, line);
        }

        // Tokenize account info
        command_line account_info_tokens = str_filler(account_info, "\n");
        free(account_info);

        // Validate the number of account_info_tokens
        if (account_info_tokens.num_token < 4)
        {
            // Handle invalid account information
            fclose(input_file);
            // Free memory allocated for account_info_tokens
            free_command_line(&account_info_tokens);
            return EXIT_FAILURE;
        }


        // Parse account info into account struct
        strcpy(accounts[i].account_number, account_info_tokens.command_list[0]);
        strcpy(accounts[i].password, account_info_tokens.command_list[1]);
        accounts[i].balance = atof(account_info_tokens.command_list[2]);
        accounts[i].reward_rate = atof(account_info_tokens.command_list[3]);
        accounts[i].transaction_tracker = 0.0;

        // Free memory allocated for account_info_tokens
        free_command_line(&account_info_tokens);
    }

    // Read and process transactions for each account
    char transaction[MAX_ACCOUNTS];

    while (fgets(transaction, MAX_ACCOUNTS, input_file) != NULL)
    {
        // Use string parser to tokenize the transaction
        command_line transaction_tokens = str_filler(transaction, " ");

        // Validate the number of transaction_tokens
        if (transaction_tokens.num_token < 2)
        {
            // Handle invalid transaction
            free_command_line(&transaction_tokens);
            continue;
        }

        // Process each tokenized transaction
        char* account_number = strdup(transaction_tokens.command_list[1]);
        account* active_account = find_account_by_number(accounts, account_number);
        process_transaction(accounts, active_account, transaction_tokens);

        fflush(stdout);

        // Free memory allocated for transaction_tokens
        free_command_line(&transaction_tokens);
    }

    // Close the input file
    fclose(input_file);

    // Update balances for all accounts
    update_balance(accounts);

    // Print the final balances to the output file
    // Print the final balances to the output file
    FILE* output_file = fopen("output-2.txt", "w");
    if (output_file == NULL)
    {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_accounts; i++)
    {
        fprintf(output_file, "%d balance:\t%.2f\n", i, accounts[i].balance);
    }

    // Close the output file
    fclose(output_file);


    return 0;
}
