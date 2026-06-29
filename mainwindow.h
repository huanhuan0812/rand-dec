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

private:
    void applyRoundedMask();
    void startRandomAnimation();
    void updateAnimation();
    void generateResult();
    void loadNamesFromFile(const QString& filename);
    void showNameWithAnimation();
    void setupLabels();

private:
    int m_radius;
    QLabel* m_numberLabel;
    QLabel* m_nameLabel;
    QTimer* m_animationTimer;
    int m_animationCount;
    int m_finalNumber;
    QString m_finalName;
    QVector<QPair<int, QString>> m_nameList;
    QPointer<QAbstractAnimation> m_nameAnimation;
};