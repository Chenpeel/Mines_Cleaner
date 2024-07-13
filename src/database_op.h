#ifndef __DATABASE_OP_H__
#define __DATABASE_OP_H__

#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseOp {
 public:
  DatabaseOp();

  void ConnectDatabase();
}

#endif