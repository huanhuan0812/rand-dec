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
#include <QSettings>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_radius(20)
    , m_animationCount(0)
    , m_finalNumber(0)
    , m_nameAnimation(nullptr)
    , m_mode(1)
    , m_isRolling(false)
    , m_modeButton(nullptr)
{
    // 设置窗口属性：无边框、置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // 设置窗口大小
    resize(400, 350);

    // 设置窗口透明度 (0.9)
    setWindowOpacity(0.9);

    // 加载配置文件
    loadConfig();

    // 从文件加载名字列表
    loadNamesFromFile("names.txt");

    // 设置标签
    setupLabels();

    // 创建模式切换按钮
    setupModeButton();

    // 创建动画定时器
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &MainWindow::updateAnimation);

    // 应用圆角遮罩
    applyRoundedMask();
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadConfig()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    m_mode = settings.value("Settings/Mode", 1).toInt();
    
    // 限制模式范围
    if (m_mode < 0 || m_mode > 1) {
        m_mode = 1;
    }
    
    qDebug() << "当前模式:" << (m_mode == 0 ? "自动停止" : "手动停止");
}

void MainWindow::setupLabels()
{
    // 创建显示学号的Label
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
    m_numberLabel->setGeometry(0, 100, width(), 150);
    m_numberLabel->setText("");

    // 创建显示姓名的Label
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
    m_nameLabel->setGeometry(0, 100, width(), 150);
    m_nameLabel->setText("");
    
    // 设置透明度效果（初始为完全透明）
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0);
    m_nameLabel->setGraphicsEffect(opacityEffect);

    // 创建提示标签
    m_hintLabel = new QLabel(this);
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setStyleSheet(
        "QLabel {"
        "   color: rgba(255, 255, 255, 150);"
        "   font-size: 24px;"
        "   font-weight: normal;"
        "   background-color: transparent;"
        "}"
    );
    m_hintLabel->setGeometry(0, 260, width(), 50);
    
    // 根据模式设置提示文字
    if (m_mode == 0) {
        m_hintLabel->setText("点击以开始");
    } else {
        m_hintLabel->setText("点击开始滚动");
    }
}

void MainWindow::setupModeButton()
{
    m_modeButton = new QPushButton(this);
    m_modeButton->setFixedSize(70, 34);
    m_modeButton->setCheckable(true);
    m_modeButton->setChecked(m_mode == 0);
    
    // 更新按钮样式
    updateModeButton();
    
    // 连接信号槽
    connect(m_modeButton, &QPushButton::clicked, this, &MainWindow::toggleMode);
    
    // 设置位置（右下角）
    updateButtonPosition();
}

void MainWindow::updateButtonPosition()
{
    if (m_modeButton) {
        int x = width() - m_modeButton->width() - 15;
        int y = height() - m_modeButton->height() - 15;
        m_modeButton->move(x, y);
    }
}

void MainWindow::updateModeButton()
{
    if (!m_modeButton) return;
    
    // Switch按钮样式 - 蓝色滑动开关
    QString switchStyle;
    
    if (m_mode == 0) {
        // 开启状态 - 蓝色
        switchStyle = 
            "QPushButton {"
            "   background-color: #2196F3;"
            "   border: 2px solid #2196F3;"
            "   border-radius: 17px;"
            "   color: white;"
            "   font-size: 11px;"
            "   font-weight: bold;"
            "   text-align: left;"
            "   padding-left: 8px;"
            "}"
            "QPushButton::indicator {"
            "   width: 0px;"
            "   height: 0px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #1976D2;"
            "   border-color: #1976D2;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #0D47A1;"
            "   border-color: #0D47A1;"
            "}";
        m_modeButton->setText("自动");
    } else {
        // 关闭状态 - 灰色
        switchStyle = 
            "QPushButton {"
            "   background-color: #BDBDBD;"
            "   border: 2px solid #BDBDBD;"
            "   border-radius: 17px;"
            "   color: white;"
            "   font-size: 11px;"
            "   font-weight: bold;"
            "   text-align: right;"
            "   padding-right: 8px;"
            "}"
            "QPushButton::indicator {"
            "   width: 0px;"
            "   height: 0px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #9E9E9E;"
            "   border-color: #9E9E9E;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #757575;"
            "   border-color: #757575;"
            "}";
        m_modeButton->setText("手动");
    }
    
    m_modeButton->setStyleSheet(switchStyle);
}

