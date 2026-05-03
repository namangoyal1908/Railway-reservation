#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "booking.h"

// Updates the catalog file (reduces seat count by 1)
// Also marks train as Not_Available if seats reach 0
void update_catalog_and_train(char* catID, char* cls, int tNum, int currentSeats) {

    FILE *f = fopen("catalog.txt", "r");
    FILE *t = fopen("temp.txt", "w");
    char line[150], fCat[10], fCode[10];
    int fTot, fWin, fAis;
    float fPrice;

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%[^|]|%[^|]|%d|%d|%d|%f", fCat, fCode, &fTot, &fWin, &fAis, &fPrice) == 6) {
            if (strcmp(fCat, catID) == 0 && strcasecmp(fCode, cls) == 0)
                fprintf(t, "%s|%s|%d|%d|%d|%.2f\n", fCat, fCode, fTot - 1, fWin, fAis, fPrice);
            else
                fprintf(t, "%s", line);
        } else {
            fprintf(t, "%s", line);
        }
    }
    fclose(f);
    fclose(t);
    remove("catalog.txt");
    rename("temp.txt", "catalog.txt");

    // If seats are now 0 or less, mark the train as not available
    if (currentSeats - 1 <= 0) {
        FILE *f2 = fopen("trains.txt", "r");
        FILE *t2 = fopen("temp2.txt", "w");
        struct Train tempT;
        while (fscanf(f2, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &tempT.trainNum, tempT.name, tempT.start, tempT.end, tempT.stops, tempT.status) == 6) {
            if (tempT.trainNum == tNum)
                fprintf(t2, "%d|%s|%s|%s|%s|Not_Available\n", tempT.trainNum, tempT.name, tempT.start, tempT.end, tempT.stops);
            else
                fprintf(t2, "%d|%s|%s|%s|%s|%s\n", tempT.trainNum, tempT.name, tempT.start, tempT.end, tempT.stops, tempT.status);
        }
        fclose(f2);
        fclose(t2);
        remove("trains.txt");
        rename("temp2.txt", "trains.txt");
    }
}

void book_ticket(int trainNum) {
    char pName[50], classChoice[10], guardianPhone[15], trainName[50] = "", catID[10] = "";
    int age, pnr, foundTrain = 0, foundClass = 0;
    int fTot, fWin, fAis;
    float basePrice, finalPrice;
    FILE *fp, *cp, *tp;
    struct Train t;

    // Step 1: Check if train exists and is available
    fp = fopen("trains.txt", "r");
    if (fp == NULL) return;
    while (fscanf(fp, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%s\n", &t.trainNum, t.name, t.start, t.end, t.stops, t.status) == 6) {
        if (t.trainNum == trainNum) {
            strcpy(trainName, t.name);
            foundTrain = 1;
            break;
        }
    }
    fclose(fp);

    if (!foundTrain || strcmp(t.status, "Available") != 0) {
        printf("\n[ALERT]: Train #%d is not available for booking.\n", trainNum);
        return;
    }

    // Step 2: Figure out which category this train belongs to
    if (trainNum == 12423 || trainNum == 12301) strcpy(catID, "CAT01");
    else if (trainNum == 12004 || trainNum == 22436) strcpy(catID, "CAT02");
    else strcpy(catID, "CAT03");

    printf("\n--- BOOKING SYSTEM: %s ---\n", trainName);
    printf("Enter Class (AC1/CC/SLP): ");
    scanf("%s", classChoice);

    // Step 3: Find the class in catalog and get its price and seat count
    cp = fopen("catalog.txt", "r");
    char line[150], fCat[10], fCode[10];
    while (fgets(line, sizeof(line), cp)) {
        if (sscanf(line, "%[^|]|%[^|]|%d|%d|%d|%f", fCat, fCode, &fTot, &fWin, &fAis, &basePrice) == 6) {
            if (strcmp(fCat, catID) == 0 && strcasecmp(fCode, classChoice) == 0) {
                foundClass = 1;
                break;
            }
        }
    }
    fclose(cp);

    if (!foundClass || fTot <= 0) {
        printf("[ERROR]: Seats full in %s.\n", classChoice);
        return;
    }

    // Step 4: Dynamic pricing based on how full the train is
    finalPrice = basePrice;
    if (fTot <= 10) {
        finalPrice = basePrice * 1.15;
        printf("[SYSTEM]: 80%% Occupancy reached. Price: Rs. %.2f (+15%%)\n", finalPrice);
    } else if (fTot <= 25) {
        finalPrice = basePrice * 1.10;
        printf("[SYSTEM]: 50%% Occupancy reached. Price: Rs. %.2f (+10%%)\n", finalPrice);
    }

    // Step 5: Get passenger details
    printf("Passenger Name: ");
    while(getchar() != '\n');
    fgets(pName, 50, stdin);
    pName[strcspn(pName, "\n")] = 0;

    printf("Age: ");
    scanf("%d", &age);
    if (age > 60) printf("[NOTE]: Senior Citizen Quota - Lower Deck assigned.\n");

    printf("Guardian Phone Number: ");
    scanf("%s", guardianPhone);

    // Step 6: Generate PNR and save ticket to file
    pnr = 100000 + (rand() % 900000);
    tp = fopen("ticket.txt", "a");
    fprintf(tp, "%d|%s|%d|%d|%s|%s|%.2f\n", pnr, pName, age, trainNum, classChoice, guardianPhone, finalPrice);
    fclose(tp);

    // Step 7: Update catalog and train availability
    update_catalog_and_train(catID, classChoice, trainNum, fTot);

    printf("\n========================================");
    printf("\n   BOOKING CONFIRMED! PNR: %d", pnr);
    printf("\n========================================\n");
}
