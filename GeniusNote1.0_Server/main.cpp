//
// Created by geniusrabbit on 18-3-4.
//

#include "include/SqliteServer.h"
#include "include/SocketServer.h"
#include "include/Log.h"
#include <pthread.h>
#include <iostream>
#include <cstring>

#define NUM_THREADS 1024

using namespace GeniusNote;

typedef struct {
  int id;
  char name[8];
}ID_Name;
ID_Name Node[10];
typedef struct {
  char* UserName;
  char* Paswd;
  char* Type;
}User_Paswd;

void* buf;

int callback(void *NotUsed,int argc,char **argv,char **azColName){
  memcpy(buf,*argv, sizeof(*argv));
  return 0;
}
static void * DataHandle(SocketServer socketserver) {
  User_Paswd *user_paswd;
  size_t UserPaswdSize = sizeof(User_Paswd);

  socketserver.Recv(buf, UserPaswdSize);
  memcpy(&user_paswd, buf, UserPaswdSize);

  if (!strcmp(user_paswd->Type, "register")) {
    ServerSqlite serverSqlite{};
    serverSqlite.SqlTableIint(user_paswd->UserName);
    //如果用户名存在，则回馈用户名存在信号
    serverSqlite.SqlTableOpen(user_paswd->UserName, user_paswd->Paswd);
  } else if (!strcmp(user_paswd->Type, "client") || !strcmp(user_paswd->Type, "android")) {
    ServerSqlite serverSqlite{};
    serverSqlite.SqlTableOpen(user_paswd->UserName, user_paswd->Paswd);
    serverSqlite.ServerTableUpdate(user_paswd->UserName, user_paswd->Type, Node);
    serverSqlite.ServerTableReturn(user_paswd->UserName, Node, user_paswd->Type, callback);
    socketserver.Send(buf, sizeof(buf));
  } else {
    LOG_ERROR("身份验证失败。。。")

  }
}
/**
 * 接收到新的连接请求
 * 建立新的线程
 * 与客户端交换数据
 * 交换完毕断开连接终止线程
 * @return
 */

int main(){
  int i=0;
  SocketServer socketServer{};
  socketServer.init(2333);
  socketServer.StartSocket();
  while(i<NUM_THREADS){
    LOG_INFO("waiting for new connection...")
    pthread_t thread_id;
    if(socketServer.AcCon()){
      LOG_ERROR("Accept connection error")
    }
    LOG_INFO("A new connection occurs")
    if (pthread_create(&thread_id, NULL, reinterpret_cast<void *(*)(void *)>(DataHandle), &socketServer)) {
      LOG_ERROR("Pthread create error...")
      break;
    }

    i++;
  }
}