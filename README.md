[Uploading README.md…]()
# 🚂 Railway Reservation System

A terminal-based Railway Reservation System built in C, featuring user authentication, train search, ticket booking, and cancellation.

---

## Features

- **User Authentication** — Sign up, log in, and password recovery via security question
- **Train Enquiry** — Search trains by number, name, or route (with intermediate stops)
- **Seat & Price Catalog** — View class-wise seat availability and dynamic pricing
- **Ticket Booking** — Book tickets with auto-generated PNR and senior citizen support
- **Ticket Management** — View and cancel booked tickets (with refund calculation)
- **Password Management** — Change password securely from the dashboard

---

## Project Structure

```
├── main.c              # Entry point — main menu
├── authentication.c/h  # Sign up, login, password recovery, encryption
├── dashboard.c/h       # User dashboard and password change
├── trains.c/h          # Train search and catalog display
├── booking.c/h         # Ticket booking and seat update logic
├── cancel.c/h          # View and cancel tickets
├── structs.h           # Shared data structures (User, Train)
├── users.txt           # Encrypted user database
├── trains.txt          # Train records database
├── catalog.txt         # Seat class and pricing catalog
└── ticket.txt          # Booked ticket records
```

---

## Getting Started

### Prerequisites

- GCC compiler
- Linux / macOS (uses `strcasecmp` and `strcasestr`)

### Compile

```bash
gcc main.c authentication.c dashboard.c trains.c booking.c cancel.c -o railway
```

### Run

```bash
./railway
```

> Make sure `users.txt`, `trains.txt`, `catalog.txt`, and `ticket.txt` are in the same directory as the executable.

---

## How It Works

### Authentication
Passwords and usernames are encrypted using a Caesar cipher (key = 5) before being stored. Phone numbers are obfuscated using a digit-to-symbol mapping. The security question ("What is your favorite color?") is used for password recovery.

### Train Categories & Pricing

| Category | Trains | Classes |
|----------|--------|---------|
| CAT01 | Rajdhani Express, Howrah Rajdhani | AC1, CC, SLP |
| CAT02 | Shatabdi Express, Vande Bharat | AC1, CC, SLP |
| CAT03 | All other trains | AC1, CC, SLP |

Dynamic pricing applies:
- **+10%** when occupancy exceeds 50%
- **+15%** when occupancy exceeds 80%
- **+6% surcharge** for window seats

### Ticket Format

Tickets are stored in `ticket.txt` as:
```
PNR|PassengerName|Age|TrainNumber|Class|GuardianPhone|Price
```

### Cancellation
- Cancelled tickets are removed from `ticket.txt`
- Refund = 85% of ticket price (15% cancellation fee)

---

## Data Files

| File | Purpose |
|------|---------|
| `users.txt` | Encrypted user credentials |
| `trains.txt` | Train names, routes, stops, and availability |
| `catalog.txt` | Seat counts and base prices per class |
| `ticket.txt` | Active ticket records |

---

## Limitations & Known Issues

- Single security question for all users ("What is your favorite color?")
- PNR is randomly generated and not guaranteed to be unique
- No date-based scheduling — trains are always shown as available or not
- `strcasestr` is a GNU extension and may not work on Windows without modification

---

## License

This project was developed for educational purposes.
