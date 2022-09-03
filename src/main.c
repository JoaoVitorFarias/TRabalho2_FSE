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
const struct FMenuDefinedoo pao = {3, 35.0};

struct bme280_dev bme280;
int uart_filestream, uart_resp = 0, uart_on_off = 0, uart_start = 0, temp = 60, menu_defined = 0, count_time = 0, uart_resp_time = 0;
float ti = 0, te = 0, tr = 0;  
int temp_array [10];
int second_array [10];
double pid = 0;
bool start_time = false, is_finished = false;

int main() {
    set_up();
    signal(SIGINT, end);

    do{
        uart_on_off = request_uart_user(uart_filestream);
    }while (uart_on_off != 1);

    send_uart_bool(uart_filestream, 1, 0xD3);

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

    FILE* reflow = fopen("curva_reflow.csv", "r");
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

    do{

        tr = request_uart_temp(uart_filestream, 0xC2);
        sleep(0.5);

        uart_resp = request_uart_user(uart_filestream);

        if (uart_resp == 5){
            temp = temp + 60;
            send_uart_int(uart_filestream, 0xD6, (temp/60));
            uart_resp = 0;
        }

        if(uart_resp == 6 && temp > 60){
            temp = temp - 60;
            send_uart_int(uart_filestream, 0xD6, (temp/60));
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
    }while(uart_resp != 3)

    send_uart_bool(uart_filestream, 1, 0xD5);
}

void reset(){
    uart_resp = 0;
    uart_resp_time = 0;
    uart_on_off = 0;
    uart_start = 0;
    temp = 60;
    menu_defined = 0;
    count_time = 0;
    ti = 0; 
    te = 0; 
    tr = 0;  
    pid = 0;
    start_time = false;
}

void end(){
    send_uart_bool(uart_filestream, 0, 0xD3);
    close_uart(uart_filestream);
    fan_off();
    resistor_off();
    exit(0);
}

void *menu(){
    int opt = 0;

    while(1){

        main_menu();
        scanf("%d", &opt);

        switch (opt) {
        case 1:
            printf("===== MONITORAMENTO =====\n");
            printf("Para sair aperte 1");
            monitoring();
            break;

        case 2:
            define_info();
            break;

        default:
            printf("Opcao Invalida!\n");
            break;;
        }

    }while (!is_finished);
    
}

void handle_menu() {
    
    switch (menu_defined)
    {
    case 1:
        temp = frango.time * 60;
        tr = frango.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        send_uart_float(uart_filestream, tr);
        break;
    case 2:
        temp = peixe.time * 60;
        tr = peixe.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        send_uart_float(uart_filestream, tr);
        break;
    case 3:
        temp = carne.time * 60;
        tr = carne.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        send_uart_float(uart_filestream, tr);
        break;
    case 4:
        temp = legumes.time * 60;
        tr = legumes.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        send_uart_float(uart_filestream, tr);
        break;
    case 5:
        temp = pao.time * 60;
        tr = pao.tr;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        send_uart_float(uart_filestream, tr);        
        break;
    
    default:
        break;
    }
}

void monitoring(){
    int exit_monitoring = 0;
    do{
        printf("TR: %f°C", tr);
        printf("TI: %f°C", ti);
        printf("Tempo: %d minutos", (temp/60));
        
        sleep(1);
    }while(scanf("%d", &exit_monitoring) != 1);    
}

void define_info(){
    int opt = 0, opt2 = 0, temp_aux = 0;
    float tr_aux = 0;
    
    do{
        secondary_menu();
        scanf("%d", &opt);

        switch (opt) {
            case 1:
                printf("\n\n\nInforme a TR (Temperatura de Referencia):\n");
                scanf("%f", &tr_aux);
                printf("\n\n\nInforme o tempo (em minutos):\n");
                scanf("%d", &tempo);

                temp = temp_aux * 60;
                tr = tr_aux;
                send_uart_int(uart_filestream, 0xD6, (temp/60));
                send_uart_float(uart_filestream, tr);
                break;

            case 2:
                pre_defined()
                scanf("%d", &opt2);
                
                if(opt2 > 0 && opt2 < 6){
                    menu_defined = opt2;
                    handle_menu();
                }

                if(opt2 > 6){
                    printf("\nOpcao Invalida!\n")
                }
                break;

            case 3:
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

        uart_resp = request_uart_user(uart_filestream);
        if(uart_resp == 2){
            is_finished = true;
        }
        
    }while(!is_finished);
    
}

void time_control(){
    
    do{
        temperature_control(); 
        handle_time();
        sleep(0.5);

        temperature_control(); 
        handle_time();
        sleep(0.5); 

        if(start_time == true){
            count_time++;
        }

        if(count_time == 60 && temp > 60){
            temp = temp - 60;
            count_time = 0;
        }

        uart_resp = request_uart_user(uart_filestream);

        if (uart_resp == 2 || uart_resp == 4){
            break;
        }

    }while(temp>0);
    
    reset();
}

void temperature_control(){

    tr = request_uart_temp(uart_filestream, 0xC2);
    ti = request_uart_temp(uart_filestream, 0xC1);
    te = request_temp(&bme280);

    if(ti >= tr){
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

    send_uart_int(uart_filestream, pid);

    add_in_csv(ti, te, tr, pid);
    print_on_display (ti, te, temp);
}


void handle_time(){
    uart_resp_time = request_uart_user(uart_filestream);

    if (uart_resp_time == 5){
        temp = temp + 60;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        uart_resp_time = 0;
    }

    if(uart_resp_time == 6 && temp > 60){
        temp = temp - 60;
        send_uart_int(uart_filestream, 0xD6, (temp/60));
        uart_resp_time = 0;
    }
}