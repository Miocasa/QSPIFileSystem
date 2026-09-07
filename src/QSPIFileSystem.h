#ifndef QSPI_FILE_SYSTEM_H
#define QSPI_FILE_SYSTEM_H

#include <Adafruit_SPIFlash.h>
#include <Adafruit_LittleFS.h>
#include <Arduino.h>

SPIFlash_Device_t const p25q16h_device = {
	.total_size = 1UL << 21,
	.start_up_time_us = 10000,
	.manufacturer_id = 0x85,
	.memory_type = 0x60,
	.capacity = 0x15,
	.max_clock_speed_mhz = 55,
	.quad_enable_bit_mask = 0x02,
	.has_sector_protection = false,
	.supports_fast_read = true,
	.supports_qspi = true,
};
/**
 * @brief QSPI Flash File System wrapper
 * Provides InternalFS-like interface for external QSPI flash
 *
 * Usage:
 *   QSPIFileSystem qspiFS(&externalFlash);
 *   qspiFS.begin();
 *   File f = qspiFS.open("/data.txt", FILE_O_READ);
 */
class QSPIFileSystem : public Adafruit_LittleFS
{
public:
	/**
	 * @brief Constructor
	 * @param flash Pointer to initialized Adafruit_SPIFlash object
	 */
	QSPIFileSystem(Adafruit_SPIFlash* flash, const SPIFlash_Device_t* flash_device = &p25q16h_device);
	// QSPIFileSystem();
	~QSPIFileSystem() override;

	/**
	 * @brief Initialize QSPI flash filesystem
	 * @param format_if_fail Format filesystem if initialization fails
	 * @return true if successful
	 */
	bool begin(bool format_if_fail = false);

	/**
	 * @brief Get total size of QSPI flash
	 * @return Size in bytes
	 */
	uint32_t getTotalSize() { return _flash->size(); }

	/**
	 * @brief Get used space (approximation)
	 * @return Used bytes
	 */
	uint32_t getUsedSize(); // todo finish function

	/**
	 * @brief Get available free space
	 * @return Free bytes
	 */
	uint32_t getFreeSpace() { return getTotalSize() - getUsedSize(); }

private:
	Adafruit_SPIFlash* _flash;
	lfs_config _cfg;
	const SPIFlash_Device_t* _flash_device;

	// LittleFS read/write callbacks
	static int _flash_read(const struct lfs_config* c, lfs_block_t block,
	                       lfs_off_t off, void* buffer, lfs_size_t size);
	static int _flash_prog(const struct lfs_config* c, lfs_block_t block,
	                       lfs_off_t off, const void* buffer, lfs_size_t size);
	static int _flash_erase(const struct lfs_config* c, lfs_block_t block);
	static int _flash_sync(const struct lfs_config* c);
};

#endif // QSPI_FILE_SYSTEM_H
