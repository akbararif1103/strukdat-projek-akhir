#include <iostream>
#include <cstring>
#include <algorithm>
#include <iomanip>

using namespace std;

// Enum untuk jenis urutan display
enum UrutanTampil
{
    SESUAI_URUTAN,
    PRE_URUTAN,
    POST_URUTAN
};

// Struktur data untuk produk
struct Produk
{
    char nama[50];
    double harga;
    char kode[10];
    // Tambahan informasi lainnya sesuai kebutuhan
};

struct RentangNama
{
    char namaAwal[50];
    char namaAkhir[50];
};

// Node untuk pohon biner
struct NodePohon
{
    Produk produk;
    NodePohon *kiri;
    NodePohon *kanan;

    NodePohon(const Produk &p) : produk(p), kiri(nullptr), kanan(nullptr) {}
};

// Node untuk produk yang dihapus
struct NodeProdukDihapus
{
    Produk data;
    NodeProdukDihapus *selanjutnya;

    NodeProdukDihapus(const Produk &p) : data(p), selanjutnya(nullptr) {}
};

// Struktur untuk manajemen toko
struct TokoKelontong
{
    Produk *daftarProduk; // Daftar produk
    int jumlahProduk;
    NodePohon *akar;                      // Akar dari pohon biner
    NodeProdukDihapus *produkDihapusAwal; // Linked list untuk menyimpan produk yang dihapus
    int jumlahProdukDihapus;
    static const int UKURAN_TABEL_HASH = 50; // Ukuran tabel hash
    Produk tabelHash[UKURAN_TABEL_HASH];     // Menggunakan satu dimensi untuk tabel hash

    TokoKelontong() : daftarProduk(nullptr), jumlahProduk(0), akar(nullptr), produkDihapusAwal(nullptr), jumlahProdukDihapus(0) {}
    ~TokoKelontong()
    {
        delete[] daftarProduk;
    }
};

// Fungsi hash sederhana
int fungsiHash(const char *kode);

// Fungsi untuk menambahkan produk
void tambahProduk(TokoKelontong &toko, const Produk &produk);

// Fungsi untuk menampilkan data produk menggunakan hash
void tampilkanDataProdukHash(const TokoKelontong &toko);

// Fungsi untuk mencari produk berdasarkan kode produk menggunakan hash
Produk cariDenganKode(const TokoKelontong &toko, const char *kodeProduk);

// Fungsi internal untuk memasukkan produk ke dalam pohon biner berdasarkan nama
NodePohon *sisipkanKePohonRentang(NodePohon *node, const Produk &produk);

// Fungsi untuk mencari dan menampilkan produk berdasarkan rentang nama pada pohon biner
void cariDenganRentangNamaPTB(const TokoKelontong &toko, const RentangNama &rentang);

void cariDenganRentangNamaPTBRekursif(const NodePohon *node, const RentangNama &rentang);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending (post-order)
void urutBerdasarkanNamaDescending(const NodePohon *node);

// Fungsi untuk mengurutkan produk berdasarkan nama secara descending pada pohon biner
void urutBerdasarkanNamaDescending(const TokoKelontong &toko);

// Fungsi untuk menambahkan produk yang dihapus ke dalam linked list
void tambahNodeProdukDihapus(TokoKelontong &toko, const Produk &produk);

// Fungsi internal untuk memasukkan produk ke dalam pohon biner
NodePohon *sisipkanKePohon(NodePohon *node, const Produk &produk);

// Fungsi untuk menghapus produk dari pohon biner
NodePohon *hapusDariPohon(NodePohon *node, const char *kodeProduk);

// Fungsi untuk menampilkan produk yang dihapus
void tampilkanRiwayatProdukDihapus(const TokoKelontong &toko, UrutanTampil urutan);

