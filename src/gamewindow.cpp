#include "gamewindow.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QStatusBar>
#include <iostream>
#include <queue>
#include <random>

#include "NumDialog.h"
#include "fielddata.h"
#include "unititem.h"
/**
 * IsFirstLeftClick : 用于防止开局点到雷而直接失败
 *
 */
bool IsFirstLeftClick = true;
QString userName;

struct Pos {
  double pos_x;
  double pos_y;
  Pos(double pos_x, double pos_y) : pos_x(pos_x), pos_y(pos_y){};
};
struct Mpos {
  int row;
  int col;
  Mpos(int row, int col) : row(row), col(col){};
};
//
GameScene::GameScene(QWidget *parent)
    : QMainWindow(parent),
      view(new QGraphicsView(this)),
      scene(new MyGraphicsScene(this)),
      timer(new QTimer(this)),
      timeLabel(new QLabel(this)),
      elapsedSeconds(0) {
  int initWidth = 1600, initHeight = 1300;
  this->setFixedSize(initWidth, initHeight);
  QString iconPath = "../resource/img/icon/icon.png";
  this->setWindowIcon(QIcon(iconPath));
  this->setWindowTitle("扫雷(All Hell's Mathematics!)");
  QPushButton *backButton = new QPushButton(this);
  backButton->setGeometry(QRect(10, 10, 15, 20));
  QString backIcon = "../resource/img/icon/back.png";
  backButton->setIcon(QIcon(backIcon));
  backButton->setIconSize(QSize(10, 10));
  connect(backButton, &QPushButton::clicked, this,
          [=]() { emit this->back_level(); });
  QStatusBar *statusBar = this->statusBar();
  timeLabel = new QLabel(this);
  statusBar->addWidget(backButton);
  statusBar->addPermanentWidget(timeLabel);
  statusBar->setFixedHeight(35);
  setCentralWidget(view);
  view->setScene(scene);
}

GameScene::~GameScene() {}

void GameScene::initializeMap() {
  scene->clear();
  Field->reset();
  remain_mines = Field->getMinesNum();
  remain_init = Field->getWidth() * Field->getHeight();
  matrix = Field->getMatrix();
  for (int i = 0; i < matrix.size(); i++) {
    for (int j = 0; j < matrix[0].size(); j++) {
      std::cout << matrix[i][j] << ",";
    }
  }
  showMap();
}
void GameScene::showMap() {
  // 视图设置
  scene = new MyGraphicsScene;
  view = new QGraphicsView;
  setCentralWidget(view);
  view->setScene(scene);
  show();
  if (!scene->umarix.empty() && !scene->umarix[0].empty()) {
    unit_w = scene->umarix[0][0]->boundingRect().width();
    unit_h = scene->umarix[0][0]->boundingRect().height();
  }
  QScreen *screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  int screen_width = screenGeometry.width();
  int screen_height = screenGeometry.height();
  int map_width = Field->getWidth() * unit_w;
  int map_height = Field->getHeight() * unit_h;
  map_width = qMin(map_width, screen_width);
  map_height = qMin(map_height, screen_height - 2 * statusBar()->height());
  this->setFixedSize(map_width, map_height + statusBar()->height());
  view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
  startTimer();
}

void GameScene::easy() {
  Field->setWidthHeight(10, 6);
  initializeMap();
}

void GameScene::medium() {
  Field->setWidthHeight(20, 12);
  initializeMap();
}

void GameScene::hard() {
  Field->setWidthHeight(35, 24);
  initializeMap();
}

void GameScene::custom(int width, int height) {
  Field->setWidthHeight(width, height);
  initializeMap();
}

void GameScene::startTimer() {
  elapsedSeconds = 0;
  timeLabel->setText(formatTime(elapsedSeconds));

  // 设置定时器每秒触发一次
  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &GameScene::updateTimer);
  timer->start();
}
void GameScene::updateTimer() {
  elapsedSeconds++;
  timeLabel->setText(formatTime(elapsedSeconds));
  // statusBar()->showMessage("用时:  " + formatTime(elapsedSeconds));
}
void GameScene::stopTimer() { timer->stop(); }
QString GameScene::formatTime(int seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  return QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));
}
/**
 * 鼠标事件
 * 左键 - 判断
 * 右键 - 标记
 *
 */
