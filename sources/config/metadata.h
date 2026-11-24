/** @file       metadata.h
 *  @brief      Modern metadata structure, constants and public API for image validation & OTA control.
 *  @copyright  (c) 2025-Beko - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Beko.
 *  @author     Samet Ak
 *  @date       22.10.2025
 */
#ifndef METADATA_H
#define METADATA_H

#include <stdint.h>


/** Total size in bytes reserved for a single metadata block. */
/** @def METADATA_SIZE
 *  @brief Total size in bytes reserved for a single metadata block.
 */
#define METADATA_SIZE   (0x100UL)


/** Signature for bootloader image record. */
/** @def META_IMAGE_SIGN_BOOTLOADER
 *  @brief Signature for bootloader image record.
 */
#define META_IMAGE_SIGN_BOOTLOADER  "BOOT"
/** Signature for application image record. */
/** @def META_IMAGE_SIGN_APPLICATION
 *  @brief Signature for application image record.
 */
#define META_IMAGE_SIGN_APPLICATION "APPL"
/** Signature for table / ancillary record. */
/** @def META_IMAGE_SIGN_TABLE
 *  @brief Signature for table / ancillary record.
 */
#define META_IMAGE_SIGN_TABLE       "TABL"


/** OTA not requested flag value. */
#define META_OTA_NULLDATA       ((uint8_t) 0x00)
/** OTA pending/requested flag value. */
#define META_OTA_REQUESTED      ((uint8_t) 0x40)
/** OTA valid flag value. */
#define META_OTA_IMAGE_VALID    ((uint8_t) 0x4C)
/** OTA in progress flag value. */
#define META_OTA_IN_PROGRESS    ((uint8_t) 0x42)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Product type enumeration describing target device category.
 */
/** \defgroup METADATA_TYPES Types
 *  @brief Core type definitions for metadata module.
 *  @{ */
typedef enum {
    PROD_ID_INVALID = 0,      /**< Uninitialized / invalid product id. */
    PROD_ID_REFRIGERATOR,     /**< Refrigerator platform. */
    PROD_ID_WASHING_MACHINE,  /**< Washing machine platform. */
    PROD_ID_DISH_WASHER,      /**< Dish washer platform. */
    PROD_ID_DRYER,            /**< Dryer platform. */
    PROD_ID_OVEN,             /**< Oven platform. */
    PROD_ID_HOB,              /**< Hob platform. */
    PROD_ID_TCM,              /**< Turkish Coffee Machine platform. */
    PROD_ID_SDA,              /**< Small Domestic Appliances platform. */
    PROD_ID_MAX,              /**< Sentinel (not a valid product). */
} t_en_product_type;

/**
 * @brief Packed metadata structure describing an image and related metadata blocks.
 *
 * Overall layout (annotated sizes):
 *  - header (20 bytes)
 *  - image  (52 bytes)
 *  - other_metadatas (3 * 4 = 12 bytes on 32-bit) pointers to sibling metadata
 *  - checksum (1 byte) additive ones' complement + 1 up to checksum field
 *  - is_ota_pending (1 byte) OTA flag
 *  - reserved (170 bytes) future use / padding up to METADATA_SIZE
 */
typedef struct __attribute__((packed)) st_metadata
{
    struct {
        char    image_sign[4];      /**< Image signature: BOOT/APPL/TABL. */
        uint8_t version;            /**< Metadata structure version. */
        uint8_t revision;           /**< Metadata structure revision. */
        uint8_t reserved[14];       /**< Reserved padding / future fields. */
    } header;                       /**< Header section (20 bytes). */
    struct {
        t_en_product_type product;  /**< Product category. */
        union {
            char product[32];       /**< Product serial / identifier. */
            char firmware[32];      /**< Alternative firmware id usage. */
        } serial;                   /**< Serial or firmware identification. */
        char     version[7];        /**< Image version string (not null-terminated if len==7). */
        uint32_t start_addr;        /**< Absolute start address of image in flash. */
        uint32_t length;            /**< Image length in bytes. */
        uint32_t crc32;             /**< CRC32 over the image (if calculator registered). */
    } image;                        /**< Image descriptor section (52 bytes). */
    struct st_metadata* other_metadatas[3]; /**< Pointers to up to three related metadata blocks. */
    uint8_t  checksum;              /**< Additive checksum of bytes prior to this field. */
    uint8_t  is_ota_pending;        /**< OTA request flag (META_OTA_REQUESTED / META_OTA_NULLDATA). */
    uint8_t  reserved[170];         /**< Reserved for future expansion. */
} t_st_metadata;
/** @} */

extern const t_st_metadata rf_app_meta;

#ifdef __cplusplus
}
#endif

#endif // METADATA_H
