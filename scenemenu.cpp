#include "scenemenu.h"
#include "connectdialog.h"
#include "serverdialog.h"
#include "draughtclient.h"
#include "DraughtManager.h"
#include "drawdialog.h"
#include "QMessageBox"
#include "queue"
#include "animation.h"
#include "moveanimation.h"
#include "dieanimation.h"
#include "QMediaPlayer"
QVector2D tween(QVector2D s,QVector2D t,float progress){
    return progress*t+(1.f-progress)*s;
}
struct Selection{
    int target;
    int kill;
};
struct SceneMenu::Internal{
    float angle=0;
    QDialog* current_dialog=nullptr;

    int state=0; //0 for menu and 1 for ingame.
    //menu section.
    float client_scale=1.f; //also for resign
    float server_scale=1.f; //also for draw

    //ingame section.
    float chessboard_delta=-1.f;

    int flying_chess=-1;
    int chess_state[40]; //0 for alive/flying, 1 for exploding and 2 for death. //0-19 for golden and 20-49 for silver.
    bool is_king[40];


    int chessboard[10][10];
    int current_turn; //0 for gold
    int my_turn; //0 for gold
    long long tick=0;
    int delta=0;
    int yaw=0;
    QOpenGLShaderProgram* current_shader;
    DraughtClient* current=nullptr;
    PacketDecoder* pm;
    bool window_open=false;
    bool game_started=false;
    int chosen_chess=0;




    std::vector<Selection> steps[40];
    int selection_stage=0; //0 for first step and 1 for jump chain.


