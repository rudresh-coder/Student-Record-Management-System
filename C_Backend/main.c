#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*================= STUDENT STRUCTURE =================*/
struct Student {
    int roll;
    char name[50];
    float marks;
    struct Student *next;
};

/*================= USING STACK FOR UNDO FEATURE =================*/
struct Stack {
    struct Student data;
    struct Stack *next;
};

/*================= USING QUEUE FOR REQUESTS =================*/
struct Queue {
    char request[100];
    struct Queue *next;
};

/*================= BINARY SEARCH TREE FOR SEARCHING =================*/
struct BST {
    int roll;
    struct BST *left, *right;
};

/*================= GLOBAL POINTERS ================*/
struct Student *head = NULL;
struct Stack *top = NULL;
struct Queue *front = NULL, *rear = NULL;
struct BST *root = NULL;

/*================= DECKEARING FUNCTIONS =================*/
void insertStudent();
void deleteStudent();
void displayStudents();
void searchStudent();
void undoDelete();
void enqueueRequest();
void dequeueRequest();

/* BST */
struct BST* insertBST(struct BST*, int);
int searchBST(struct BST*, int);
struct BST* deleteBST(struct BST*, int);

/* Helper Function */
struct Student* findStudentByRoll(int);

/*================= MAIN FUNCTION =================*/
int main() {
    int choice;

    do {
        printf("\n--- Student Record Management System ---\n");
        printf("1. Add Student\n");
        printf("2. Delete Student\n");
        printf("3. Display Student\n");
        printf("4. Search Student\n");
        printf("5. Undo Delete\n");
        printf("6. Add Request\n");
        printf("7. Process Request\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: insertStudent(); break;
            case 2: deleteStudent(); break;
            case 3: displayStudents(); break;
            case 4: searchStudent(); break;
            case 5: undoDelete(); break;
            case 6: enqueueRequest(); break;
            case 7: dequeueRequest(); break;
            case 0: printf("Exiting program...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);

    return 0;
}

/*================= INSERT STUDENT =================*/
void insertStudent() {
    struct Student *newNode = (struct Student*)malloc(sizeof(struct Student));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter Roll Number: ");
    scanf("%d", &newNode->roll);

    if (searchBST(root, newNode->roll)) {
        printf("Roll number %d already exists. Insertion cancelled.\n", newNode->roll);
        free(newNode);
        return;
    }

    printf("Enter Name: ");
    scanf(" %49[^\n]", newNode->name);
    printf("Enter Marks: ");
    scanf("%f", &newNode->marks);

    newNode->next = head;
    head = newNode;

    root = insertBST(root, newNode->roll);

    printf("Student added successfully.\n");
}

/*================= DELETE STUDENT =================*/
void deleteStudent() {
    int roll;
    struct Student *temp = head, *prev = NULL;

    printf("Enter roll number to delete: ");
    scanf("%d", &roll);

    while (temp != NULL && temp->roll != roll) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Student with roll number %d not found.\n", roll);
        return;
    }

    /*Pushing to stack for undo*/
    struct Stack *s = (struct Stack*)malloc(sizeof(struct Stack));
    if (s == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    s->data.roll = temp->roll;
    strcpy(s->data.name, temp->name);
    s->data.marks = temp->marks;
    s->data.next = NULL;     // important: do NOT copy list linkage
    s->next = top;
    top = s;

    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;

    root = deleteBST(root, roll);

    free(temp);
    printf("Student deleted successfully.\n");
}

/*================= DISPLAY STUDENTS =================*/
void displayStudents() {
    struct Student *temp = head;

    if (temp == NULL) {
        printf("No student records available.\n");
        return;
    }

    printf("\nRoll\tName\t\tMarks\n");
    while (temp != NULL) {
        printf("%d\t%s\t\t%.2f\n", temp->roll, temp->name, temp->marks);
        temp = temp->next;
    }
}

/* ================= SEARCH STUDENT ================= */
void searchStudent() {
    int roll;
    printf("Enter roll number to search: ");
    scanf("%d", &roll);

    if (!searchBST(root, roll)) {
        printf("Student not found.\n");
        return;
    }

    struct Student *s = findStudentByRoll(roll);
    if (s == NULL) { // safety if structures are out of sync
        printf("Student not found.\n");
        return;
    }

    printf("\nStudent Found:\n");
    printf("Roll : %d\n", s->roll);
    printf("Name : %s\n", s->name);
    printf("Marks: %.2f\n", s->marks);
}

/* ================= UNDO DELETE ================= */
void undoDelete() {
    if (top == NULL) {
        printf("Nothing to undo.\n");
        return;
    }

    struct Student *newNode = (struct Student*)malloc(sizeof(struct Student));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newNode->roll = top->data.roll;
    strcpy(newNode->name, top->data.name);
    newNode->marks = top->data.marks;

    newNode->next = head;
    head = newNode;

    root = insertBST(root, newNode->roll);  // ISSUE 1 fix

    struct Stack *temp = top;
    top = top->next;
    free(temp);

    printf("Undo successful. Student restored.\n");
}

/* ================= QUEUE OPERATIONS ================= */
void enqueueRequest() {
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    if (q == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter request: ");
    scanf(" %99[^\n]", q->request); // also add width limit
    q->next = NULL;

    if (rear == NULL)
        front = rear = q;
    else {
        rear->next = q;
        rear = q;
    }

    printf("Request added.\n");
}

void dequeueRequest() {
    if (front == NULL) {
        printf("No request to process.\n");
        return;
    }

    struct Queue *temp = front;
    printf("Processing request: %s\n", temp->request);
    front = front->next;

    if (front == NULL)
        rear = NULL;
    
    free(temp);
}

/* ================= BST FUNCTIONS ================= */
struct BST* insertBST(struct BST *node, int roll) {
    if (node == NULL) {
        struct BST *temp = (struct BST*)malloc(sizeof(struct BST));
        temp->roll = roll;
        temp->left = temp->right = NULL;
        return temp;
    }

    if (roll < node->roll)
        node->left = insertBST(node->left, roll);
    else if (roll > node->roll)
        node->right = insertBST(node->right, roll);

    return node;
}

int searchBST(struct BST *node, int roll) {
    if (node == NULL)
        return 0;
    if (node->roll == roll)
        return 1;
    if (roll < node->roll)
        return searchBST(node->left, roll);
    return searchBST(node->right, roll);
}

static struct BST* minValueNode(struct BST* node) {
    struct BST* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

struct BST* deleteBST(struct BST* node, int roll) {
    if (node == NULL) return NULL;

    if (roll < node->roll) {
        node->left = deleteBST(node->left, roll);
    } else if (roll > node->roll) {
        node->right = deleteBST(node->right, roll);
    } else {
        // node with 0 or 1 child
        if (node->left == NULL) {
            struct BST* temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            struct BST* temp = node->left;
            free(node);
            return temp;
        }

        // node with 2 children
        struct BST* temp = minValueNode(node->right);
        node->roll = temp->roll;
        node->right = deleteBST(node->right, temp->roll);
    }
    return node;
}

/* ================= FIND STUDENT BY ROLL ================= */
struct Student* findStudentByRoll(int roll) {
    struct Student *cur = head;
    while (cur != NULL) {
        if (cur->roll == roll) return cur;
        cur = cur->next;
    }
    return NULL;
}