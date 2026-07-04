#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <QString>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPointer>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void toggleMode();

private:
    void applyRoundedMask();
    void startRandomAnimation();      // 自动停止模式
    void startRolling();              // 手动停止模式：开始滚动
    void stopRolling();               // 手动停止模式：停止滚动
    void updateAnimation();
    void generateResult();
    void loadNamesFromFile(const QString& filename);
    void showNameWithAnimation();
    void setupLabels();
    void loadConfig();
    void setupModeButton();
    void updateButtonPosition();
    void updateModeButton();

private:
    int m_radius;
    QLabel* m_numberLabel;
    QLabel* m_nameLabel;
    QLabel* m_hintLabel;
    QTimer* m_animationTimer;
    int m_animationCount;
    int m_finalNumber;
    QString m_finalName;
    QVector<QPair<int, QString>> m_nameList;
    QPointer<QAbstractAnimation> m_nameAnimation;
    
    // 模式控制
    int m_mode;           // 0: 自动停止, 1: 手动停止
    bool m_isRolling;     // 是否正在滚动
    
    QPushButton* m_modeButton;  // 模式切换按钮
};