    int flying_target=-1; //idx
    int flying_animation_tick=0;
    int exploding_animation_tick=0;
    int rotate_delta=0;
    bool now_turn=true;
    QMatrix4x4 lightpos;
    std::queue<Animation*> anim_queue;
    QMediaPlayer* player;
    void playSound(QString name){
        player->setMedia(QUrl(QString("qrc:/assets/sounds/%1").arg(name)));
        player->play();
    }

};
//float delta=0.f;
SceneMenu::SceneMenu(GLManager *gl, Widget *parent):Scene(gl,parent)
{
    data=new Internal;
    data->player=new QMediaPlayer(this);
    gl->LoadModel("chess");
    gl->bakeModel("chess");
    gl->LoadModel("chessboard");
    gl->bakeModel("chessboard");
    gl->LoadModel("font_server");
    gl->bakeModel("font_server");
    gl->LoadModel("font_client");
    gl->bakeModel("font_client");
    gl->LoadModel("font_draw");
    gl->bakeModel("font_draw");
    gl->LoadModel("font_resign");
    gl->bakeModel("font_resign");
    gl->LoadModel("single_grid");
    gl->bakeModel("single_grid");
    gl->LoadTexture("grid_fill");
    gl->LoadSound("chess.wav");
    gl->LoadSound("move.wav");
    initBoard();
}
void SceneMenu::handleSituationRotation(){
    bool roll=context->input()->isRolling();
    if(context->input()->isKeyDown(SimpleInputManager::DOWN)|| (roll && context->input()->getRollDelta()<0)){
        if(data->delta<30) data->delta++;
        if(data->delta<30) if(roll) data->delta++;


    }
    if(context->input()->isKeyDown(SimpleInputManager::UP) || (roll && context->input()->getRollDelta()>0)){
        if(data->delta>-30) data->delta--;
        if(data->delta>-30) if(roll) data->delta--;
    }
    if(context->input()->isKeyDown(SimpleInputManager::LEFT)){
        data->yaw--;
        if(data->yaw<0) data->yaw=360+data->yaw;

    }
    if(context->input()->isKeyDown(SimpleInputManager::RIGHT)){
        data->yaw++;
        if(data->yaw>=360) data->yaw=data->yaw-360;

    }
    if(data->rotate_delta>4){
        data->yaw+=4;
        data->rotate_delta-=4;

    }else
    if(data->rotate_delta<-4){
        data->yaw-=4;
        data->rotate_delta+=4;

    }else
    if(data->rotate_delta!=0){
        data->yaw+=data->rotate_delta;
        data->rotate_delta=0;

    }

}
void SceneMenu::handleMenuAction(){
    if(data->state==0){
        QVector4D mouse_trace_meta=context->input()->getMeta();
        //qDebug()<<mouse_trace_meta;
        float delta=0.02f;
        float maxs=1.2f;
        if(round(mouse_trace_meta.x()*100)==1){
            if(round(mouse_trace_meta.y()*100)==1){
                data->server_scale+=delta;
                if(data->server_scale>maxs) data->server_scale=maxs;

            }else{
                data->server_scale-=delta;
                if(data->server_scale<1.0) data->server_scale=1.0;

            }

            if(round(mouse_trace_meta.y()*100)==2){
                data->client_scale+=delta;
                if(data->client_scale>maxs) data->client_scale=maxs;

            }else{
                data->client_scale-=delta;
                if(data->client_scale<1.0) data->client_scale=1.0;

            }
        }else{
            data->server_scale-=delta;
            if(data->server_scale<1.0) data->server_scale=1.0;
            data->client_scale-=delta;
            if(data->client_scale<1.0) data->client_scale=1.0;

        }
        if(context->input()->checkTap() && !data->window_open){
            QVector4D mouse_trace_meta=context->input()->getTapMeta();
            if(round(mouse_trace_meta.x()*100)==1){
                if(round(mouse_trace_meta.y()*100)==1){
                    //data->client_explode_tick=120;
                    ServerDialog* dlg=new ServerDialog(this,context);
                    data->current_dialog=dlg;
                    dlg->show();
                    data->window_open=true;
                    QObject::connect(dlg,&ServerDialog::rejected,this,&SceneMenu::onDialogClose);

                }
                if(round(mouse_trace_meta.y()*100)==2){
                    //data->server_explode_tick=120;
                    ConnectDialog* dlg=new ConnectDialog(this,context);
                    data->current_dialog=dlg;
                    dlg->show();
                    data->window_open=true;
                    QObject::connect(dlg,&ServerDialog::rejected,this,&SceneMenu::onDialogClose);
                }

            }

        }
    }

}
void SceneMenu::handleIngameMenuAction(){
    if(data->state==1 && data->game_started){
        QVector4D mouse_trace_meta=context->input()->getMeta();
        //qDebug()<<mouse_trace_meta;
        float delta=0.02f;
        float maxs=1.2f;
        if(round(mouse_trace_meta.x()*100)==1){
            if(round(mouse_trace_meta.y()*100)==3){
                data->server_scale+=delta;
                if(data->server_scale>maxs) data->server_scale=maxs;

            }else{
                data->server_scale-=delta;
                if(data->server_scale<1.0) data->server_scale=1.0;

            }

            if(round(mouse_trace_meta.y()*100)==4){
                data->client_scale+=delta;
                if(data->client_scale>maxs) data->client_scale=maxs;

            }else{
                data->client_scale-=delta;
                if(data->client_scale<1.0) data->client_scale=1.0;

            }
        }else{
            data->server_scale-=delta;
            if(data->server_scale<1.0) data->server_scale=1.0;
            data->client_scale-=delta;
            if(data->client_scale<1.0) data->client_scale=1.0;

        }
        if(context->input()->checkTap()){
            QVector4D mouse_trace_meta=context->input()->getTapMeta();
            if(round(mouse_trace_meta.x()*100)==1){
                if(round(mouse_trace_meta.y()*100)==3){
                    holdDraw();

                }
                if(round(mouse_trace_meta.y()*100)==4){
                    holdResign();
                }

            }

        }
    }

}

void SceneMenu::holdDraw()
{
    if(data->current_turn!=data->my_turn) return;
    data->current->sendPacket("DrawReq","0");
}

