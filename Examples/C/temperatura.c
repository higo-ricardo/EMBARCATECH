#include <stdio.h>

/* Algoritmo que gera tabela de corelação de temperaturas para as escalas Fahrenheit-Celsius*/

void EsperaESC()  // Definição da função "EsperaEnter"
{
    int tecla;

    do
    {
        tecla = getchar();
        if (tecla !=13) // Se nao for ESC
        {
            float fahr;
            int celsius;
            printf("Digite o valor da temperatura em Celsius: ");
            scanf("%d", &celsius);
            if (celsius) {
            fahr = (9.0/5.0) * celsius + 32.0; /* Fórmula de conversão entre as escalas Fahrenheit-Celsius */
            printf(" A temperatura de %dC corresponde a %6.1fF\n\n", celsius, fahr);
            }
            printf("Pressione A tecla ESC para SAIR\n\n");

        }
    } while(tecla != 13); // 13 e' o codigo ASCII do ENTER
}
int main()
{
    EsperaESC();  
}