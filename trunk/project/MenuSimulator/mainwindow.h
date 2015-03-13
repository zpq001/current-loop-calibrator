#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static void updateDisplayWrapper(char **data);
private slots:
    void onKeyDriverEvent(int id, int keyEventType);


    void on_pb_Update_clicked();

private:
    Ui::MainWindow *ui;

    uint32_t encodeGuiKey(int id);


};

#endif // MAINWINDOW_H