void SceneMenu::holdResign()
{
    if(data->current_turn!=data->my_turn) return;
    data->current->sendPacket("Resign","0");
}

void SceneMenu::checkSteppable(int* array)
{
    if(!data->game_started) return;
    if(data->chosen_chess==-1) return;
    auto& vec=data->steps[data->chosen_chess];
    for(Selection sel:vec){
        array[sel.target]=1;

    }
}

void SceneMenu::drawGreenGrid(QMatrix4x4 model, QVector4D meta)
{
    Material material;
    material.specular=0.f;
    material.emissive=0.3f;
    data->current_shader->setUniformValue("explode_distance",0);
    data->current_shader->setUniformValue("tone",QVector3D(1.f,1.f,1.f));
    context->renderModel(gl->getBakedModel("single_grid"),model,meta,"grid_fill",data->current_shader,material);
}
void SceneMenu::handleSituationStep()
{
    if(data->state==1 && data->current_turn==data->my_turn){
        if(context->input()->checkTap()){
            bool dif=true;
            if(data->selection_stage==0){

                auto meta=context->input()->getTapMeta();
                if(round(meta.x()*10)==6){
                    data->chosen_chess=round(meta.y()*100);
                    dif=false;

                }


            }

            auto meta=context->input()->getTapMeta();
            if(round(meta.x()*10)==7){
                int row=round(meta.y()*100);
                int column=round(meta.z()*100);
                qDebug()<<row<<column;
                data->current->sendPacket("ChooseStep",QString("%1,%2").arg(data->chosen_chess).arg(row*10+column));
                for(int i=0;i<40;i++){
                    data->steps[i].clear();

                }
                //data->current_turn=1-data->my_turn;
            }
            if(dif) data->chosen_chess=-1;

        }

    }else{
        data->selection_stage=0;
        data->chosen_chess=-1;

    }
}
void SceneMenu::act()
{
    data->tick++;
    if(!data->anim_queue.empty()){
        Animation* anim=data->anim_queue.front();
        bool result=anim->tick();
        if(result){
            if(anim->type==2){
                DieAnimation* danim=(DieAnimation*)anim;
                for(int i=0;i<40;i++){
                    if(danim->dying_mask[i]) data->chess_state[i]=2;

                }

            }
            if(anim->type==1){
                MoveAnimation* manim=(MoveAnimation*)anim;
                if(manim->leap) gl->getSound("chess.wav")->play();

            }
            delete anim;
            data->anim_queue.pop();
        }


    }
    handleSituationRotation();
    handleMenuAction();
    handleSituationStep();
    handleIngameMenuAction();
    if(data->state==1){
        data->chessboard_delta+=0.05;
        if(data->chessboard_delta>0) data->chessboard_delta=0;

    }



    context->input()->clearTap();
}

