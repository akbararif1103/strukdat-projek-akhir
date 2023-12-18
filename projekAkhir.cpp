#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

// Enum untuk jenis urutan display
enum DisplayOrder
{
    IN_ORDER,
    PRE_ORDER,
    POST_ORDER
};

// Struktur data untuk produk
struct Product
{
    char name[50];
    double price;
    char code[10];
    // Tambahan informasi lainnya sesuai kebutuhan
};

// Node untuk pohon biner
struct TreeNode
{
    Product product;
    TreeNode *left;
    TreeNode *right;

    TreeNode(const Product &p) : product(p), left(nullptr), right(nullptr) {}
};

// Node untuk produk yang dihapus
struct DeletedProductNode
{
    Product data;
    DeletedProductNode *next;

    DeletedProductNode(const Product &p) : data(p), next(nullptr) {}
};

// Struktur untuk manajemen toko
struct GroceryStore
{
    Product *productList;                    // Daftar produk
    int productCount;
    TreeNode *root;                          // Akar dari pohon biner
    DeletedProductNode *deletedProductsHead; // Linked list untuk menyimpan produk yang dihapus
    int deletedProductCount;
    static const int HASH_TABLE_SIZE = 50;   // Ukuran tabel hash
    Product hashTable[HASH_TABLE_SIZE];      // Menggunakan satu dimensi untuk tabel hash

    GroceryStore() : productList(nullptr), productCount(0), root(nullptr), deletedProductsHead(nullptr), deletedProductCount(0) {}
    ~GroceryStore()
    {
        delete[] productList;
    }
};

// Fungsi hash sederhana
int hashFunction(const char *code);

// Fungsi untuk menambahkan produk
void addProduct(GroceryStore &store, const Product &product);

// Fungsi untuk mencari produk berdasarkan kode produk menggunakan hash
Product searchByCode(const GroceryStore &store, const char *productCode);

// Fungsi untuk mencari dan menampilkan produk berdasarkan rentang nama pada pohon biner
void searchByNameRange(const GroceryStore &store, const char *startName, const char *endName);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending (post-order)
void sortByNameDescending(const TreeNode *node);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending pada pohon biner
void sortByNameDescending(const GroceryStore &store);

// Fungsi untuk menambahkan produk yang dihapus ke dalam linked list
void addDeletedProductNode(GroceryStore &store, const Product &product);

// Fungsi internal untuk memasukkan produk ke dalam pohon biner
TreeNode *insertIntoTree(TreeNode *node, const Product &product);

// Fungsi untuk menghapus produk dari pohon biner
TreeNode *deleteFromTree(TreeNode *node, const char *productCode);

// Fungsi untuk menampilkan produk yang dihapus
void displayDeletedProductsHistory(const GroceryStore &store, DisplayOrder order);

// Fungsi-fungsi untuk menampilkan produk yang dihapus dengan urutan tertentu
void displayDeletedProductsHistoryInOrder(const DeletedProductNode *node);
void displayDeletedProductsHistoryPreOrder(const DeletedProductNode *node);
void displayDeletedProductsHistoryPostOrder(const DeletedProductNode *node);

// Fungsi untuk menampilkan produk yang dihapus
void displayDeletedProduct(const Product &deletedProduct);

TreeNode *deleteFromTree(TreeNode *node, const Product &product);

bool isEmpty(const GroceryStore &store);

int hashFunction(const char *code)
{
    int sum = 0;
    for (int i = 0; code[i] != '\0'; ++i)
    {
        // Menambahkan kode ASCII huruf ke sum
        sum += tolower(code[i]);
    }
    return sum % GroceryStore::HASH_TABLE_SIZE;
}

void addProduct(GroceryStore &store, const Product &product)
{
    // Periksa apakah kode produk sudah ada dalam toko
    for (int i = 0; i < store.productCount; ++i)
    {
        if (strcmp(store.productList[i].code, product.code) == 0)
        {
            cout << "Product with code " << product.code << " already exists. Cannot add duplicate products." << endl;
            return;
        }
    }

    // Periksa apakah produk dengan kode yang sama telah dihapus sebelumnya
    DeletedProductNode *current = store.deletedProductsHead;
    while (current != nullptr)
    {
        if (strcmp(current->data.code, product.code) == 0)
        {
            cout << "Product with code " << product.code << " has been deleted. Cannot add the same product again." << endl;
            return;
        }
        current = current->next;
    }

    // Alokasi dinamis untuk menyimpan daftar produk
    Product *temp = new Product[store.productCount + 1];
    memcpy(temp, store.productList, sizeof(Product) * store.productCount);

    // Menambahkan produk ke daftar
    temp[store.productCount] = product;

    // Hapus daftar produk yang lama
    delete[] store.productList;

    // Menetapkan daftar produk yang baru
    store.productList = temp;
    ++store.productCount;

    // Menambahkan produk ke tabel hash
    int index = hashFunction(product.code);
    store.hashTable[index] = product;

    // Menambahkan produk ke dalam pohon biner
    store.root = insertIntoTree(store.root, product);

    cout << "Product added successfully." << endl;
}

