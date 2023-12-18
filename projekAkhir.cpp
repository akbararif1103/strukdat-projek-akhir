#include <iostream>
#include <cstring>

using namespace std;

// Enum untuk jenis urutan display
enum DisplayOrder {
    IN_ORDER,
    PRE_ORDER,
    POST_ORDER
};

// Struktur data untuk produk
struct Product {
    char name[50];
    double price;
    char code[10];
    // Tambahan informasi lainnya sesuai kebutuhan
};

// Node untuk pohon biner
struct TreeNode {
    Product product;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Product& p) : product(p), left(nullptr), right(nullptr) {}
};

// Struktur untuk manajemen toko
struct GroceryStore {
    Product* productList;  // Daftar produk
    int productCount;
    static const int HASH_TABLE_SIZE = 50;  // Ukuran tabel hash
    Product hashTable[HASH_TABLE_SIZE];     // Menggunakan satu dimensi untuk tabel hash
    TreeNode* root;  // Akar dari pohon biner
    Product deletedProducts[50];  // Menggunakan array untuk menyimpan produk yang dihapus
    int deletedProductCount;

    GroceryStore() : productList(nullptr), productCount(0), root(nullptr), deletedProductCount(0) {}
    ~GroceryStore() {
        delete[] productList;
    }
};

// Fungsi hash sederhana
int hashFunction(const char* code);

// Fungsi untuk menambahkan produk
void addProduct(GroceryStore& store, const Product& product);

// Fungsi untuk mencari produk berdasarkan kode produk menggunakan hash
Product searchByCode(const GroceryStore& store, const char* productCode);

// Fungsi untuk mencari dan menampilkan produk berdasarkan rentang nama pada pohon biner
void searchByNameRange(const TreeNode* node, const char* startName, const char* endName);

// Fungsi untuk mencari dan menampilkan produk berdasarkan rentang nama pada pohon biner
void searchByNameRange(const GroceryStore& store, const char* startName, const char* endName);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending (post-order)
void sortByNameDescending(const TreeNode* node);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending pada pohon biner
void sortByNameDescending(const GroceryStore& store);

// Fungsi untuk menghapus produk berdasarkan kode produk
void deleteProduct(GroceryStore& store, const char* productCode);

// Function to display deleted products history
// Fungsi untuk menampilkan riwayat produk yang dihapus berdasarkan urutan tertentu
void displayDeletedProductsHistory(const GroceryStore& store, DisplayOrder order);

// Function to display deleted products history from the tree
void displayDeletedProductsHistory(const TreeNode* node, DisplayOrder order);

// Deklarasi fungsi
void displayDeletedProduct(const Product& deletedProduct);
void displayDeletedProductsHistoryInOrder(const TreeNode* node);
void displayDeletedProductsHistoryPreOrder(const TreeNode* node);
void displayDeletedProductsHistoryPostOrder(const TreeNode* node);

// Fungsi internal untuk memasukkan produk ke dalam pohon biner
TreeNode* insertIntoTree(TreeNode* node, const Product& product);

// Fungsi untuk menghapus produk dari pohon biner
TreeNode* deleteFromTree(TreeNode* node, const Product& product);

// Fungsi untuk memeriksa apakah daftar produk kosong
bool isEmpty(const GroceryStore& store);

