#include <menu.h>

void main_menu(){
    printf("\n\n\n===== MENU PRINCIPAL =====");
    printf("Escolha uma opcao:\n\t1-Monitoramento\n\t2-Configurar AirFryer");
}

void secondary_menu(){
    printf("\n\n\n===== MENU =====");
    printf("Escolha uma opcao:\n\t1-Definir manualmente temperatura e tempo \n\t2-Selecionar pre-definicao\n\t3-Voltar");
}

void pre_defined(){
    printf("\n\n\n===== MENU PRE_DEFINICAO =====");
    printf("Escolha uma opcao:\n\t1-Frango \n\t2-Peixe\n\t3-Carne\n\t4-Legumes\n\t5-Pao\n\t6-Voltar");
}