Product searchByCode(const GroceryStore &store, const char *productCode)
{
    int index = hashFunction(productCode);
    return store.hashTable[index];
}

void searchByNameRange(const GroceryStore &store, const char *startName, const char *endName)
{
    bool found = false;

    cout << "Products within the specified name range:" << endl;
    for (int i = 0; i < store.productCount; ++i)
    {
        if (strcmp(store.productList[i].name, startName) >= 0 && strcmp(store.productList[i].name, endName) <= 0)
        {
            cout << store.productList[i].name << " - " << store.productList[i].price << endl;
            found = true;
        }
    }

    if (!found)
    {
        cout << "No products found within the specified name range." << endl;
    }
}

void sortByNameDescending(GroceryStore &store)
{
    // Menggunakan algoritma sort() dari C++ STL
    sort(store.productList, store.productList + store.productCount, [](const Product &a, const Product &b) {
        return strcmp(a.name, b.name) > 0;
    });

    cout << "Products sorted in descending order by name:" << endl;
    for (int i = 0; i < store.productCount; ++i)
    {
        cout << store.productList[i].name << " - " << store.productList[i].price << endl;
    }
}



void addDeletedProductNode(GroceryStore &store, const Product &product)
{
    // Buat node baru
    DeletedProductNode *newNode = new DeletedProductNode(product);

    // Tambahkan ke depan linked list
    newNode->next = store.deletedProductsHead;
    store.deletedProductsHead = newNode;
}

void deleteProduct(GroceryStore &store, const char *productCode)
{
    for (int i = 0; i < store.productCount; ++i)
    {
        if (strcmp(store.productList[i].code, productCode) == 0)
        {
            // Pindahkan produk yang dihapus ke dalam linked list produk yang dihapus
            addDeletedProductNode(store, store.productList[i]);

            // Hapus produk dari daftar produk yang masih ada
            for (int j = i; j < store.productCount - 1; ++j)
            {
                store.productList[j] = store.productList[j + 1];
            }
            --store.productCount;

            // Memanggil deleteFromTree untuk menghapus dari pohon biner
            store.root = deleteFromTree(store.root, productCode);

            cout << "Product deleted successfully." << endl;

            // Jika diperlukan, urutkan ulang daftar produk yang masih ada
            // Tidak perlu diurutkan jika hanya menghapus satu elemen
            if (store.productCount > 1)
            {
                sortByNameDescending(store);
            }

            return;
        }
    }
    cout << "Product with code " << productCode << " not found for deletion." << endl;
}


TreeNode *findMin(TreeNode *node)
{
    while (node->left != nullptr)
    {
        node = node->left;
    }
    return node;
}

TreeNode *deleteFromTree(TreeNode *node, const char *productCode)
{
    if (node == nullptr)
    {
        return node;
    }

    if (strcmp(productCode, node->product.code) < 0)
    {
        node->left = deleteFromTree(node->left, productCode);
    }
    else if (strcmp(productCode, node->product.code) > 0)
    {
        node->right = deleteFromTree(node->right, productCode);
    }
    else
    {
        // Node dengan satu anak atau tanpa anak
        if (node->left == nullptr)
        {
            TreeNode *temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr)
        {
            TreeNode *temp = node->left;
            delete node;
            return temp;
        }

        // Node dengan dua anak, dapatkan node pengganti terkecil di subpohon kanan
        TreeNode *temp = findMin(node->right);

        // Salin data dari node pengganti ke node saat ini
        node->product = temp->product;

        // Hapus node pengganti
        node->right = deleteFromTree(node->right, temp->product.code);
    }

    return node;
}


void displayDeletedProductsHistory(const GroceryStore &store, DisplayOrder order)
{
    cout << "Deleted Products History:" << endl;

    if (order == IN_ORDER)
    {
        // Menampilkan berdasarkan urutan in-order
        displayDeletedProductsHistoryInOrder(store.deletedProductsHead);
    }
    else if (order == PRE_ORDER)
    {
        // Menampilkan berdasarkan urutan pre-order
        displayDeletedProductsHistoryPreOrder(store.deletedProductsHead);
    }
    else if (order == POST_ORDER)
    {
        // Menampilkan berdasarkan urutan post-order
        displayDeletedProductsHistoryPostOrder(store.deletedProductsHead);
    }
}

