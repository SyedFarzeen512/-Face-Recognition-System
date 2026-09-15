# Face Recognition System

## Features

* Register a new person
* Store person's ID, name and face features
* Display registered persons
* Search person by ID
* Face matching using stored feature values
* Attendance recording
* View attendance
* Delete a person
* Save data to files
* Load data automatically when program starts
* Menu-driven interface
* Simple authentication/matching threshold

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define FEATURES 5
#define THRESHOLD 15

typedef struct {
    int id;
    char name[50];
    int face[FEATURES];
} Person;

typedef struct {
    int id;
    char name[50];
    char date[20];
    char time[20];
} Attendance;

Person users[MAX_USERS];
Attendance attendance[500];

int userCount = 0;
int attendanceCount = 0;

/* ---------- Save Users ---------- */

void saveUsers() {
    FILE *file = fopen("users.dat", "wb");

    if (file == NULL) {
        printf("Error saving users!\n");
        return;
    }

    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(users, sizeof(Person), userCount, file);

    fclose(file);
}

/* ---------- Load Users ---------- */

void loadUsers() {
    FILE *file = fopen("users.dat", "rb");

    if (file == NULL)
        return;

    fread(&userCount, sizeof(int), 1, file);
    fread(users, sizeof(Person), userCount, file);

    fclose(file);
}

/* ---------- Save Attendance ---------- */

void saveAttendance() {
    FILE *file = fopen("attendance.dat", "wb");

    if (file == NULL)
        return;

    fwrite(&attendanceCount, sizeof(int), 1, file);
    fwrite(attendance, sizeof(Attendance), attendanceCount, file);

    fclose(file);
}

/* ---------- Load Attendance ---------- */

void loadAttendance() {
    FILE *file = fopen("attendance.dat", "rb");

    if (file == NULL)
        return;

    fread(&attendanceCount, sizeof(int), 1, file);
    fread(attendance, sizeof(Attendance),
          attendanceCount, file);

    fclose(file);
}

/* ---------- Find User ---------- */

int findUser(int id) {

    for (int i = 0; i < userCount; i++) {
        if (users[i].id == id)
            return i;
    }

    return -1;
}

/* ---------- Register Person ---------- */