// Fungsi-fungsi untuk menampilkan produk yang dihapus dengan urutan tertentu
void tampilkanRiwayatProdukDihapusSesuaiUrutan(const NodeProdukDihapus *node);
void tampilkanRiwayatProdukDihapusPreUrutan(const NodeProdukDihapus *node);
void tampilkanRiwayatProdukDihapusPostUrutan(const NodeProdukDihapus *node);

// Fungsi untuk menampilkan produk yang dihapus
void tampilkanProdukDihapus(const Produk &produkDihapus);

// Prototipe fungsi untuk menampilkan tabel produk
void tampilkanTabelProduk(const TokoKelontong &toko);

NodePohon *hapusDariPohon(NodePohon *node, const Produk &produk);

bool kosong(const TokoKelontong &toko);

int fungsiHash(const char *kode)
{
    int hash = 0;
    int len = strlen(kode);

    for (int i = 0; i < len; ++i)
    {
        hash = 31 * hash + kode[i];
    }

    return abs(hash) % TokoKelontong::UKURAN_TABEL_HASH;
}

void tambahProduk(TokoKelontong &toko, const Produk &produk)
{
    // Periksa apakah kode produk sudah ada dalam toko
    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        if (strcmp(toko.daftarProduk[i].kode, produk.kode) == 0)
        {
            cout << "Produk dengan kode " << produk.kode << " sudah ada. Tidak dapat menambahkan produk duplikat." << endl;
            return;
        }
    }

    // Periksa apakah produk dengan kode yang sama telah dihapus sebelumnya
    NodeProdukDihapus *saatIni = toko.produkDihapusAwal;
    while (saatIni != nullptr)
    {
        if (strcmp(saatIni->data.kode, produk.kode) == 0)
        {
            cout << "Produk dengan kode " << produk.kode << " telah dihapus. Tidak dapat menambahkan produk yang sama lagi." << endl;
            return;
        }
        saatIni = saatIni->selanjutnya;
    }

    // Alokasi dinamis untuk menyimpan daftar produk
    Produk *temp = new Produk[toko.jumlahProduk + 1];
    memcpy(temp, toko.daftarProduk, sizeof(Produk) * toko.jumlahProduk);

    // Menambahkan produk ke daftar
    temp[toko.jumlahProduk] = produk;

    // Hapus daftar produk yang lama
    delete[] toko.daftarProduk;

    // Menetapkan daftar produk yang baru
    toko.daftarProduk = temp;
    ++toko.jumlahProduk;

    // Menambahkan produk ke tabel hash
    int indeks = fungsiHash(produk.kode);
    toko.tabelHash[indeks] = produk;

    // Menambahkan produk ke dalam pohon biner
    toko.akar = sisipkanKePohon(toko.akar, produk);

    cout << "Produk berhasil ditambahkan." << endl;
}

void tampilkanDataProdukInput(const TokoKelontong &toko)
{
    cout << "-------------Menu Tampilan Data---------------\n\n";
    cout << "+" << setfill('=') << setw(74) << "+" << endl;
    cout << left << setfill(' ') << setw(9) << "|" << setw(13) << "Kode"
         << "|";
    cout << left << setfill(' ') << setw(15) << " " << setw(20) << "Nama"
         << "|";
    cout << left << setfill(' ') << setw(5) << " " << setw(10) << "Harga"
         << "|" << endl;

    cout << right << "+" << setfill('=') << setw(74) << "+" << endl;

    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        cout << left << setfill(' ') << setw(9) << "|" << setw(13) << toko.daftarProduk[i].kode
             << "|";
        cout << left << setfill(' ') << setw(15) << " " << setw(20) << toko.daftarProduk[i].nama
             << "|";
        cout << left << setfill(' ') << setw(5) << " " << setw(10) << toko.daftarProduk[i].harga
             << "|" << endl;
    }
    cout << right << "+" << setfill('=') << setw(74) << "+" << endl;
}

