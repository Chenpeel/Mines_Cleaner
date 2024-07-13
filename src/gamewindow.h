#ifndef __GAME_WINDOW_H__
#define __GAME_WINDOW_H__
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>

#include "MyGraphicsScene.h"
#include "fielddata.h"
#include "unititem.h"
class GameScene : public QMainWindow {
  Q_OBJECT
 public:
  GameScene(QWidget *parent = nullptr);
  ~GameScene();

 signals:
  void back_level();

 public slots:
  void easy();
  void medium();
  void hard();
  void custom(int width, int height);

 private:
  int unit_w;
  int unit_h;
  int map_width;
  int map_height;
  QGraphicsView *view;
  MyGraphicsScene *scene;
  QTimer *timer;
  QLabel *timeLabel;
  int elapsedSeconds;
  void showMap();
  void startTimer();
  void updateTimer();
  void stopTimer();
  QString formatTime(int seconds);

 private:
  UnitItem *uitem;
  UnitMatrix map;
  Matrix matrix;
  int remain_init;
  int remain_mines;
  void initializeMap();
  void empty_extand(double pos_x, double pos_y, Matrix matrix);
  void surround_extand(int x, int y, double pos_x, double pos_y, Matrix matrix);
  void youLose();
  bool isWin();
  bool isNumComputeCorrect(int N);
  void newGame();
  void recordTime(int time, QString name);  // local record
  void recordTimeOnline(int time, QString name,
                        QString id);  // onlinesql record

 protected:
  void mousePressEvent(QMouseEvent *event) override;
};
#endif