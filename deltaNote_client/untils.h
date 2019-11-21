#ifndef UNTILS_H
#define UNTILS_H

#include <string.h>

#include "pack.h"
#include "log.h"
#include "sqlite.h"


void makeSocketPack(MSG &synPack, int msgSize, char msgSeg, char msgOp);
void makeDataPack(MSG_OP_PACK &opPack, char *opTimestamp, char *createTimestamp, char op, char isCheck, char *data);

void parserServerPort(char *serverPort);

#endif // UNTILS_H
