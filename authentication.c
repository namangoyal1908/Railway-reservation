#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "authentication.h"
#include "dashboard.h"

// This global variable stores whoever is logged in right now
struct User currentUser;

// Caesar cipher: shifts every character by 5
void encryptText(char *str) {
    int key = 5;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = str[i] + key;
    }
}

void decryptText(char *str) {
    int key = 5;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = str[i] - key;
    }
}

// Replaces each digit with a symbol so phone number is not stored as plain text
void encryptPhone(char *phone) {
    for (int i = 0; phone[i] != '\0'; i++) {
        if (phone[i] == '0') phone[i] = ')';
        else if (phone[i] == '1') phone[i] = '!';
        else if (phone[i] == '2') phone[i] = '@';
        else if (phone[i] == '3') phone[i] = '#';
        else if (phone[i] == '4') phone[i] = '$';
        else if (phone[i] == '5') phone[i] = '%';
        else if (phone[i] == '6') phone[i] = '^';
        else if (phone[i] == '7') phone[i] = '&';
        else if (phone[i] == '8') phone[i] = '*';
        else if (phone[i] == '9') phone[i] = '(';
    }
}

void log_in() {
    int logChoice;
    struct User input, temp;
    int found = 0;

    printf("\n--- LOGIN MODULE ---\n");
    printf("1. Enter Password\n");
    printf("2. Forgot Password\n");
    printf("Enter choice: ");
    scanf("%d", &logChoice);

    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) {
        printf("\nNo users found. Please sign up first.\n");
        return;
    }

    if (logChoice == 1) {

        printf("Username: ");
        while ((getchar()) != '\n');
        fgets(input.username, 50, stdin);
        input.username[strcspn(input.username, "\n")] = 0;

        printf("Password: ");
        scanf("%s", input.password);

        encryptText(input.username);
        encryptText(input.password);

        while (fscanf(fp, " %[^|]|%[^|]|%[^|]|%s", temp.username, temp.password, temp.phone, temp.security_ans) == 4) {
            if (strcmp(temp.username, input.username) == 0 && strcmp(temp.password, input.password) == 0) {
                found = 1;
                break;
            }
        }

        if (found) {
            currentUser = temp;
            decryptText(temp.username);
            printf("\nAccess Granted! Welcome, %s.\n", temp.username);
            fclose(fp);
            user_dashboard();  // go to dashboard
        } else {
            printf("\nInvalid Username or Password.\n");
            fclose(fp);
        }

    } else if (logChoice == 2) {

        printf("Enter Registered Phone Number: ");
        scanf("%s", input.phone);
        encryptPhone(input.phone);

        while (fscanf(fp, " %[^|]|%[^|]|%[^|]|%s", temp.username, temp.password, temp.phone, temp.security_ans) == 4) {
            if (strcmp(temp.phone, input.phone) == 0) {
                found = 1;
                break;
            }
        }

        if (found) {
            printf("\nSecurity Question: What is your favorite color?\n");
            printf("Your Answer: ");
            while ((getchar()) != '\n');
            fgets(input.security_ans, 50, stdin);
            input.security_ans[strcspn(input.security_ans, "\n")] = 0;

            encryptText(input.security_ans);

            if (strcmp(temp.security_ans, input.security_ans) == 0) {
                decryptText(temp.password);
                printf("\nVerification Successful! Your password is: %s\n", temp.password);
            } else {
                printf("\nWrong Answer! Verification failed.\n");
            }
        } else {
            printf("\nPhone number not found.\n");
        }
        fclose(fp);
    }
}

void sign_up() {
    struct User newUser;
    struct User temp;
    int found = 0;

    printf("\n--- SIGN UP ---\n");

    while ((getchar()) != '\n');

    printf("Enter Full Name: ");
    fgets(newUser.username, 50, stdin);
    newUser.username[strcspn(newUser.username, "\n")] = 0;

    printf("Enter Password (No spaces): ");
    scanf("%s", newUser.password);

    printf("Enter Phone Number: ");
    scanf("%s", newUser.phone);
    encryptPhone(newUser.phone);

    // Check if phone already exists
    FILE *fp = fopen("users.txt", "r");
    if (fp != NULL) {
        while (fscanf(fp, " %[^|]|%[^|]|%[^|]|%s", temp.username, temp.password, temp.phone, temp.security_ans) == 4) {
            if (strcmp(temp.phone, newUser.phone) == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (found == 1) {
        printf("\nERROR: Phone number already exists! Please try to log in.\n");
        return;
    }

    printf("\nPhone verified! Setting up security recovery...\n");
    printf("Security Question: What is your favorite color?\n");
    printf("Answer: ");
    while ((getchar()) != '\n');
    fgets(newUser.security_ans, 50, stdin);
    newUser.security_ans[strcspn(newUser.security_ans, "\n")] = 0;

    // Save display name before encrypting
    char displayName[50];
    strcpy(displayName, newUser.username);

    encryptText(newUser.username);
    encryptText(newUser.password);
    encryptText(newUser.security_ans);

    fp = fopen("users.txt", "a");
    if (fp == NULL) {
        printf("\nFile error!\n");
        return;
    }

    fprintf(fp, "%s|%s|%s|%s\n", newUser.username, newUser.password, newUser.phone, newUser.security_ans);
    fclose(fp);

    printf("\nSuccess: Account created for %s.\n", displayName);
}
