//
//  main.cpp
//  sudoku_cpp
//
//  Created by EIJI OGA on 12/23/19.
//  Copyright © 2019 Eiji Oga. All rights reserved.
//

#include <iostream>

const int sudoku_size = 9;

struct aslot
{
    int fixed;      // 0 ... not find yet.
    int try1;
    int try2;
    int numList[sudoku_size]; // candidate numbers
};
const struct aslot aslotInit = {0, 0, 0, {1,2,3,4,5,6,7,8,9}};

class SudokuMap
{
private:
    struct aslot sudokuForm[sudoku_size][sudoku_size];
    
    int VerifyNumber(int row, int column);
    int checkWholeNumMap();

public:
    SudokuMap();

    void print();
    void printNumOfList();
    
    void fillNumMap(const int pS[sudoku_size*sudoku_size]);
    int NumOfNoneResolvedSlot();
    int numOfList(int row, int column);
    int sudokuChecker();
    void captureNumMap(int pNm[sudoku_size*sudoku_size]);
    void restoreNumMap(int pNm[sudoku_size*sudoku_size]);
    int getTrialNumber(int flag, int row, int column);
    void SetFixedValue(int row, int column, int TrialNum);
};

SudokuMap::SudokuMap()
{
    for(int i=0; i<sudoku_size*sudoku_size; i++){
        sudokuForm[i/sudoku_size][i%sudoku_size] = aslotInit;
    }
}

void SudokuMap::print()
{
    for(int i=0; i<sudoku_size*sudoku_size; i++){
        if(sudokuForm[i/sudoku_size][i%sudoku_size].fixed != 0) {
            printf("%d ", sudokuForm[i/sudoku_size][i%sudoku_size].fixed);
        }
        else printf("_ ");
        if(i%sudoku_size == sudoku_size-1) printf("\n");
    }
}

// look at the number of candidates in each slot
// then, return one of trial number when candiadtes numbers is list on 2.
//
int SudokuMap::numOfList(int row, int column)
{
    int cnt = 0;
    int t[2] = {0,0};
    
    if(sudokuForm[row][column].fixed != 0) return (0);
    
    for(int i=0; i<sudoku_size; i++){
        if(sudokuForm[row][column].numList[i] != 0){
            cnt++;
            if(cnt==1){
                t[0] = sudokuForm[row][column].numList[i];
            };
            if(cnt==2) {
                t[1] = sudokuForm[row][column].numList[i];
            };
        }
    }
    
    if(cnt == 2)
    {
        sudokuForm[row][column].try1 = t[0];
        sudokuForm[row][column].try2 = t[1];
    }
    return(cnt);
}

void SudokuMap::printNumOfList()
{
    printf("%s\n", __FUNCTION__);
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            printf("%d ", numOfList(i,j));
        }
        printf("\n");
    }
}

void SudokuMap::fillNumMap(const int pS[])
{
    for (int i=0; i<sudoku_size*sudoku_size; i++){
        sudokuForm[i/sudoku_size][i%sudoku_size].fixed = pS[i];
    }
}

int SudokuMap::VerifyNumber(int row, int column)
{
    int cnt;
        
    // it's alreay fixed number
    if(sudokuForm[row][column].fixed != 0 ){
        return(sudokuForm[row][column].fixed);
    }
    
    // Check at Horizontal Line
    for(int i=0; i<sudoku_size; i++){
        if(i != column){                // skip by itself
            if(sudokuForm[row][i].fixed != 0){   // exit this number. then, drop this number from list
                sudokuForm[row][column].numList[ sudokuForm[row][i].fixed - 1] = 0;
            }
        }
    }
    // Check at Vertical Line
    for(int i=0; i<sudoku_size; i++){
        if(i != row){                   // skip by itself
            if(sudokuForm[i][column].fixed != 0){    // exit this number. then, drop this number from list
                sudokuForm[row][column].numList[ sudokuForm[i][column].fixed - 1] = 0;
            }
        }
    }
    // Check Block
    const int map[sudoku_size][sudoku_size] = {
        {0,0,0,1,1,1,2,2,2},
        {0,0,0,1,1,1,2,2,2},
        {0,0,0,1,1,1,2,2,2},
        {3,3,3,4,4,4,5,5,5},
        {3,3,3,4,4,4,5,5,5},
        {3,3,3,4,4,4,5,5,5},
        {6,6,6,7,7,7,8,8,8},
        {6,6,6,7,7,7,8,8,8},
        {6,6,6,7,7,7,8,8,8}
    };
    
    int targetBlock;
    int checkCnt = 8;
    
    targetBlock = map[row][column];
    
    for(int i=0; i<sudoku_size; i++){
        for(int j=0; j<sudoku_size; j++){
            if(map[i][j] == targetBlock){       // in same block
                if(i != row && j != column){    // skip by itself
                    if(sudokuForm[i][j].fixed != 0){     // exit this number. then, drop this number from list
                        sudokuForm[row][column].numList[ sudokuForm[i][j].fixed - 1] = 0;
                    }
                    checkCnt--;
                    if (checkCnt == 0) break;   // verify all number in a block
                }
            }
        }
    }
    
    // check how many candidate in numList
    cnt = 0;
    for(int i=0; i<sudoku_size; i++)
    {
        if( sudokuForm[row][column].numList[i] != 0 ) {
            cnt++;
            sudokuForm[row][column].fixed = sudokuForm[row][column].numList[i];
        }
#ifdef DEBUG_PRINT1
        printf("%d", sudokuForm[row][column].numList[i]);
#endif
    }
#ifdef DEBUG_PRINT1
    printf("\n");
#endif
    // cnt = 0 ... ERROR cannot find a number. all number are already there.
    // cnt = 1 ... find a number
    // cnt > 1 ... cannot fix because of 2 and more candidate remain. then, reset fixed value.
    if(cnt > 1) sudokuForm[row][column].fixed = 0;
    if(sudokuForm[row][column].fixed == 0 && cnt == 0) return (-1);
    return(sudokuForm[row][column].fixed);
}

