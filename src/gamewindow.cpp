#include "gamewindow.h"

#include "NumDialog.h"
#include "fielddata.h"
#include "unititem.h"

/**
 * IsFirstLeftClick : 用于防止开局点到雷而直接失败
 */
bool IsFirstLeftClick = true;

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
      timeLabel(new QLabel(this)) {
  int initWidth = 1600, initHeight = 1300;
  this->setFixedSize(initWidth, initHeight);
  scene = new MyGraphicsScene;
  QString iconPath = "../resource/img/icon/icon.png";
  this->setWindowIcon(QIcon(iconPath));
  this->setWindowTitle("扫雷(All Hell's Mathematics!)");
  QPushButton *backButton = new QPushButton(this);
  backButton->setGeometry(QRect(10, 10, 15, 20));
  QString backIcon = "../resource/img/icon/back.png";
  backButton->setIcon(QIcon(backIcon));
  backButton->setIconSize(QSize(10, 10));
  QStatusBar *statusBar = this->statusBar();
  timeLabel = new QLabel(this);
  statusBar->addWidget(backButton);
  statusBar->addPermanentWidget(timeLabel);
  statusBar->setFixedHeight(35);
  this->setCentralWidget(view);
  view->setScene(scene);
  connect(backButton, &QPushButton::clicked, this,
          [=]() { emit this->back_level(); });
}

GameScene::~GameScene() {
  delete scene;
  delete view;
}

void GameScene::initializeMap() {
  scene = new MyGraphicsScene;
  Field->reset();
  remain_mines = Field->getMinesNum();
  remain_init = Field->getWidth() * Field->getHeight();
  view->setScene(scene);
  showMap();
}
void GameScene::showMap() {
  if (!scene->umarix.empty() && !scene->umarix[0].empty()) {
    unit_w = scene->umarix[0][0]->boundingRect().width();
    unit_h = scene->umarix[0][0]->boundingRect().height();
  }
  int map_width = (Field->getWidth() + 2) * unit_w;
  int map_height = (Field->getHeight() + 3) * unit_h;
  this->setFixedSize(map_width, map_height);
  this->show();
  startTimer();
}

void GameScene::easy() {
  record.level = "easy: 10*6";
  Field->setWidthHeight(10, 6);
  initializeMap();
}

void GameScene::medium() {
  record.level = "medium: 20*12";
  Field->setWidthHeight(20, 12);
  initializeMap();
}

void GameScene::hard() {
  record.level = "hard: 35*24";
  Field->setWidthHeight(35, 24);
  initializeMap();
}

void GameScene::custom(int width, int height) {
  record.level =
      "custom: " + QString::number(width) + "*" + QString::number(height);
  Field->setWidthHeight(width, height);
  initializeMap();
}

void GameScene::startTimer() {
  record.elapsedTime = 0;
  timeLabel->setText(formatTime(record.elapsedTime));
  // 设置定时器每秒触发一次
  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &GameScene::updateTimer);
  timer->start();
}
void GameScene::updateTimer() {
  record.elapsedTime++;
  timeLabel->setText(formatTime(record.elapsedTime));
}
void GameScene::stopTimer() {
  timer->stop();
  // 记录系统日期+时间
  QDateTime current_date_time = QDateTime::currentDateTime();
  QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
  record.realTime = current_date;
  recordToLocal(record);
}
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

/**
 * Q:
 * libc++abi: terminating due to uncaught exception of type
 * std::out_of_range:vector position function is not correct
 *
 */

