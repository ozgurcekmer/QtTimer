#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Work Timer");
    resize(300, 120);

    timeLabel = new QLabel("00:00:00.00");
    QFont font("monospace");
    font.setPointSize(20);
    timeLabel->setFont(font);
    timeLabel->setAlignment(Qt::AlignCenter);

    startBtn = new QPushButton("Start");
    pauseBtn = new QPushButton("Pause");
    resetBtn = new QPushButton("Reset");

    auto btnLayout = new QHBoxLayout;
    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(pauseBtn);
    btnLayout->addWidget(resetBtn);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(timeLabel);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::start);
    connect(pauseBtn, &QPushButton::clicked, this, &MainWindow::pause);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::reset);

    updateTimer.setInterval(10);
    connect(&updateTimer, &QTimer::timeout, this, &MainWindow::updateDisplay);
}

void MainWindow::start()
{
    if (running) return;
    running = true;
    elapsed.restart();
    updateTimer.start();
}

void MainWindow::pause()
{
    if (!running) return;
    accumulatedMs += elapsed.elapsed();
    running = false;
    updateTimer.stop();
}

void MainWindow::reset()
{
    running = false;
    accumulatedMs = 0;
    updateTimer.stop();
    timeLabel->setText("00:00:00.00");
}

void MainWindow::updateDisplay()
{
    qint64 ms = accumulatedMs + elapsed.elapsed();
    timeLabel->setText(formatTime(ms));
}

QString MainWindow::formatTime(qint64 ms) const
{
    qint64 totalSeconds = ms / 1000;
    qint64 hours = totalSeconds / 3600;
    qint64 minutes = (totalSeconds % 3600) / 60;
    qint64 seconds = totalSeconds % 60;
    qint64 centisec = (ms % 1000) / 10;

    return QString("%1:%2:%3.%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(centisec, 2, 10, QChar('0'));
}