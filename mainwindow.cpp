#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene->setSceneRect(0,0,SCENE_WIDTH,SCENE_HEIGHT);

    view->setFixedSize(SCENE_WIDTH,SCENE_HEIGHT);
    view->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setCentralWidget(view);

    auto add_wall=[this](int64_t x,int64_t y
                           ,size_t width,size_t height){
        QGraphicsRectItem *new_wall=new QGraphicsRectItem(0,0,width,height);
        scene->addItem(new_wall);
        items_wall.insert(new_wall);
        new_wall->setPos(x,y);
        new_wall->setBrush(Qt::blue);
    };
    add_wall(0,0,MAP_WIDTH,TILE_SIZE);
    add_wall(0,0,TILE_SIZE,MAP_HEIGHT);
    add_wall(0,MAP_HEIGHT-TILE_SIZE,MAP_WIDTH,TILE_SIZE);
    add_wall(MAP_WIDTH-TILE_SIZE,0,TILE_SIZE,MAP_HEIGHT);

//    初始蛇
    QGraphicsRectItem *snake_head=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
    snake_head->setBrush(Qt::black);
    scene->addItem(snake_head);
    snake_head->setPos(start_pos.first,start_pos.second);
    items_snake.push_back(snake_head);

//    初始食物
    QGraphicsRectItem *food=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
    food->setBrush(Qt::red);
    scene->addItem(food);
    food->setPos(TILE_SIZE*5,TILE_SIZE*5);
    items_food.insert(food);

//    设置计时器
    connect(timer,&QTimer::timeout,
            this,&MainWindow::loop);
    timer->start(200);
}

MainWindow::~MainWindow()
{
    delete ui;
//    其他变量设置this为父对象，不需要手动析构
}

void MainWindow::loop(){
    time++;

//    移动
    const auto &items=scene->items();
    QGraphicsItem *tail_tile=items_snake.empty()?nullptr:items_snake.front();
    QGraphicsItem *head_tile=items_snake.empty()?nullptr:items_snake.back();
    QGraphicsRectItem *tile;
    int64_t head_x=0,head_y=0;
    if(head_tile!=nullptr){
        head_x=head_tile->pos().x();
        head_y=head_tile->pos().y();
    }

//    根据方向移动头部
//    更新头部坐标
    if(dirt==UP){
        if(ongoing_dirt!=DOWN){
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x,head_y-TILE_SIZE);
            items_snake.push_back(tile);

            ongoing_dirt=UP;
        }
        else{
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x,head_y+TILE_SIZE);
            items_snake.push_back(tile);
        }
    }
    else if(dirt==DOWN){
        if(ongoing_dirt!=UP){
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x,head_y+TILE_SIZE);
            items_snake.push_back(tile);

            ongoing_dirt=DOWN;
        }
        else{
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x,head_y-TILE_SIZE);
            items_snake.push_back(tile);
        }
    }
    else if(dirt==LEFT){
        if(ongoing_dirt!=RIGHT){
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x-TILE_SIZE,head_y);
            items_snake.push_back(tile);

            ongoing_dirt=LEFT;
        }
        else{
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x+TILE_SIZE,head_y);
            items_snake.push_back(tile);
        }
    }
    else if(dirt==RIGHT){
        if(ongoing_dirt!=LEFT){
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x+TILE_SIZE,head_y);
            items_snake.push_back(tile);

            ongoing_dirt=RIGHT;
        }
        else{
            tile=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
            tile->setBrush(Qt::black);
            scene->addItem(tile);
            tile->setPos(head_x-TILE_SIZE,head_y);
            items_snake.push_back(tile);
        }
    }

//    查找是否吃掉果实
    bool is_eat=false;
    const QGraphicsRectItem *new_head=items_snake.back();
    for(auto i=items_food.begin();i!=items_food.end();i++){
        if((*i)->pos()==new_head->pos()){
            is_eat=true;

            scene->removeItem(*i);
            items_food.erase(i);
            break;
        }
    }

//    删除尾部
//    更新尾部坐标
//    若吃掉果实则不必删除尾部
    if(tail_tile!=nullptr){
        if(!is_eat){
            scene->removeItem(items_snake.front());
            items_snake.pop_front();
        }
//        吃掉果实
        else{
            generate_food();
        }
    }

//    检测碰撞
    if(check_collision<std::set<QGraphicsRectItem*>>(new_head,items_wall)){
        qDebug()<<"collision";
        close();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_W){
        dirt=UP;
    }
    else if(event->key()==Qt::Key_S){
        dirt=DOWN;
    }
    else if(event->key()==Qt::Key_A){
        dirt=LEFT;
    }
    else if(event->key()==Qt::Key_D){
        dirt=RIGHT;
    }
}

void MainWindow::generate_food(){
    std::random_device rd;
    std::mt19937 gen(rd());
    int64_t lo_x=1;
    int64_t hi_x=MAP_WIDTH/TILE_SIZE-2;
    int64_t lo_y=1;
    int64_t hi_y=MAP_HEIGHT/TILE_SIZE-2;
    std::uniform_int_distribution<int64_t> x_dist(lo_x,hi_x);
    std::uniform_int_distribution<int64_t> y_dist(lo_y,hi_y);
    while(1){
        //    产生随机坐标
        int64_t cnt_x=x_dist(gen);
        int64_t cnt_y=y_dist(gen);
        qDebug()<<cnt_x<<" "<<cnt_y;

        //    （暂时）添加到地图中，不涂色
        QGraphicsRectItem *food=new QGraphicsRectItem(0,0,TILE_SIZE,TILE_SIZE);
        scene->addItem(food);
        food->setPos(TILE_SIZE*cnt_x,TILE_SIZE*cnt_y);

        //    判断是否与墙相撞
        if(check_collision<std::deque<QGraphicsRectItem*>>(food,items_snake)){
            scene->removeItem(food);
            continue;
        }
        //    判断是否与蛇身相撞
        else if(check_collision<std::set<QGraphicsRectItem*>>(food,items_wall)){
            scene->removeItem(food);
            continue;
        }

//        无相撞，确认添加，退出
        food->setBrush(Qt::red);
        items_food.insert(food);
        return;
    }
}

template<class Tlist>
bool MainWindow::check_collision(const QGraphicsRectItem *rect,Tlist l){
    for(auto i=l.begin();i!=l.end();i++){
        if(
            (*i)->pos().x()<=rect->pos().x()&&
            (*i)->pos().y()<=rect->pos().y()&&
            rect->pos().x()<(*i)->pos().x()+(*i)->rect().width()&&
            rect->pos().y()<(*i)->pos().y()+(*i)->rect().height()){
            return true;
        }
    }
    return false;
}
