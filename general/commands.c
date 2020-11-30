#include <stdio.h>
#include <string.h>

void convertToFormat(char *path, char *toExt)
{
    char temp1[100], temp2[100], temp3[10];
    strcpy(temp1, path);
    strcpy(temp2, path);
    strcpy(temp3, toExt);
    char cmd[100] = "convert ";
    strtok(temp1, ".");
    strcat(cmd, temp2);
    strcat(cmd, " ");
    strcat(cmd, temp1);
    strcat(cmd, toExt);
    strcpy(path, cmd);
}

void convertToFormatSize(char *path, char *size, char *toExt)
{
    char temp1[100], temp2[100], temp3[10];
    strcpy(temp1, path);
    strcpy(temp2, path);
    strcpy(temp3, toExt);
    char cmd[100] = "convert -size ";
    strcat(cmd, size);
    strcat(cmd, " -depth 8 ");
    strtok(temp1, ".");
    strcat(cmd, temp2);
    strcat(cmd, " ");
    strcat(cmd, temp1);
    strcat(cmd, "_sobel");
    strcat(cmd, toExt);
    strcpy(path, cmd);
}

void convertString(char *path, char *toExt)
{
    char temp1[50], temp2[10];
    strcpy(temp1, path);
    strcpy(temp2, toExt);
    strtok(temp1, ".");
    strcat(temp1, temp2);
    strcpy(path, temp1);
}
