#pragma once

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

class QCheckBox;
class QLabel;
class QPushButton;
class QCloseEvent;
class QResizeEvent;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    friend class TimerTest;
    void start();
    void pause();
    void reset();
    void updateDisplay();
    void updateControls();
    void fitTimeFont();
    void setCompact(bool compact);
    qint64 currentMs() const;
    QString formatTime(qint64 ms) const;

    QLabel* timeLabel;
    QLabel* statusLabel;
    QPushButton* startBtn;
    QPushButton* pauseBtn;
    QPushButton* resetBtn;
    QCheckBox* onTop;
    QCheckBox* compactMode;
    QCheckBox* hundredths;
    QTimer updateTimer;
    QElapsedTimer elapsed;
    qint64 accumulatedMs = 0;
    bool running = false;
    QByteArray normalGeometry;
};
