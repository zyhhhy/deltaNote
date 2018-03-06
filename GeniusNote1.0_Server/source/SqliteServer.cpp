//
// Created by geniusrabbit on 18-2-27.
//

/**
 * 数据库设计：
 * 每个用户一张表，另写一张表保存用户名和密码，用来校验用户是否具有访问权限
 *
 * 函数实现：
 * １．初始化用户，新建表
 * ２．根据返回内容修改表中各条信息参数
 * time_tag  server  client android
 *    tag     true    true    true
 *
 *    建立状态码：已同步、未同步，已删除
 *
 * 各端分别维护一个表，更新时，由socket建立联系，并将表发送给服务器，服务器更新内容后给客户端更新内容
 * 客户端定时向服务器请求更新
 *
 *
 * １．初始化函数：输入用户名，存储用户名及其密码
 * ２．修改状态函数：输入客户端发送的数组，及其客户端种类，将客户端发来的
 * 表逐条与服务器的表对照，将客户端已同步的而服务器未同步的添加到表中
 * ，将服务器端已同步而客户端未同步的发送给客户端，完成同步
 */

#include <cstdio>
#include <cstring>
#include "../include/SqliteServer.h"
#include "../include/Log.h"

#define MAXLINE 10

enum SqliteState {
  Deleted=2,
  Sync=1,
  UnSync=0
};

sqlite3 *db= nullptr;
char *zErrMsg= nullptr;
int ret=0;

namespace GeniusNote{
/*
int callback(void *NotUsed,int argc,char **argv,char **azColName){
  for(int i=0;i<argc;i++){
    printf("%s\n",argv[i]);
  }
  return 0;
}
*/
int callback(void *NotUsed,int argc,char **argv,char **azColName){
  return 0;
}

bool ServerSqlite::SqlTableOpen(char* name,char* paswd){
  //创建用户密码表
  const char* Name_paswd="CRREATE TABLE IF NOT EXITS GeniusNote.Name_paswd("\
      "Name KEY NOTNULL,"\
      "Paswd CHAR(256) NOTNULL);";
  ret =sqlite3_exec(db,Name_paswd, nullptr, nullptr,&zErrMsg);
  CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})

  //查找用户名，对比密码
  LOG_INFO("Name and password selecting...")
  const char* Find_name="SELECT Name,Paswd From Geniusrabbit.Name_Paswd"\
      "WHERE Name=(%Q) AND Paswd=(%Q)";
  char* SqlFind=sqlite3_mprintf(Find_name,name,paswd);
  ret =sqlite3_exec(db,SqlFind,nullptr, nullptr,&zErrMsg);
  CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
  return ret==SQLITE_OK;
}
int ServerSqlite::SqlAddUser(char* UserName,char* Paswd){
  LOG_INFO("Add user...")
  const char* Add="INSERT INTO GeniusNote.Name_paswd(Name,Paswd) VALUES((%Q),(%Q))";
  const char* AddUser=sqlite3_mprintf(Add,UserName,Paswd);

  ret =sqlite3_exec(db,AddUser,nullptr, nullptr,&zErrMsg);
  CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
}
int ServerSqlite::SqlTableInit(char* UserName){
  const char *CreateTable="CREATE TABLE GeniusNote.(%Q)("\
      "Time_Tag       KEY       NOTNULL,"\
      "ServerState    CHAR(1)   NOTNULL,"\
      "ClientState    CHAR(1)   NOTNULL,"\
      "AndroidState   CHAR(1)   NOTNULL);";
  char* SqlInit=sqlite3_mprintf(CreateTable,UserName);
  ret =sqlite3_exec(db,SqlInit, nullptr, nullptr,&zErrMsg);
  CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
  LOG_INFO("User create success~")
}

/**
  * 更新数据库中的值
  * （假设从客户端传递过来的都是服务器端未同步的）
  * １．客户端新建的条目：此时需要在服务器端新建该条目
  * ２．客户端删除的条目：将客户端做出已删除的状态标识
  */
int ServerSqlite::ServerTableUpdate(char* UserName,const char* terminal,NoteStruct* Note){
  int type=0;
  if(terminal=="Android"){
    type=2;
  }else if(terminal=="client"){
    type=1;
  }else{
    LOG_ERROR("Uknow types...")
  }

  for(int i=0;i<MAXLINE;i++){
    if(Note[i].Type[type]==Deleted){
      const char* SqlUpdate="UPDATE (%Q) SET ServerState=(%Q) WHERE TimeTag==(%Q)";
      char* SqlDel=sqlite3_mprintf(SqlUpdate,UserName,Deleted,Note[i].Time_Tag);
      ret =sqlite3_exec(db,SqlDel, nullptr,nullptr,&zErrMsg);
      CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
    }else if(Note[i].Type[type]==UnSync){
      const char*SqlUpdate="INSERT INTO GeniusNote.(%Q)(Time_tag,ServerState,ClientState,AndroidState)"\
                                      "VALUES((%Q),(%Q),(%Q),(%Q))";
      char* SqlSync=sqlite3_mprintf(SqlUpdate,UserName,Note[i].Time_Tag,1,((char)(type==1)),((char)(type != 1)));
      ret =sqlite3_exec(db,SqlSync,nullptr,nullptr,&zErrMsg);
      CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
    }else if(Note[i].Type[type]==Sync){
      LOG_ERROR("This note is Sync...")
    } else{
      LOG_ERROR("Unknow sqlite state type...")
    }
  }

  LOG_INFO("Updated success~")
}
/**
 * 如何返回数组？？？
 * @param UserName
 * @param Note
 * @param terminal
 * @return
 */
int ServerSqlite::ServerTableReturn(char* UserName,NoteStruct* Note,char* terminal,
                                    int (*callback)(void *, int, char **, char **)){
  const char* SqlReturn=sqlite3_mprintf("SELECT FROM (%Q) WHERE (%Q)==(%Q)",UserName,terminal);
  ret =sqlite3_exec(db,SqlReturn,callback, nullptr,&zErrMsg);
  CHECK(ret,SQLITE_OK,{LOG_ERROR(sqlite3_mprintf("SQL error:%s\n",zErrMsg))})
  LOG_INFO("Return success~")
}
}
