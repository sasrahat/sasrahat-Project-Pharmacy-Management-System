#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "pharmacy_data.txt"
#define TAB "\t\t\t\t"

typedef struct Medicine
{
    char name[50];
    int quantity;
    float price;
    char expiryDate[15];
    char shelf[20];

    struct Medicine *left;
    struct Medicine *right;
    struct Medicine *next;
} M;

typedef struct
{
    M *front;
    M *rear;
} Q;

M *createMedicine(char *name, int qty, float price, char *expiry, char *shelf)
{
    M *m = (M *)malloc(sizeof(M));
    if (!m)
    {
        printf(TAB "Memory allocation failed!\n");
        exit(1);
    }
    strcpy(m->name, name);
    strcpy(m->expiryDate, expiry);
    strcpy(m->shelf, shelf);
    m->quantity = qty;
    m->price = price;
    m->left = m->right = m->next = NULL;
    return m;
}

M *insertBST(M *root, char *name, int qty, float price, char *expiry, char *shelf)
{
    if (root == NULL)
        return createMedicine(name, qty, price, expiry, shelf);

    int cmp = strcmp(name, root->name);

    if (cmp == 0)
        root->quantity += qty;
    else if (cmp < 0)
        root->left = insertBST(root->left, name, qty, price, expiry, shelf);
    else
        root->right = insertBST(root->right, name, qty, price, expiry, shelf);

    return root;
}

M *searchBST(M *root, char *name)
{
    if (!root || strcmp(root->name, name) == 0)
        return root;

    if (strcmp(name, root->name) < 0)
        return searchBST(root->left, name);
    else
        return searchBST(root->right, name);
}

void displayStock(M *root)
{
    if (!root)
        return;

    displayStock(root->left);
    printf(TAB "%-15s %-8d %-8.2f %-12s %-10s\n",
           root->name, root->quantity, root->price,
           root->expiryDate, root->shelf);
    displayStock(root->right);
}

void updateMedicine(M *root, char *name)
{
    M *m = searchBST(root, name);
    if (!m)
    {
        printf(TAB "Medicine not found!\n");
        return;
    }

    int ch;
    printf(TAB "1. Update Quantity\n");
    printf(TAB "2. Update Price\n");
    printf(TAB "3. Update Expiry Date\n");
    printf(TAB "4. Update Shelf\n");
    printf(TAB "Enter choice: ");
    scanf("%d", &ch);

    switch (ch)
    {
    case 1:
        printf(TAB "Enter new quantity: ");
        scanf("%d", &m->quantity);
        break;
    case 2:
        printf(TAB "Enter new price: ");
        scanf("%f", &m->price);
        break;
    case 3:
        printf(TAB "Enter new expiry date: ");
        scanf("%s", m->expiryDate);
        break;
    case 4:
        printf(TAB "Enter new shelf: ");
        scanf("%s", m->shelf);
        break;
    default:
        printf(TAB "Invalid option!\n");
        return;
    }
    printf(TAB "Medicine updated successfully!\n");
}

void enqueue(Q *q, char *name, int qty, float price)
{
    M *order = createMedicine(name, qty, price, "N/A", "N/A");
    if (!q->rear)
        q->front = q->rear = order;
    else
    {
        q->rear->next = order;
        q->rear = order;
    }
}

void displayOrders(Q *q)
{
    if (!q->front)
    {
        printf(TAB "No pending orders!\n");
        return;
    }

    printf(TAB "Pending Orders:\n");
    printf(TAB "----------------------------------\n");
    printf(TAB "%-15s %-10s %-10s\n", "Name", "Quantity", "Price");
    printf(TAB "----------------------------------\n");

    M *cur = q->front;
    while (cur)
    {
        printf(TAB "%-15s %-10d %-10.2f\n",
               cur->name, cur->quantity, cur->price);
        cur = cur->next;
    }
}