void registerPerson() {

    if (userCount >= MAX_USERS) {
        printf("\nDatabase is full!\n");
        return;
    }

    Person p;

    printf("\nEnter ID: ");
    scanf("%d", &p.id);

    if (findUser(p.id) != -1) {
        printf("ID already exists!\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", p.name);

    printf("\nEnter 5 face feature values\n");
    printf("(Example values between 0 and 100)\n");

    for (int i = 0; i < FEATURES; i++) {
        printf("Feature %d: ", i + 1);
        scanf("%d", &p.face[i]);
    }

    users[userCount] = p;
    userCount++;

    saveUsers();

    printf("\nPerson registered successfully!\n");
}

/* ---------- Display Users ---------- */

void displayUsers() {

    if (userCount == 0) {
        printf("\nNo registered persons.\n");
        return;
    }

    printf("\n========== REGISTERED PERSONS ==========\n");

    for (int i = 0; i < userCount; i++) {

        printf("\nID   : %d", users[i].id);
        printf("\nName : %s", users[i].name);

        printf("\nFace Features: ");

        for (int j = 0; j < FEATURES; j++)
            printf("%d ", users[i].face[j]);

        printf("\n----------------------------------------\n");
    }
}

/* ---------- Search User ---------- */

void searchUser() {

    int id;

    printf("\nEnter ID to search: ");
    scanf("%d", &id);

    int index = findUser(id);

    if (index == -1) {
        printf("\nPerson not found!\n");
        return;
    }

    printf("\nPerson Found!\n");
    printf("ID   : %d\n", users[index].id);
    printf("Name : %s\n", users[index].name);
}

/* ---------- Face Recognition ---------- */

void recognizeFace() {

    int input[FEATURES];

    printf("\n========== FACE RECOGNITION ==========\n");

    printf("Enter face feature values:\n");

    for (int i = 0; i < FEATURES; i++) {
        printf("Feature %d: ", i + 1);
        scanf("%d", &input[i]);
    }

    int bestIndex = -1;
    int bestDifference = 999999;

    for (int i = 0; i < userCount; i++) {

        int difference = 0;

        for (int j = 0; j < FEATURES; j++) {

            int d = input[j] - users[i].face[j];

            if (d < 0)
                d = -d;

            difference += d;
        }

        if (difference < bestDifference) {
            bestDifference = difference;
            bestIndex = i;
        }
    }

    if (bestIndex != -1 &&
        bestDifference <= THRESHOLD) {

        printf("\nFACE RECOGNIZED!\n");
        printf("Person : %s\n", users[bestIndex].name);
        printf("ID     : %d\n", users[bestIndex].id);
        printf("Match Score: %d\n", bestDifference);

        /* Attendance */
        if (attendanceCount < 500) {

            time_t now = time(NULL);
            struct tm *t = localtime(&now);

            attendance[attendanceCount].id =
                users[bestIndex].id;

            strcpy(attendance[attendanceCount].name,
                   users[bestIndex].name);

            strftime(attendance[attendanceCount].date,
                     sizeof(attendance[attendanceCount].date),
                     "%Y-%m-%d", t);

            strftime(attendance[attendanceCount].time,
                     sizeof(attendance[attendanceCount].time),
                     "%H:%M:%S", t);

            attendanceCount++;

            saveAttendance();

            printf("Attendance marked successfully!\n");
        }

    } else {

        printf("\nFACE NOT RECOGNIZED!\n");
        printf("No matching person found.\n");
    }
}

/* ---------- View Attendance ---------- */

void viewAttendance() {

    if (attendanceCount == 0) {
        printf("\nNo attendance records.\n");
        return;
    }

    printf("\n========== ATTENDANCE ==========\n");

    for (int i = 0; i < attendanceCount; i++) {

        printf("\nID   : %d",
               attendance[i].id);

        printf("\nName : %s",
               attendance[i].name);

        printf("\nDate : %s",
               attendance[i].date);

        printf("\nTime : %s",
               attendance[i].time);

        printf("\n--------------------------------\n");
    }
}

/* ---------- Delete User ---------- */

void deleteUser() {

    int id;

    printf("\nEnter ID to delete: ");
    scanf("%d", &id);

    int index = findUser(id);

    if (index == -1) {
        printf("Person not found!\n");
        return;
    }

    for (int i = index; i < userCount - 1; i++) {
        users[i] = users[i + 1];
    }

    userCount--;

    saveUsers();

    printf("\nPerson deleted successfully!\n");
}

/* ---------- Main Menu ---------- */

void menu() {

    int choice;

    do {

        printf("\n\n");
        printf("====================================\n");
        printf("       FACE RECOGNITION SYSTEM\n");
        printf("====================================\n");

        printf("1. Register Person\n");
        printf("2. Display Persons\n");
        printf("3. Search Person\n");
        printf("4. Face Recognition\n");
        printf("5. View Attendance\n");
        printf("6. Delete Person\n");
        printf("7. Exit\n");

        printf("====================================\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                registerPerson();
                break;

            case 2:
                displayUsers();
                break;

            case 3:
                searchUser();
                break;

            case 4:
                recognizeFace();
                break;

            case 5:
                viewAttendance();
                break;

            case 6:
                deleteUser();
                break;

            case 7:
                printf("\nThank you for using the system!\n");
                break;

            default:
                printf("\nInvalid choice!\n");
        }

    } while (choice != 7);
}

/* ---------- Main ---------- */

int main() {

    loadUsers();
    loadAttendance();

    printf("\n====================================\n");
    printf("   FACE RECOGNITION SYSTEM\n");
    printf("        C LANGUAGE PROJECT\n");
    printf("====================================\n");

    menu();

    return 0;
}
```