void displayDeletedProductsHistoryInOrder(const DeletedProductNode *node)
{
    if (node != nullptr)
    {
        displayDeletedProductsHistoryInOrder(node->next);
        displayDeletedProduct(node->data);
    }
}

void displayDeletedProductsHistoryPreOrder(const DeletedProductNode *node)
{
    if (node != nullptr)
    {
        displayDeletedProduct(node->data);
        displayDeletedProductsHistoryPreOrder(node->next);
    }
}

void displayDeletedProductsHistoryPostOrder(const DeletedProductNode *node)
{
    if (node != nullptr)
    {
        displayDeletedProductsHistoryPostOrder(node->next);
        displayDeletedProduct(node->data);
    }
}

void displayDeletedProduct(const Product &deletedProduct)
{
    cout << "Product Code: " << deletedProduct.code << " - Product Name: " << deletedProduct.name
         << " - Product Price: " << deletedProduct.price << endl;
}

TreeNode *insertIntoTree(TreeNode *node, const Product &product)
{
    if (node == nullptr)
    {
        return new TreeNode(product);
    }

    if (strcmp(product.name, node->product.name) <= 0)
    {
        node->left = insertIntoTree(node->left, product);
    }
    else
    {
        node->right = insertIntoTree(node->right, product);
    }

    return node;
}

bool isEmpty(const GroceryStore &store)
{
    return store.productCount == 0;
}


int main()
{
    // Contoh penggunaan
    GroceryStore groceryStore;

    int choice;
    do
    {
        cout << "\nMenu:\n";
        cout << "1. Add Product\n";
        cout << "2. Search Products by Code\n";
        cout << "3. Search Products by Name Range\n";
        cout << "4. Sort Products by Name (Descending)\n";
        cout << "5. Delete Product\n";
        cout << "6. View Deleted Products History\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            Product newProduct;
            cout << "Enter product name: ";
            cin.ignore(); // Membersihkan newline character dari masukan sebelumnya
            cin.getline(newProduct.name, sizeof(newProduct.name));
            cout << "Enter product price: ";
            cin >> newProduct.price;
            cout << "Enter product code: ";
            cin >> newProduct.code;
            addProduct(groceryStore, newProduct);
            break;
        }
        case 2:
        {
            char productCode[10];
            cout << "Enter product code to search: ";
            cin >> productCode;
            Product result = searchByCode(groceryStore, productCode);
            if (result.name[0] != '\0')
            {
                cout << "Product Found: " << result.name << " - " << result.price << endl;
            }
            else
            {
                cout << "Product Not Found." << endl;
            }
            break;
        }
        case 3:
        {
            char startName[50], endName[50];
            if (!isEmpty(groceryStore))
            {
                cout << "Enter start name for range: ";
                cin.ignore(); // Membersihkan newline character dari masukan sebelumnya
                cin.getline(startName, sizeof(startName));

                cout << "Enter end name for range: ";
                cin.getline(endName, sizeof(endName));

                searchByNameRange(groceryStore, startName, endName);
            }
            else
            {
                cout << "Product list is empty." << endl;
            }
            break;
        }
        case 4:
            if (!isEmpty(groceryStore))
            {
                sortByNameDescending(groceryStore);
            }
            else
            {
                cout << "Product list is empty." << endl;
            }
            break;
        case 5:
        { // Tambahkan case untuk menu menghapus produk
            if (!isEmpty(groceryStore))
            {
                char productCode[10];
                cout << "Enter product code to delete: ";
                cin >> productCode;
                deleteProduct(groceryStore, productCode);
            }
            else
            {
                cout << "Product list is empty." << endl;
            }
            break;
        }
        case 6:
        {
            int displayChoice;

            cout << "Choose display order for deleted products history:\n";
            cout << "1. In-order\n";
            cout << "2. Pre-order\n";
            cout << "3. Post-order\n";
            cout << "Enter your choice: ";
            cin >> displayChoice;

            DisplayOrder displayOrder;
            switch (displayChoice)
            {
            case 1:
                displayOrder = IN_ORDER;
                break;
            case 2:
                displayOrder = PRE_ORDER;
                break;
            case 3:
                displayOrder = POST_ORDER;
                break;
            default:
                cout << "Invalid choice. Defaulting to In-order display.\n";
                displayOrder = IN_ORDER;
                break;
            }
            displayDeletedProductsHistory(groceryStore, displayOrder);

            break;
        }
        case 0:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}