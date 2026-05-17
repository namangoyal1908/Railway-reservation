#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "pricing.h"

// ================================================================
//  FESTIVAL DATES TABLE
//  Any booking made ON these dates gets +20% festival surcharge
// ================================================================

struct FestivalDate {
    int day;
    int month;
    char name[30];
};

struct FestivalDate festivals[] = {
    {1,  1,  "New Year"},
    {14, 1,  "Makar Sankranti"},
    {26, 1,  "Republic Day"},
    {10, 3,  "Holi"},          // approximate, changes yearly
    {14, 4,  "Baisakhi"},
    {15, 8,  "Independence Day"},
    {2,  10, "Gandhi Jayanti"},
    {2,  11, "Diwali"},        // approximate, changes yearly
    {3,  11, "Diwali"},        // keeping 2 days for Diwali
    {25, 12, "Christmas"},
    {31, 12, "New Year Eve"},
    {5,  11, "Bhai Dooj"},
    {27, 10, "Navratri"},
    {15, 11, "Chhath Puja"},
    {7,  4,  "Ram Navami"},
};

// Total number of festival dates in the table above
int totalFestivals = 15;

// ================================================================
//  HELPER: CHECK IF DATE IS A FESTIVAL
// ================================================================

int is_festival_date(int day, int month) {
    for (int i = 0; i < totalFestivals; i++) {
        if (festivals[i].day == day && festivals[i].month == month) {
            printf("[FESTIVAL]: %s surge pricing applied (+20%%)\n", festivals[i].name);
            return 1;
        }
    }
    return 0;
}

// ================================================================
//  HELPER: COUNT HOW MANY TICKETS USER HAS BOOKED BEFORE
//  Used to give loyalty discount
// ================================================================

int count_user_tickets(char *username) {
    FILE *fp = fopen("ticket.txt", "r");
    if (fp == NULL) return 0;

    int pnr, age, trainNum, count = 0;
    char pName[50], classCode[10], guardPhone[15];
    float price;

    while (fscanf(fp, "%d|%[^|]|%d|%d|%[^|]|%[^|]|%f\n",
                  &pnr, pName, &age, &trainNum, classCode, guardPhone, &price) == 7) {
        if (strcmp(pName, username) == 0) {
            count++;
        }
    }
    fclose(fp);
    return count;
}

// ================================================================
//  MAIN PRICING FUNCTION
//  Takes all inputs, applies every rule, fills the breakdown struct
// ================================================================

float calculate_price(float basePrice,
                      int seatsLeft,
                      int travelDay, int travelMonth,
                      int bookingDay, int bookingMonth,
                      char seatType,
                      int age,
                      char *username,
                      struct PriceBreakdown *bd) {

    // Start fresh
    bd->basePrice          = basePrice;
    bd->occupancySurcharge = 0;
    bd->tatkalSurcharge    = 0;
    bd->festivalSurcharge  = 0;
    bd->windowSurcharge    = 0;
    bd->seniorDiscount     = 0;
    bd->childDiscount      = 0;
    bd->loyaltyDiscount    = 0;

    float price = basePrice;

    // ── RULE 1: Occupancy based surge ──────────────────────────
    // Less seats remaining = higher price
    if (seatsLeft <= 10) {
        bd->occupancySurcharge = basePrice * 0.15;
        printf("[PRICING]: Very few seats left! +15%% surge applied.\n");
    } else if (seatsLeft <= 25) {
        bd->occupancySurcharge = basePrice * 0.10;
        printf("[PRICING]: Filling fast! +10%% surge applied.\n");
    }
    price += bd->occupancySurcharge;

    // ── RULE 2: Tatkal surcharge ───────────────────────────────
    // If travel date is within 2 days of booking date = Tatkal
    // Simple check: same month and day difference <= 2
    int dayDiff = travelDay - bookingDay;
    if (travelMonth == bookingMonth && dayDiff >= 0 && dayDiff <= 2) {
        bd->tatkalSurcharge = basePrice * 0.30;
        printf("[PRICING]: Tatkal booking (+30%% Tatkal surcharge applied).\n");
    } else if (travelMonth > bookingMonth) {
        // travel is next month, no tatkal
        bd->tatkalSurcharge = 0;
    } else if (travelMonth == bookingMonth && dayDiff < 0) {
        // travel day already passed - should not happen but just in case
        bd->tatkalSurcharge = 0;
    }
    price += bd->tatkalSurcharge;

    // ── RULE 3: Festival surcharge ─────────────────────────────
    if (is_festival_date(travelDay, travelMonth)) {
        bd->festivalSurcharge = basePrice * 0.20;
    }
    price += bd->festivalSurcharge;

    // ── RULE 4: Window seat surcharge ─────────────────────────
    if (seatType == 'W' || seatType == 'w') {
        bd->windowSurcharge = basePrice * 0.06;
        printf("[PRICING]: Window seat selected (+6%% surcharge).\n");
    }
    price += bd->windowSurcharge;

    // ── RULE 5: Senior citizen discount ───────────────────────
    // Age above 60 gets 10% off
    if (age > 60) {
        bd->seniorDiscount = basePrice * 0.10;
        printf("[PRICING]: Senior citizen discount applied (-10%%).\n");
    }
    price -= bd->seniorDiscount;

    // ── RULE 6: Child discount ────────────────────────────────
    // Age below 5 gets 50% off (no separate seat)
    if (age < 5) {
        bd->childDiscount = basePrice * 0.50;
        printf("[PRICING]: Child below 5 years discount applied (-50%%).\n");
    }
    price -= bd->childDiscount;

    // ── RULE 7: Loyalty discount ──────────────────────────────
    // If user has booked 3 or more tickets before, give 10% off
    int pastTickets = count_user_tickets(username);
    if (pastTickets >= 3) {
        bd->loyaltyDiscount = basePrice * 0.10;
        printf("[PRICING]: Loyalty discount! You have %d past bookings (-10%%).\n", pastTickets);
    } else if (pastTickets > 0) {
        printf("[INFO]: Book %d more ticket(s) to unlock loyalty discount!\n", 3 - pastTickets);
    }
    price -= bd->loyaltyDiscount;

    // Make sure price never goes below 0
    if (price < 0) price = 0;

    bd->finalPrice = price;
    return price;
}

