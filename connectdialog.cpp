#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "scenemenu.h"
ConnectDialog::ConnectDialog(SceneMenu* game,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog),game(game)
{
    this->setAttribute( Qt::WA_DeleteOnClose );
    ui->setupUi(this);
}


ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::accept()
{
    game->connectToServer(ui->boxIp->text(),ui->boxPort->value());
    QDialog::accept();
}
