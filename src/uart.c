#include <uart.h>

int init_uart() {
    int uart_filestream = -1;

    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart_filestream == -1) {
        printf("Não foi possível iniciar a UART.\n");
    } else {
        printf("UART inicializada\n");
    }

    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);

    return uart_filestream;
}

void write_uart(int uart_filestream, char subcode) {
    unsigned char tx_buffer[9];

    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x23;
    tx_buffer[2] = subcode;

    char registration[4] = {0,2,5,1};

    memcpy(&tx_buffer[3], &registration, 4);

    short crc = calcula_CRC(tx_buffer, 7);

    memcpy(&tx_buffer[7], &crc, 2);

    if (uart_filestream != -1) {
        int count = write(uart_filestream, tx_buffer, 9);
        if (count < 0) {
            printf("Erro na UART\n");
        }
    }
}

void read_uart(int uart_filestream, unsigned char *rx_buffer) {
    int rx_length = read(uart_filestream, (void*)rx_buffer, 255);  

    if (rx_length < 0) {
        printf("Erro na leitura\n");
    } else if (rx_length == 0) {
        printf("Nenhum dado\n");
    } else {
        short crc = calcula_CRC(rx_buffer, rx_length - 2);
        short old_crc;
        memcpy(&old_crc, rx_buffer + (rx_length - 2), 2);

        if (crc != old_crc) {
            printf("Problema na UART\n");
            close_uart(uart_filestream);
        }

        rx_buffer[rx_length] = '\0';
    }
}

float request_uart_temp(int uart_filestream, char subcode) {

    write_uart(uart_filestream, subcode);

    sleep(1);

    unsigned char *rx_buffer;
    rx_buffer = (unsigned char*) malloc(9 * sizeof(unsigned char));
    read_uart(uart_filestream, rx_buffer);

    float resp;
    memcpy(&resp, rx_buffer+3, 4);

    free(rx_buffer);

    return resp;
}

int request_uart_user(int uart_filestream) {
    write_uart(uart_filestream, 0xC3);

    sleep(1);

    unsigned char *rx_buffer;
    rx_buffer = (unsigned char*) malloc(9 * sizeof(unsigned char));
    read_uart(uart_filestream, rx_buffer);

    int resp;
    memcpy(&resp, rx_buffer+3, 4);

    free(rx_buffer);

    return resp;
}

void send_uart_int(int uart_filestream, char subcode, int value) {
    unsigned char tx_buffer[13];

    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x16;
    tx_buffer[2] = subcode;

    char registration[4] = {0,2,5,1};

    memcpy(&tx_buffer[3], &registration, 4);

    memcpy(&tx_buffer[7], &value, 4);

    short crc = calcula_CRC(tx_buffer, 11);

    memcpy(&tx_buffer[11], &crc, 2);

    if (uart_filestream != -1) {
        int count = write(uart_filestream, tx_buffer, 13);
        if (count < 0) {
            printf("Erro na UART\n");
        }
    }
}

void send_uart_float(int uart_filestream, float value) {
    unsigned char tx_buffer[13];

    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x16;
    tx_buffer[2] = 0xD2;

    char registration[4] = {0,2,5,1};

    memcpy(&tx_buffer[3], &registration, 4);

    memcpy(&tx_buffer[7], &value, 4);

    short crc = calcula_CRC(tx_buffer, 11);

    memcpy(&tx_buffer[11], &crc, 2);

    if (uart_filestream != -1) {
        int count = write(uart_filestream, tx_buffer, 13);
        if (count < 0) {
            printf("Erro na UART\n");
        }
    }
}

void send_uart_bool(int uart_filestream, bool value, char subcode) {
    unsigned char tx_buffer[13];

    tx_buffer[0] = 0x01;
    tx_buffer[1] = 0x16;
    tx_buffer[2] = subcode;

    char registration[4] = {0,2,5,1};

    memcpy(&tx_buffer[3], &registration, 4);

    memcpy(&tx_buffer[7], &value, 1);

    short crc = calcula_CRC(tx_buffer, 8);

    memcpy(&tx_buffer[11], &crc, 2);

    if (uart_filestream != -1) {
        int count = write(uart_filestream, tx_buffer, 10);
        if (count < 0) {
            printf("Erro na UART\n");
        }
    }
}

void close_uart(int uart_filestream) {
  close(uart_filestream);
}