void SceneMenu::render()
{
    gl->initStack();
    gl->pushProjMatrix();
    gl->getProjMatrix().perspective(60.f,4.f/3,1.f,100.f);


    //qDebug()<<gl->getProjMatrix();

    //gl->getProjMatrix().ortho(-1.f,1.f,-2.f,2.f,-8.f,8.f);
    gl->pushModelMatrix();
    //gl->getMVMatrix().translate(0.f,0.f,0.f);
    gl->getModelMatrix().scale(0.5f);

    gl->pushViewMatrix();
    QVector3D cameraPos={-5.f,5.f,0.f};
    QVector3D lookAtPos={0.f,0.f,0.f};
    QVector3D cameraFront=QVector3D(cameraPos-lookAtPos);
    cameraFront.normalize();
    QVector3D up={1.f,0.f,0.f};
    QVector3D cameraRight=QVector3D::crossProduct(up,cameraFront);
    cameraRight.normalize();
    QVector3D cameraUp=QVector3D::crossProduct(cameraFront,cameraRight);
    gl->getViewMatrix().lookAt(cameraPos,lookAtPos,cameraUp);


    ModelData chess=gl->getBakedModel("chess");
    //gl->getModelMatrix().rotate(angle,1.f,0.f,0.f);

    //mat.translate(-2.5f,0.f,2.f);
    //mat.rotate(60,1.f,0.f,0.f);
    //mat.rotate(90,0.f,1.f,0.f);
    //mat.translate(0,0,-2.f);
    //mat.rotate(45,0.f,0.f,1.f);

    //if(angle>180) angle=-180+(angle-180);
    context->glClearColor(0.f, 0.f, 0.f, 1.0f);
    context->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //qDebug("hi");

    QMatrix4x4 mat;
    //mat.rotate(90,0.f,1.f,0.f);
    mat.translate(-2*data->chessboard_delta,0,2*data->chessboard_delta);
    mat.scale(0.5);

    QMatrix4x4 lightpos;
    lightpos.translate(0,6.f,0.f);
    QOpenGLShaderProgram* shader=gl->getShader("default");
    shader->bind();
    data->current_shader=shader;
    shader->setUniformValue("matrix",gl->getMatrix());
    shader->setUniformValue("brightness",1.f);

    shader->setUniformValue("posEye",cameraPos);

    data->current_shader->setUniformValue("explode_distance",0);
    QMatrix4x4 situation_rotate;

    situation_rotate.rotate(data->delta,QVector3D(0.f,0.f,1.f));
    situation_rotate.rotate(data->yaw,QVector3D(0.f,1.f,0.f));
    shader->setUniformValue("posLight",mat*situation_rotate*lightpos*QVector4D(0.f,0.f,0.f,1.f));
    drawSituation(mat*situation_rotate);
    data->lightpos=lightpos;
    if(data->state==0){

        renderMenu();


    }else if(data->state==1){
        if(data->game_started){
            renderIngameMenu();

        }

    }


    /*
    data->chess.vertex_buffer->bind();
    shader->setAttributeBuffer("vertAttr",GL_FLOAT,offsetof(Vertex,vertAttr),3,sizeof(Vertex));
    shader->setAttributeBuffer("vertNormal",GL_FLOAT,offsetof(Vertex,vertNormal),3,sizeof(Vertex));
    shader->setAttributeBuffer("texCoord",GL_FLOAT,offsetof(Vertex,texCoord),2,sizeof(Vertex));
    data->chess.vertex_buffer->release();
    shader->enableAttributeArray("vertAttr");
    shader->enableAttributeArray("vertNormal");
    shader->enableAttributeArray("texCoord");
    gl->getTexture("gold_chess")->bind();
    //qDebug()<<data->chess.index_buffer->bind()<<data->chess.index_buffer->type();
    //qDebug()<<data->chess.idx_count;
    //data->chess.vertex_buffer->write(0,vertices,sizeof(Vertex)*3);
    //this->glDrawArrays(GL_TRIANGLES,0,data->chess.idx_count);
    //int indices[]={0,1,2};
    data->chess.index_buffer->bind();
    this->glDrawElements(GL_TRIANGLES,data->chess.idx_count,GL_UNSIGNED_INT,0);
    //qDebug("Error?");
    //qDebug()<<this->glGetError();
    //data->chess.vertex_buffer->release();
    data->chess.index_buffer->release();
    */

    //shader->setAttributeArray("vertAttr",GL_FLOAT,(char*)vertices+offsetof(Vertex,vertAttr),3,sizeof(Vertex));
   // shader->setAttributeArray("vertNormal",GL_FLOAT,(char*)vertices+offsetof(Vertex,vertNormal),3,sizeof(Vertex));
    //shader->setAttributeArray("texCoord",GL_FLOAT,(char*)vertices+offsetof(Vertex,texCoord),3,sizeof(Vertex));
    //this->glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,indices);
    //qDebug()<<this->glGetError();
    //this->glDrawArrays(GL_TRIANGLES,0,3);
    //qDebug()<<this->glGetError();

    shader->release();
}

