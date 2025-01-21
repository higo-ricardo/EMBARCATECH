#include <stdio.h>
#include <math.h>

#define MAXCHARS 100

int initial, dblrate, maxinfected;

int infected(int day);
void plot(int day);

int main()
{
    int i, days;
    printf("Digite o numero atual de casos: ");
    scanf("%d", &initial);
    printf("Digite o numero de dias para dobrar os casos: ");
    scanf("%d", &dblrate);
    printf("Digite o periodo de dias: ");
    scanf("%d", &days);

    maxinfected = infected(days);
    printf("Ao final de %d dias existirao %d infectados\n", days, maxinfected);
    printf("Grafico:\n");
    for(i = 0; i <= days; ++i)
        plot(i);

    return 0;
}

int infected(int day) {
    return initial * pow(2.0, (double)day / dblrate );
}

void plot(int day) {
    int i, numbars, total;
    /* Obtem o numero de infectados para o dia */
    total = infected(day);
    printf("Dia %3d (%7d infectados): ", day+1, total);
    /* Calcula o numero de barras */
    numbars = (double)total/ maxinfected * MAXCHARS;
    /* Imprime as barras na tela */
    for(i = 0; i < numbars; ++i)
        printf("#");
    printf("\n");
}