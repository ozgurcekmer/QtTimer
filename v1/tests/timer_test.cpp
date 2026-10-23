#include "mainwindow.h"
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

class TimerTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        QCoreApplication::setOrganizationName("QtTimerTests");
        QCoreApplication::setApplicationName("QtTimerTests");
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    }

    void pauseResumeAndReset()
    {
        MainWindow window;
        QVERIFY(!window.pauseBtn->isEnabled());
        QVERIFY(!window.resetBtn->isEnabled());
        QCOMPARE(window.timeLabel->text(), QString("00:00:00"));
        window.hundredths->setChecked(true);
        window.start();
        QVERIFY(!window.startBtn->isEnabled());
        QTest::qWait(45);
        window.pause();
        const qint64 paused = window.currentMs();
        QVERIFY(paused > 0);
        QCOMPARE(window.timeLabel->text(), window.formatTime(paused));
        QTest::qWait(30);
        window.updateDisplay();
        QCOMPARE(window.currentMs(), paused);
        QCOMPARE(window.startBtn->text(), QString("Resume"));
        window.start();
        QTest::qWait(30);
        window.pause();
        QVERIFY(window.currentMs() > paused);
        window.reset();
        QCOMPARE(window.currentMs(), qint64(0));
        QCOMPARE(window.timeLabel->text(), QString("00:00:00.00"));
    }

    void formatting()
    {
        MainWindow window;
        QCOMPARE(window.formatTime(360000000), QString("100:00:00"));
        window.hundredths->setChecked(true);
        QCOMPARE(window.formatTime(3661234), QString("01:01:01.23"));
    }

    void protectedReset()
    {
        MainWindow window;
        window.accumulatedMs = 60000;
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            if (dialog) dialog->button(QMessageBox::Cancel)->click();
        });
        window.reset();
        QCOMPARE(window.currentMs(), qint64(60000));
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            if (dialog) dialog->button(QMessageBox::Reset)->click();
        });
        window.reset();
        QCOMPARE(window.currentMs(), qint64(0));
    }

    void preferencesAndShortcuts()
    {
        {
            MainWindow window;
            window.show();
            QTest::qWait(10);
            QTest::keyClick(&window, Qt::Key_Space);
            QVERIFY(window.running);
            QTest::keyClick(&window, Qt::Key_Space);
            QVERIFY(!window.running);
            window.compactMode->setChecked(true);
            window.hundredths->setChecked(true);
            window.onTop->setChecked(true);
            window.close();
        }
        MainWindow restored;
        QVERIFY(restored.compactMode->isChecked());
        QVERIFY(restored.hundredths->isChecked());
        QVERIFY(restored.onTop->isChecked());
        QVERIFY(restored.windowFlags().testFlag(Qt::WindowStaysOnTopHint));
    }

private:
    QTemporaryDir settingsDir;
};

QTEST_MAIN(TimerTest)
#include "timer_test.moc"