void SceneMenu::paint()
{

}

void SceneMenu::connectToServer(QString ip, int port)
{
    qDebug()<<QString("Connecting to %1:%2 ...").arg(ip).arg(port);
    data->current=new DraughtClient();
    connect(data->current,&DraughtClient::ready,this,&SceneMenu::connected);
    connect(data->current,&DraughtClient::packet,this,&SceneMenu::onPacket);
    connect(data->current,&DraughtClient::disconnected,this,&SceneMenu::disconnected);
    data->current->connectHost(ip,port);


}

void SceneMenu::onDialogClose()
{
    data->window_open=false;
    //delete data->current_dialog;
}

void SceneMenu::connected()
{
    data->current->sendPacket("Fuckyou","The lether club is two blocks down.");
    data->state=1;
    data->client_scale=1.f;
    data->server_scale=1.f;
}

void SceneMenu::onPacket(QString head, QString body)
{
    qDebug("Client Packet!");
    if(head=="SetSide"){
        data->rotate_delta=(body=="0"?0:180)-data->yaw;
        data->my_turn=body.toInt();
        data->game_started=true;
    }
    if(head=="Init"){
        QStringList list=body.split(",");
        for(int i=0;i<40;i++){
            data->chess_state[i]=2;
            data->steps[i].clear();
            data->is_king[i]=(bool)(list[100+i].toInt());

        }
        for(int i=0;i<100;i++){
            int id=list[i].toInt();
            qDebug()<<id;
            data->chessboard[i/10][i%10]=id;
            if(id>=0) data->chess_state[id]=0;

        }


    }
    if(head=="ReqStep"){
        QStringList list=body.split(",");
        std::vector<int> arg_list;
        for(QString string:list) arg_list.push_back(string.toInt());
        auto iter=arg_list.begin();

        for(int i=0;i<40;i++){
            data->steps[i].clear();
            int count=*(iter++);
            for(int j=0;j<count;j++){
                Selection sel;
                sel.target=*(iter++);
                sel.kill=*(iter++);
                data->steps[i].push_back(sel);

            }

        }
        data->current_turn=data->my_turn;
    }
    if(head=="DrawReq"){
        DrawDialog* dialog=new DrawDialog(this->context);
        int result=dialog->exec();
        if(result==QDialog::Accepted){
            data->current->sendPacket("DrawAns","1");

        }else{
            data->current->sendPacket("DrawAns","0");

        }

    }
    if(head=="Draw"){
        data->game_started=false;
        QMessageBox::information(context,"Draw!","Game draw!",QMessageBox::Ok);

    }
    if(head=="DrawRejected"){
        QMessageBox::information(context,"Draw!","Get dunked on!",QMessageBox::Ok);
    }
    if(head=="Resign"){ //UNUSED
        data->game_started=false;
        QMessageBox::information(context,"You won!","Your opponent resigned!",QMessageBox::Ok);

    }
    if(head=="AnimateStep"){
        QStringList list=body.split(",");
        int chess_idx=list[0].toInt();
        int target=list[1].toInt();
        int chess_killed=list[2].toInt();
        int src;
        for(int i=0;i<10;i++){
            for(int j=0;j<10;j++){
                if(data->chessboard[i][j]==chess_idx){
                    src=i*10+j;
                    data->chessboard[i][j]=-1;
                    break;
                }
            }

        }
        data->chessboard[target/10][target%10]=chess_idx;

        MoveAnimation* anim=new MoveAnimation;
        anim->chess=chess_idx;
        anim->src=src;
        anim->target=target;
        if(chess_killed!=-1){
            anim->leap=true;

        }else{
            gl->getSound("move.wav")->play();

        }
        anim->percentage=0.f;
        data->anim_queue.push(anim);
        if(chess_killed>=0)
        data->chess_state[data->chessboard[chess_killed/10][chess_killed%10]]=1;

    }
    if(head=="AnimateStepFinish"){
        data->now_turn=!data->now_turn;
        DieAnimation* anim=new DieAnimation;
        bool empty=true;
        anim->percentage=0.f;
        for(int i=0;i<40;i++){
            if(data->chess_state[i]==1){
                data->chess_state[i]=1;
                anim->dying_mask[i]=true;
                empty=true;
            }

        }
        if(empty) anim->percentage=0.8f;
        data->anim_queue.push(anim);
        for(int i=0;i<10;i++){
            if(data->chessboard[0][i]>=20 && data->chessboard[0][i]<40) data->is_king[data->chessboard[0][i]]=true;
            if(data->chessboard[9][i]>=0 && data->chessboard[9][i]<20) data->is_king[data->chessboard[9][i]]=true;
        }

    }
    if(head=="GameFinish"){
        data->game_started=false;
        int winner=body.toInt();
        QMessageBox* msgbox=new QMessageBox(QMessageBox::Information,"Game finished",(data->my_turn==winner?"You win!":"You lose!"),QMessageBox::Ok,context);
        msgbox->setModal(false);
        msgbox->show();


    }
}

