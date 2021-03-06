#include "tools.h"

char* atoChar(char _data[])
{
    char c;
    char* res;
    int j;
    int taille = 0;
    while((c=_data[taille]) != '\0')
    {
        taille++;
    }
    res = (char*)malloc(sizeof(char) * taille);
    for(j=0 ; j<taille ; j++)
    {
        res[j] = '0';
    }
    taille = 0;
    while((c = _data[taille]) != '\0')
    {
        res[taille] = c;
        taille++;
    }
    res[taille-1] = '\0';
    return res;
}

char* itoChar(int _value)
{
    char* string;
    int residual;
    int it;
    int string_l = 0;
    if(_value < 10)  string_l = 1;
    else if(_value < 100)  string_l = 2;
    else if(_value < 1000)  string_l = 3;
    else if(_value < 10000)  string_l = 4;
    else string_l = 5;
    string = (char*)malloc((string_l+1) * sizeof(char));
    residual = _value;
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