// ================================================================
//  SHOW FULL PRICE BREAKDOWN (like a bill receipt)
// ================================================================

void show_price_breakdown(struct PriceBreakdown *bd) {
    printf("\n--------------------------------------------\n");
    printf("         PRICE BREAKDOWN\n");
    printf("--------------------------------------------\n");
    printf("  Base Price              : Rs. %8.2f\n", bd->basePrice);

    if (bd->occupancySurcharge > 0)
        printf("  Occupancy Surge (+)     : Rs. %8.2f\n", bd->occupancySurcharge);
    if (bd->tatkalSurcharge > 0)
        printf("  Tatkal Surcharge (+)    : Rs. %8.2f\n", bd->tatkalSurcharge);
    if (bd->festivalSurcharge > 0)
        printf("  Festival Surge (+)      : Rs. %8.2f\n", bd->festivalSurcharge);
    if (bd->windowSurcharge > 0)
        printf("  Window Seat (+)         : Rs. %8.2f\n", bd->windowSurcharge);
    if (bd->seniorDiscount > 0)
        printf("  Senior Citizen Disc (-) : Rs. %8.2f\n", bd->seniorDiscount);
    if (bd->childDiscount > 0)
        printf("  Child Discount (-)      : Rs. %8.2f\n", bd->childDiscount);
    if (bd->loyaltyDiscount > 0)
        printf("  Loyalty Discount (-)    : Rs. %8.2f\n", bd->loyaltyDiscount);

    printf("--------------------------------------------\n");
    printf("  TOTAL TO PAY            : Rs. %8.2f\n", bd->finalPrice);
    printf("--------------------------------------------\n");
}

// ================================================================
//  REFUND POLICY - show it before user confirms cancellation
// ================================================================

void show_refund_policy() {
    printf("\n--------------------------------------------\n");
    printf("         REFUND POLICY\n");
    printf("--------------------------------------------\n");
    printf("  7+ days before travel   : 100%% refund\n");
    printf("  3 to 7 days before      :  75%% refund\n");
    printf("  1 to 3 days before      :  50%% refund\n");
    printf("  Less than 1 day         :   0%% refund (No refund)\n");
    printf("--------------------------------------------\n");
}

// ================================================================
//  CALCULATE REFUND AMOUNT
//  daysBeforeTravel = how many days between today and travel date
// ================================================================

float calculate_refund(float paidPrice, int daysBeforeTravel) {
    float refund = 0;

    if (daysBeforeTravel >= 7) {
        refund = paidPrice * 1.00;   // 100%
        printf("[REFUND]: 7+ days before travel. Full refund: Rs. %.2f\n", refund);
    } else if (daysBeforeTravel >= 3) {
        refund = paidPrice * 0.75;   // 75%
        printf("[REFUND]: 3-7 days before travel. 75%% refund: Rs. %.2f\n", refund);
    } else if (daysBeforeTravel >= 1) {
        refund = paidPrice * 0.50;   // 50%
        printf("[REFUND]: 1-3 days before travel. 50%% refund: Rs. %.2f\n", refund);
    } else {
        refund = 0;                  // 0%
        printf("[REFUND]: Less than 1 day before travel. No refund.\n");
    }

    return refund;
}
