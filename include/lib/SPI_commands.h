enum SPI_command {
  CMD_TX = 0x1,
  CMD_RX = 0x2,

};

#define TX_FTCRL_LENGTH 6
#define TX_FTCRL_BASE_REGISTER 0x0
#define TX_FTCRL_SUB_ADDRESS 0x24

#define SYS_STATUS_LENGTH 6
#define SYS_STATUS_BASE_REGISTER 0x0
#define SYS_STATUS_SUB_ADDRESS 0x44

enum SYS_STATUS {
  TXFRS = 1 << 7,
  RXFR = 1 << 13,
};

enum transaction_mode {
  transaction_RD_RW = 0,
  transaction_mask8 = 1,
  transaction_mask16 = 2,
  transaction_mask32 = 3,
};

enum transaction_type {
  transaction_read = 0,
  transaction_write = 1,
};