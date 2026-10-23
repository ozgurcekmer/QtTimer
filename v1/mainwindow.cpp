#include "mainwindow.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QPalette>
#include <QResizeEvent>
#include <QSettings>
#include <QShortcut>
#include <QStyle>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setWindowTitle(tr("Work Timer"));
    setObjectName("timerWindow");
    auto interfaceFont = font();
    interfaceFont.setPointSize(14);
    setFont(interfaceFont);
    auto bluePalette = palette();
    bluePalette.setColor(QPalette::Window, QColor("#0f1d32"));
    bluePalette.setColor(QPalette::WindowText, QColor("#e2edff"));
    bluePalette.setColor(QPalette::Base, QColor("#152a45"));
    bluePalette.setColor(QPalette::Text, QColor("#e2edff"));
    bluePalette.setColor(QPalette::Button, QColor("#203f63"));
    bluePalette.setColor(QPalette::ButtonText, QColor("#e2edff"));
    bluePalette.setColor(QPalette::Highlight, QColor("#3078c6"));
    bluePalette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    setPalette(bluePalette);
    setStyleSheet(R"(
        QWidget#timerWindow, QMessageBox { background-color: #0f1d32; }
        QLabel, QCheckBox { color: #e2edff; }
        QLabel#appHeading { color: #c9e5ff; font-size: 22pt; font-weight: 600; }
        QLabel#timeDisplay { color: #a9d6ff; }
        QLabel#timerStatus { color: #b0c9e5; }
        QLabel#timerStatus[state="running"] { color: #80ceff; }
        QLabel#timerStatus[state="paused"] { color: #c4cfff; }
        QPushButton {
            background-color: #203f63;
            color: #e2edff;
            border: 1px solid #527eaa;
            border-radius: 6px;
            padding: 8px 14px;
            font-size: 15pt;
        }
        QPushButton:hover { background-color: #2b527d; }
        QPushButton:pressed { background-color: #16314f; }
        QPushButton#startButton { background-color: #245e9e; border-color: #73b6f5; }
        QPushButton#startButton:hover { background-color: #2d71b9; }
        QPushButton#startButton:pressed { background-color: #194978; }
        QPushButton:disabled, QPushButton#startButton:disabled {
            background-color: #172c45;
            color: #92a7bf;
            border-color: #354c67;
        }
        QPushButton:focus, QCheckBox:focus { outline: 2px solid #a9d6ff; }
        QToolTip { background-color: #203f63; color: #e2edff; border: 1px solid #73b6f5; }
    )");
    resize(640, 360);
    setMinimumSize(520, 320);

    auto* heading = new QLabel(tr("Work Timer"));
    heading->setObjectName("appHeading");
    heading->setAlignment(Qt::AlignCenter);

    statusLabel = new QLabel;
    statusLabel->setObjectName("timerStatus");
    statusLabel->setAlignment(Qt::AlignCenter);
    timeLabel = new QLabel;
    timeLabel->setObjectName("timeDisplay");
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    timeLabel->setMinimumSize(0, 64);
    timeLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    timeLabel->setAccessibleName(tr("Elapsed time"));

    startBtn = new QPushButton(tr("Start"));
    startBtn->setObjectName("startButton");
    pauseBtn = new QPushButton(tr("Pause"));
    resetBtn = new QPushButton(tr("Reset"));
    startBtn->setToolTip(tr("Start or resume (Space)"));
    pauseBtn->setToolTip(tr("Pause (Space)"));
    resetBtn->setToolTip(tr("Reset elapsed time (Ctrl+R)"));
    // Space belongs to the window shortcut, even when a button has focus.
    for (auto* button : {startBtn, pauseBtn, resetBtn})
        button->setFocusPolicy(Qt::ClickFocus);

    auto* buttons = new QHBoxLayout;
    buttons->addWidget(startBtn);
    buttons->addWidget(pauseBtn);
    buttons->addWidget(resetBtn);

    onTop = new QCheckBox(tr("Always on top"));
    compactMode = new QCheckBox(tr("Compact"));
    hundredths = new QCheckBox(tr("Hundredths"));
    onTop->setToolTip(tr("Keep the timer above other windows"));
    compactMode->setToolTip(tr("Toggle compact mode (Ctrl+M)"));
    hundredths->setToolTip(tr("Show hundredths of a second"));
    auto* options = new QHBoxLayout;
    options->addWidget(onTop);
    options->addWidget(compactMode);
    options->addWidget(hundredths);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(heading);
    layout->addWidget(statusLabel);
    layout->addWidget(timeLabel, 1);
    layout->addLayout(buttons);
    layout->addLayout(options);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::start);
    connect(pauseBtn, &QPushButton::clicked, this, &MainWindow::pause);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::reset);
    connect(&updateTimer, &QTimer::timeout, this, &MainWindow::updateDisplay);
    connect(onTop, &QCheckBox::toggled, this, [this](bool checked) {
        const auto geometry = saveGeometry();
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        restoreGeometry(geometry);
        show();
    });
    connect(compactMode, &QCheckBox::toggled, this, &MainWindow::setCompact);
    connect(hundredths, &QCheckBox::toggled, this, [this](bool checked) {
        updateTimer.setInterval(checked ? 30 : 100);
        updateDisplay();
    });
    auto* toggleShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(toggleShortcut, &QShortcut::activated, this, [this] {
        if (running) pause(); else start();
    });
    auto* resetShortcut = new QShortcut(QKeySequence(tr("Ctrl+R")), this);
    connect(resetShortcut, &QShortcut::activated, this, &MainWindow::reset);
    auto* compactShortcut = new QShortcut(QKeySequence(tr("Ctrl+M")), this);
    connect(compactShortcut, &QShortcut::activated, compactMode, &QCheckBox::toggle);

    QSettings settings;
    updateTimer.setInterval(100);
    hundredths->setChecked(settings.value("hundredths", false).toBool());
    restoreGeometry(settings.value("normalGeometry").toByteArray());
    normalGeometry = saveGeometry();
    compactMode->setChecked(settings.value("compact", false).toBool());
    restoreGeometry(settings.value("geometry").toByteArray());
    onTop->setChecked(settings.value("onTop", false).toBool());
    updateControls();
    updateDisplay();
}

qint64 MainWindow::currentMs() const
{
    return accumulatedMs + (running ? elapsed.elapsed() : 0);
}

void MainWindow::start()
{
    if (running) return;
    elapsed.start();
    running = true;
    updateTimer.start();
    updateControls();
    updateDisplay();
}

void MainWindow::pause()
{
    if (!running) return;
    accumulatedMs = currentMs();
    running = false;
    updateTimer.stop();
    updateDisplay();
    updateControls();
}

void MainWindow::reset()
{
    if (currentMs() >= 60000) {
        const auto answer = QMessageBox::question(this, tr("Reset timer?"),
            tr("Reset the elapsed time to zero? This cannot be undone."),
            QMessageBox::Reset | QMessageBox::Cancel, QMessageBox::Cancel);
        if (answer != QMessageBox::Reset) return;
    }
    running = false;
    accumulatedMs = 0;
    updateTimer.stop();
    updateDisplay();
    updateControls();
}

void MainWindow::updateControls()
{
    startBtn->setEnabled(!running);
    startBtn->setText(accumulatedMs > 0 ? tr("Resume") : tr("Start"));
    pauseBtn->setEnabled(running);
    resetBtn->setEnabled(running || accumulatedMs > 0);
    statusLabel->setText(running ? tr("● Running")
                                   : accumulatedMs > 0 ? tr("Ⅱ Paused") : tr("Ready"));
    statusLabel->setProperty("state", running ? "running"
                                              : accumulatedMs > 0 ? "paused" : "ready");
    statusLabel->style()->unpolish(statusLabel);
    statusLabel->style()->polish(statusLabel);
    auto font = statusLabel->font();
    font.setBold(running);
    statusLabel->setFont(font);
}

void MainWindow::updateDisplay()
{
    const QString text = formatTime(currentMs());
    if (timeLabel->text() != text) {
        timeLabel->setText(text);
        fitTimeFont();
    }
}

QString MainWindow::formatTime(qint64 ms) const
{
    const qint64 seconds = ms / 1000;
    QString text = QString("%1:%2:%3")
        .arg(seconds / 3600, 2, 10, QChar('0'))
        .arg((seconds / 60) % 60, 2, 10, QChar('0'))
        .arg(seconds % 60, 2, 10, QChar('0'));
    if (hundredths->isChecked())
        text += QString(".%1").arg((ms % 1000) / 10, 2, 10, QChar('0'));
    return text;
}

void MainWindow::fitTimeFont()
{
    auto font = timeLabel->font();
    // Measure the widest digits so the font does not pulse as time changes.
    QString sample = timeLabel->text();
    for (auto& character : sample)
        if (character.isDigit()) character = QChar('8');
    const int width = qMax(1, timeLabel->contentsRect().width() - 12);
    const int height = qMax(1, timeLabel->contentsRect().height() - 8);
    int low = 1;
    int high = qMin(height, 160);
    while (low < high) {
        const int size = (low + high + 1) / 2;
        font.setPixelSize(size);
        const QFontMetrics metrics(font);
        if (metrics.horizontalAdvance(sample) <= width && metrics.height() <= height)
            low = size;
        else
            high = size - 1;
    }
    font.setPixelSize(low);
    timeLabel->setFont(font);
}

void MainWindow::setCompact(bool compact)
{
    if (compact) {
        normalGeometry = saveGeometry();
        resize(520, 320);
    } else {
        restoreGeometry(normalGeometry);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    fitTimeFont();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("normalGeometry", compactMode->isChecked() ? normalGeometry : saveGeometry());
    settings.setValue("compact", compactMode->isChecked());
    settings.setValue("onTop", onTop->isChecked());
    settings.setValue("hundredths", hundredths->isChecked());
    QWidget::closeEvent(event);
}
