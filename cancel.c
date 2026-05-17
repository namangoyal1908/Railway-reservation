#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "cancel.h"
#include "authentication.h"
#include "pricing.h"

// Ticket file format: PNR|Name|Age|TrainNum|Class|GuardianPhone|Price|TravelDay|TravelMonth

void view_ticket() {
    FILE *fp = fopen("ticket.txt", "r");
    if (fp == NULL) {
        printf("\nNo tickets found.\n");
        return;
    }

    int pnr, age, trainNum, travelDay, travelMonth;
    char pName[50], classCode[10], guardPhone[15];
    float price;
    int found = 0;

    // Decrypt username to compare with ticket names
    char myName[50];
    strcpy(myName, currentUser.username);
    decryptText(myName);

    printf("\n--- MY TICKETS ---\n");
    printf("%-10s | %-20s | %-5s | %-8s | %-6s | %-12s | %-15s\n",
           "PNR", "NAME", "AGE", "TRAIN", "CLASS", "PRICE", "TRAVEL DATE");
    printf("--------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d|%[^|]|%d|%d|%[^|]|%[^|]|%f|%d|%d\n",
                  &pnr, pName, &age, &trainNum, classCode, guardPhone, &price, &travelDay, &travelMonth) == 9) {
        if (strcmp(pName, myName) == 0) {
            printf("%-10d | %-20s | %-5d | %-8d | %-6s | Rs. %-8.2f | %02d/%02d\n",
                   pnr, pName, age, trainNum, classCode, price, travelDay, travelMonth);
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
    int pnr, age, trainNum, travelDay, travelMonth;
    char pName[50], classCode[10], guardPhone[15];
    float price;
    int found = 0;

    // Decrypt username to match with ticket
    char myName[50];
    strcpy(myName, currentUser.username);
    decryptText(myName);

    view_ticket();  // Show tickets first so user can pick a PNR

    // Show refund policy before user decides
    show_refund_policy();

    printf("\nEnter PNR to Cancel (0 to go back): ");
    scanf("%d", &targetPNR);
    if (targetPNR == 0) return;

    // Ask for today's date to calculate days before travel
    int todayDay, todayMonth;
    printf("Enter Today's Date (DD MM): ");
    scanf("%d %d", &todayDay, &todayMonth);

    FILE *fp = fopen("ticket.txt", "r");
    FILE *tp = fopen("temp_ticket.txt", "w");

    if (fp == NULL) {
        printf("\nNo tickets found.\n");
        return;
    }

    while (fscanf(fp, "%d|%[^|]|%d|%d|%[^|]|%[^|]|%f|%d|%d\n",
                  &pnr, pName, &age, &trainNum, classCode, guardPhone, &price, &travelDay, &travelMonth) == 9) {
        if (pnr == targetPNR && strcmp(pName, myName) == 0) {
            found = 1;

            // Calculate how many days before travel the cancellation is happening
            // Simple calculation: works within same month
            int daysBeforeTravel = 0;
            if (travelMonth == todayMonth) {
                daysBeforeTravel = travelDay - todayDay;
            } else if (travelMonth > todayMonth) {
                // Rough estimate for next month
                daysBeforeTravel = (travelMonth - todayMonth) * 30 + (travelDay - todayDay);
            }

            if (daysBeforeTravel < 0) daysBeforeTravel = 0;

            // Calculate refund using the pricing module
            float refundAmount = calculate_refund(price, daysBeforeTravel);

            printf("\n[INFO]: Ticket PNR %d cancelled.\n", pnr);
            printf("[INFO]: Amount Paid    : Rs. %.2f\n", price);
            printf("[INFO]: Refund Amount  : Rs. %.2f\n", refundAmount);
            printf("[INFO]: Deducted       : Rs. %.2f\n", price - refundAmount);

            // Don't write this ticket back (it's deleted)
        } else {
            // Keep all other tickets
            fprintf(tp, "%d|%s|%d|%d|%s|%s|%.2f|%d|%d\n",
                    pnr, pName, age, trainNum, classCode, guardPhone, price, travelDay, travelMonth);
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
