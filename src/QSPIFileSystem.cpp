#include "QSPIFileSystem.h"

QSPIFileSystem::QSPIFileSystem(Adafruit_SPIFlash* flash, const SPIFlash_Device_t* flash_device)
    : Adafruit_LittleFS(), _flash(flash), _cfg(), _flash_device(flash_device)
{
    memset(&_cfg, 0, sizeof(_cfg));
}

// QSPIFileSystem::QSPIFileSystem()
//     : Adafruit_LittleFS(), _flashTransport(new Adafruit_FlashTransport_QSPI()), _flash(nullptr)
// {
//     memset(&_cfg, 0, sizeof(_cfg));
// }

QSPIFileSystem::~QSPIFileSystem()
{
    end();
}

bool QSPIFileSystem::begin(bool format_if_fail)
{
    if (!_flash) return false;

    // _flash = new Adafruit_SPIFlash(_flashTransport);

    bool flashBegin = _flash->begin(&p25q16h_device);
    if (!flashBegin) return false;

    uint32_t jedec = _flash->getJEDECID();
    if (jedec == 0xFFFFFF || jedec == 0x000000 || !_flash)
    {
        LOG_LV1("QSpiFS", "ERROR: QSPI Flash not initialized!");
    }

    // _cfg.cache_size    = 16,

    _cfg.read_size = 256;
    _cfg.prog_size = 256;
    _cfg.block_size = 4096;
    _cfg.block_count = _flash->size() / _cfg.block_size;
    _cfg.lookahead = 128; // lookahead_siz;

    // callbacks
    _cfg.read = _flash_read;
    _cfg.prog = _flash_prog;
    _cfg.erase = _flash_erase;
    _cfg.sync = _flash_sync;
    _cfg.context = (void*)this;


    // Try to mount
    if (!Adafruit_LittleFS::begin(&_cfg))
    {
        LOG_LV1("QSpiFS", "LittleFS mount failed!");

        if (format_if_fail)
        {
            LOG_LV1("QSpiFS", "Formatting QSPI flash...");
            if (format())
            {
                LOG_LV1("QSpiFS", "Format successful, trying to mount again...");
                return Adafruit_LittleFS::begin(&_cfg);
            }
            LOG_LV1("QSpiFS", "Format failed!");
            return false;
        }
        return false;
    }

    LOG_LV1("QSpiFS", "QSPI LittleFS mounted successfully! (%dKB)", getTotalSize() / 1024);


    return true;
}

uint32_t QSPIFileSystem::getUsedSize()
{
    // Simple approximation by checking directory size
    // In a real implementation, you might traverse all files
    uint32_t used = 0;

    Adafruit_LittleFS_Namespace::File root = open("/");
    if (root)
    {
        // This is a simplified version
        // A more accurate implementation would recursively calculate
        root.close();
    }

    return used;
}

// LittleFS callback implementations
int QSPIFileSystem::_flash_read(const struct lfs_config* c, lfs_block_t block,
                                lfs_off_t off, void* buffer, lfs_size_t size)
{
    QSPIFileSystem* fs = static_cast<QSPIFileSystem*>(c->context);

    uint32_t addr = block * c->block_size + off;

    if (!fs->_flash->readBuffer(addr, static_cast<uint8_t*>(buffer), size))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

int QSPIFileSystem::_flash_prog(const struct lfs_config* c, lfs_block_t block,
                                lfs_off_t off, const void* buffer, lfs_size_t size)
{
    QSPIFileSystem* fs = static_cast<QSPIFileSystem*>(c->context);

    uint32_t addr = block * c->block_size + off;

    if (!fs->_flash->writeBuffer(addr, (uint8_t*)buffer, size))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

int QSPIFileSystem::_flash_erase(const struct lfs_config* c, lfs_block_t block)
{
    QSPIFileSystem* fs = static_cast<QSPIFileSystem*>(c->context);

    uint32_t addr = block * c->block_size;

    if (!fs->_flash->eraseSector(addr / 4096))
    {
        return LFS_ERR_IO;
    }

    return LFS_ERR_OK;
}

int QSPIFileSystem::_flash_sync(const struct lfs_config* c)
{
    // QSPI flash doesn't require sync
    // All operations are atomic
    return LFS_ERR_OK;
}
