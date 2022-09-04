#include <main.h>

pthread_t control_threads[3];

struct MenuDefined {
    int time;
    float tr;
};

const struct MenuDefined frango = {10, 40.0};
const struct MenuDefined peixe = {6, 40.0};
const struct MenuDefined carne = {10, 45.0};
const struct MenuDefined legumes = {4, 35.0};
const struct MenuDefined pao = {3, 35.0};

struct bme280_dev bme280;
int uart_filestream, uart_resp = 0, uart_on_off = 0, temp = 20, menu_defined = 0, count_time = 0, uart_resp_time = 0;
float ti = 0, te = 0, tr = 0;  
int temp_array [10];
int second_array [10];
double pid = 0;
bool start_time = false, is_finished = false, in_terminal = false;

int main() {
    set_up();
    signal(SIGINT, end);

    do{
        uart_on_off = request_uart_user(uart_filestream);
    }while (uart_on_off != 1);

    send_uart_bool(uart_filestream, 1, 0xD3);
    print_msg_on_display("Iniciando...");

    start();

    pthread_create(&control_threads[0], NULL,*menu, NULL);
    pthread_create(&control_threads[1], NULL,*control, NULL);

    pthread_join(control_threads[0], NULL);
    pthread_join(control_threads[1], NULL);

    return 0;
}

void set_up(){
    wiringPiSetup();

    pid_configura_constantes(30.0, 0.2, 400.0);
    uart_filestream = init_uart();
    bme280 = init_bme();
    create_csv();
    lcd_init();

    fan_off();
    resistor_off();

    send_uart_bool(uart_filestream, 0, 0xD3);

    FILE* reflow = fopen("log.csv", "r");
    int count = 0;

    if(!reflow){
        printf("Erro ao abrir o arquivo\n");
    }

    char line[100];
    char *sp;

    while(fgets(line, 100, reflow) != NULL){
        if (count == 0){
            count++;
            continue;
        }

        sp = strtok(line, ",");
        second_array[count-1] = atoi(sp);

        sp = strtok(NULL, ",");
        temp_array[count-1] = atoi(sp);

        count++;
    }
    fclose(reflow);

    printf("Sistema inicializado\n"); 
}

void start(){

    send_uart_bool(uart_filestream, 0, 0xD4);
    do{

        tr = request_uart_temp(uart_filestream, 0xC2);

        uart_resp = request_uart_user(uart_filestream);

        if (uart_resp == 5){
            temp = temp + 20;
            send_uart_int(uart_filestream, 0xD6, (temp/20));
            uart_resp = 0;
        }

        if(uart_resp == 6 && temp > 20){
            temp = temp - 20;
            send_uart_int(uart_filestream, 0xD6, (temp/20));
            uart_resp = 0;
        }

        if (uart_resp == 7){
            
            if(menu_defined >= 5){
                menu_defined = 1;
            }else{
                menu_defined++;
            }
            
            handle_menu();
            uart_resp = 0;
        }
    }while(uart_resp != 3);

    send_uart_bool(uart_filestream, 1, 0xD5);
}

void reset(){
    uart_resp = 0;
    uart_resp_time = 0;
    uart_on_off = 0;
    temp = 20;
    menu_defined = 0;
    count_time = 0;
    ti = 0; 
    te = 0; 
    tr = 0;  
    pid = 0;
    start_time = false;
    is_finished = false;
    in_terminal = false;
}

void end(){
    print_msg_on_display("Desligando...");
    send_uart_bool(uart_filestream, 0, 0xD3);
    close_uart(uart_filestream);
    fan_off();
    resistor_off();
    exit(0);
}

void *menu(){
    
    do{
        define_info();
    }while (!is_finished);
    
}

void handle_menu() {
    
    switch (menu_defined)
    {
    case 1:
        print_msg_on_display("Frango");
        temp = frango.time * 20;
        tr = frango.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        send_uart_float(uart_filestream, tr);
        break;
    case 2:
        print_msg_on_display("Peixe");
        temp = peixe.time * 20;
        tr = peixe.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        send_uart_float(uart_filestream, tr);
        break;
    case 3:
        print_msg_on_display("Carne");
        temp = carne.time * 20;
        tr = carne.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        send_uart_float(uart_filestream, tr);
        break;
    case 4:
        print_msg_on_display("Legumes");
        temp = legumes.time * 20;
        tr = legumes.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        send_uart_float(uart_filestream, tr);
        break;
    case 5:
        print_msg_on_display("Pao");
        temp = pao.time * 20;
        tr = pao.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        send_uart_float(uart_filestream, tr);        
        break;
    
    default:
        break;
    }
}

void define_info(){
    int opt = 0, opt2 = 0, temp_aux = 0;
    float tr_aux = 0;
    
    do{
        secondary_menu();
        scanf("%d", &opt);
        send_uart_bool(uart_filestream, 1, 0xD4);
        in_terminal = true;

        switch (opt) {
            case 1:
                printf("\n\n\nInforme a TR (Temperatura de Referencia):\n");
                scanf("%f", &tr_aux);
                printf("\n\n\nInforme o tempo (em minutos):\n");
                scanf("%d", &temp_aux);

                temp = temp_aux * 20;
                tr = tr_aux;
                send_uart_int(uart_filestream, 0xD6, (temp/20));
                send_uart_float(uart_filestream, tr);
                break;

            case 2:
                send_uart_bool(uart_filestream, 1, 0xD4);
                pre_defined();
                scanf("%d", &opt2);
                
                if(opt2 > 0 && opt2 < 6){
                    menu_defined = opt2;
                    handle_menu();
                }

                if(opt2 > 6){
                    printf("\nOpcao Invalida!\n");
                }
                break;

            default:
                printf("Opção invalida\n");
                break;;
        }
    }while(opt != 3);
}        

void *control(){

    do{
        time_control();
        start();
        
    }while(!is_finished);
    
}

void time_control(){
    
    do{
        temperature_control(); 
        
        if(start_time == true){
            count_time++;
        }

        if(count_time == 20 && temp >= 20){
            temp = temp - 20;
            count_time = 0;
        }

        if (uart_resp == 4){
            break;
        }

        if(uart_resp == 2){
            is_finished = true;
            break;
        }

    }while(temp>0);

    print_msg_on_display("Pronto!");

    reset();
}

void temperature_control(){

    uart_resp = request_uart_user(uart_filestream);

    if (!in_terminal){
        tr = request_uart_temp(uart_filestream, 0xC2);
    }
    ti = request_uart_temp(uart_filestream, 0xC1);
    te = request_temp(&bme280);

    if (uart_resp == 5){
        temp = temp + 20;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        uart_resp = 0;
    }

    if(uart_resp == 6 && temp > 20){
        temp = temp - 20;
        send_uart_int(uart_filestream, 0xD6, (temp/20));
        uart_resp = 0;
    }

    if(ti == tr){
        start_time = true;
    }

    pid_atualiza_referencia(tr);
    pid = pid_controle(ti);
    send_uart_float(uart_filestream, tr);

    if (pid > 0){
        fan_off();
        resistor_on(pid);
    }
    
    if (pid == 0){
        fan_off();
        resistor_off();
    }

    if (pid < 0){
        resistor_off();
        if (pid <= -40){
            fan_on(pid * -1);
        }else{
            fan_off();
        }
    }

    send_uart_int(uart_filestream, 0xD1, pid);

    add_in_csv(ti, te, tr, pid);
    print_on_display (ti, tr, (temp/20));
}