int SudokuMap::checkWholeNumMap()
{
    int result = 1; // success
    
    for(int i=0; i<sudoku_size; i++)
    {
        for(int j=0; j<sudoku_size; j++)
        {
            result = VerifyNumber(i, j);
            if (result == -1) return (-1);
        }
    }
    return(1);
}

// count the number of none-resolved slot
// when result is ZERO, that's mean resolved all.
int SudokuMap::NumOfNoneResolvedSlot()
{
    int result = 0;
    
    //printf("%s\n", __FUNCTION__);
    for(int i=0; i<sudoku_size; i++){
        for(int j=0; j<sudoku_size; j++){
            if (sudokuForm[i][j].fixed == 0 ) result++;
        }
    }
    return (result);
}

// Check then fill the number in resolved slot
// return value
//  -1 ... Sudoku Numbering Failure. need try another candidate number.
//  0 ... complete
//  1-81 ... the number of unresolved slot
int SudokuMap::sudokuChecker()
{
    static int lastNum = 0;
    int openSlot = -1;
    int result = 0;
    
    while(result != -1)
    {
        result = checkWholeNumMap();
        printf("checkWholeTable = %d\n", result);
        
        print();
        printNumOfList();
        openSlot = NumOfNoneResolvedSlot();
        printf("Is complete %d\n", openSlot);
        if (openSlot == lastNum) break;
        else lastNum = openSlot;
    }
    
    if (result == -1) return (result);
    return (openSlot);
}

// Capture the snapshot of intrim Sudoku Form
// Intrim captured form is used when rollback then try another candidate number.
void SudokuMap::captureNumMap(int pNm[sudoku_size*sudoku_size])
{
    // printf(">>>> %s\n", __FUNCTION__);
    for(int i=0; i < sudoku_size*sudoku_size; i++)
    {
        pNm[i] = sudokuForm[i/sudoku_size][i%sudoku_size].fixed;
    }
}

// Restore the captured intrim numbers to resume trial when rollback.
void SudokuMap::restoreNumMap(int pNm[sudoku_size*sudoku_size])
{
    // printf(">>>> %s\n", __FUNCTION__);
    
    for(int i=0; i < sudoku_size*sudoku_size; i++)    {
        if(pNm[i] == 0 && sudokuForm[i/sudoku_size][i%sudoku_size].fixed != 0)
        {
            sudokuForm[i/sudoku_size][i%sudoku_size] = aslotInit;
        }
        else if(sudokuForm[i/sudoku_size][i%sudoku_size].fixed == 0)
        {   // clean up numList for next verification
            for(int j=0; j<sudoku_size; j++) sudokuForm[i/sudoku_size][i%sudoku_size].numList[j] = j+1;
        }
        else if(pNm[i] != sudokuForm[i/sudoku_size][i%sudoku_size].fixed)
        {
            // asert
            printf("ERROR %s: pNm[%d]=%d, f[%d][%d]=%d\n", __FUNCTION__, i, pNm[i], i/sudoku_size, i%sudoku_size, sudokuForm[i/sudoku_size][i%sudoku_size].fixed);
        }
    }
}
int SudokuMap::getTrialNumber(int flag, int row, int column)
{
    if(flag == 1){
        return(sudokuForm[row][column].try1);
    }
    if(flag == 2){
        return(sudokuForm[row][column].try2);
    }
    return(-1); // error
}

void SudokuMap::SetFixedValue(int row, int column, int TrialNum)
{
    sudokuForm[row][column].fixed = TrialNum;
}

//------------
struct checkPoint
{
    struct checkPoint   *pPrev;     // Null = head
    int                 slotNum;    // 0 to 80
    int                 capMap[sudoku_size*sudoku_size];
    int                 try1;       // 0 is fail
    struct checkPoint   *pNextTry1; // next pointer
    int                 try2;
    struct checkPoint   *pNextTry2;
};

