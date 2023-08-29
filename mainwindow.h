#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTimer>
#include <QKeyEvent>
#include <queue>
#include <vector>
#include <set>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene{new QGraphicsScene(this)};
    QGraphicsView *view{new QGraphicsView(scene,this)};

//    地图尺寸信息

    const size_t TILE_SIZE{20};
    const size_t SCENE_WIDTH{TILE_SIZE*50};
    const size_t SCENE_HEIGHT{TILE_SIZE*50};
    const size_t MAP_WIDTH{TILE_SIZE*30};
    const size_t MAP_HEIGHT{TILE_SIZE*30};

    enum DIRT{UP=1234,DOWN,LEFT,RIGHT};

//    逻辑组件

    QTimer *timer{new QTimer(this)};
    size_t time{0};

    void loop();

    void keyPressEvent(QKeyEvent *event) override;

    DIRT dirt{RIGHT};
//    实际正在前进的方向，用来避免180度转弯
    DIRT ongoing_dirt{dirt};

//  蛇的坐标信息
//    front: 晚加入的，即蛇尾
//    end: 新加入的，即蛇头
    std::deque<QGraphicsRectItem*> items_snake{};

//    蛇起点
    std::pair<int64_t,int64_t> start_pos{TILE_SIZE*1,TILE_SIZE*1};

//    食物
//    频繁删除插入，使用红黑树存储
    std::set<QGraphicsItem*> items_food{};

//    墙列表
//    基本不变动
    std::set<QGraphicsRectItem*> items_wall{};

//    生成食物函数
    void generate_food();

//    检测一个方块是否与一个队列相撞。
    template<class Tlist>
    bool check_collision(const QGraphicsRectItem *rect,Tlist l);
};
#endif // MAINWINDOW_H
