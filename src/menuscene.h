#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace UI {
    class MenuScene;
}
QT_END_NAMESPACE

class MenuScene : public QMainWindow {
  Q_OBJECT
 public:
  MenuScene(QWidget *parent = nullptr);
  ~MenuScene();
 private:
  UI::MenuScene *ui;
};

#endif  // __MENU_SCENE_H__