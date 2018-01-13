/**
 * Liz Aharonian
 * 316584960
 */
#include <stdio.h>
#include "string.h"
//define
#define SIZE 2
#define BULK 1
//enum encoding
enum Encoding {Unix, Mac, Win, Illegal};

/**
 * getEndLineCharByOsFlag function.
 * @param os - os type.
 * @return - the end line char.
 */
char getEndLineCharByOsFlag(enum Encoding os){
    char endLine;
    switch (os){
        case Unix:
            endLine = '\n';
            break;
        case Mac:
            endLine = '\r';
            break;
    }
    return endLine;

}

/**
 * readAndCopy function.
 * reads and copys src file to trg file.
 * @param src - source file.
 * @param trg - target file.
 * @param isSwap - tells if we should change file's endiannes
 */
void readAndCopy(char* src, char* trg, int isSwap) {
    FILE* source = fopen(src, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE* target = fopen(trg, "wb");
    if (target == NULL) {
        return;
    }
    char buffer[SIZE];
    //read each char by buffer
    while ((fread(buffer, sizeof(buffer), BULK, source)!=0)) {
        if (!isSwap) {
            fwrite(buffer, sizeof(buffer), BULK, target);
        } else{
            //swap in case user asked to do so
            char temp = buffer[0];
            buffer[0]=buffer[1];
            buffer[1]=temp;
            fwrite(buffer, sizeof(buffer), BULK, target);
        }
    }
    //close the files
    fclose(target);
    fclose(source);
}

/**
 * isBigE (endian) function.
 * @param buffer - the readed char.
 * @return 1 if big endiann, o othewise.
 */
int isBigE(char buffer[]) {
    int isBig;
    unsigned short int r = (unsigned short int) buffer[0];
    if ((r == (unsigned short int)0xfffe)) {
        isBig = 1;
    } else if ((r= (unsigned short int) buffer[1])== (unsigned short int)0xfffe){
        isBig = 0;
    }
    return isBig;

}

/**
 * updateNewLineEncodingUnixMac function.
 * @param src - source file.
 * @param trg - target file.
 * @param srcEnco - source os.
 * @param trgEnco - target os.
 * @param isSwap - tells if we should change file's endiannes.
 */
void updateNewLineEncodingUnixMac(char* src, char* trg, enum Encoding srcEnco, enum Encoding trgEnco, int isSwap) {

    FILE* source = fopen(src, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE* target = fopen(trg, "wb");
    if (target == NULL) {
        return;
    }
    char endLineSymbolSrc = getEndLineCharByOsFlag(srcEnco);
    char endLineSymboltrg = getEndLineCharByOsFlag(trgEnco);
    char buffer[SIZE];
    int size = sizeof(buffer);
    int count=0;
    int isBig =0;
    while ((fread(buffer, sizeof(buffer), BULK, source) != 0)) {
        if (count == 0) {
            isBig =isBigE(buffer);
            count++;
        }
        if (!isSwap) {
            if (buffer[isBig] == endLineSymbolSrc && buffer[!isBig] == '\0') {
                buffer[isBig] = endLineSymboltrg;
                buffer[!isBig] = '\0';

            }
        } else {
            if (buffer[isBig] == endLineSymbolSrc && buffer[!isBig] == '\0') {
                buffer[!isBig] = endLineSymboltrg;
                buffer[isBig] = '\0';


            } else {
                //swap other chars of file
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1]=temp;
            }
        }
        //write the buffer to target file
        fwrite(buffer, sizeof(buffer), BULK, target);

    }
    fclose(target);
    fclose(source);

}

/**
 * getEncodingType function.
 * @param strType - os type.
 * @return enum encoding type.
 */
enum Encoding getEncodingType(char * strType) {
    enum Encoding type;
    //make compare between inpute to our enum
    if (strcmp(strType, "-win") == 0) {
        type = Win;

    } else if (strcmp(strType, "-unix") == 0) {
        type = Unix;
    } else if (strcmp(strType, "-mac") == 0) {
        type = Mac;
    } else {
        //illegal param
        type = Illegal;
    }
    return type;
}

/**
 * updateNewLineEncodingWin function.
 * @param src - source file.
 * @param trg - target file.
 * @param srcEnco - source os.
 * @param trgEnco - target os.
 */
void updateNewLineEncodingWin(char* src, char* trg, enum Encoding srcEnco, enum Encoding trgEnco) {
    FILE* source = fopen(src, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE* target = fopen(trg, "wb");
    if (target == NULL) {
        return;
    }
    char endLineSymbolSrc = getEndLineCharByOsFlag(srcEnco);
    char buffer[SIZE];
    int count=0;
    int isBig=0;
    while ((fread(buffer, sizeof(buffer), BULK, source)!=0)) {
        if (count == 0) {
            isBig =isBigE(buffer);
            count++;
        }
        //unix\mac to win
        if (srcEnco!=Win && trgEnco==Win) {
            //mac to win
            if (buffer[isBig] == endLineSymbolSrc) {
                buffer[isBig] = '\n';
                buffer[!isBig] = '\0';
                char tmpBuffer[SIZE];
                tmpBuffer[isBig] = '\r';
                tmpBuffer[!isBig] = '\0';
                fwrite(tmpBuffer, sizeof(tmpBuffer), BULK, target);
            }
            fwrite(buffer, sizeof(buffer), BULK, target);
        } else if (srcEnco==Win && trgEnco==Unix){
            //win to unix
            if (buffer[isBig]=='\n' || buffer[isBig]!='\r') {
                fwrite(buffer, sizeof(buffer), BULK, target);
            }
        } else if (srcEnco==Win && trgEnco==Mac){
            //win to mac
            if (buffer[isBig]!='\n' || buffer[isBig]=='\r') {
                fwrite(buffer, sizeof(buffer), BULK, target);
            }
        }
    }
    //close the files
    fclose(target);
    fclose(source);
}

/**
 * updateNewLineEncodingWinSwap function.
 * @param src - source file.
 * @param trg - target file.
 * @param srcEnco - source os.
 * @param trgEnco - target os
 */
void updateNewLineEncodingWinSwap(char* src, char* trg, enum Encoding srcEnco, enum Encoding trgEnco) {
    FILE* source = fopen(src, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE* target = fopen(trg, "wb");
    if (target == NULL) {
        return;
    }
    char endLineSymbolSrc = getEndLineCharByOsFlag(srcEnco);
    char buffer[SIZE];
    int count=0;
    int isBig=0;
    while ((fread(buffer, sizeof(buffer), BULK, source)!=0)) {
        if (count == 0) {
            isBig =isBigE(buffer);
            count++;
        }
        //unix\mac to win
        if (srcEnco!=Win && trgEnco==Win) {
            //mac/unix to win
            if (buffer[isBig] == endLineSymbolSrc) {
                buffer[!isBig] = '\n';
                buffer[isBig] = '\0';
                char tmpBuffer[SIZE];
                tmpBuffer[!isBig] = '\r';
                tmpBuffer[isBig] = '\0';
                fwrite(tmpBuffer, sizeof(tmpBuffer), BULK, target);
            } else{
                //swap other chars
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1]=temp;
            }
            fwrite(buffer, sizeof(buffer), BULK, target);
        } else if (srcEnco==Win && trgEnco==Unix){
            //win to unix
            if (buffer[isBig]=='\n' || buffer[isBig]!='\r') {
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1]=temp;
                fwrite(buffer, sizeof(buffer), BULK, target);

            }
        } else if (srcEnco==Win && trgEnco==Mac){
            //win to mac
            if (buffer[isBig]!='\n' || buffer[isBig]=='\r') {
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1]=temp;
                fwrite(buffer, sizeof(buffer), BULK, target);

            }
        }
    }
    //close the files
    fclose(target);
    fclose(source);
}