void processOrder(Q *q, M *stock)
{
    if (!q->front)
    {
        printf(TAB "No orders to process!\n");
        return;
    }

    M *order = q->front;
    M *med = searchBST(stock, order->name);

    if (med && med->quantity >= order->quantity)
    {
        med->quantity -= order->quantity;
        printf(TAB "Order processed: %s | Shelf: %s | Total: %.2f\n",
               med->name, med->shelf,
               order->quantity * order->price);
    }
    else
        printf(TAB "Insufficient stock or medicine not found!\n");

    q->front = q->front->next;
    if (!q->front)
        q->rear = NULL;
    free(order);
}

void saveToFile(M *root, FILE *fp)
{
    if (!root)
        return;
    saveToFile(root->left, fp);
    fprintf(fp, "%s %d %.2f %s %s\n",
            root->name, root->quantity, root->price,
            root->expiryDate, root->shelf);
    saveToFile(root->right, fp);
}

M *loadFromFile()
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp)
        return NULL;

    M *root = NULL;
    char name[50], expiry[15], shelf[20];
    int qty;
    float price;

    while (fscanf(fp, "%s %d %f %s %s",
                  name, &qty, &price, expiry, shelf) == 5)
        root = insertBST(root, name, qty, price, expiry, shelf);

    fclose(fp);
    return root;
}

int main()
{
    M *stock = loadFromFile();
    Q orders = {NULL, NULL};

    int choice, qty;
    float price;
    char name[50], expiry[15], shelf[20];

    while (1)
    {
        printf(TAB "========= Pharmacy Management System =========\n");
        printf(TAB "1. Register New Medicine\n");
        printf(TAB "2. Display Stock\n");
        printf(TAB "3. Record Customer Order\n");
        printf(TAB "4. Display Order Queue\n");
        printf(TAB "5. Process Order\n");
        printf(TAB "6. Search Medicine\n");
        printf(TAB "7. Update Medicine Information\n");
        printf(TAB "8. Exit\n");
        printf(TAB "==============================================\n");
        printf(TAB "Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf(TAB "Enter medicine name: ");
            scanf("%s", name);
            printf(TAB "Enter quantity: ");
            scanf("%d", &qty);
            printf(TAB "Enter price: ");
            scanf("%f", &price);
            printf(TAB "Enter expiry date: ");
            scanf("%s", expiry);
            printf(TAB "Enter shelf: ");
            scanf("%s", shelf);
            stock = insertBST(stock, name, qty, price, expiry, shelf);
            printf(TAB "Medicine added successfully!\n");
            break;

        case 2:
            printf("\n" TAB "%-15s %-8s %-8s %-12s %-10s\n",
                   "Name", "Qty", "Price", "Expiry", "Shelf");
            displayStock(stock);
            break;

        case 3:
            printf(TAB "Enter medicine name: ");
            scanf("%s", name);
            printf(TAB "Enter quantity: ");
            scanf("%d", &qty);
            printf(TAB "Enter selling price: ");
            scanf("%f", &price);
            enqueue(&orders, name, qty, price);
            printf(TAB "Order added successfully!\n");
            break;

        case 4:
            displayOrders(&orders);
            break;

        case 5:
            processOrder(&orders, stock);
            break;

        case 6:
            printf(TAB "Enter medicine name to search: ");
            scanf("%s", name);
            {
                M *m = searchBST(stock, name);
                if (m)
                    printf(TAB "Found: %s | Qty:%d | Price:%.2f | Exp:%s | Shelf:%s\n",
                           m->name, m->quantity, m->price, m->expiryDate, m->shelf);
                else
                    printf(TAB "Medicine not found!\n");
            }
            break;

        case 7:
            printf(TAB "Enter medicine name to update: ");
            scanf("%s", name);
            updateMedicine(stock, name);
            break;

        case 8:
        {
            FILE *fp = fopen(DATA_FILE, "w");
            if (fp)
            {
                saveToFile(stock, fp);
                fclose(fp);
            }
            printf(TAB "Data saved. Exiting system...\n");
            exit(0);
        }

        default:
            printf(TAB "Invalid choice! Try again.\n");
        }
    }
}