void MainWindow::toggleMode()
{
    // 如果正在滚动，先停止
    if (m_isRolling) {
        if (m_animationTimer->isActive()) {
            m_animationTimer->stop();
        }
        m_isRolling = false;
    }
    
    // 如果有动画在播放，停止
    if (m_nameAnimation) {
        m_nameAnimation->stop();
    }
    
    // 切换模式
    m_mode = (m_mode == 0) ? 1 : 0;
    
    // 更新配置文件
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("Settings/Mode", m_mode);
    
    // 更新按钮显示
    updateModeButton();
    
    // 更新提示文字
    if (m_hintLabel) {
        if (m_mode == 0) {
            m_hintLabel->setText("点击以开始");
        } else {
            m_hintLabel->setText("点击开始滚动");
        }
        m_hintLabel->show();
    }
    
    // 重置显示
    m_numberLabel->setText("");
    m_nameLabel->setText("");
    if (QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_nameLabel->graphicsEffect())) {
        effect->setOpacity(0);
    }
    m_numberLabel->move(0, 100);
    m_nameLabel->move(0, 100);
    
    qDebug() << "切换到模式:" << (m_mode == 0 ? "自动停止" : "手动停止");
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
    if (m_hintLabel) {
        m_hintLabel->setGeometry(0, 260, width(), 50);
    }
    
    // 更新按钮位置
    updateButtonPosition();
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
    
    if (m_mode == 0) {
        // 自动停止模式：点击开始，自动停止
        startRandomAnimation();
    } else {
        // 手动停止模式：点击切换滚动/停止
        if (m_isRolling) {
            // 正在滚动，停止
            stopRolling();
        } else {
            // 未滚动，开始
            startRolling();
        }
    }
}

void MainWindow::startRolling()
{
    if (m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }

    // 隐藏提示标签
    if (m_hintLabel) {
        m_hintLabel->hide();
    }

    // 重置学号到屏幕中间并清空
    m_numberLabel->move(0, 100);
    m_numberLabel->setGeometry(0, 100, width(), 150);
    m_numberLabel->setText("");

    // 隐藏姓名
    if (QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_nameLabel->graphicsEffect())) {
        effect->setOpacity(0);
    }
    m_nameLabel->setText("");
    m_nameLabel->setGeometry(0, 100, width(), 150);

    // 重置计数
    m_animationCount = 0;
    m_isRolling = true;

    // 启动定时器，每50ms更新一次
    m_animationTimer->start(50);
}

void MainWindow::stopRolling()
{
    // 停止定时器
    if (m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }
    
    m_isRolling = false;
    
    // 生成最终结果
    generateResult();
    
    // 显示姓名动画
    showNameWithAnimation();
}

void MainWindow::startRandomAnimation()
{
    if (m_animationTimer->isActive()) {
        m_animationTimer->stop();
    }

    // 隐藏提示标签
    if (m_hintLabel) {
        m_hintLabel->hide();
    }

    // 重置学号到屏幕中间并清空
    m_numberLabel->move(0, 100);
    m_numberLabel->setGeometry(0, 100, width(), 150);
    m_numberLabel->setText("");

    // 隐藏姓名
    if (QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(m_nameLabel->graphicsEffect())) {
        effect->setOpacity(0);
    }
    m_nameLabel->setText("");
    m_nameLabel->setGeometry(0, 100, width(), 150);

    // 重置计数
    m_animationCount = 0;

    // 启动定时器，每50ms更新一次
    m_animationTimer->start(25);
}

void MainWindow::updateAnimation()
{
    m_animationCount++;

    if (m_mode == 0) {
        // 自动停止模式：20次后停止 (40 * 25ms = 1秒)
        if (m_animationCount <= 40) {
            if (!m_nameList.isEmpty()) {
                int randomIndex = QRandomGenerator::global()->bounded(m_nameList.size());
                int randomNum = m_nameList[randomIndex].first;
                m_numberLabel->setText(QString::number(randomNum));
            }
        } else {
            m_animationTimer->stop();
            generateResult();
            showNameWithAnimation();
        }
    } else {
        // 手动停止模式：持续滚动
        if (!m_nameList.isEmpty()) {
            int randomIndex = QRandomGenerator::global()->bounded(m_nameList.size());
            int randomNum = m_nameList[randomIndex].first;
            m_numberLabel->setText(QString::number(randomNum));
        }
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

    int numberStartY = 100;
    int numberEndY = 60;
    int nameStartY = 100;
    int nameEndY = 140;

    // 先设置初始位置
    m_numberLabel->move(0, numberStartY);
    m_nameLabel->move(0, nameStartY);
    effect->setOpacity(0);

    // 创建学号平移动画
    QPropertyAnimation* numberMoveAnimation = new QPropertyAnimation(m_numberLabel, "pos", this);
    numberMoveAnimation->setDuration(600);
    numberMoveAnimation->setStartValue(QPoint(0, numberStartY));
    numberMoveAnimation->setEndValue(QPoint(0, numberEndY));
    numberMoveAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 创建姓名平移动画
    QPropertyAnimation* nameMoveAnimation = new QPropertyAnimation(m_nameLabel, "pos", this);
    nameMoveAnimation->setDuration(600);
    nameMoveAnimation->setStartValue(QPoint(0, nameStartY));
    nameMoveAnimation->setEndValue(QPoint(0, nameEndY));
    nameMoveAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // 创建姓名透明度动画
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

    m_nameAnimation = group;

    // 动画结束后重新显示提示
    connect(group, &QParallelAnimationGroup::finished, [this]() {
        qDebug() << "动画完成，学号位置:" << m_numberLabel->pos() 
                 << "姓名位置:" << m_nameLabel->pos()
                 << "姓名:" << m_nameLabel->text();
        
        if (m_hintLabel) {
            if (m_mode == 0) {
                m_hintLabel->setText("点击再次抽选");
            } else {
                m_hintLabel->setText("点击开始滚动");
            }
            m_hintLabel->show();
        }
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
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