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
    Produk *daftarProduk;                     // Daftar produk
    int jumlahProduk;
    NodePohon *akar;                          // Akar dari pohon biner
    NodeProdukDihapus *produkDihapusAwal;    // Linked list untuk menyimpan produk yang dihapus
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

// Fungsi untuk mencari produk berdasarkan kode produk menggunakan hash
Produk cariDenganKode(const TokoKelontong &toko, const char *kodeProduk);

// Fungsi untuk mencari dan menampilkan produk berdasarkan rentang nama pada pohon biner
void cariDenganRentangNama(const TokoKelontong &toko, const char *namaAwal, const char *namaAkhir);

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
    int jumlah = 0;
    for (int i = 0; kode[i] != '\0'; ++i)
    {
        // Menambahkan kode ASCII huruf ke jumlah
        jumlah += tolower(kode[i]);
    }
    return jumlah % TokoKelontong::UKURAN_TABEL_HASH;
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

Produk cariDenganKode(const TokoKelontong &toko, const char *kodeProduk)
{
    int indeks = fungsiHash(kodeProduk);
        return toko.tabelHash[indeks];
}

void cariDenganNama(const TokoKelontong &toko, const char *nama)
{
    bool ditemukan = false;

    cout << "Produk dengan nama yang sesuai:" << endl;
    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        if (strstr(toko.daftarProduk[i].nama, nama) != nullptr)
        {
            cout << toko.daftarProduk[i].nama << " - " << toko.daftarProduk[i].harga << endl;
            ditemukan = true;
        }
    }

    if (!ditemukan)
    {
        cout << "Tidak ada produk dengan nama yang sesuai." << endl;
    }
}


void urutBerdasarkanNamaDescending(TokoKelontong &toko)
{
    // Menggunakan algoritma sort() dari C++ STL
    sort(toko.daftarProduk, toko.daftarProduk + toko.jumlahProduk, [](const Produk &a, const Produk &b) {
        return strcmp(a.nama, b.nama) > 0;
    });

    cout << "Produk diurutkan secara descending berdasarkan nama:" << endl;
    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        cout << toko.daftarProduk[i].nama << " - " << toko.daftarProduk[i].harga << endl;
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
    cout << "Tabel Produk:" << endl;
    cout << setw(10) << "Kode" << setw(15) << "Harga" << setw(30) << "Nama" << endl;
    cout << setfill('-') << setw(55) << "-" << setfill(' ') << endl;

    for (int i = 0; i < toko.jumlahProduk; ++i)
    {
        cout << setw(10) << toko.daftarProduk[i].kode << setw(15) << toko.daftarProduk[i].harga << setw(30) << toko.daftarProduk[i].nama << endl;
    }
}


int main()
{
    // Contoh penggunaan
    TokoKelontong tokoKelontong;
   

    int pilihan;
    do
    {
        cout << "\n------------|Menu|--------------\n\n";
        cout << " 1. Tambah Produk\n";
        cout << " 2. Searching (Kode)\n";
        cout << " 3. Searching (Rentang Nama)\n";
        cout << " 4. Sorting Descending (Nama)\n";
        cout << " 5. Hapus Produk\n";
        cout << " 6. Riwayat Produk yang Dihapus\n";
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
            cout << "Masukkan Data Produk :" << endl << endl;

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
            char kodeProduk[10];
            cout << "Masukkan kode produk untuk dicari: ";
            cin >> kodeProduk;
            Produk hasil = cariDenganKode(tokoKelontong, kodeProduk);
            if (hasil.nama[0] != '\0')
            {
                cout << "Produk Ditemukan: " << hasil.nama << " - " << hasil.harga << endl;
            }
            else
            {
                cout << "Produk Tidak Ditemukan." << endl;
            }
            break;
        }
        case 3:
{
    char namaCari[50];
    if (!kosong(tokoKelontong))
    {
        cout << "Masukkan nama produk yang dicari: ";
        cin.ignore(); // Membersihkan karakter newline dari input sebelumnya
        cin.getline(namaCari, sizeof(namaCari));
        cariDenganNama(tokoKelontong, namaCari);
    }
    else
    {
        cout << "Daftar produk kosong." << endl;
    }
    break;
}

        case 4:
            if (!kosong(tokoKelontong))
            {
                urutBerdasarkanNamaDescending(tokoKelontong);
            }
            else
            {
                cout << "Daftar produk kosong." << endl;
            }
            break;
       case 5:
{
    if (!kosong(tokoKelontong))
    {
        // Tampilkan tabel produk sebelum menghapus
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

        case 6:
        {
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
    } while (pilihan != 0);

    return 0;
}


