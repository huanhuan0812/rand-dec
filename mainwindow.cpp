#include "mainwindow.h"
#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_radius(20)
    , m_animationCount(0)
    , m_finalNumber(0)
    , m_nameAnimation(nullptr)
{
    // 设置窗口属性：无边框、置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // 设置窗口大小
    resize(400, 350);

    // 设置窗口透明度 (0.9)
    setWindowOpacity(0.9);

    // 从文件加载名字列表
    loadNamesFromFile("names.txt");

    // 设置标签
    setupLabels();

    // 创建动画定时器
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &MainWindow::updateAnimation);

    // 应用圆角遮罩
    applyRoundedMask();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupLabels()
{
    // 创建显示学号的Label - 初始在屏幕中间
    m_numberLabel = new QLabel(this);
    m_numberLabel->setAlignment(Qt::AlignCenter);
    m_numberLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 72px;"
        "   font-weight: bold;"
        "   background-color: transparent;"
        "}"
    );
    // 学号初始在屏幕中间
    m_numberLabel->setGeometry(0, 100, width(), 150);

    // 创建显示姓名的Label - 初始在屏幕中间，但透明
    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "   color: #FFD700;"
        "   font-size: 36px;"
        "   font-weight: bold;"
        "   background-color: transparent;"
        "}"
    );
    // 姓名初始也在屏幕中间
    m_nameLabel->setGeometry(0, 100, width(), 150);

    // 设置透明度效果（初始为完全透明）
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0);
    m_nameLabel->setGraphicsEffect(opacityEffect);

    // 初始显示随机学号
    if (!m_nameList.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(m_nameList.size());
        m_finalNumber = m_nameList[randomIndex].first;
        m_finalName = m_nameList[randomIndex].second;
        m_numberLabel->setText(QString::number(m_finalNumber));
    } else {
        m_finalNumber = QRandomGenerator::global()->bounded(1, 48);
        m_finalName = "未知";
        m_numberLabel->setText(QString::number(m_finalNumber));
    }

    // 确保姓名标签初始为空
    m_nameLabel->setText("");
}

void MainWindow::loadNamesFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filename << "，使用默认数据";
        m_nameList.append({1, "张三"});
        m_nameList.append({2, "李四"});
        m_nameList.append({3, "王五"});
        m_nameList.append({4, "赵六"});
        m_nameList.append({5, "孙七"});
        m_nameList.append({6, "周八"});
        m_nameList.append({7, "吴九"});
        m_nameList.append({8, "郑十"});
        return;
    }

    QTextStream in(&file);
    m_nameList.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(',');
        if (parts.size() >= 2) {
            bool ok;
            int id = parts[0].trimmed().toInt(&ok);
            if (ok) {
                QString name = parts[1].trimmed();
                m_nameList.append({id, name});
                qDebug() << "加载: 学号=" << id << "姓名=" << name;
            }
        }
    }

    file.close();
    qDebug() << "成功加载" << m_nameList.size() << "个名字";

    if (m_nameList.isEmpty()) {
        qDebug() << "加载数据为空，使用默认数据";
        m_nameList.append({1, "张三"});
        m_nameList.append({2, "李四"});
        m_nameList.append({3, "王五"});
        m_nameList.append({4, "赵六"});
        m_nameList.append({5, "孙七"});
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), m_radius, m_radius);
    painter.fillPath(path, Qt::black);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    applyRoundedMask();

    // 更新标签位置
    if (m_numberLabel) {
        m_numberLabel->setGeometry(0, 100, width(), 150);
    }
    if (m_nameLabel) {
        m_nameLabel->setGeometry(0, 100, width(), 150);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (!isActiveWindow()) {
            QApplication::quit();
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    startRandomAnimation();
}

void MainWindow::applyRoundedMask()
{
    if (width() <= 0 || height() <= 0)
        return;

    QPainterPath path;
    path.addRoundedRect(rect(), m_radius, m_radius);
    QRegion region = QRegion(path.toFillPolygon().toPolygon());
    setMask(region);
}

void MainWindow::startRandomAnimation()
{
    if (m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }

    // 重置学号到屏幕中间
    m_numberLabel->move(0, 100);
    m_numberLabel->setGeometry(0, 100, width(), 150);

    // 隐藏姓名
    if (QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_nameLabel->graphicsEffect())) {
        effect->setOpacity(0);
    }
    m_nameLabel->setText("");
    // 重置姓名位置到中间
    m_nameLabel->setGeometry(0, 100, width(), 150);

    // 重置计数
    m_animationCount = 0;

    // 启动定时器，每50ms更新一次
    m_animationTimer->start(50);
}

