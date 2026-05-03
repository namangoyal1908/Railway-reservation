#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "cancel.h"
#include "authentication.h"

// Ticket file format: PNR|Name|Age|TrainNum|Class|GuardianPhone|Price

void view_ticket() {
    FILE *fp = fopen("ticket.txt", "r");
    if (fp == NULL) {
        printf("\nNo tickets found.\n");
        return;
    }

    int pnr, age, trainNum;
    char pName[50], classCode[10], guardPhone[15];
    float price;
    int found = 0;

    // Decrypt username to compare with ticket names
    char myName[50];
    strcpy(myName, currentUser.username);
    decryptText(myName);

    printf("\n--- MY TICKETS ---\n");
    printf("%-10s | %-20s | %-5s | %-8s | %-6s | %-10s\n", "PNR", "NAME", "AGE", "TRAIN", "CLASS", "PRICE");
    printf("-----------------------------------------------------------------------\n");

    while (fscanf(fp, "%d|%[^|]|%d|%d|%[^|]|%[^|]|%f\n", &pnr, pName, &age, &trainNum, classCode, guardPhone, &price) == 7) {
        if (strcmp(pName, myName) == 0) {
            printf("%-10d | %-20s | %-5d | %-8d | %-6s | Rs. %.2f\n", pnr, pName, age, trainNum, classCode, price);
            found = 1;
        }
    }

    if (!found) {
        printf("No tickets booked under your account.\n");
    }

    fclose(fp);
}

void cancel_ticket() {
    int targetPNR;
    int pnr, age, trainNum;
    char pName[50], classCode[10], guardPhone[15];
    float price;
    int found = 0;

    // Decrypt username to match with ticket
    char myName[50];
    strcpy(myName, currentUser.username);
    decryptText(myName);

    view_ticket();  // Show tickets first so user can pick a PNR

    printf("\nEnter PNR to Cancel (0 to go back): ");
    scanf("%d", &targetPNR);
    if (targetPNR == 0) return;

    FILE *fp = fopen("ticket.txt", "r");
    FILE *tp = fopen("temp_ticket.txt", "w");

    if (fp == NULL) {
        printf("\nNo tickets found.\n");
        return;
    }

    while (fscanf(fp, "%d|%[^|]|%d|%d|%[^|]|%[^|]|%f\n", &pnr, pName, &age, &trainNum, classCode, guardPhone, &price) == 7) {
        if (pnr == targetPNR && strcmp(pName, myName) == 0) {
            // This is the ticket to cancel - skip writing it (deletes it)
            found = 1;
            printf("\n[INFO]: Ticket PNR %d cancelled. Refund: Rs. %.2f (15%% fee deducted)\n", pnr, price * 0.85);
        } else {
            // Keep all other tickets
            fprintf(tp, "%d|%s|%d|%d|%s|%s|%.2f\n", pnr, pName, age, trainNum, classCode, guardPhone, price);
        }
    }

    fclose(fp);
    fclose(tp);
    remove("ticket.txt");
    rename("temp_ticket.txt", "ticket.txt");

    if (!found) {
        printf("\n[ERROR]: PNR %d not found under your account.\n", targetPNR);
    }
}
