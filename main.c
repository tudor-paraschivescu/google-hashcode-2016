#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    int x;
    int y;
}coord;


int CalculateDistance(int x1, int x2, int y1, int y2) {

    double d = sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
    int distance = (int) ceil(d);
    return distance;
}


int main() {

    int i, j, k, l, var;

    FILE *input;

    input = fopen("mother_of_all_warehouses.in", "r");

    int rows; //rows in the area of simulation
    int columns; //columns in the area of simulation
    int D; //number of drones available
    int turns; //deadline of the simulation
    int load; //maximum load of a drone

    fscanf(input, "%d %d %d %d %d", &rows, &columns, &D, &turns, &load);
    getc(input);

    int P; //number of product types

    fscanf(input, "%d", &P);
    getc(input);

    int weights[P]; //weights of the products
    for(i = 0; i < P; i++)
        fscanf(input, "%d", &weights[i]);
    getc(input);

    int W; //number of warehouses
    fscanf(input, "%d", &W);
    getc(input);

    typedef struct {
        coord coordW;
        int products[P];
    }warehouse; //description of a warehouse

    warehouse warehouses[W]; //W warehouses

    for(i = 0; i < W; i++) {

        fscanf(input, "%d", &warehouses[i].coordW.x);
        fscanf(input, "%d", &warehouses[i].coordW.y);
        getc(input);

        for(j = 0; j < P; j++)
           fscanf(input, "%d", &warehouses[i].products[j]);
        getc(input);
    }

    int C; //number of custom orders
    fscanf(input, "%d", &C);
    getc(input);

    typedef struct {
        coord coordC;
        int L;
        int *products;
    }order; //description of an order

    order orders[C]; //C orders

    for(i = 0; i < C; i++) {

        fscanf(input, "%d", &orders[i].coordC.x);
        fscanf(input, "%d", &orders[i].coordC.y);
        getc(input);

        fscanf(input, "%d", &orders[i].L);
        getc(input);

        orders[i].products = malloc(orders[i].L * sizeof(int));

        for(j = 0; j < orders[i].L; j++)
           fscanf(input, "%d", &orders[i].products[j]);
        getc(input);

        //Sorting the ordered products after weight
        for(j = 0; j < orders[i].L - 1; j++)
            for(k = j + 1; k < orders[i].L; k++)
                if(weights[orders[i].products[j]] > weights[orders[i].products[k]]) {
                    int aux = orders[i].products[j];
                    orders[i].products[j] = orders[i].products[k];
                    orders[i].products[k] = aux;
                }
    }

    fclose(input);

    FILE *output = fopen("mother_of_all_warehouses.out", "w");

    int x = warehouses[0].coordW.x;
    int y = warehouses[0].coordW.y;
    int commands = 0;

    for(i = 0; i < D; i++)
        while(1) {
            int Turns = turns; //copy of the number of turns
            //do (i + j)th order
            for(j = 0; i + j < C; j += D) {

                int weightD = 0;
                int products = 0;

                for(k = 0; k < orders[i + j].L; k++)
                    if(weightD + weights[orders[i + j].products[k]] <= load) {
                        //LOAD
                        int close[W];
                        for(var = 0; var < W; var++)
                            if(warehouses[var].products[orders[i + j].products[k]] > 0)
                                close[var] = CalculateDistance(x, warehouses[var].coordW.x, y, warehouses[var].coordW.y);
                            else
                                close[var] = -1;

                        int min;
                        for(var = 0; var < W; var++)
                            if(close[var] != -1) {

                                min = var;
                                for(l = var; l < W; l++)
                                    if(close[l] != -1 && min > close[l])
                                        min = l;

                                break;
                            }

                        fprintf(output, "%d L %d %d 1\n", i, min, orders[i + j].products[k]);
                        commands++;
                        x = warehouses[min].coordW.x;
                        y = warehouses[min].coordW.y;
                        weightD += weights[orders[i + j].products[k]];
                        products++;
                        warehouses[min].products[orders[i + j].products[k]]--;
                        Turns = Turns - close[min] - 1;

                    }
                    else {
                        //DELIVER
                        int d = CalculateDistance(x, orders[i + j].coordC.x, y, orders[i + j].coordC.y);
                        int check = products;

                        while(products) {

                            fprintf(output, "%d D %d %d 1\n", i, i + j, orders[i + j].products[k - products]);
                            commands++;
                            x = orders[i + j].coordC.x;
                            y = orders[i + j].coordC.y;

                            if(check == products)
                                Turns = Turns - d - 1;
                            else
                                Turns--;

                            products--;
                        }

                        weightD = 0;

                        //LOAD
                        int close[W];
                        for(var = 0; var < W; var++)
                            if(warehouses[var].products[orders[i + j].products[k]] > 0)
                                close[var] = CalculateDistance(x, warehouses[var].coordW.x, y, warehouses[var].coordW.y);
                            else
                                close[var] = -1;

                        int min;
                        for(var = 0; var < W; var++)
                            if(close[var] != -1) {

                                min = var;
                                for(l = var; l < W; l++)
                                    if(close[l] != -1 && min > close[l])
                                        min = l;

                                break;
                            }

                        fprintf(output, "%d L %d %d 1\n", i, min, orders[i + j].products[k]);
                        commands++;
                        x = warehouses[min].coordW.x;
                        y = warehouses[min].coordW.y;
                        weightD += weights[orders[i + j].products[k]];
                        products++;
                        warehouses[min].products[orders[i + j].products[k]]--;
                        Turns = Turns - close[min] - 1;

                    }

                //LAST DELIVERY
                int d = CalculateDistance(x, orders[i + j].coordC.x, y, orders[i + j].coordC.y);
                int check = products;

                while(products) {

                    fprintf(output, "%d D %d %d 1\n", i, i + j, orders[i + j].products[k - products]);
                    commands++;
                    x = orders[i + j].coordC.x;
                    y = orders[i + j].coordC.y;

                    if(check == products)
                        Turns = Turns - d - 1;
                    else
                        Turns--;

                    products--;
                }

                weightD = 0;
            }
            printf("%d - %d\n", i, Turns);
            break;
        }

    fclose(output);

    printf("\n%d\n", commands);


    return 0;
}