Produk cariDenganKode(const TokoKelontong &toko, const char *kodeProduk)
{
    int indeks = fungsiHash(kodeProduk);

    // Cek apakah produk ditemukan pada indeks hash
    if (strcmp(toko.tabelHash[indeks].kode, kodeProduk) == 0)
    {
        return toko.tabelHash[indeks];
    }
    else
    {
        // Handle collision by searching linearly in the hash table
        for (int i = (indeks + 1) % TokoKelontong::UKURAN_TABEL_HASH; i != indeks; i = (i + 1) % TokoKelontong::UKURAN_TABEL_HASH)
        {
            if (strcmp(toko.tabelHash[i].kode, kodeProduk) == 0)
            {
                return toko.tabelHash[i];
            }
        }
    }

    // Product not found
    Produk notFound;
    notFound.nama[0] = '\0';
    return notFound;
}

NodePohon *sisipkanKePohonRentang(NodePohon *node, const Produk &produk)
{
    if (node == nullptr)
    {
        return new NodePohon(produk);
    }

    if (strcmp(produk.nama, node->produk.nama) <= 0)
    {
        node->kiri = sisipkanKePohonRentang(node->kiri, produk);
    }
    else
    {
        node->kanan = sisipkanKePohonRentang(node->kanan, produk);
    }

    return node;
}

void cariDenganRentangNamaPTB(const TokoKelontong &toko, const RentangNama &rentang)
{
    cout << "Produk dengan nama dalam rentang (" << rentang.namaAwal << " - " << rentang.namaAkhir << "):" << endl;
    cariDenganRentangNamaPTBRekursif(toko.akar, rentang);
}

void cariDenganRentangNamaPTBRekursif(const NodePohon *node, const RentangNama &rentang)
{
    if (node != nullptr)
    {
        cout << "Hasil : \ny";
        // Cek apakah produk dalam rentang nama
        if (strcmp(node->produk.nama, rentang.namaAwal) >= 0 && strcmp(node->produk.nama, rentang.namaAkhir) <= 0)
        {
            cout << " Kode Produk\t :" << node->produk.kode << endl;
            cout << " Nama Produk\t :" << node->produk.nama << endl;
            cout << " Harga Produk\t :" << node->produk.harga << endl;
        }

        // Cek apakah pencarian perlu dilanjutkan pada sub-pohon kiri
        if (strcmp(node->produk.nama, rentang.namaAwal) > 0)
        {
            cariDenganRentangNamaPTBRekursif(node->kiri, rentang);
        }

        // Cek apakah pencarian perlu dilanjutkan pada sub-pohon kanan
        if (strcmp(node->produk.nama, rentang.namaAkhir) < 0)
        {
            cariDenganRentangNamaPTBRekursif(node->kanan, rentang);
        }
    }
}

// Implementasi fungsi tampilkanDataProdukHash
void tampilkanDataProdukHash(const TokoKelontong &toko)
{
    cout << "Tampilan Data Produk Menggunakan Hash:" << endl;

    for (int i = 0; i < TokoKelontong::UKURAN_TABEL_HASH; ++i)
    {
        // Cek apakah slot tidak kosong dalam tabel hash
        if (toko.tabelHash[i].nama[0] != '\0')
        {
            cout << "Slot " << i << ":" << endl;
            cout << " Kode Produk\t :" << toko.tabelHash[i].kode << endl;
            cout << " Nama Produk\t :" << toko.tabelHash[i].nama << endl;
            cout << " Harga Produk\t :" << toko.tabelHash[i].harga << endl;
            cout << "-------------------------------------------" << endl;
        }
    }
}

void urutBerdasarkanNamaDescending(TokoKelontong &toko)
{
    // Menggunakan algoritma sort() dari C++ STL
    sort(toko.daftarProduk, toko.daftarProduk + toko.jumlahProduk, [](const Produk &a, const Produk &b)
         { return strcmp(a.nama, b.nama) > 0; });

    cout << "Produk diurutkan secara descending berdasarkan nama:" << endl;
    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        cout << toko.daftarProduk[i].kode << " - " << toko.daftarProduk[i].nama << " - " << toko.daftarProduk[i].harga << endl;
    }
}

