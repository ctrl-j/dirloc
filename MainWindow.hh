#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override;
public:
    MainWindow(int w, int h, QWidget *parent = nullptr);
};

#endif // MAINWINDOW_HH