void SceneMenu::disconnected()
{
    if(data->game_started){
        QMessageBox::information(context,"Disconnected","For some reason you disconnected!",QMessageBox::Ok);
        data->game_started=false;

    }

}
QVector2D calcTranspose(float row,float column){
    const float delta=1.1; //A good number...or bad?
    return QVector2D(-9*delta+column*2*delta,-9*delta+row*2*delta);

}
void SceneMenu::drawSituation(QMatrix4x4 model)
{

    drawChessboard(model);
    for(int i=0;i<10;i++){

    }
    /*
    //ingame section.
    float draw_scale=0.5f;
    float resign_scale=0.5f;
    int flying_animation_tick=0;
    int flying_chess=-1;
    int chess_state[40]; //0 for alive/flying, 1 for exploding and 2 for death. //0-19 for golden and 20-49 for silver.
    bool is_king[40];
    int exploding_animation_tick=0;
    int flying_target; //row*10+column
    int chessboard[10][10];
    int current_turn; //0 for gold
    int my_turn; //0 for gold
    */
    bool empty=data->anim_queue.empty();
    int anim_type=0;
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            QMatrix4x4 current=model;
            QVector2D vec=calcTranspose(i,j);
            float h=-0.2f;
            if(!empty){
                Animation* canim=data->anim_queue.front();
                //qDebug()<<"Anim"<<canim->type;
                if(canim->type==1){
                    MoveAnimation* moveanim=static_cast<MoveAnimation*>(canim);
                    //qDebug()<<"MoveAnim"<<moveanim->chess;
                    if(data->chessboard[i][j]==moveanim->chess){
                        anim_type=1;
                        QVector2D src_grid=calcTranspose(moveanim->src/10,moveanim->src%10);
                        QVector2D target_grid=calcTranspose(moveanim->target/10,moveanim->target%10);
                        vec=tween(src_grid,target_grid,moveanim->percentage);
                        if(moveanim->leap){
                            float f=1.f-abs(moveanim->percentage-0.5f)*2;
                            qDebug()<<f;
                            h+=1.f*f;

                        }
                    }

                }
                if(canim->type==2){

                    DieAnimation* dieanim=static_cast<DieAnimation*>(canim);
                    //qDebug("DieAnim");
                    if(data->chessboard[i][j]>=0){
                        if(dieanim->dying_mask[data->chessboard[i][j]]){
                            anim_type=2;
                            h-=0.4f*dieanim->percentage;
                        }

                    }

                }

            }

            current.translate(vec.y(),h,vec.x());
            int idx=data->chessboard[i][j];
            if(idx<20 && idx>=0) current.rotate(180,0,1,0);


            current.scale(0.5);
            if(empty){
                int mask[100];
                memset(mask,0,sizeof(mask));
                this->checkSteppable(mask);
                if(mask[i*10+j]){
                    drawGreenGrid(current,QVector3D(0.7f,(float)i/100,(float)j/100));

                }

            }

            if(data->chessboard[i][j]!=-1){

                bool isking=data->is_king[idx];
                int chess_state=data->chess_state[idx];
                if(chess_state!=2){



                    //current.translate(0,-1.f,0);

                    QVector3D tone={1.f,1.f,1.f};
                    if(data->chosen_chess==idx){
                        auto shine=QVector3D(0.4f,1.f,0.4f);

                        float perc=((float)(data->tick%60)/60);
                        if(perc>0.5f) perc=1-perc;
                        perc=perc*2;
                        tone=perc*shine+(1-perc)*tone;

                    }else
                    if(chess_state==1){
                        auto shine=QVector3D(1.f,0.4f,0.4f);

                        float perc=((float)(data->tick%60)/60);
                        if(perc>0.5f) perc=1-perc;
                        perc=perc*2;
                        tone=perc*shine+(1-perc)*tone;

                    }
                    drawChess(current,(idx<20?0:1),isking,QVector4D(0.6f,(float)idx/100,0,0),0,tone);

                }


            }


        }

    }
}