void tambahNodeProdukDihapus(TokoKelontong &toko, const Produk &produk)
{
    // Buat node baru
    NodeProdukDihapus *nodeBaru = new NodeProdukDihapus(produk);

    // Tambahkan ke depan linked list
    nodeBaru->selanjutnya = toko.produkDihapusAwal;
    toko.produkDihapusAwal = nodeBaru;
}

void hapusProduk(TokoKelontong &toko, const char *kodeProduk)
{
    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        if (strcmp(toko.daftarProduk[i].kode, kodeProduk) == 0)
        {
            // Pindahkan produk yang dihapus ke dalam linked list produk yang dihapus
            tambahNodeProdukDihapus(toko, toko.daftarProduk[i]);

            // Hapus produk dari daftar produk yang masih ada
            for (int j = i; j < toko.jumlahProduk - 1; ++j)
            {
                toko.daftarProduk[j] = toko.daftarProduk[j + 1];
            }
            --toko.jumlahProduk;

            // Memanggil hapusDariPohon untuk menghapus dari pohon biner
            toko.akar = hapusDariPohon(toko.akar, kodeProduk);

            cout << "Produk berhasil dihapus." << endl;

            // Jika diperlukan, urutkan ulang daftar produk yang masih ada
            // Tidak perlu diurutkan jika hanya menghapus satu elemen
            if (toko.jumlahProduk > 1)
            {
                urutBerdasarkanNamaDescending(toko);
            }

            return;
        }
    }
    cout << "Produk dengan kode " << kodeProduk << " tidak ditemukan untuk dihapus." << endl;
}

NodePohon *temukanMin(NodePohon *node)
{
    while (node->kiri != nullptr)
    {
        node = node->kiri;
    }
    return node;
}

NodePohon *hapusDariPohon(NodePohon *node, const char *kodeProduk)
{
    if (node == nullptr)
    {
        return node;
    }

    if (strcmp(kodeProduk, node->produk.kode) < 0)
    {
        node->kiri = hapusDariPohon(node->kiri, kodeProduk);
    }
    else if (strcmp(kodeProduk, node->produk.kode) > 0)
    {
        node->kanan = hapusDariPohon(node->kanan, kodeProduk);
    }
    else
    {
        // Node dengan satu anak atau tanpa anak
        if (node->kiri == nullptr)
        {
            NodePohon *temp = node->kanan;
            delete node;
            return temp;
        }
        else if (node->kanan == nullptr)
        {
            NodePohon *temp = node->kiri;
            delete node;
            return temp;
        }

        // Node dengan dua anak, dapatkan node pengganti terkecil di subpohon kanan
        NodePohon *temp = temukanMin(node->kanan);

        // Salin data dari node pengganti ke node saat ini
        node->produk = temp->produk;

        // Hapus node pengganti
        node->kanan = hapusDariPohon(node->kanan, temp->produk.kode);
    }

    return node;
}

void tampilkanRiwayatProdukDihapus(const TokoKelontong &toko, UrutanTampil urutan)
{
    cout << "Riwayat Produk yang Dihapus:" << endl;

    if (urutan == SESUAI_URUTAN)
    {
        // Menampilkan berdasarkan urutan sesuai
        tampilkanRiwayatProdukDihapusSesuaiUrutan(toko.produkDihapusAwal);
    }
    else if (urutan == PRE_URUTAN)
    {
        // Menampilkan berdasarkan urutan pre
        tampilkanRiwayatProdukDihapusPreUrutan(toko.produkDihapusAwal);
    }
    else if (urutan == POST_URUTAN)
    {
        // Menampilkan berdasarkan urutan post
        tampilkanRiwayatProdukDihapusPostUrutan(toko.produkDihapusAwal);
    }
}

