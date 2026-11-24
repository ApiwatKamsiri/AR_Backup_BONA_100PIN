# IAR Post-Build: Automatic Firmware Builder

## 📋 Genel Bakış

Bu script, IAR Embedded Workbench build işleminden sonra otomatik olarak çalışarak üretim için hazır **4 farklı firmware dosyası** oluşturur:

### Özellikler
✅ **CRC32 checksum** ekleme (STM32 format, 0x3FF44)  
✅ **Metadata checksum** ekleme (negative byte, 0x3FF54)  
✅ **Bootloader işleme** (CRC32 + Checksum otomatik ekleme)  
✅ **Dinamik dosya adı** oluşturma (source code'dan version okuma)  
✅ **Factory image** (Bootloader + Application merge)  
✅ **OTA-ready image** (Metadata 0x04B00'e taşınmış)  
✅ **Binary format** (OTA transfer için optimize edilmiş)  

---

## � Sistem Gereksinimleri

### Windows (Önerilen - Production)
| Bileşen | Gereksinim | Durum |
|---------|-----------|-------|
| **OS** | Windows 10/11 | ✅ Native |
| **PowerShell** | 5.1+ (built-in) | ✅ Built-in |
| **IAR EW** | 8.0+ (Renesas) | ✅ Resmi destek |
| **srec_cat** | SRecord Tools | ⚙️ Kurulum gerekli |

**Kurulum:**
```powershell
# PowerShell zaten yüklü (Windows built-in)

# srec_cat kurulumu:
# 1. http://srecord.sourceforge.net/ adresinden indir
# 2. PATH'e ekle veya srec_cat.exe'yi project klasörüne kopyala
# 3. Test:
Get-Command srec_cat
```

### Linux (Experimental - CLI Only)
| Bileşen | Gereksinim | Durum |
|---------|-----------|-------|
| **OS** | Ubuntu 20.04+ | ⚠️ IAR yok |
| **PowerShell** | PowerShell Core 7.0+ | ⚙️ Kurulum gerekli |
| **srec_cat** | srecord paketi | ⚙️ apt install |

**Kurulum:**
```bash
# PowerShell Core kurulumu
wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb
sudo dpkg -i packages-microsoft-prod.deb
sudo apt update
sudo apt install powershell

# srec_cat kurulumu
sudo apt install srecord

# Script manuel çalıştırma (IAR yok)
pwsh scripts/build_app_with_crc_csum.ps1 -InputHex "app.hex"
```

### macOS (Experimental - CLI Only)
| Bileşen | Gereksinim | Durum |
|---------|-----------|-------|
| **OS** | macOS 11+ | ⚠️ IAR yok |
| **PowerShell** | PowerShell Core 7.0+ | ⚙️ brew install |
| **srec_cat** | srecord paketi | ⚙️ brew install |

**Kurulum:**
```bash
# PowerShell Core kurulumu
brew install powershell

# srec_cat kurulumu
brew install srecord

# Script manuel çalıştırma (IAR yok)
pwsh scripts/build_app_with_crc_csum.ps1 -InputHex "app.hex"
```

### 🎯 Önerilen Platform
**Windows** - IAR Embedded Workbench sadece Windows'ta çalışır. Linux/macOS sadece script'i manuel test için kullanılabilir.

---

## �📦 Üretilen Dosyalar

### 1. Application Hex (Base)
```
RF_6175242600_V01R20_V85R02_crc_csum.hex (571 KB)
```
- Temel application firmware
- CRC32 + Checksum eklenmiş
- Metadata @ 0x3FF00
- **Kullanım:** Test ve debugging

### 2. Factory Image (B Prefix)
```
RF_6175242600_V01R20_BV85R02_crc_csum.hex (609 KB)
```
- **B** = Bootloader included
- Bootloader (0x00000-0x04BFF) + Application (0x04C00+)
- **Kullanım:** İlk kart programlama (factory)

### 3. OTA Hex (O Prefix)
```
RF_6175242600_V01R20_OV85R02_crc_csum.hex (570 KB)
```
- **O** = OTA ready
- Metadata 0x04B00'e taşınmış (bootloader için)
- Original metadata @ 0x3FF00 kaldırılmış
- Type 04/05 records var (JFlash uyumlu)
- **Kullanım:** OTA sunucuya yükleme, debug

### 4. OTA Binary (O Prefix)
```
RF_6175242600_V01R20_OV85R02_crc_csum.bin (237 KB)
```
- Binary format (hex'ten %58 daha küçük)
- 0x04B00-0x40000 crop + offset çıkarılmış
- **Kullanım:** Gerçek OTA transfer (optimize boyut)

---

## 📝 Dosya Adı Formatı

Script, source code'daki version bilgilerini otomatik okur:

**Format:**
```
RF_<StockNo>_V<Version>R<Revision>_[B/O]V<CoreVer>R<CoreRev>_crc_csum.[hex/bin]
```

**Örnek:**
```
RF_6175242600_V01R20_BV85R02_crc_csum.hex
   │           │  │    ││  │
   │           │  │    ││  └─ Core Revision (02)
   │           │  │    │└─ Core Version (85)
   │           │  │    └─ B=Bootloader / O=OTA
   │           │  └─ Stock Revision (20, leading zero)
   │           └─ Stock Version (01, leading zero)
   └─ Stock Number (10 digit)
```

**Kaynak Dosyalar:**
- `config/rf_configs.c`:
  ```c
  #define _StockNo       "6175242600"
  #define _StockVersion  1
  #define _StockRevision 20
  ```
- `sources/system.h`:
  ```c
  #define UXPURE_CORE_VER 85
  #define UXPURE_CORE_REV 02
  ```

**Format Kuralları:**
- Version/Revision: 2 haneli (leading zero)
- Örnek: V=1 → V01, R=5 → R05

---

## ⚙️ IAR Konfigürasyonu

### Adım 1: IAR Project Options
1. IAR Embedded Workbench'i aç
2. **Project** → **Options...** (Alt+F7)

### Adım 2: Build Actions
1. Sol menüden **Build Actions** seç
2. **Post-build command line** alanına:

```cmd
powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\scripts\build_app_with_crc_csum.ps1"
```

### Adım 3: Test
1. **OK** ile kaydet
2. Project build et (F7)
3. Build log'da script output'u göreceksin

---

## 🏗️ Build İşlemi Akışı

```
IAR Build
   ↓
ALESTA_RCOS2.hex (391 KB)
   ↓
┌──────────────── Script Otomatik Çalışır ────────────────┐
│                                                          │
│ Step 1: Application CRC32 Ekleme (0x3FF44)              │
│ Step 2: Application Checksum Ekleme (0x3FF54)           │
│         → RF_..._V85R02_crc_csum.hex (571 KB)           │
│                                                          │
│ Step 3: Bootloader İşleme ve Factory Image              │
│    3.1: Bootloader CRC32 (0x03F44)                      │
│    3.2: Bootloader Checksum (0x3F54)                    │
│         → BootloaderONE_DBG_crc_csum.hex                │
│    3.3: Bootloader + Application Merge                  │
│         → RF_..._BV85R02_crc_csum.hex (609 KB)          │
│                                                          │
│ Step 4: OTA Hazırlama                                   │
│    4.1: Metadata 0x3FF00 → 0x04B00                      │
│    4.2: Original metadata kaldır                        │
│    4.3: Merge                                           │
│         → RF_..._OV85R02_crc_csum.hex (570 KB)          │
│                                                          │
│    4.4: Binary Conversion                               │
│         → RF_..._OV85R02_crc_csum.bin (237 KB)          │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

---

## 📂 Dosya Yapısı

```
RCOS2_Alesta/
├── config/
│   └── rf_configs.c              ← Version: StockNo, StockVersion, StockRevision
├── sources/
│   └── system.h                  ← Version: UXPURE_CORE_VER, UXPURE_CORE_REV
├── scripts/
│   ├── build_app_with_crc_csum.ps1  ← Ana script (Bootloader işleme dahil)
│   └── README_IAR_POSTBUILD.md      ← Bu dosya
├── Debug/Exe/
│   ├── ALESTA_RCOS2.hex             ← IAR output
│   ├── RF_..._V85R02_crc_csum.hex   ← Application
│   ├── RF_..._BV85R02_crc_csum.hex  ← Factory (Bootloader)
│   ├── RF_..._OV85R02_crc_csum.hex  ← OTA hex
│   └── RF_..._OV85R02_crc_csum.bin  ← OTA binary
└── Bootloader/
    └── BootloaderONE_RenesasRA2/
        └── BootloaderONE/Debug/Exe/
            ├── BootloaderONE_DBG.hex            ← Bootloader source
            └── BootloaderONE_DBG_crc_csum.hex   ← Script tarafından oluşturulur
```

---

## 🎯 Kullanım Senaryoları

### Senaryo 1: Yeni Kart (Factory Programming)
```
1. Build yap (F7)
2. RF_..._BV85R02_crc_csum.hex dosyasını al
3. JFlash ile karta yükle
4. ✅ Hem bootloader hem application yüklendi
```

### Senaryo 2: OTA Güncelleme
```
1. Build yap (F7)
2. RF_..._OV85R02_crc_csum.bin dosyasını al
3. OTA sunucuya yükle
4. Cihaz OTA ile günceller
5. ✅ Bootloader metadata'yı 0x04B00'de okur
```

### Senaryo 3: Debug
```
1. Build yap (F7)
2. RF_..._OV85R02_crc_csum.hex dosyasını al
3. JFlash ile açıp içeriği incele
4. ✅ Type 04/05 records var, açılır
```

---

## 🔧 Gelişmiş Kullanım

### Manuel Parametre
```powershell
powershell.exe -ExecutionPolicy Bypass -File "scripts\build_app_with_crc_csum.ps1" `
  -InputHex "MyApp.hex" `
  -OutputBaseName "CustomName" `
  -WorkPath "C:\Output"
```

### Farklı Build Configurations
**Debug:**
```cmd
powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\scripts\build_app_with_crc_csum.ps1"
```

**Release:**
```cmd
powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\scripts\build_app_with_crc_csum.ps1" -WorkPath "$PROJ_DIR$\Release\Exe"
```

**Bootloader:**
```cmd
powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\scripts\build_app_with_crc_csum.ps1"
```
(Bootloader için OTA step'leri atlanır)

---

## ✅ Doğrulama

Build sonrası kontrol edilecekler:

### 1. Build Log
```
============================================================
  ALESTA RCOS2 - CRC & Checksum Builder (srec_cat)
============================================================
[OK] Generated version string: RF_6175242600_V01R20_V85R02
[OK] CRC32 added successfully
[OK] Checksum added successfully
[OK] Factory image created
[OK] OTA image created
[OK] OTA binary created
```

### 2. Dosya Boyutları
- Application: ~571 KB
- Factory (B): ~609 KB  
- OTA Hex (O): ~570 KB
- OTA Bin (O): ~237 KB

### 3. Memory Layout
```
Factory Image:
  0x00000 - 0x04BFF : Bootloader (19 KB)
  0x04C00 - 0x3FFFF : Application (~250 KB)

OTA Image:
  0x04B00 - 0x04BFF : Metadata Header (256 bytes)
  0x04C00 - 0x3FEFF : Application Code
  0x3FF00 - 0x3FFFF : [Removed - metadata taşındı]
```

---

## 🐛 Sorun Giderme

### ❌ Script çalışmıyor
**Sebep:** PowerShell execution policy
```powershell
# Kontrol
Get-ExecutionPolicy

# Çözüm (Admin PowerShell)
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### ❌ srec_cat bulunamadı
**Sebep:** srec_cat PATH'de değil
```powershell
# Kontrol
Get-Command srec_cat

# Çözüm: SRecord tools'u yükle ve PATH'e ekle
```

### ❌ Version okunamadı
**Sebep:** Source dosyalar bulunamıyor
- `config/rf_configs.c` var mı?
- `sources/system.h` var mı?
- `#define` formatı doğru mu?

### ❌ Bootloader merge başarısız
**Sebep:** Bootloader source hex dosyası yok
```powershell
# Kontrol et (Source dosyası)
D:\RD_REPO\Bootloader\BootloaderONE_RenesasRA2\BootloaderONE\Debug\Exe\BootloaderONE_DBG.hex

# Not: Script otomatik olarak CRC/Checksum ekler, manuel işlem gerekmez
```

### ❌ JFlash "areas overlap" hatası
**Sebep:** Eski script versiyonu (Type 04/05 kaldırılmıştı)
- Script güncel mi? (2026-01-13+)
- Type 04/05 records hex'te var mı?

---

## 📊 Memory Map

### Application Memory (0x04C00-0x3FFFF)
```
0x04C00 ─┬─ Vector Table
         │
         ├─ Code Section
         │
0x3FF00 ─┼─ Metadata (256 bytes)
         │  ├─ 0x3FF00: Signature "APPL"
         │  ├─ 0x3FF44: CRC32 (4 bytes)
         │  ├─ 0x3FF54: Checksum (1 byte)
         │  └─ ...
0x40000 ─┴─ End of Flash
```

### OTA Image Memory
```
0x04B00 ─┬─ Metadata Header (256 bytes) ← Bootloader okur
0x04C00 ─┼─ Application Start
         │
0x3FF00 ─┼─ [Empty - metadata burada yok]
0x40000 ─┴─ End
```

---

## 🚀 Başarı Durumu

✅ **Factory Programming:**  
→ B dosyası ile ilk kart programlama başarılı

✅ **OTA Update:**  
→ O dosyası (bin) ile OTA güncelleme başarılı

✅ **Automation:**  
→ Manuel JFlash işlemi tarihe karıştı

---

## � Destek

Sorun yaşarsanız:
1. Build log'u kontrol edin
2. Script output'unu okuyun (renkli mesajlar)
3. File sizes doğruluğunu check edin
4. Memory addresses doğru mu bakın

---

**Son Güncelleme:** 2026-01-16  
**Script Version:** 2.1 (Bootloader Auto-Processing)  
**Status:** ✅ Production Ready

---

## 📝 Değişiklik Geçmişi

### v2.1 (2026-01-16)
- ✅ Bootloader CRC32/Checksum otomatik işleme eklendi
- ✅ Harici txt dosya bağımlılığı kaldırıldı
- ✅ Application ve Bootloader işleme mantığı birleştirildi
- ✅ Script tamamen bağımsız hale getirildi

### v2.0 (2026-01-13)
- ✅ OTA image desteği eklendi
- ✅ Binary format conversion
- ✅ Metadata relocation (0x3FF00 → 0x04B00)
- ✅ Factory image (Bootloader merge)
- ✅ Dinamik version naming

### v1.0 (Initial)
- ✅ Basic CRC32/Checksum ekleme
