#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>

namespace Ui {
class ServerDialog;
}
class SceneMenu;
class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(SceneMenu* game,QWidget *parent = 0);
    ~ServerDialog();
public slots:
    void accept();
private:
    Ui::ServerDialog *ui;
    SceneMenu* game;
};

#endif // SERVERDIALOG_H
