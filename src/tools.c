#include "tools.h"

int toDecimal(char* binString) 
{
    int i;
    int res = 0;
    int pow2 = 1;
    for (i = strlen(binString) - 1; i >=0; i--)
    {
        if (binString[i] == '1')
        {
            res += pow2;
        }
        pow2 <<= 1;
    }
    return res;
}

char* toBinary(int n)
{
    int taille;
    char* res;
    int i;
    int pow;
    int temp;
    int number = n;
    if (n == 0)
    {
        taille = 1;
    }
    else
    {
        taille = 0;
        temp = n;

        while (temp > 0)
        {
            temp >>= 1;
            taille++;
        }
    }

    res = (char*)malloc(sizeof(char) * taille);
    pow = 1 << (taille - 1);

    for (i = 0; i < taille ; i++)
    {
        if (pow <= number)
        {
            number -= pow;
            res[i] = '1';
        }
        else
        {
            res[i] = '0';
        }
        pow >>= 1;
    }
    return res;
}

char* atoChar(char data[])
{
    char c;
    char* res;
    int j;
    int taille = 0;
    while((c=data[taille]) != '\0')
    {
        taille++;
    }
    res = (char*)malloc(sizeof(char) * taille);
    for(j=0 ; j<taille ; j++)
    {
        res[j] = '0';
    }
    taille = 0;
    while((c = data[taille]) != '\0')
    {
        res[taille] = c;
        taille++;
    }
    res[taille-1] = '\0';
    return res;
}

char* itoChar(int value)
{
    char* string;
    int residual;
    int it;
    int string_l = 0;
    if(value < 10)  string_l = 1;
    else if(value < 100)  string_l = 2;
    else if(value < 1000)  string_l = 3;
    else if(value < 10000)  string_l = 4;
    else string_l = 5;
    string = (char*)malloc((string_l+1) * sizeof(char));
    residual = value;
    for (it = string_l - 1; it >= 0; it--)
    {
        int digit;
        digit = residual % 10;
        residual = residual / 10;
        string[it] = '0' + digit;
    }
    string[string_l] = '\0';
    return string;
}

