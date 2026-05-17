#ifndef PRICING_H
#define PRICING_H

// Stores a full breakdown of how the price was calculated
// So we can show the user exactly what they are paying for
struct PriceBreakdown {
    float basePrice;
    float occupancySurcharge;  // +10% if <25 seats, +15% if <10 seats
    float tatkalSurcharge;     // +30% if booking within 2 days of travel
    float festivalSurcharge;   // +20% on festival/holiday dates
    float windowSurcharge;     // +6% for window seat
    float seniorDiscount;      // -10% for passengers above 60
    float childDiscount;       // -50% for children below 5
    float loyaltyDiscount;     // -10% if user has 3 or more past tickets
    float finalPrice;
};

// Calculates the full final price and fills the breakdown struct
// Returns the final price
float calculate_price(float basePrice,
                      int seatsLeft,
                      int travelDay, int travelMonth,
                      int bookingDay, int bookingMonth,
                      char seatType,   // 'W' window, 'A' aisle, 'N' none
                      int age,
                      char *username,
                      struct PriceBreakdown *bd);

// Prints a nice bill/receipt showing every charge and discount
void show_price_breakdown(struct PriceBreakdown *bd);

// Calculates refund amount based on how many days before travel
float calculate_refund(float paidPrice, int daysBeforeTravel);

// Prints the refund policy so user knows before cancelling
void show_refund_policy();

// Counts how many tickets this user has booked before (for loyalty)
int count_user_tickets(char *username);

// Checks if a date falls on a festival (returns 1 if yes)
int is_festival_date(int day, int month);

#endif