/**
 * isSwapBytes function.
 * @param flag - input param (keep\swap)
 * @return 1 if swap is needed, 0 otherwise
 */
int isSwapBytes(char* flag) {
    if (strcmp(flag,"-swap")==0) {
        return 1;
    } else if (strcmp(flag,"-keep")==0) {
        return 0;
    } else {
        return -1;
    }
}

/**
 * inputValidation function.
 * @param nameOfTrgFile - target file name
 * @return 1 if the is point in file name, 0 otherwise
 */
int inputValidation(char * nameOfTrgFile) {
    int len = strlen(nameOfTrgFile);
    int i = 0;
    for (; i < len; i++) {
        if (nameOfTrgFile[i] == '.') {
            return 1;
        }
    }
    return 0;
}

/**
 * main function.
 * runs the program.
 * @param argc - number of params.
 * @param argv - arr of input strings (char *)
 * @return o.
 */
int main(int argc, char* argv[]) {

    switch (argc) {
        case 3:
            if (inputValidation(argv[1]) ==1 && inputValidation(argv[2])==1){
                readAndCopy(argv[1], argv[2],0);

            }
            break;
        case 5: {
            enum Encoding srcEnco = getEncodingType(argv[3]);
            enum Encoding trgEncoding = getEncodingType(argv[4]);
            if (srcEnco == Illegal || trgEncoding == Illegal) {
                return 0;
            }
            if(srcEnco==trgEncoding){
                readAndCopy(argv[1], argv[2],0);
                return 0;
            }
            if (srcEnco!=Win && trgEncoding!=Win) {
                updateNewLineEncodingUnixMac(argv[1], argv[2], srcEnco, trgEncoding,0);
            } else if (srcEnco==Win || trgEncoding ==Win) {
                updateNewLineEncodingWin(argv[1], argv[2], srcEnco, trgEncoding);

            }
            break;
        }
        case 6: {
            enum Encoding srcEnco = getEncodingType(argv[3]);
            enum Encoding trgEncoding = getEncodingType(argv[4]);
            if (srcEnco == Illegal || trgEncoding == Illegal) {
                return 0;
            }
            int isSwap =isSwapBytes(argv[5]);
            if (isSwap == -1) {
                return 0;
            }
            if(srcEnco==trgEncoding){
                readAndCopy(argv[1], argv[2], isSwap);
                return 0;
            }
            if (srcEnco!=Win && trgEncoding!=Win) {
                updateNewLineEncodingUnixMac(argv[1], argv[2], srcEnco, trgEncoding,isSwap);

            } else if (srcEnco==Win || trgEncoding ==Win) {
                if(isSwap) {
                    updateNewLineEncodingWinSwap(argv[1], argv[2], srcEnco, trgEncoding);

                } else {
                    updateNewLineEncodingWin(argv[1], argv[2], srcEnco, trgEncoding);

                }
            }
            break;
        }
    }

    return 0;
}