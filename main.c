#include <stdio.h>
#include <stdlib.h>
#include "authentication.h"

// This is the first screen the user sees
void user_menu() {
    int choice;
    while (1) {
        printf("\nWELCOME TO RAILWAY RESERVATION SYSTEM\n");
        printf("1. LOG IN\n");
        printf("2. SIGN UP\n");
        printf("3. EXIT\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                log_in();
                break;
            case 2:
                sign_up();
                break;
            case 3:
                exit(0);
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
}

int main() {
    user_menu();
    return 0;
}