void MainWindow::updateAnimation()
{
    m_animationCount++;

    if (m_animationCount <= 20) { // 20次 * 50ms = 1秒
        // 动画过程中在屏幕中间显示随机学号
        if (!m_nameList.isEmpty()) {
            int randomIndex = QRandomGenerator::global()->bounded(m_nameList.size());
            int randomNum = m_nameList[randomIndex].first;
            m_numberLabel->setText(QString::number(randomNum));
        }
    } else {
        // 动画结束，停止定时器并生成最终结果
        m_animationTimer->stop();
        generateResult();
        // 显示姓名动画（学号上移，姓名从中间淡入并下移）
        showNameWithAnimation();
    }
}

void MainWindow::generateResult()
{
    if (!m_nameList.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(m_nameList.size());
        m_finalNumber = m_nameList[randomIndex].first;
        m_finalName = m_nameList[randomIndex].second;
        m_numberLabel->setText(QString::number(m_finalNumber));
        m_nameLabel->setText(m_finalName);
        qDebug() << "最终结果: 学号=" << m_finalNumber << "姓名=" << m_finalName;
    } else {
        m_finalNumber = QRandomGenerator::global()->bounded(1, 48);
        m_finalName = "未知";
        m_numberLabel->setText(QString::number(m_finalNumber));
        m_nameLabel->setText(m_finalName);
        qDebug() << "最终结果(默认): 学号=" << m_finalNumber << "姓名=" << m_finalName;
    }
}

void MainWindow::showNameWithAnimation()
{
    // 清理旧动画
    if (m_nameAnimation) {
        m_nameAnimation->stop();
    }

    // 获取透明度效果
    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_nameLabel->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(this);
        effect->setOpacity(0);
        m_nameLabel->setGraphicsEffect(effect);
    }

    // 确保姓名文本已设置
    if (m_nameLabel->text().isEmpty()) {
        m_nameLabel->setText(m_finalName);
        qDebug() << "设置姓名文本:" << m_finalName;
    }

    // 缩小间距：学号向上移动少一点，姓名向下移动少一点
    int numberStartY = 100;     // 学号起始位置（中间）
    int numberEndY = 60;        // 学号结束位置（稍微靠上）

    // 姓名从中间平移到学号下方
    int nameStartY = 100;       // 姓名起始位置（中间）
    int nameEndY = 140;         // 姓名结束位置（学号下方）

    // 先设置初始位置
    m_numberLabel->move(0, numberStartY);
    m_nameLabel->move(0, nameStartY);
    effect->setOpacity(0);

    // 创建学号平移动画（向上移动）- 使用平滑缓动，无弹跳
    QPropertyAnimation* numberMoveAnimation = new QPropertyAnimation(m_numberLabel, "pos", this);
    numberMoveAnimation->setDuration(600);
    numberMoveAnimation->setStartValue(QPoint(0, numberStartY));
    numberMoveAnimation->setEndValue(QPoint(0, numberEndY));
    numberMoveAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 创建姓名平移动画（向下移动）- 使用平滑缓动，无弹跳
    QPropertyAnimation* nameMoveAnimation = new QPropertyAnimation(m_nameLabel, "pos", this);
    nameMoveAnimation->setDuration(600);
    nameMoveAnimation->setStartValue(QPoint(0, nameStartY));
    nameMoveAnimation->setEndValue(QPoint(0, nameEndY));
    nameMoveAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 创建姓名透明度动画（从透明到不透明）
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(effect, "opacity", this);
    opacityAnimation->setDuration(600);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 并行播放三个动画
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    group->addAnimation(numberMoveAnimation);
    group->addAnimation(nameMoveAnimation);
    group->addAnimation(opacityAnimation);

    // 使用智能指针保存
    m_nameAnimation = group;

    // 连接信号，用于调试
    connect(group, &QParallelAnimationGroup::finished, [this]() {
        qDebug() << "动画完成，学号位置:" << m_numberLabel->pos() 
                 << "姓名位置:" << m_nameLabel->pos()
                 << "姓名:" << m_nameLabel->text();
    });

    // 启动动画，设置自动删除
    group->start(QAbstractAnimation::DeleteWhenStopped);
}