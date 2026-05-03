#ifndef STRUCTS_H
#define STRUCTS_H

struct Train {
    int trainNum;
    char name[50];
    char start[50];
    char end[50];
    char stops[100];
    char status[20];
};

struct User {
    char username[50];
    char password[50];
    char phone[15];
    char security_ans[50];
};

#endif