void GameScene::mousePressEvent(QMouseEvent *event) {
  double x, y;
  x = event->position().x();
  y = event->position().y();
  int col = x / unit_w - 1;
  int row = y / unit_h - 1;
  if (row >= Field->getHeight() + 2 || col >= Field->getWidth() + 2) return;
  if (row < 0 || row >= Field->getMatrix().size() || col < 0 ||
      col >= Field->getMatrix()[row].size()) {
    qDebug() << "Invalid row or col index: (" << row << ", " << col << ")";
    return;
  }
  qDebug() << "(" << x << "," << y << ")" << "---" << "(" << col << ", " << row
           << ")";
  /**
   * 左键点击事件
   */
  QGraphicsItem *item = view->itemAt(x, y);
  if (item == nullptr) return;
  UnitItem *uitem = dynamic_cast<UnitItem *>(item);
  if (uitem == nullptr) return;
  if (event->button() == Qt::LeftButton) {
    Field->visited[row][col] = 1;

    int N = Field->getMatrix().at(row).at(col);
    if (INIT == uitem->getState()) {
      qDebug() << N;
      // 踩雷
      if (N == -1) {
        --remain_init;
        uitem->setState(EXPLOSIVE);
        if (IsFirstLeftClick) {
          do {
            initializeMap();
          } while (Field->getMatrix().at(row).at(col) != -1);
          IsFirstLeftClick = false;
          N = Field->getMatrix().at(row).at(col);
          if (N == 0) {
            uitem->setState(EMPTY);
            empty_extand(x, y);
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
          emit newGame(Field->getWidth(), Field->getHeight());
        }
        qDebug() << "-1";
        scene->update();
      }
      // 空格
      else if (N == 0) {
        --remain_init;
        IsFirstLeftClick = false;
        empty_extand(x, y);
        uitem->setState(EMPTY);
        isWin();
        qDebug() << "0";
      }
      // 数字
      else {
        if (N >= 1 && N <= 8) {
          uitem->setState(NUM);
          surround_extand(row, col, x, y);
          IsFirstLeftClick = false;
          if (isNumComputeCorrect(N)) {
            uitem->setState(DIGIT, N);
            --remain_init;
            isWin();
            qDebug() << N;
          } else {
            youLose();
            emit newGame(Field->getWidth(), Field->getHeight());
          }
        } else {
          qDebug() << "error num";
        }
      }
    }
    // NUM标记处理
    else if (NUM == uitem->getState()) {
      int N = Field->getMatrix().at(row).at(col);
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
  scene->update();
}

// 递归扩展
void GameScene::empty_extand(double x, double y) {
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
        QGraphicsItem *item = view->itemAt(ny * unit_w, nx * unit_h);
        if (item != nullptr) {
          UnitItem *uitem = dynamic_cast<UnitItem *>(item);
          if (uitem != nullptr) {
            int N = Field->getMatrix().at(nx).at(ny);
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
}

void GameScene::surround_extand(int row, int col, double pos_x, double pos_y) {
  if (IsFirstLeftClick) {
    empty_extand(pos_x, pos_y);
  }
  if (Field->getMatrix().at(row).at(col) > 0) {
    int emptyCount = 0;

    for (int i = -1; i <= 1; ++i) {
      for (int j = -1; j <= 1; ++j) {
        int nx = row + i;
        int ny = col + j;
        if (nx >= 0 && nx < Field->getMatrix().size() && ny >= 0 &&
            ny < Field->getMatrix().at(0).size()) {
          if (Field->getMatrix().at(row).at(col) == 0) {
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
        empty_extand(pos_x, pos_y);
      }
    }
  }
}

void GameScene::youLose() {
  stopTimer();
  QMessageBox::information(this, "Game Over", "You Lose");
}

bool GameScene::isWin() {
  if (remain_init == remain_mines) {
    stopTimer();
    QMessageBox::information(this, "You Win!", "\n用时: " + timeLabel->text());
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

void GameScene::recordToLocal(recordInfo record) {
  QString examineLoginInfoPath = "../data/loginInfo";
  QFile examineLoginFile(examineLoginInfoPath);
  if (examineLoginFile.exists()) {
    if (examineLoginFile.open(QIODevice::ReadOnly)) {
      QJsonObject json =
          QJsonDocument::fromJson(examineLoginFile.readAll()).object();
      record.userName = json["user"].toObject()["username/email"].toString();
    }
  }
  examineLoginFile.close();
  QString recordPath = "../data/record.txt";
  QFile recordFile(recordPath);
  if (recordFile.open(QIODevice::ReadWrite)) {
    QTextStream in(&recordFile);
    QString recordIn = in.readAll();
    recordIn.replace(QRegularExpression("^[^\n]+"),
                     record.userName + " " + record.realTime + " " +
                         record.level + " " +
                         QString::number(record.elapsedTime));
    recordFile.resize(0);
    in << recordIn;
    recordFile.close();
  }
  recordFile.close();
}
