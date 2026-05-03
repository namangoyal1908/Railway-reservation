#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "dashboard.h"
#include "authentication.h"
#include "trains.h"
#include "booking.h"
#include "cancel.h"

void change_password() {
    char oldPass[50], newPass[50], confirmPass[50];
    char encOldPass[50];
    struct User temp;
    int found = 0;

    printf("\n--- CHANGE PASSWORD ---\n");
    printf("Enter Current Password: ");
    while(getchar() != '\n');
    fgets(oldPass, 50, stdin);
    oldPass[strcspn(oldPass, "\n")] = 0;

    // Encrypt old password to compare with what is stored
    strcpy(encOldPass, oldPass);
    encryptText(encOldPass);

    if (strcmp(encOldPass, currentUser.password) != 0) {
        printf("[ERROR]: Current password is wrong.\n");
        return;
    }

    printf("Enter New Password: ");
    scanf("%s", newPass);
    printf("Confirm New Password: ");
    scanf("%s", confirmPass);

    if (strcmp(newPass, confirmPass) != 0) {
        printf("[ERROR]: Passwords do not match.\n");
        return;
    }

    // Rewrite the users file with the updated password
    FILE *fp = fopen("users.txt", "r");
    FILE *tp = fopen("temp_users.txt", "w");

    if (fp == NULL || tp == NULL) {
        printf("[ERROR]: File error.\n");
        return;
    }

    while (fscanf(fp, " %[^|]|%[^|]|%[^|]|%s", temp.username, temp.password, temp.phone, temp.security_ans) == 4) {
        if (strcmp(temp.username, currentUser.username) == 0) {
            // Encrypt the new password before saving
            char encNew[50];
            strcpy(encNew, newPass);
            encryptText(encNew);
            fprintf(tp, "%s|%s|%s|%s\n", temp.username, encNew, temp.phone, temp.security_ans);

            // Also update in memory
            strcpy(currentUser.password, encNew);
            found = 1;
        } else {
            fprintf(tp, "%s|%s|%s|%s\n", temp.username, temp.password, temp.phone, temp.security_ans);
        }
    }

    fclose(fp);
    fclose(tp);
    remove("users.txt");
    rename("temp_users.txt", "users.txt");

    if (found) {
        printf("\n[SUCCESS]: Password changed successfully!\n");
    }
}

void user_dashboard() {
    int choice;

    // Decrypt just for display
    char displayName[50];
    strcpy(displayName, currentUser.username);
    decryptText(displayName);

    while (1) {
        printf("\n--- DASHBOARD: Welcome %s ---\n", displayName);
        printf("1. View Trains\n");
        printf("2. Book Ticket\n");
        printf("3. View My Tickets\n");
        printf("4. Cancel Ticket\n");
        printf("5. Change Password\n");
        printf("6. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                view_trains();
                break;
            case 2: {
                int tNum;
                printf("Enter Train Number to Book: ");
                scanf("%d", &tNum);
                book_ticket(tNum);
                break;
            }
            case 3:
                view_ticket();
                break;
            case 4:
                cancel_ticket();
                break;
            case 5:
                change_password();
                break;
            case 6:
                printf("\nLogged out. thankyou for choosing us %s!\n", displayName);
                return;
            default:
                printf("\nInvalid choice!\n");
        }
    }
}
