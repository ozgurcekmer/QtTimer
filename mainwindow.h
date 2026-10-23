#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void start();
    void pause();
    void reset();
    void updateDisplay();

private:
    QString formatTime(qint64 ms) const;

    QLabel* timeLabel;
    QPushButton* startBtn;
    QPushButton* pauseBtn;
    QPushButton* resetBtn;

    QTimer updateTimer;
    QElapsedTimer elapsed;
    qint64 accumulatedMs = 0;
    bool running = false;
};