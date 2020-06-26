#pragma once
#include <vector>
#include <QString>
//Note that 2-dim arrays here are arranged in GL-order!
/*
 9
 8
 7
 6
 5
 4
 3
 2
 1
 0
  0 1 2 3 4 5 6 7 8 9
 */
class DraughtManager
{
public:
    int chessboard[10][10];
    bool isking[40];
    DraughtManager();
    struct Step {
        int src;
        int land_place;
        int killing_place; //-1 for no killing.
        int len;
        std::vector<Step*> substeps;
        ~Step() {
            for (Step* step : substeps) {
                delete step;
            }
        }
    };
    /*
    struct StepChain {
        int src;
        int step_count;
        std::vector<int> args;
    };
    */
    //std::vector<StepChain> Solve(int draught[10][10] /* 0-19 for golden and 20-39 for silver */, int side /* 0 for golden */, bool isking[40]);
    std::vector<std::vector<DraughtManager::Step*>> SolveTree(int draught[10][10] /* 0-19 for golden and 20-39 for silver */, int side /* 0 for golden */, bool isking[40]);

    //QString serialize(std::vector<std::vector<DraughtManager::Step*>> tree);
    //std::vector<std::vector<DraughtManager::Step*>> deserialize(QString stree);
    ~DraughtManager();
private:



    std::vector<Step*> dfs(int src_pos, int draught[10][10], int side, bool isking, bool initial = true);
    //void dfs_steps(Step* current,std::vector<StepChain>& chains,StepChain& chain,int depth);
};