void tampilkanRiwayatProdukDihapusSesuaiUrutan(const NodeProdukDihapus *node)
{
    if (node != nullptr)
    {
        tampilkanRiwayatProdukDihapusSesuaiUrutan(node->selanjutnya);
        tampilkanProdukDihapus(node->data);
    }
}

void tampilkanRiwayatProdukDihapusPreUrutan(const NodeProdukDihapus *node)
{
    if (node != nullptr)
    {
        tampilkanProdukDihapus(node->data);
        tampilkanRiwayatProdukDihapusPreUrutan(node->selanjutnya);
    }
}

void tampilkanRiwayatProdukDihapusPostUrutan(const NodeProdukDihapus *node)
{
    if (node != nullptr)
    {
        tampilkanRiwayatProdukDihapusPostUrutan(node->selanjutnya);
        tampilkanProdukDihapus(node->data);
    }
}

void tampilkanProdukDihapus(const Produk &produkDihapus)
{
    cout << "Kode Produk: " << produkDihapus.kode << " - Nama Produk: " << produkDihapus.nama
         << " - Harga Produk: " << produkDihapus.harga << endl;
}

NodePohon *sisipkanKePohon(NodePohon *node, const Produk &produk)
{
    if (node == nullptr)
    {
        return new NodePohon(produk);
    }

    if (strcmp(produk.nama, node->produk.nama) <= 0)
    {
        node->kiri = sisipkanKePohon(node->kiri, produk);
    }
    else
    {
        node->kanan = sisipkanKePohon(node->kanan, produk);
    }

    return node;
}

bool kosong(const TokoKelontong &toko)
{
    return toko.jumlahProduk == 0;
}

void tampilkanTabelProduk(const TokoKelontong &toko)
{
    cout << "Tabel Produk:\n"
         << endl;
    cout << setw(10) << "Kode" << setw(30) << "Nama" << setw(15) << "Harga" << endl;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;

    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        cout << setw(10) << toko.daftarProduk[i].kode << setw(30) << toko.daftarProduk[i].nama << setw(15) << toko.daftarProduk[i].harga << endl;
    }
}