int main() {
    // Contoh penggunaan
    GroceryStore groceryStore;

    int choice;
    do {
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

        switch (choice) {
            case 1: {
                Product newProduct;
                cout << "Enter product name: ";
                cin >> newProduct.name;
                cout << "Enter product price: ";
                cin >> newProduct.price;
                cout << "Enter product code: ";
                cin >> newProduct.code;
                addProduct(groceryStore, newProduct);
                break;
            }
            case 2: {
                char productCode[10];
                cout << "Enter product code to search: ";
                cin >> productCode;
                Product result = searchByCode(groceryStore, productCode);
                if (result.name[0] != '\0') {
                    cout << "Product Found: " << result.name << " - " << result.price << endl;
                } else {
                    cout << "Product Not Found." << endl;
                }
                break;
            }
            case 3: {
                char startName[50], endName[50];
                cout << "Enter start name for range: ";
                cin >> startName;
                cout << "Enter end name for range: ";
                cin >> endName;
                if (!isEmpty(groceryStore)) {
                    searchByNameRange(groceryStore, startName, endName);
                } else {
                    cout << "Product list is empty." << endl;
                }
                break;
            }
            case 4:
                if (!isEmpty(groceryStore)) {
                    sortByNameDescending(groceryStore);
                } else {
                    cout << "Product list is empty." << endl;
                }
                break;
            case 5: {  // Tambahkan case untuk menu menghapus produk
                if (!isEmpty(groceryStore)) {
                    char productCode[10];
                    cout << "Enter product code to delete: ";
                    cin >> productCode;
                    deleteProduct(groceryStore, productCode);
                } else {
                    cout << "Product list is empty." << endl;
                }
                break;
            }
            case 6: {
                int displayChoice;

                cout << "Choose display order for deleted products history:\n";
                cout << "1. In-order\n";
                cout << "2. Pre-order\n";
                cout << "3. Post-order\n";
                cout << "Enter your choice: ";
                cin >> displayChoice;

                DisplayOrder displayOrder;
                switch (displayChoice) {
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

// Implementasi fungsi-fungsi

int hashFunction(const char* code) {
    int sum = 0;
    for (int i = 0; code[i] != '\0'; ++i) {
        sum += code[i];
    }
    return sum % GroceryStore::HASH_TABLE_SIZE;
}

void addProduct(GroceryStore& store, const Product& product) {
    // Periksa apakah kode produk sudah ada dalam toko
    for (int i = 0; i < store.productCount; ++i) {
        if (strcmp(store.productList[i].code, product.code) == 0) {
            cout << "Product with code " << product.code << " already exists. Cannot add duplicate products." << endl;
            return;
        }
    }

    // Alokasi dinamis untuk menyimpan daftar produk
    if (store.productCount == 0) {
        store.productList = new Product[1];
    } else {
        Product* temp = new Product[store.productCount + 1];
        memcpy(temp, store.productList, sizeof(Product) * store.productCount);
        delete[] store.productList;
        store.productList = temp;
    }

    // Menambahkan produk ke daftar
    store.productList[store.productCount++] = product;

    // Menambahkan produk ke tabel hash
    int index = hashFunction(product.code);
    store.hashTable[index] = product;

    // Menambahkan produk ke dalam pohon biner
    store.root = insertIntoTree(store.root, product);

    cout << "Product added successfully." << endl;
}


Product searchByCode(const GroceryStore& store, const char* productCode) {
    int index = hashFunction(productCode);
    return store.hashTable[index];
}

void searchByNameRange(const TreeNode* node, const char* startName, const char* endName) {
    if (node != nullptr) {
        if (strcmp(node->product.name, startName) >= 0 && strcmp(node->product.name, endName) <= 0) {
            cout << node->product.name << " - " << node->product.price << endl;
        }

        if (strcmp(node->product.name, startName) > 0) {
            searchByNameRange(node->left, startName, endName);
        }

        if (strcmp(node->product.name, endName) < 0) {
            searchByNameRange(node->right, startName, endName);
        }
    }
}

void searchByNameRange(const GroceryStore& store, const char* startName, const char* endName) {
    cout << "Products in the range " << startName << " to " << endName << ":" << endl;
    searchByNameRange(store.root, startName, endName);
}

void sortByNameDescending(const TreeNode* node) {
    if (node != nullptr) {
        sortByNameDescending(node->left);
        sortByNameDescending(node->right);
        cout << node->product.name << " - " << node->product.price << endl;
    }
}

void sortByNameDescending(const GroceryStore& store) {
    sortByNameDescending(store.root);
    cout << "Products sorted in descending order by name (post-order):" << endl;
}

void deleteProduct(GroceryStore& store, const char* productCode) {
    int index = hashFunction(productCode);
    for (int i = 0; i < store.productCount; ++i) {
        if (strcmp(store.productList[i].code, productCode) == 0) {
            // Pindahkan produk yang dihapus ke dalam daftar produk yang dihapus
            store.deletedProducts[store.deletedProductCount++] = store.productList[i];

            // Hapus produk dari daftar produk
            store.productList[i] = store.productList[store.productCount - 1];
            --store.productCount;

            // Hapus produk dari tabel hash
            store.hashTable[index] = Product{"", 0.0, ""};

            // Hapus produk dari pohon biner
            store.root = deleteFromTree(store.root, store.productList[i]);

            cout << "Product deleted successfully." << endl;
            return;
        }
    }
    cout << "Product with code " << productCode << " not found for deletion." << endl;
}

void displayDeletedProductsHistory(const GroceryStore& store, DisplayOrder order) {
    cout << "Riwayat Produk Yang Dihapus:" << endl;

    if (order == IN_ORDER) {
        // Menampilkan berdasarkan urutan in-order
        displayDeletedProductsHistoryInOrder(store.root);
    } else if (order == PRE_ORDER) {
        // Menampilkan berdasarkan urutan pre-order
        displayDeletedProductsHistoryPreOrder(store.root);
    } else if (order == POST_ORDER) {
        // Menampilkan berdasarkan urutan post-order
        displayDeletedProductsHistoryPostOrder(store.root);
    }
}


void displayDeletedProductsHistoryInOrder(const TreeNode* node) {
    if (node != nullptr) {
        displayDeletedProductsHistoryInOrder(node->left);
        displayDeletedProduct(node->product);
        displayDeletedProductsHistoryInOrder(node->right);
    }
}

void displayDeletedProductsHistoryPreOrder(const TreeNode* node) {
    if (node != nullptr) {
        displayDeletedProduct(node->product);
        displayDeletedProductsHistoryPreOrder(node->left);
        displayDeletedProductsHistoryPreOrder(node->right);
    }
}

void displayDeletedProductsHistoryPostOrder(const TreeNode* node) {
    if (node != nullptr) {
        displayDeletedProductsHistoryPostOrder(node->left);
        displayDeletedProductsHistoryPostOrder(node->right);
        displayDeletedProduct(node->product);
    }
}

void displayDeletedProduct(const Product& deletedProduct) {
    cout << "Product Code: " << deletedProduct.code << " - Product Name: " << deletedProduct.name
         << " - Product Price: " << deletedProduct.price << endl;
}


TreeNode* insertIntoTree(TreeNode* node, const Product& product) {
    if (node == nullptr) {
        return new TreeNode(product);
    }

    if (strcmp(product.name, node->product.name) <= 0) {
        node->left = insertIntoTree(node->left, product);
    } else {
        node->right = insertIntoTree(node->right, product);
    }

    return node;
}

TreeNode* deleteFromTree(TreeNode* node, const Product& product) {
    if (node == nullptr) {
        return node;
    }

    if (strcmp(product.name, node->product.name) < 0) {
        node->left = deleteFromTree(node->left, product);
    } else if (strcmp(product.name, node->product.name) > 0) {
        node->right = deleteFromTree(node->right, product);
    } else {
        // Node dengan satu anak atau tanpa anak
        if (node->left == nullptr) {
            TreeNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            TreeNode* temp = node->left;
            delete node;
            return temp;
        }

        // Node dengan dua anak, dapatkan node pengganti terkecil di subpohon kanan
        TreeNode* temp = node->right;
        while (temp->left != nullptr) {
            temp = temp->left;
        }

        // Salin data dari node pengganti ke node saat ini
        node->product = temp->product;

        // Hapus node pengganti
        node->right = deleteFromTree(node->right, temp->product);
    }

    return node;
}

bool isEmpty(const GroceryStore& store) {
    return store.productCount == 0;
}
