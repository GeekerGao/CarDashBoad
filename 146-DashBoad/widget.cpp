#include "widget.h"
#include "ui_widget.h"

#include <QPainter>
#include <QTimer>
#include <QtMath>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(800,600);
    startAngle = 150;
    mark = 0;
    currentValue = 0;
    startSpeed();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::startSpeed()
{
    timer = new QTimer();
    connect(timer,&QTimer::timeout,this,[=](){
        if(mark == 0){
            currentValue++;
            if(currentValue >= 61)
                mark = 1;
        }
        if(mark == 1)
            currentValue--;
        if(currentValue == 0){
            mark = 0;
        }
        update();
    });
    timer->start(50);
}

void Widget::initCanvas(QPainter& painter)
{
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    //底色设置成黑色
    painter.setBrush(Qt::black);
    painter.drawRect(rect());
    //坐标系平移到坐标中心
    QPoint cent(rect().width()/2,rect().height()*0.6);
    painter.translate(cent);
}

void Widget::drawMiddleCircle(QPainter &painter, int radius)
{
    painter.setPen(QPen(Qt::white,3));
    painter.drawEllipse(QPoint(0,0),radius,radius);
}

void Widget::drawCurrentSpeed(QPainter &painter)
{
    painter.setPen(Qt::white);
    QFont font("Arial",30);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRect(-60,-60,120,70),Qt::AlignCenter,QString::number(currentValue*4));
    QFont font2("Arial",13);
    font.setBold(true);
    painter.setFont(font2);
    painter.drawText(QRect(-60,-60,120,160),Qt::AlignCenter,"Km/h");
}

void Widget::drawScale(QPainter &painter,int radius)
{
    //1.算出1个刻度需要旋转的角度
    angle = 240*1.0 / 60;//240*1.0作用是扩大变量类型,把int型扩大为double类型
    //2.设置起始刻度位置
    painter.save();//保存当前坐标位置，此时此刻是在原点
    painter.setPen(QPen(Qt::white,5));
    painter.rotate(startAngle);
    for(int i=0;i<=60;i++){
        if(i >= 40){
            painter.setPen(QPen(Qt::red,5));
        }
        if(i % 5 == 0){
            //画长刻度线
            painter.drawLine(radius - 20,0,radius - 3,0);
        }else{//画短刻度线
            painter.drawLine(radius - 8,0,radius - 3,0);
        }
        //画完后旋转
        painter.rotate(angle);
    } 
    painter.restore();
    painter.setPen(QPen(Qt::white,5));
}

void Widget::drawScaleText(QPainter &painter, int radius)
{
    QFont font(QFont("Arial",15));
    font.setBold(true);
    painter.setFont(font);
    int r = radius - 49;
    for(int i=0;i<=60;i++){
        if(i % 5 == 0){
            //保存坐标系
            painter.save();
            //算出平移点
            int delX = qCos(qDegreesToRadians(210-angle*i)) * r;
            int delY = qSin(qDegreesToRadians(210-angle*i)) * r;
            //平移坐标系
            painter.translate(QPoint(delX,-delY));
            //旋转坐标系
            painter.rotate(-120+angle*i);//angle=4,30*4=120的时候，实参是0,
            //写上文字
            painter.drawText(-25,-25,50,30,Qt::AlignCenter,QString::number(i*4));
            //恢复坐标系
            painter.restore();
        }
    }
}

void Widget::drawPointLine(QPainter &painter,int lenth)
{

    painter.save();
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    static const QPointF points[4] = {
        QPointF(0,0),
        QPointF(200.0,-1.1),
        QPointF(200.0,1.1),
        QPointF(0,15.0),
    };
    painter.rotate(startAngle + angle * currentValue);
    painter.drawPolygon(points, 4);
    painter.restore();
}

void Widget::drawSpeedPie(QPainter &painter, int radius)
{
    QRect rentangle(-radius,-radius,radius*2,radius*2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255,0,0,80));
    painter.drawPie(rentangle,(360-startAngle)*16,-angle*currentValue*16);//angle前面取负数为了让它顺时针方向画
}

void Widget::drawEllipseInnerBlack(QPainter &painter, int radius)
{
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPoint(0,0),radius,radius);
}

void Widget::drawEllipseInnerShine(QPainter &painter, int radius)
{
    QRadialGradient radaGradient(0,0,radius);
    radaGradient.setColorAt(0.0,QColor(255,0,0,200));
    radaGradient.setColorAt(1.0,QColor(0,0,0,100));
    painter.setBrush(radaGradient);
    painter.drawEllipse(QPoint(0,0),radius,radius);
}

void Widget::drawEllipseOutShine(QPainter &painter, int radius)
{
    QRect rentangle(-radius,-radius,radius*2,radius*2);
    painter.setPen(Qt::NoPen);

    QRadialGradient radiaGradient(0,0,radius);
    radiaGradient.setColorAt(1.0,QColor(255,0,0,200));
    radiaGradient.setColorAt(0.97,QColor(255,0,0,70));
    radiaGradient.setColorAt(0.9,QColor(0,0,0,0));
    radiaGradient.setColorAt(0,QColor(0,0,0,0));
    painter.setBrush(radiaGradient);

    painter.drawPie(rentangle,(360-150)*16,-angle*61*16);//angle前面取负数为了让它顺时针方向画
}

void Widget::drawLogo(QPainter &painter, int radius)
{
    QRect rectangle(-65,radius*0.38,130,50);
    painter.drawPixmap(rectangle,QPixmap(":/icon.png"));
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int radius = height()/2;
    //初始化画布
    initCanvas(painter);
    //画小圆
    drawMiddleCircle(painter,60);
    //画刻度
    drawScale(painter,radius);
    //画刻度文字
    drawScaleText(painter,radius);
    //画指针
    drawPointLine(painter,radius-58);
    //画扇形
    drawSpeedPie(painter,radius+25);
    //画渐变内圈圆
    drawEllipseInnerShine(painter,110);
    //画黑色内圈
    drawEllipseInnerBlack(painter,80);
    //画当前速度
    drawCurrentSpeed(painter);
    //画外圆圆
    drawEllipseOutShine(painter,radius+25);
    //画一个汽车Logo
    drawLogo(painter,radius);
}