void SceneMenu::drawChessboard(QMatrix4x4 model)
{
    //QMatrix4x4 current=model;

    Material material;
    material.specular=0.f;
    material.emissive=0.f;
    data->current_shader->setUniformValue("explode_distance",0);
    data->current_shader->setUniformValue("tone",QVector3D(1.f,1.f,1.f));
    context->renderModel(gl->getBakedModel("chessboard"),model,QVector4D(0.f,0.f,0.f,1.f),"chessboard",data->current_shader,material);
}

void SceneMenu::drawChess(QMatrix4x4 model, int side, bool isking,QVector4D meta,float explosion_distance,QVector3D tone,float delta_em)
{
    Material material;
    material.specular=5.f;
    material.emissive=0.3f+delta_em;
    data->current_shader->setUniformValue("explode_distance",explosion_distance);
    data->current_shader->setUniformValue("tone",tone);
    context->renderModel(gl->getBakedModel("chess"),model,meta,QString("%1_chess%2").arg(side==0?"gold":"silver").arg(isking?"_king":""),data->current_shader,material);
}

void SceneMenu::initBoard()
{


    memset(data->chess_state,0,sizeof(data->chess_state));
    memset(data->is_king,0,sizeof(data->is_king));
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            data->chessboard[i][j]=-1;

        }

    }
    data->current_turn=0;
    data->my_turn=0;
    data->flying_target=-1;
    data->flying_chess=-1;

    for(int i=0;i<20;i++){

        int gold_id=i;
        int gold_column=i%5;
        gold_column=2*gold_column;
        int gold_row=i/5;
        if(gold_row&1){
            gold_column++;

        }
        //gold_row=2*gold_row;

        data->chessboard[gold_row][gold_column]=gold_id;
        data->chessboard[9-gold_row][9-gold_column]=20+gold_id;

    }
}

