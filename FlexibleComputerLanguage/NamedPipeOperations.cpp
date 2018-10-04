//
//  NamedPipeOperations.cpp
//  Named Pipes C/C++
//
//  Created by Murtaza Anverali on 7/4/18.
//  Copyright © 2018 Murtaza Anverali. All rights reserved.
//
#include <unistd.h>
#include <iostream>
#include "NamedPipeOperations.h"
#include <string.h>

#define BUF_SIZE 1000000

std::string NamedPipeOperations::readFromPipe(int fd)
{
    //    int BUF_SIZE = 4;
    //    char str1[BUF_SIZE];
    //    std::string resultString;
    char buff[BUF_SIZE];
    char *chrp;
    std::string resultString = "";
    FILE *stream;
    int c;
    stream = fdopen(fd, "r");
    chrp = fgets(buff, BUF_SIZE, stream);
    while ((chrp != 0))
    {
        resultString = resultString + buff;
        chrp = fgets(buff, BUF_SIZE, stream);
    }
    // while ((c = fgetc (stream)) != EOF)
    // {
    //     // putchar (c);
    //     resultString = resultString + (char)c;
    // }
    // char** line;
    // size_t* n = 0;
    // getline(line, n, stream);
    fclose(stream);

    close(fd);
    //    while (1)
    //    {
    //        int read2result = read(fd, str1, BUF_SIZE);
    //        if (read2result < BUF_SIZE && read2result >= 0)
    //        {
    //            if (read2result != 0)
    //            {
    //                resultString += str1;
    //            }
    //            break;
    //        }
    //        resultString += str1;
    //        memset(str1, 0, sizeof(str1));
    //    }
    //    std::cout << "The result is " << resultString;
    return resultString;
};

int NamedPipeOperations::writeToPipe(int fd, std::string s)
{
    //    int BUF_SIZE = 4;
    //    char str1[BUF_SIZE];
    //    char *sc;
    //    sc = (char *)s.c_str();
    //    int i, j = 0;
    //    for (i = 0; sc[i]; i++)
    //    {
    //        str1[i % 4] = sc[i];
    //        // std::cout << "i is " << i << "sc is " << sc[i] << std::endl;
    //        if (i % 4 == 3 && i != 0)
    //        {
    //            // std::cout << "arr1 is " << str2 << std::endl;
    //            int status = write(fd, str1, strlen(str1));
    //            // std::cout << "write status " << status << std::endl;
    //            // arr1 = new char[4];
    //            memset(str1, 0, sizeof(str1));
    //        }
    //        // printf("%c", sc[ i ]);
    //    }
    //    // std::cout << "i is " << i << std::endl;
    //    if (i % 4 < 4 && i % 4 != 0)
    //    {
    //        // std::cout << "arr1 is " << str2 << std::endl;
    //        int status = write(fd, str1, strlen(str1));
    //        // std::cout << "write status " << status << std::endl;
    //    }
    FILE *stream;
    stream = fdopen(fd, "w");
    fprintf(stream, (char *)s.c_str());
    fclose(stream);
    close(fd);
    return 0;
};
