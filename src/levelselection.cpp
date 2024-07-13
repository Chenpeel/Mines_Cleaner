#include "levelselection.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>

#include "InputDialog.h"
#include "gamewindow.h"
#include "menuscene.h"

LevelScene::LevelScene() {
  game = new GameScene(this);

  this->setFixedSize(400, 330);
  this->setWindowTitle("扫雷(All Hells Mathematic!)");
  this->setWindowIcon(QIcon("../resource/img/icon/icon.png"));
  /**
   * btns
   *
   * back_menu_btn : back_menu()
   * btn0 : easy()   10 * 6
   * btn1 : medium() 20 * 12
   * btn2 : hard()  35 * 24
   * btn3 : custom(int width,int height)
   *
   */
  QPushButton *back_menu_btn = new QPushButton(this);
  back_menu_btn->setIcon(QIcon("../resource/img/icon/back.png"));
  back_menu_btn->setStyleSheet(
      "QPushButton {"
      "background-color: #D7BEF7;"
      "color: white;"
      "border-radius: 10px;"
      "padding: 0px;"
      "}"
      "QPushButton:hover {"
      "background-color: #887C9F;"
      "}"
      "QPushButton:pressed {"
      "background-color: #1abc9c;"
      "}");
  back_menu_btn->setFixedSize(20, 20);
  back_menu_btn->move(10, 10);
  connect(back_menu_btn, &QPushButton::clicked, this,
          [=]() { emit this->back_menu(); });
  connect(game, &GameScene::back_level, [=]() {
    QTimer::singleShot(50, this, [=]() {
      game->hide();
      this->show();
    });
  });

  QPushButton *btn[4];
  for (int i = 0; i < 4; ++i) {
    btn[i] = new QPushButton(QString("btn %1").arg(i), this);
    btn[i]->setFixedSize(160, 40);
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
  btn[0]->move(120, 70);
  btn[1]->move(120, 130);
  btn[2]->move(120, 190);
  btn[3]->move(120, 250);
  btn[0]->setText("简单");
  btn[1]->setText("中等");
  btn[2]->setText("偏上");
  btn[3]->setText("创造");

  connect(btn[0], &QPushButton::clicked, game, [=]() {
    QTimer::singleShot(50, this, [=]() {
      game->easy();
      this->hide();
    });
  });
  connect(btn[1], &QPushButton::clicked, game, [=]() {
    QTimer::singleShot(50, this, [=]() {
      game->medium();
      this->hide();
    });
  });
  connect(btn[2], &QPushButton::clicked, game, [=]() {
    QTimer::singleShot(50, this, [=]() {
      game->hard();
      this->hide();
    });
  });
  connect(btn[3], &QPushButton::clicked, this, [=]() {
    InputDialog dialog(this);
    while (dialog.exec() == QDialog::Accepted) {
      int width = dialog.getWidth();
      int height = dialog.getHeight();
      if (width > 7 && width <= 100 && height > 5 && height <= 80) {
        QTimer::singleShot(50, this, [=]() {
          game->custom(width, height);
          this->hide();
        });
        break;
      }
    }
  });
}
LevelScene::~LevelScene() {}