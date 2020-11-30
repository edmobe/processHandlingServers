#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
/*
convert imgs/betico.jpg imgs/img.rgb
./src/sobel imgs/img.rgb imgs/img_out.gray 640x640 
convert -size 640x640 -depth 8 imgs/img_out.gray imgs/img_out.png
*/
int main(int argc, char const *argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        system("convert image_0.jpg image_0.rgb");
        system("../filter/src/sobel image_0.rgb image_0.gray 1072x1340");
        system("convert -size 1072x1340 -depth 8 image_0.gray image_0.jpg");
    }
    else
    {
        printf("Soy el padre\n");
    }

    return 0;
}
