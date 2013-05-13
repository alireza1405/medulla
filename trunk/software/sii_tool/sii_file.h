#ifndef SII_FILE_H
#define SII_FILE_H

// This header file contains defines describing the contense of the header file.
// It is mostly addresses in the binary file of the values, but it also contains
// category IDs and the like.

// Defines of location of data in SII file
#define SII_CONFIG                  (0x0000*2)      // Start of Slave Information configuration area
#define SII_CATEGORIES              (0x0040*2)      // Start of Slave categories area

// Slave config Section
#define CONFIG_DATA                 (0x0000*2)      // Start of config data
#define CONFIG_CHECK                (0x0007*2)      // Address of config data checksum
#define VENDOR_ID                   (0x0008*2)      // Address of vendor ID in sii file
#define PRODUCT_CODE                (0x000A*2)
#define REV_NUMBER                  (0x000C*2)
#define SERIAL_NUMBER               (0x000E*2)
#define BOOTSTRAP_DATA              (0x0014*2)      // Start of Bootstrap data section
#define RX_MBOX_OFF                 (0x0018*2)
#define RX_MBOX_SIZE                (0x0019*2)
#define TX_MBOX_OFF                 (0x001A*2)
#define TX_MBOX_SIZE                (0x001B*2)
#define MBOX_PROTOCOL               (0x001C*2)
#define EEPROM_SIZE                 (0x003E*2)
#define VERSION                     (0x003F*2)

// Category section
#define CAT_TYPE_OFF                (0x0000*2)      // Offset from beginning of category entry of type value
#define CAT_SIZE_OFF                (0x0001*2)      // Offset from start of cateogry entry of size
#define CAT_DATA_OFF                (0x0002*2)      // Offset of begging of actual category data

// Category Types
#define CAT_TYPE_NOP                        0       // No category information, don't know why this is even a thing
#define CAT_TYPE_STRINGS                    10      // Strings category, all string data is stored here
#define CAT_TYPE_DATATYPES                  20      // Data Types category, for future use
#define CAT_TYPE_GENERAL                    30      // Generate slave information category
#define CAT_TYPE_FMMU                       40
#define CAT_TYPE_SYNCM                      41
#define CAT_TYPE_TXPDO                      50
#define CAT_TYPE_RXPDO                      51
#define CAT_TYPE_DC                         60
#define CAT_TYPE_END                    0xFFFF      // This category type ID indicates the end of the category section

// General Category Structure
#define GENERAL_CAT_LENGTH                (32)
#define GROUP_TYPE_INDEX_OFF          (0x0000)      // Offset of group string index in category (Index into strings)
#define IMG_INDEX_OFF                 (0x0001)
#define ORDER_INDEX_OFF               (0x0002)
#define NAME_INDEX_OFF                (0x0003)
#define COE_DETAILS_OFF               (0x0005)
#define FOE_DETAILS_OFF               (0x0006)
#define EOE_DETAILS_OFF               (0x0007)
#define FLAGS_OFF                     (0x000B)
#define EBUS_CURRENT_OFF              (0x000C)
#define GROUP_NAME_INDEX_OFF          (0x000E)
#define PHY_PORT_OFF                  (0x0010)

// Sync Manager Category structure, repeated for as many sync managers as used
#define SYNCM_CAT_LEN                      (8)
#define SYNCM_PHY_ADDR_OFF            (0x0000)
#define SYNCM_LENGTH_OFF              (0x0002)
#define SYNCM_CTRL_REG_OFF            (0x0004)
#define SYNCM_ENABLE_OFF              (0x0006)
#define SYNCN_TYPE_OFF                (0x0007)

// PDO Category strucure, repeated for each PDO
#define PDO_CAT_FIXED_LEN                  (8)      // Length of fixed section of PDO category in addition to size of entry list
#define PDO_INDEX_OFF                 (0x0000)      // Offset of PDO index from beginning of PDO
#define PDO_NENTRY_OFF                (0x0002)      // Offset of stored value of entries
#define PDO_SYNCM_OFF                 (0x0003)
#define PDO_SYNCRONIZATION            (0x0004)
#define PDO_NAME_INDEX_OFF            (0x0005)      // Index into strings category
#define PDO_ENTRY_START               (0x0008)      // Offset of begging of PDO entry list

// PDO Entry subcategory structure, repeated for each PDO entry
#define ENTRY_CAT_LENGTH                   (8)
#define ENTRY_INDEX_OFF               (0x0000)
#define ENTRY_SUBINDEX_OFF            (0x0002)
#define ENTRY_NAME_IDX_OFF            (0x0003)      // Index into strings category
#define ENTRY_DATA_TYPE_OFF           (0x0004)
#define ENTRY_BITLEN_OFF              (0x0005)

// DC Clock Category structure
#define DC_CAT_LENGTH                     (24)
#define DC_CYCLE_SYNC0_OFF            (0x0000)
#define DC_SHIFT_SYNC0_OFF            (0x0004)
#define DC_SHIFT_SYNC1_OFF            (0x0008)
#define DC_CYCLE_SYNC1_FACTOR_OFF     (0x000C)
#define DC_ASSIGN_ACTIVATE_OFF        (0x000E)
#define DC_CYCLE_SYNC0_FACTOR_OFF     (0x0010)
#define DC_SYNC_NAME_INDEX_OFF        (0x0012)      // Index into strings category

#endif // SII_FILE_H
