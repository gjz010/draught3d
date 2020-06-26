#include "DraughtManager.h"
#include <algorithm>
#include <QStringList>
inline bool V(int row, int column) {
    return row >= 0 && row <= 9 && column >= 0 && column <= 9;
}
inline int C(int row, int column) {
    return row * 10 + column;
}
inline int S(int idx) {
    return (idx >= 0 && idx <= 40) ? (idx < 20 ? 0 : 1): idx;
}
DraughtManager::DraughtManager()
{
    memset(isking, 0, sizeof(isking));
    for (int i = 0; i<10; i++) {
        for (int j = 0; j<10; j++) {
            chessboard[i][j]=-1;

        }

    }
    for (int i = 0; i<20; i++) {

        int gold_id = i;
        int gold_column = i % 5;
        gold_column = 2 * gold_column;
        int gold_row = i / 5;
        if (gold_row & 1) {
            gold_column++;

        }
        //gold_row=2*gold_row;

        chessboard[gold_row][gold_column] = gold_id;
        chessboard[9 - gold_row][9 - gold_column] = 20 + gold_id;

    }
}

std::vector<std::vector<DraughtManager::Step*>> DraughtManager::SolveTree(int draught[10][10], int side, bool isking[40])
{
    std::vector<std::vector<DraughtManager::Step*>> result;
    std::vector<std::vector<DraughtManager::Step*>> new_result;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int grididx = C(i, j);
            if (S(draught[i][j]) == side) {
                auto vec = dfs(grididx, draught, side, isking[draught[i][j]], true);
                if (!vec.empty())
                    result.push_back(std::move(vec));
            }


        }
    }

    if (result.empty()) return result;
    //We make sure there are no empty vectors in result.
    std::sort(result.begin(), result.end(), [&](const std::vector<Step*>& lh, const std::vector<Step*>& rh) {
        int lenlh, lenrh;
        if (lh.empty()) lenlh = 0;
        else lenlh = lh[0]->len;
        if (rh.empty()) lenrh = 0;
        else lenrh = rh[0]->len;
        return lenlh > lenrh;

    });
    int len = result[0][0]->len; //So the greatest one should have a length.
    for (int i = 0; i < result.size(); i++) {
        if (result[i][0]->len == len) {
            //std::vector<DraughtManager::Step> line;
            new_result.push_back(std::move(result[i]));

        }
        else {
            for (Step* s : result[i]) delete s;

        }
    }
    return new_result;
}
/*
static QString serial_dfs(DraughtManager::Step* step){
    QStringList nodes;
    nodes.append(QString("%1,%2,%3").arg(step->killing_place).arg(step->land_place).arg(step->substeps.size()));
    for(DraughtManager::Step* s:step->substeps) nodes.append(serial_dfs(s));
    return QString(nodes.join(','));

}
QString DraughtManager::serialize(std::vector<std::vector<DraughtManager::Step *> > tree)
{
    QStringList nodes;
    nodes.append(QString("%1").arg(tree.size()));
    for(std::vector<Step*> chess:tree){
        nodes.append(QString("%1,%2").arg(chess[0]->src).arg(chess.size()));

        for(Step* step:chess){
            nodes.append(serial_dfs(step));

        }

    }
    return QString(nodes.join(','));
}

DraughtManager::Step* readStepChain(int src,std::vector<int>::iterator& iter){
    DraughtManager::Step* result=new DraughtManager::Step;
    result->killing_place=*(iter++);
    result->land_place=*(iter++);
    result->src=src;
    int counter=*(iter++);
    for(int i=0;i<counter;i++){
        result->substeps.push_back(std::move(readStepChain(src,iter)));

    }
    return result;

}
std::vector<DraughtManager::Step*> readChessTree(std::vector<int>::iterator& iter){
    std::vector<DraughtManager::Step*> result;
    int src=*(iter++);
    int size=*(iter++);
    for(int i=0;i<size;i++){
        result.push_back(std::move(readStepChain(src,iter)));

    }
    return result;

}
std::vector<std::vector<DraughtManager::Step*> > readBigTree(std::vector<int>::iterator& iter){
    std::vector<std::vector<DraughtManager::Step*> > result;
    int count=*(iter++);
    for(int i=0;i<count;i++){
        result.push_back(std::move(readChessTree(iter)));

    }
    return result;
}

std::vector<std::vector<DraughtManager::Step *> > DraughtManager::deserialize(QString stree)
{
    QStringList nodes=stree.split(',');
    std::vector<int> numberlist;
    for(auto iter=nodes.begin();iter!=nodes.end();iter++){
        int num=iter->toInt(); //This must succeed!
        numberlist.push_back(num);

    }
    auto iter=numberlist.begin();
    return readBigTree(iter);
}

*/
std::vector<DraughtManager::Step*> DraughtManager::dfs(int src_pos, int draught[10][10], int side, bool isking,bool initial)
{
    printf("Calling DFS at %d\n",src_pos);
    fflush(stdout);
    //Every dfs caller should restore draught array.
    std::vector<Step*> result;
    int idx = draught[src_pos / 10][src_pos % 10];
    //Let's just talk about what grids can the chess go and think whether they really can reach.
    if (isking) {
        int row = src_pos / 10;
        int column = src_pos % 10;
        int dirs[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
        for (int i = 0; i < 4; i++) {
            int nrow = row, ncolumn = column;
            while (true) {
                nrow += dirs[i][0];
                ncolumn += dirs[i][1];
                if (!V(nrow, ncolumn)) break;
                if (S(draught[nrow][ncolumn]) == side || draught[nrow][ncolumn]==-2) break;
                if (draught[nrow][ncolumn] == -1 && initial) {
                    Step* step = new Step;
                    step->src = idx;
                    step->land_place = C(nrow, ncolumn);
                    step->killing_place = -1;
                    step->len = 1;
                    result.push_back(std::move(step));

                }
                if (S(draught[nrow][ncolumn]) == 1 - side) {
                    //printf("Jumping!\n");
                    int nnrow = nrow, nncolumn = ncolumn;
                    int killing = draught[nrow][ncolumn];
                    draught[row][column] = -1;
                    draught[nrow][ncolumn] = -2;
                    while (true) {
                        nnrow+=dirs[i][0];
                        nncolumn+=dirs[i][1];
                        if (!V(nnrow, nncolumn)) break;
                        if (draught[nnrow][nncolumn] != -1) break;
                        draught[nnrow][nncolumn] = idx;
                        auto vec=dfs(C(nnrow, nncolumn), draught, side, isking, false);
                        draught[nnrow][nncolumn] = -1;
                        Step* step = new Step;
                        step->src = idx;
                        step->land_place = C(nnrow, nncolumn);
                        step->killing_place = C(nrow, ncolumn);
                        step->len = vec.empty() ? 2 : vec[0]->len + 2;
                        step->substeps = std::move(vec);
                        result.push_back(std::move(step));

                    }
                    draught[row][column] = idx;
                    draught[nrow][ncolumn] = killing;


                    break;
                }

            }
        }
    }
    else {
        int row = src_pos / 10;
        int column = src_pos % 10;
        int delta_row = side ? -1 : 1;
        int dirs[2] = {-1,1};
        int dir4[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
        for(int i=0;i<4;i++){
            int nrow = row+dir4[i][0], ncolumn = column+dir4[i][1];
            if(V(nrow,ncolumn)){
                if (S(draught[nrow][ncolumn]) == 1 - side) {
                    //printf("Trying to jump...\n");
                    int nnrow=nrow+dir4[i][0];
                    int nncolumn=ncolumn+dir4[i][1];
                    if (V(nnrow, nncolumn)) {
                        if (draught[nnrow][nncolumn] == -1) {
                            draught[row][column] = -1; //Removed
                            int killed = draught[nrow][ncolumn];
                            draught[nrow][ncolumn] = -2; //Butchered
                            draught[nnrow][nncolumn] = idx; //Chess placed.
                            auto vec = dfs(C(nnrow, nncolumn), draught, side, isking, false);
                            draught[nnrow][nncolumn] = -1;
                            draught[nrow][ncolumn] = killed;
                            draught[row][column] = idx;
                            Step* step = new Step;
                            step->src = idx;
                            step->land_place = C(nnrow, nncolumn);
                            step->killing_place = C(nrow, ncolumn);
                            step->len = vec.empty() ? 2 : vec[0]->len + 2;
                            step->substeps = std::move(vec);
                            result.push_back(std::move(step));

                        }
                    }
                }

            }


        }
        for (int i = 0; i < 2; i++) {
            int nrow = row, ncolumn = column;
            nrow += delta_row;
            ncolumn += dirs[i];
            if (V(nrow,ncolumn)) {
                if (initial) {

                    if (draught[nrow][ncolumn] == -1) {

                        Step* step = new Step;
                        step->src = idx;
                        step->land_place = C(nrow, ncolumn);
                        step->killing_place = -1;
                        step->len = 1;
                        result.push_back(std::move(step));
                    }
                }

                printf("%d %d\n", draught[nrow][ncolumn],S(draught[nrow][ncolumn]));
                fflush(stdout);
            }
        }


    }



    //We only keep the longest one each dfs call.
    if (result.empty()) return result; //Only happens after one jump and no others.
    std::sort(result.begin(), result.end(), [=](const Step* lh, const Step* rh) {
        return lh->len > rh->len;
    });
    int len = result[0]->len;
    std::vector<Step*> new_result;
    for (int i = 0; i < result.size(); i++) {
        if (result[i]->len == len) {
            new_result.push_back(std::move(result[i]));
        }
        else {
            delete result[i];
        }
    }
    return new_result;
}
/*
void DraughtManager::dfs_steps(Step* current,std::vector<StepChain>& chains, StepChain & chain, int depth)
{
    printf("Push 1!\n");
    chain.args.push_back(current->killing_place);
    printf("Push 2!\n");
    chain.args.push_back(current->land_place);
    fflush(stdout);
    if (current->substeps.empty()) {
        chain.step_count = depth;
        chains.push_back(chain);

    }
    else {
        for (Step* next : current->substeps) {
            dfs_steps(next, chains, chain, depth + 1);
        }

    }
    printf("Pop 1!\n");
    chain.args.pop_back();
    printf("Pop 2!\n");
    chain.args.pop_back();
    fflush(stdout);
}


std::vector<DraughtManager::StepChain> DraughtManager::Solve(int draught[10][10], int side, bool isking[40])
{
    std::vector<StepChain> chain;
    auto vec = SolveTree(draught, side, isking);
    for (std::vector<Step*> choice : vec) {
        for (Step* stepchain : choice) {
            StepChain schain;
            schain.args.reserve(stepchain->len+1);
            schain.src = stepchain->src;
            dfs_steps(stepchain,chain, schain, 1);
        }

    }
    return chain;
}
*/
DraughtManager::~DraughtManager()
{
}
