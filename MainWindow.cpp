#include "MainWindow.hh"

#include <QKeyEvent>

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        close();
    }
}

MainWindow::MainWindow(int w, int h, QWidget *parent) : QMainWindow(parent) {
    // Set window sizing
    setMinimumHeight(h);
    setMinimumWidth(w);
    setMaximumHeight(h);
    setMaximumWidth(w);

    // Open the window
    show();
    // Set window focus
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}
