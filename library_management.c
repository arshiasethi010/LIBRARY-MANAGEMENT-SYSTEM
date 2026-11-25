#include <stdio.h>
#include <string.h>

#define FILE_NAME "library.dat"

struct Book {
    int  id;
    char title[60];
    char author[40];
    int  issued;          /* 0 = available, 1 = issued */
    char issuedTo[40];
};

/* Check whether a book ID already exists */
int idExists(int id) {
    struct Book b;
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return 0;
    while (fread(&b, sizeof b, 1, fp) == 1)
        if (b.id == id) { fclose(fp); return 1; }
    fclose(fp);
    return 0;
}

void addBook(void) {
    struct Book b;
    printf("Enter book ID: ");
    scanf("%d", &b.id);
    if (idExists(b.id)) { printf("A book with this ID already exists!\n"); return; }
    printf("Enter title: ");
    scanf(" %59[^\n]", b.title);
    printf("Enter author: ");
    scanf(" %39[^\n]", b.author);
    b.issued = 0;
    strcpy(b.issuedTo, "-");

    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) { printf("Error opening file!\n"); return; }
    fwrite(&b, sizeof b, 1, fp);
    fclose(fp);
    printf("Book added successfully.\n");
}

void displayBooks(void) {
    struct Book b;
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("No books in the library yet.\n"); return; }
    printf("\n%-6s %-28s %-18s %-10s %-15s\n", "ID", "Title", "Author", "Status", "Issued To");
    printf("---------------------------------------------------------------------------------\n");
    while (fread(&b, sizeof b, 1, fp) == 1)
        printf("%-6d %-28s %-18s %-10s %-15s\n", b.id, b.title, b.author,
               b.issued ? "Issued" : "Available", b.issuedTo);
    fclose(fp);
}

void searchBook(void) {
    char key[60];
    int found = 0;
    struct Book b;
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("No books in the library yet.\n"); return; }
    printf("Enter title or author keyword: ");
    scanf(" %59[^\n]", key);
    while (fread(&b, sizeof b, 1, fp) == 1) {
        if (strstr(b.title, key) || strstr(b.author, key)) {
            printf("ID: %d | %s by %s | %s\n", b.id, b.title, b.author,
                   b.issued ? "Issued" : "Available");
            found = 1;
        }
    }
    if (!found) printf("No matching book found.\n");
    fclose(fp);
}

/* mode 1 = issue, mode 0 = return */
void changeStatus(int mode) {
    int id, found = 0;
    struct Book b;
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (!fp) { printf("No books in the library yet.\n"); return; }
    printf("Enter book ID: ");
    scanf("%d", &id);
    while (fread(&b, sizeof b, 1, fp) == 1) {
        if (b.id == id) {
            found = 1;
            if (mode == 1 && b.issued) {
                printf("Book is already issued to %s.\n", b.issuedTo);
            } else if (mode == 0 && !b.issued) {
                printf("This book was not issued.\n");
            } else {
                if (mode == 1) {
                    printf("Enter student name: ");
                    scanf(" %39[^\n]", b.issuedTo);
                    b.issued = 1;
                    printf("Book issued successfully.\n");
                } else {
                    b.issued = 0;
                    strcpy(b.issuedTo, "-");
                    printf("Book returned successfully.\n");
                }
                fseek(fp, -(long)sizeof b, SEEK_CUR);
                fwrite(&b, sizeof b, 1, fp);
            }
            break;
        }
    }
    if (!found) printf("Book ID not found.\n");
    fclose(fp);
}

int main(void) {
    int choice;
    do {
        printf("\n===== LIBRARY MANAGEMENT SYSTEM =====\n");
        printf("1. Add Book\n2. Display All Books\n3. Search Book\n");
        printf("4. Issue Book\n5. Return Book\n6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); choice = 0; }
        switch (choice) {
            case 1: addBook();       break;
            case 2: displayBooks();  break;
            case 3: searchBook();    break;
            case 4: changeStatus(1); break;
            case 5: changeStatus(0); break;
            case 6: printf("Thank you!\n"); break;
            default: printf("Invalid choice, try again.\n");
        }
    } while (choice != 6);
    return 0;
}