int main()
{
    // Contoh penggunaan
    TokoKelontong tokoKelontong;

    int pilihan = -1; // Inisialisasi agar loop pertama kali dijalankan
    while (pilihan != 0)
    {
        system("cls");
        cout << "\n------------|Menu|--------------\n\n";
        cout << " 1. Tambah Produk\n";
        cout << " 2. Tampilkan Semua Produk\n";
        cout << " 3. Searching (Kode)\n";
        cout << " 4. Searching (Rentang Nama)\n";
        cout << " 5. Sorting Descending (Nama)\n";
        cout << " 6. Hapus Produk\n";
        cout << " 7. Riwayat Produk yang Dihapus\n";
        cout << " 0. Keluar\n";
        cout << "\n================================\n\n";
        cout << "Masukkan pilihan Anda: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
        {
            system("cls");
            int jumlahBarang;
            cout << "Masukkan banyak jenis produk yang ingin Anda inputkan: ";
            cin >> jumlahBarang;

            system("cls");
            cout << "-------------Menu Input Data---------------\n\n";
            cout << "Masukkan Data Produk :" << endl
                 << endl;

            for (int i = 0; i < jumlahBarang; ++i)
            {
                Produk produkBaru;
                cout << " Nama Produk\t\t: ";
                cin.ignore();
                cin.getline(produkBaru.nama, sizeof(produkBaru.nama));
                cout << " Harga Produk\t\t: ";
                cin >> produkBaru.harga;
                cout << " Buat Kode Produk\t: ";
                cin >> produkBaru.kode;
                cout << endl;
                tambahProduk(tokoKelontong, produkBaru);
                cout << "\n-------------------------------------------" << endl;
            }

            break;
        }
        case 2:
        {
            system("cls");
            if (!kosong(tokoKelontong))
            {
                // Tampilkan data produk menggunakan struktur hash
                tampilkanDataProdukInput(tokoKelontong);
            }
            else
            {
                cout << "Daftar produk kosong." << endl;
            }
            break;
        }
        case 3:
        {
            system("cls");
            char kodeProduk[10];
            cout << "-------------Menu Pencarian Menggunakan Kode Produk---------------\n\n";
            cout << "Masukkan kode produk untuk dicari:";
            cin >> kodeProduk;

            cout << "\nHASIL : " << endl;

            Produk hasil = cariDenganKode(tokoKelontong, kodeProduk);
            if (hasil.nama[0] != '\0')
            {
                cout << " Kode Produk\t :" << hasil.kode << endl;
                cout << " Nama Produk\t :" << hasil.nama << endl;
                cout << " Harga Produk\t :" << hasil.harga << endl;
            }
            else
            {
                cout << " Produk dengan kode '" << kodeProduk << "' tidak ditemukan." << endl;
            }
            break;
        }

        case 4:
        {
            system("cls");
            char namaAwal[50];
            char namaAkhir[50];

            cout << "-------------Menu Pencarian Rentang Nama---------------\n\n";
            cout << "Masukkan nama awal rentang: ";
            cin.ignore();
            cin.getline(namaAwal, sizeof(namaAwal));

            cout << "Masukkan nama akhir rentang: ";
            cin.getline(namaAkhir, sizeof(namaAkhir));

            RentangNama rentang;
            strcpy(rentang.namaAwal, namaAwal);
            strcpy(rentang.namaAkhir, namaAkhir);

            cariDenganRentangNamaPTB(tokoKelontong, rentang);
            break;
        }

        case 5:
            if (!kosong(tokoKelontong))
            {
                system("cls");
                urutBerdasarkanNamaDescending(tokoKelontong);
            }
            else
            {
                cout << "Daftar produk kosong." << endl;
            }
            break;
        case 6:
        {
            if (!kosong(tokoKelontong))
            {
                // Tampilkan tabel produk sebelum menghapus
                system("cls");
                tampilkanTabelProduk(tokoKelontong);

                char kodeProduk[10];
                cout << "Masukkan kode produk untuk dihapus: ";
                cin >> kodeProduk;
                hapusProduk(tokoKelontong, kodeProduk);
            }
            else
            {
                cout << "Daftar produk kosong." << endl;
            }
            break;
        }

        case 7:
        {
            system("cls");
            int pilihanTampil;

            cout << "Pilih urutan tampilan untuk riwayat produk yang dihapus:\n";
            cout << "1. Sesuai urutan\n";
            cout << "2. Pre-urutan\n";
            cout << "3. Post-urutan\n";
            cout << "Masukkan pilihan Anda: ";
            cin >> pilihanTampil;

            UrutanTampil urutanTampil;
            switch (pilihanTampil)
            {
            case 1:
                urutanTampil = SESUAI_URUTAN;
                break;
            case 2:
                urutanTampil = PRE_URUTAN;
                break;
            case 3:
                urutanTampil = POST_URUTAN;
                break;
            default:
                cout << "Pilihan tidak valid. Kembali ke tampilan sesuai urutan secara default.\n";
                urutanTampil = SESUAI_URUTAN;
                break;
            }
            tampilkanRiwayatProdukDihapus(tokoKelontong, urutanTampil);

            break;
        }
        case 0:
            cout << "Keluar dari program. Selamat tinggal!\n";
            break;
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            break;
        }

        if (pilihan != 0)
        {
            char kembali;
            cout << "\nKembali ke menu utama? (y/n): ";
            cin >> kembali;
            if (tolower(kembali) != 'y')
            {
                pilihan = 0; // Keluar dari loop jika tidak ingin kembali ke menu utama
            }
        }
    }

    return 0;
}