void GameScene::mousePressEvent(QMouseEvent *event) {
  double x, y;
  x = event->position().x();
  y = event->position().y();

  int col = x / unit_w;
  int row = y / unit_h;
  qDebug() << "(" << x << "," << y << ")" << "---" << "(" << col << ", " << row
           << ")";

  /**
   * 左键点击事件
   */
  QGraphicsItem *item = view->itemAt(x, y);
  uitem = dynamic_cast<UnitItem *>(item);
  if (item != nullptr) {
    if (event->button() == Qt::LeftButton) {
      Field->visited[row][col] = 1;
      int N = matrix[row][col];
      if (INIT == uitem->getState()) {
        qDebug() << N;
        // 踩雷
        if (N == -1) {
          --remain_init;
          uitem->setState(EXPLOSIVE);
          if (IsFirstLeftClick) {
            do {
              initializeMap();
            } while (matrix[row][col] != -1);
            IsFirstLeftClick = false;
            N = matrix[row][col];
            if (N == 0) {
              uitem->setState(EMPTY);
              empty_extand(x, y, matrix);
            } else if (N >= 1 && N <= 8) {
              uitem->setState(NUM);
              if (isNumComputeCorrect(N)) {
                uitem->setState(DIGIT, N);
              };
            }
          }
          // 非首次踩雷 ，失败
          else {
            youLose();
            newGame();
          }
          qDebug() << "-1";
          scene->update();
        }
        // 空格
        else if (N == 0) {
          --remain_init;
          IsFirstLeftClick = false;
          empty_extand(x, y, matrix);
          uitem->setState(EMPTY);
          isWin();
          qDebug() << "0";
        }
        // 数字
        else {
          if (N >= 1 && N <= 8) {
            uitem->setState(NUM);
            surround_extand(row, col, x, y, matrix);
            IsFirstLeftClick = false;
            if (isNumComputeCorrect(N)) {
              uitem->setState(DIGIT, N);
              --remain_init;
              isWin();
              qDebug() << N;
            } else {
              youLose();
              newGame();
            }
          } else {
            qDebug() << "error num";
          }
        }
      }
      // NUM标记处理
      else if (NUM == uitem->getState()) {
        int N = matrix[row][col];
        qDebug() << N;
        if (isNumComputeCorrect(N)) {
          uitem->setState(DIGIT, N);
          --remain_init;
          isWin();
        }
      }
    } else if (event->button() == Qt::RightButton) {
      if (INIT == uitem->getState()) {
        uitem->setState(PUZZLE);
        --remain_init;
        if (!isWin()) ++remain_init;
      } else if (PUZZLE == uitem->getState()) {
        uitem->setState(INIT);
      }
    }
  }
}

void GameScene::empty_extand(double x, double y, Matrix matrix) {
  int col = std::floor(x / unit_w);
  int row = std::floor(y / unit_h);
  const int direction[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

  std::queue<std::pair<int, int>> que;
  que.push(std::make_pair(row, col));
  Field->visited[row][col] = 1;
  while (!que.empty()) {
    std::pair<int, int> cur = que.front();
    que.pop();
    int r = cur.first;
    int c = cur.second;

    for (int i = 0; i < 4; ++i) {
      int nx = r + direction[i][0];
      int ny = c + direction[i][1];

      if (nx >= 0 && nx < Field->getHeight() && ny >= 0 &&
          ny < Field->getWidth() && Field->visited[nx][ny] == 0) {
        Field->visited[nx][ny] = 1;
        QGraphicsItem *item = view->itemAt(ny * unit_w + 1, nx * unit_h + 1);

        if (item == nullptr) {
          qDebug() << "nullptr at (" << nx << ", " << ny << ")";
          continue;
        }

        UnitItem *uitem = dynamic_cast<UnitItem *>(item);
        if (uitem != nullptr) {
          int N = matrix[nx][ny];
          if (N == 0) {
            --remain_init;
            que.push(std::make_pair(nx, ny));
            uitem->setState(EMPTY);
            scene->update();
          } else if (N >= 1 && N <= 8) {
            uitem->setState(NUM);
            scene->update();
          }
        } else {
          qDebug() << "uitem is nullptr at (" << nx << ", " << ny << ")";
        }
      } else {
        qDebug() << "Out of bounds or already visited: (" << nx << ", " << ny
                 << ")";
      }
    }
  }
}

void GameScene::surround_extand(int row, int col, double pos_x, double pos_y,
                                Matrix matrix) {
  if (IsFirstLeftClick) {
    empty_extand(pos_x, pos_y, matrix);
  }
  if (matrix[row][col] > 0) {
    int emptyCount = 0;

    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        int nx = row + i;
        int ny = col + j;
        if (nx >= 0 && nx < matrix.size() && ny >= 0 && ny < matrix[0].size()) {
          if (matrix[nx][ny] == 0) {
            ++emptyCount;
          }
        }
      }
    }

    if (emptyCount > 1) {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0, 99);
      int randomValue = dis(gen);
      if (randomValue < 95) {
        empty_extand(pos_x, pos_y, matrix);
      }
    }
  }
}

void GameScene::youLose() {
  stopTimer();
  recordTime(elapsedSeconds, userName);
  QMessageBox::information(this, "Game Over", "You Lose");
}

void GameScene::newGame() {
  qDebug() << "New Game";
  initializeMap();
}

bool GameScene::isWin() {
  if (remain_init == remain_mines) {
    stopTimer();
    QMessageBox::information(this, "You Win!", "\n用时: " + timeLabel->text());
    recordTime(elapsedSeconds, userName);
    return true;
  };
  return false;
}
bool GameScene::isNumComputeCorrect(int N) {
  NumDialog numDialog(N, this);
  if (numDialog.exec() == QDialog::Accepted && numDialog.getResult()) {
    qDebug() << "Correct Num Computer: " << N;
    return true;
  }
  return false;
}

void GameScene::recordTime(int elapsedSeconds, QString userName) {
  QString timeString = QString::number(elapsedSeconds);
  userName = "Player";
  if (!userName.isEmpty()) {
    ;
  }
  QString dataPath = "../data/record.txt";
  QFile data(dataPath);
  if (data.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&data);
    out << userName << " " << timeString << "\n";
    data.close();
  } else {
    qDebug() << "Failed to open file for writing";
  }
}
