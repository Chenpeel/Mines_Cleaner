#include "menuscene.h"

#include <QCoreApplication>
#include <QPushButton>
#include <QTimer>

#include "gamewindow.h"
#include "levelselection.h"
#include "ui_menuscene.h"


MenuScene::MenuScene(QWidget *parent)
    : QMainWindow(parent), ui(new UI::MenuScene) {
  ui->setupUI(this);
  LevelScene *level = new LevelScene;
  QString IconPrefix = "../resource/img/icon/";

  setFixedSize(800, 610);
  QString iconPath = IconPrefix + "icon.png";
  setWindowIcon(QIcon(iconPath));
  setWindowTitle("扫雷(All Hell's Mathematics!)");
  // 背景
  QPixmap background(IconPrefix + "ms_bg.png");
  QPalette palette(this->palette());
  palette.setBrush(QPalette::Window, background);
  this->setPalette(palette);
  this->setAutoFillBackground(true);
  /**
   * btn0 : Register / Login , 如未登入默认使用 name = sweeper
   * btn1 : 开始 --> 选择难度
   * btn2 : 记录查询
   * btn3 : 退出
   */
  // 创建按钮
  QPushButton *btn[4];
  for (int i = 0; i < 4; ++i) {
    btn[i] = new QPushButton(QString("btn %1").arg(i), this);
    btn[i]->setFixedSize(180, 40);
    btn[i]->setStyleSheet(
        "QPushButton {"
        "background-color: #D7BEF7;"
        "color: white;"
        "border-radius: 7px;"
        "padding: 10px;"
        "}"
        "QPushButton:hover {"
        "background-color: #887C9F;"
        "}"
        "QPushButton:pressed {"
        "background-color: #1abc9c;"
        "}");
  }
  //
  QString login_register_IconPath = (QString)(IconPrefix + "icon.png");
  btn[0]->setIcon(QIcon(login_register_IconPath));
  btn[0]->setText(" 注册/登入");
  btn[0]->move(150, 277);

  //

  QString startIconPath = (QString)(IconPrefix + "start.png");
  btn[1]->setIcon(QIcon(startIconPath));
  btn[1]->setText("  开始吧！");
  btn[1]->move(175, 350);
  connect(btn[1], &QPushButton::clicked, this, [=]() {
    this->hide();
    level->show();
  });
  //

  QString settingIconPath = (QString)(IconPrefix + "setting.png");
  btn[2]->setIcon(QIcon(settingIconPath));
  btn[2]->setText("  游戏记录");
  btn[2]->move(200, 423);

  //
  QString exitIconPath = (QString)(IconPrefix + "exit.png");
  btn[3]->setIcon(QIcon(exitIconPath));
  btn[3]->setText("    退出  ");
  btn[3]->move(225, 496);
  connect(btn[3], &QPushButton::clicked, this, [=]() {
    QTimer::singleShot(50, this, [=]() { QCoreApplication::quit(); });
  });
  //
  connect(level,&LevelScene::back_menu,[=](){
    QTimer::singleShot(50,this,[=](){
        level->hide();
        this->show();
    });
  });
}



MenuScene::~MenuScene() { delete ui; }