struct checkPoint *addCheckPoint(struct checkPoint *pCP, int flag)
{
    struct checkPoint *pTmp;
    
    pTmp = (checkPoint*)malloc(sizeof(struct checkPoint));

    if (flag == 1){
        pCP->pNextTry1 = pTmp;
    }
    if (flag == 2){
        pCP->pNextTry2 = pTmp;
    }
    
    pTmp->pPrev = pCP;
    pTmp->slotNum = -1;
    pTmp->try1 = 0;
    pTmp->pNextTry1 = NULL;
    pTmp->try2 = 0;
    pTmp->pNextTry2 = NULL;
    
    return(pTmp);
}

// ----------

int main(int argc, const char * argv[])
{
    const int test02[] = {
        1,5,7,6,4,0,0,9,8,
        2,0,9,0,0,0,0,0,0,
        0,0,0,9,1,0,0,0,4,
        
        8,0,0,4,3,0,0,5,0,
        0,0,0,0,0,0,0,0,0,
        0,2,0,0,6,8,0,0,7,
        
        7,0,0,0,8,6,0,0,0,
        0,0,0,0,0,0,0,0,1,
        0,9,0,0,0,4,8,6,2
    };
    
    SudokuMap map;
    
    // Sudoku Resolver
    int result;
    struct checkPoint topCP =
        {NULL, -1,
        {    0,0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,0,
             0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,
         },
            0, NULL, 0, NULL};
    struct checkPoint *pHead = NULL;
    
    map.fillNumMap(test02);
    map.sudokuChecker();
    
    int i, loop, flag = 3;
    
    if(topCP.slotNum == -1){        // initial loop
        pHead = &topCP;
    }
    map.captureNumMap(pHead->capMap);
    
    loop = 0;
    while(map.NumOfNoneResolvedSlot() != 0)
    {
        loop++;
        printf("===== LOOP %d ============================\n", loop);
        
        // find 2 candidates slot if checkPoint is new.
        // then, capture the number's map before try
        if(pHead->try1 == 0 && pHead->try2 == 0)
        {
            map.captureNumMap(pHead->capMap);
            for (i=0; i < (sudoku_size*sudoku_size); i++){
                if ( map.numOfList(i/sudoku_size, i%sudoku_size) == 2)
                {
                    pHead->slotNum = i;
                    pHead->try1 = map.getTrialNumber(1, i/sudoku_size, i%sudoku_size);
                    pHead->try2 = map.getTrialNumber(2, i/sudoku_size, i%sudoku_size);
                    break;
                }
            }
        }
        printf("pHead pPrev 0x%08x, slotNum %d(%d-%d), try1 %d, 0x%08x, try2 %d, try2 ptr 0x%08x\n",
               pHead->pPrev, pHead->slotNum, pHead->slotNum/9,pHead->slotNum%9, pHead->try1, pHead->pNextTry1, pHead->try2, pHead->pNextTry2 );
        
        // Before try to fill-in number re-store captured numbers
        // to verify Sudoku form using refresh candidate lists
        map.restoreNumMap(pHead->capMap);
        
        // Try candidate
        if (pHead->try1 > 0 && pHead->pNextTry1 == NULL){
            map.SetFixedValue(pHead->slotNum/sudoku_size, pHead->slotNum%sudoku_size, pHead->try1);
            pHead->try1 = -1;       // marked for checked candidate.
            flag = 1;
        }
        else if (pHead->try2 > 0 && pHead->pNextTry2 == NULL){
            map.SetFixedValue(pHead->slotNum/sudoku_size, pHead->slotNum%sudoku_size, pHead->try2);
            pHead->try2 = -1;      // marked for checked candidate.
            flag = 2;
        }
        else if (pHead->try1 == -1 && pHead->try2 == -1){
            void *pTmp;
            
            // roll back previous check point
            printf("BOTH Trial number are failed... Rollback to previous candidate\n");
            map.SetFixedValue(pHead->slotNum/sudoku_size, pHead->slotNum%sudoku_size, 0);
            
            pTmp = (void*) pHead;
            pHead = pHead->pPrev;
            free(pTmp);
            
            continue;
        }
        
        // Check Sudoku Numbers
        result = map.sudokuChecker();
        
        if (result >= 1)
        {
            struct checkPoint *pTmp;
            // if result is good. add next slot, link then move a head pointer
            pTmp = addCheckPoint(pHead, flag);
            
            if (pTmp != 0) pHead = pTmp;
            else{
                printf("Error %d\n", __LINE__);
                return(-1);
            }
        }
        else if (result == -1)
        {
            printf("sudokuChecker FAILED flag = %d\n", flag);
            map.SetFixedValue(pHead->slotNum/sudoku_size, pHead->slotNum%sudoku_size, 0);
        }
    } // while(loop)
    return 0;
}
