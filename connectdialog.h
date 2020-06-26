#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
class SceneMenu;
namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(SceneMenu* game,QWidget *parent = 0);
    ~ConnectDialog();
public slots:
    void accept();
private:
    Ui::ConnectDialog *ui;
    SceneMenu* game;
};

#endif // CONNECTDIALOG_H
