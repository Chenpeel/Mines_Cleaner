#ifndef __LEVEL_SELECTION_H__
#define __LEVEL_SELECTION_H__
#include <QWidget>

#include "gamewindow.h"
class LevelScene : public QWidget {
  Q_OBJECT

 private:
  GameScene *game;

 public:
  explicit LevelScene();
  ~LevelScene();

 signals:
  void back_menu();
  void easy();
  void medium();
  void hard();
  void custom(int width, int height);
};

#endif