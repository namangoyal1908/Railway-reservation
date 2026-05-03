#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "trains.h"
#include "booking.h"

// Shows the seat/price catalog for a given train
// Also gives option to book from here
void view_catalog(int trainNum, char* trainName) {
    int subChoice;
    char catID[10];
    FILE *cp;
    char line[150];

    printf("\n************************************************");
    printf("\n   MENU FOR: %-20s", trainName);
    printf("\n************************************************");
    printf("\n1. View Full Seat & Price Catalog (Tabular)");
    printf("\n2. Exit to Main Search");
    printf("\n************************************************");
    printf("\nEnter your choice: ");
    scanf("%d", &subChoice);
    while(getchar() != '\n');

    if (subChoice == 2) return;

    if (subChoice == 1) {
        // Decide which catalog category this train belongs to
        if (trainNum == 12423 || trainNum == 12301) strcpy(catID, "CAT01");
        else if (trainNum == 12004 || trainNum == 22436) strcpy(catID, "CAT02");
        else strcpy(catID, "CAT03");

        cp = fopen("catalog.txt", "r");
        if (cp == NULL) {
            printf("\n[ERROR]: catalog.txt missing.\n");
            return;
        }

        printf("\n%-10s | %-6s | %-6s | %-6s | %-10s\n", "CLASS", "TOTAL", "WINDOW", "AISLE", "PRICE");
        printf("------------------------------------------------------------\n");

        char fCat[10], fCode[10], fExtra[50];
        int fTot, fWin, fAis;
        float fPrice;

        while (fgets(line, sizeof(line), cp)) {
            if (line[0] == '#' || line[0] == '\n') continue;

            if (sscanf(line, "%[^|]|%[^|]|%d|%d|%d|%f", fCat, fCode, &fTot, &fWin, &fAis, &fPrice) == 6) {
                if (strcmp(fCat, catID) == 0) {
                    printf("%-10s | %-6d | %-6d | %-6d | Rs. %-8.2f\n", fCode, fTot, fWin, fAis, fPrice);
                }
            } else if (sscanf(line, "%[^|]|LOW|%d|%[^|\n]", fCat, &fTot, fExtra) == 3) {
                if (strcmp(fCat, catID) == 0) {
                    printf("------------------------------------------------------------\n");
                    printf("INFO: %d Lower Berths Reserved for %s\n", fTot, fExtra);
                }
            }
        }
        fclose(cp);

        printf("============================================================\n");
        printf("** SYSTEM NOTE: **\n");
        printf("1. The rates displayed above are BASE PRICES.\n");
        printf("2. A 6%% 'Convenience Surcharge' is applicable for Window Seats.\n");
        printf("3. Handicap Reserved Quota is EXEMPTED from all extra charges.\n");
        printf("============================================================\n");

        int action;
        printf("\n>>> ACTION MENU <<<\n");
        printf("1. Proceed to Book Ticket\n");
        printf("2. Back to User Dashboard\n");
        printf("Select: ");
        scanf("%d", &action);
        while(getchar() != '\n');

        if (action == 1) {
            book_ticket(trainNum);
        } else {
            printf("\nReturning to Dashboard...\n");
        }
    }
}

void view_trains() {
    struct Train t;
    int choice, searchNum, found = 0;
    char searchName[50], sSource[30], sDest[30];
    FILE *fp;

    printf("\n--- RAILWAY SMART ENQUIRY ---\n");
    printf("1. Search by Train Number\n");
    printf("2. Search by Train Name\n");
    printf("3. Find Trains by Route (Checks Start, End, & Stops)\n");
    printf("4. View All Trains\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    while(getchar() != '\n');

    fp = fopen("trains.txt", "r");
    if (fp == NULL) {
        printf("\n[ERROR]: Could not open train database.\n");
        return;
    }

    switch (choice) {
        case 1:
            printf("Enter Train Number: ");
            scanf("%d", &searchNum);
            while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &t.trainNum, t.name, t.start, t.end, t.stops, t.status) == 6) {
                if (t.trainNum == searchNum) {
                    printf("\n%-8s | %-20s | %-12s | %-12s | %-15s\n", "NUMBER", "NAME", "FROM", "TO", "STATUS");
                    printf("--------------------------------------------------------------------------------\n");
                    printf("%-8d | %-20s | %-12s | %-12s | %-15s\n", t.trainNum, t.name, t.start, t.end, t.status);
                    found = 1;
                    if (strcmp(t.status, "Available") == 0) view_catalog(t.trainNum, t.name);
                    break;
                }
            }
            break;

        case 2:
            printf("Enter Train Name: ");
            fgets(searchName, 50, stdin);
            searchName[strcspn(searchName, "\n")] = 0;
            while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &t.trainNum, t.name, t.start, t.end, t.stops, t.status) == 6) {
                if (strcasecmp(t.name, searchName) == 0) {
                    printf("\n%-8s | %-20s | %-12s | %-12s | %-15s\n", "NUMBER", "NAME", "FROM", "TO", "STATUS");
                    printf("--------------------------------------------------------------------------------\n");
                    printf("%-8d | %-20s | %-12s | %-12s | %-15s\n", t.trainNum, t.name, t.start, t.end, t.status);
                    found = 1;
                    if (strcmp(t.status, "Available") == 0) view_catalog(t.trainNum, t.name);
                }
            }
            break;

        case 3:
            printf("Enter Source: ");
            fgets(sSource, 30, stdin);
            sSource[strcspn(sSource, "\n")] = 0;
            printf("Enter Destination: ");
            fgets(sDest, 30, stdin);
            sDest[strcspn(sDest, "\n")] = 0;

            while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &t.trainNum, t.name, t.start, t.end, t.stops, t.status) == 6) {
                if ((strcasecmp(t.start, sSource) == 0 || strcasestr(t.stops, sSource)) &&
                    (strcasecmp(t.end, sDest) == 0 || strcasestr(t.stops, sDest))) {
                    printf("\n%-8s | %-20s | %-12s | %-12s | %-15s\n", "NUMBER", "NAME", "FROM", "TO", "STATUS");
                    printf("--------------------------------------------------------------------------------\n");
                    printf("%-8d | %-20s | %-12s | %-12s | %-15s\n", t.trainNum, t.name, t.start, t.end, t.status);
                    found = 1;
                    if (strcmp(t.status, "Available") == 0) view_catalog(t.trainNum, t.name);
                }
            }
            break;

        case 4:
            printf("\n%-8s | %-20s | %-12s | %-12s | %-15s\n", "NUMBER", "NAME", "FROM", "TO", "STATUS");
            printf("--------------------------------------------------------------------------------\n");
            while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &t.trainNum, t.name, t.start, t.end, t.stops, t.status) == 6) {
                printf("%-8d | %-20s | %-12s | %-12s | %-15s\n", t.trainNum, t.name, t.start, t.end, t.status);
                found = 1;
            }
            break;

        default:
            printf("\nInvalid choice.\n");
            break;
    }

    if (!found && (choice >= 1 && choice <= 4)) {
        printf("\nNo matching trains found for your criteria.\n");
    }

    fclose(fp);
}
