#include "database_op.h"

#include <QDebug>
void DatabaseOp::DatabaseOp() {
  QStringList list = QSqlDatabase::drivers();
  QDebug() << "Available databases are:" << list;
}

void DatabaseOp::ConnectDatabase() {}