void SceneMenu::renderMenu()
{
    static int angle=0;
    QMatrix4x4 mat;
    mat.rotate(45,0.f,0.f,1.f);


    QMatrix4x4 matr;
    matr.translate(2.5f,0.f,5.f);


    QMatrix4x4 matchess;
    matr.scale(0.5f);
    matchess.rotate(angle++,0.f,1.f,0.f);
    QMatrix4x4 temp=matchess;
    matchess.scale(data->server_scale);
    QMatrix4x4 matfont;
    matfont.rotate(-90,0.f,1.f,0.f);
    matfont.translate(3.f,0.f,1.f);
    matfont.scale(3.f);
    //this->drawChess(mat*lightpos,0,true,QVector4D(1,10,0,0));
    data->current_shader->setUniformValue("posLight",mat*matr*data->lightpos*QVector4D(0.f,0.f,0.f,1.f));
    this->drawChess(mat*matr*matchess,0,true,QVector4D(0.01,0.01,0,0),0,QVector3D(1.f,1.f,1.f),0.3f);
    matchess=temp;
    Material font_mat;
    font_mat.emissive=0.3f;
    font_mat.specular=0.f;
    context->renderModel(gl->getBakedModel("font_server"),mat*matr*matfont,QVector4D(),"font_fill",data->current_shader,font_mat);
    matr=QMatrix4x4();
    matr.translate(0.f,0.f,5.7f);
    //matr.rotate(angle++,0.f,1.f,0.f);
    matr.scale(0.5f);
    matchess.scale(data->client_scale);
    data->current_shader->setUniformValue("posLight",mat*matr*data->lightpos*QVector4D(0.f,0.f,0.f,1.f));
    this->drawChess(mat*matr*matchess,1,true,QVector4D(0.01,0.02,0,0),0,QVector3D(1.f,1.f,1.f),0.3f);
    context->renderModel(gl->getBakedModel("font_client"),mat*matr*matfont,QVector4D(),"font_fill",data->current_shader,font_mat);
}
void SceneMenu::renderIngameMenu()
{
    static int angle=0;
    QMatrix4x4 mat;
    mat.rotate(45,0.f,0.f,1.f);


    QMatrix4x4 matr;
    matr.translate(6.f,0.f,-8.f);


    QMatrix4x4 matchess;
    matr.scale(0.5f);

    QMatrix4x4 temp=matchess;
    matchess.rotate(angle++,0.f,1.f,0.f);
    matchess.scale(data->server_scale);
    QMatrix4x4 matfont;
    matfont.rotate(-90,0.f,1.f,0.f);
    matfont.translate(3.f,0.f,1.f);
    matfont.scale(3.f);
    //this->drawChess(mat*lightpos,0,true,QVector4D(1,10,0,0));
    data->current_shader->setUniformValue("posLight",mat*matr*data->lightpos*QVector4D(0.f,0.f,0.f,1.f));
    this->drawChess(mat*matr*matchess,0,true,QVector4D(0.01,0.03,0,0),0,QVector3D(1.f,1.f,1.f),0.3f);
    matchess=temp;
    matchess.rotate(angle++,0.f,1.f,0.f);
    Material font_mat;
    font_mat.emissive=0.3f;
    font_mat.specular=0.f;
    context->renderModel(gl->getBakedModel("font_draw"),mat*matr*matfont,QVector4D(),"font_fill",data->current_shader,font_mat);
    matr=QMatrix4x4();
    matr.translate(6.f,0.f,5.f);
    //matr.rotate(angle++,0.f,1.f,0.f);
    matr.scale(0.5f);
    matchess.scale(data->client_scale);
    data->current_shader->setUniformValue("posLight",mat*matr*data->lightpos*QVector4D(0.f,1.f,0.f,1.f));
    this->drawChess(mat*matr*matchess,1,true,QVector4D(0.01,0.04,0,0),0,QVector3D(1.f,1.f,1.f),0.3f);
    context->renderModel(gl->getBakedModel("font_resign"),mat*matr*matfont,QVector4D(),"font_fill",data->current_shader,font_mat);

    matchess=temp;
    matchess.rotate(180,0.f,1.f,0.f);
    matr=QMatrix4x4();
    matr.translate(4.f,0.f,6.f);
    matr.scale(0.3f);
    data->current_shader->setUniformValue("posLight",mat*matr*data->lightpos*QVector4D(0.f,0.f,0.f,1.f));
    this->drawChess(mat*matr*matchess,data->now_turn?0:1,true,QVector4D(0,0,0,0),0,QVector3D(1.f,1.f,1.f),0.3f);
}

