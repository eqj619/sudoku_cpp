//
//  main.cpp
//  sudoku_cpp
//
//  Created by EIJI OGA on 12/23/19.
//  Copyright © 2019 Eiji Oga. All rights reserved.
//
//#define DEBUG_PRINT1
#define RECURSIVE_CALL

#include <iostream>
#include <cstdio>
#include <vector>
#include <chrono>
using namespace std;

const int sudoku_size = 9;

struct aslot
{
    int fixed;      // 0 ... not find yet.
    int numList[sudoku_size]; // candidate numbers
};
const struct aslot aslotInit = {0, {1,2,3,4,5,6,7,8,9}};

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
    void SetFixedValue(int row, int column, int TrialNum);
    
    int resolve_sodoku(int slotNum);
    int getNextSlotNum(int no);
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
    
    if(sudokuForm[row][column].fixed != 0) return (0);
    
    for(int i=0; i<sudoku_size; i++){
        if(sudokuForm[row][column].numList[i] != 0){
            cnt++;
        }
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
#ifdef DEBUG_PRINT1
            printf("%d:%d ", row, column);
#endif
    
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
#ifdef DEBUG_PRINT2
        printf("checkWholeTable = %d\n", result);
        print();
        printNumOfList();
#endif
        openSlot = NumOfNoneResolvedSlot();
        //printf("Is complete %d\n", openSlot);
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
    //printf(">>>> %s\n", __FUNCTION__);
    for(int i=0; i < sudoku_size*sudoku_size; i++)
    {
        pNm[i] = sudokuForm[i/sudoku_size][i%sudoku_size].fixed;
    }
}

// Restore the captured intrim numbers to resume trial when rollback.
void SudokuMap::restoreNumMap(int pNm[sudoku_size*sudoku_size])
{
    //printf(">>>> %s\n", __FUNCTION__);
    
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

void SudokuMap::SetFixedValue(int row, int column, int TrialNum)
{
    sudokuForm[row][column].fixed = TrialNum;
}

// slotNum ... 0 to 80
// return
//      -1 ... Unresolved
//      0 ... Resolved
//      1-81 .. remained unresolved slot
int SudokuMap::resolve_sodoku(int slotNum)
{
    static bool initialCall = false;
    static int fLoop = 0;
    int result = 0;
    int sN = 0;
    
    printf("%s loop %d for slot %d(%d:%d)\n",
            __FUNCTION__, fLoop++, slotNum,
           slotNum/sudoku_size, slotNum%sudoku_size);
    
    if (initialCall == false)
    {
        result = sudokuChecker();
        printf("initial sudokuChecker %d\n", result);
        printNumOfList();
        
        if (result == 0) return(0);
        initialCall = true;
    }
    // find next unresolved slot
    sN = getNextSlotNum(slotNum);
    
    // capture a list of possible numbers
    int possibleList[sudoku_size] = {0,0,0,0,0,0,0,0,0};
    for(int i=0; i<sudoku_size; i++)
    {
        possibleList[i] = sudokuForm[sN/sudoku_size][sN%sudoku_size].numList[i];
        printf("%d ",possibleList[i] );
    }
    printf("\n");
    
    // Loop until try all possible numbers or Resolved
    int capMap[sudoku_size*sudoku_size];

    captureNumMap(capMap);
    
    for(int i=0; i<sudoku_size; i++)
    {
        if (possibleList[i] == 0) continue;
        
        restoreNumMap(capMap);
        
        // set trial number
        SetFixedValue(sN/sudoku_size, sN%sudoku_size, possibleList[i]);
        printf("slot %d (%d:%d) try %d\n", sN, sN/sudoku_size, sN%sudoku_size, possibleList[i]);
        // execute sudoku checker
        result = sudokuChecker();

        //printf(" RESULT for slot %d at sudokuChecker %d\n", slotNum, result);
        
        // if Completed, return 0
        if (result == 0) return(0);
        // if Unresolved, then re-store and try next number
        if (result == -1)
        {
            continue;
        }
        // succeed to fill in candidate number. then, try next slot.
        if (result > 0)
        {
            int ret;
            
            if (sN < sudoku_size*sudoku_size)
            {
                ret = resolve_sodoku(getNextSlotNum(sN));
            }
            else{
                printf("reach slot number > 81\n");
                return (-1);
            }
            if (ret == 0) return(0);
            if (ret == -1) {
                // printf("Return from nesting resolve_sudoku call i=%d\n",i);
                continue;
            }
        }
    }
    
    // Unresolved if un-resolved instead of try all candidates
    printf("Unresolved slot %d - Rollback\n",sN);
    return(-1);
}

int SudokuMap::getNextSlotNum(int slotNo)
{
    int min = sudoku_size;
    int minSlotNo = slotNo;
    
    for(int i=0; i<sudoku_size*sudoku_size; i++)
    {
         if(sudokuForm[i/sudoku_size][i%sudoku_size].fixed == 0)
         {
             if (min > numOfList(i/sudoku_size, i%sudoku_size))
             {
                 min = numOfList(i/sudoku_size, i%sudoku_size);
                 minSlotNo = i;
             }
         }
    }
    return(minSlotNo);
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
    
    const int test03[] = {
        0,0,5,4,0,1,6,0,0,
        0,0,0,0,6,0,0,0,0,
        0,9,4,0,0,7,0,2,0,
        
        3,7,0,0,2,0,0,4,0,
        0,0,0,0,1,0,0,0,0,
        0,1,0,0,4,0,0,5,6,
        
        0,5,0,8,0,0,2,6,0,
        0,0,0,0,9,0,0,0,0,
        0,0,2,3,0,6,9,0,0
    };
    
    const int test04[] = {
        5,0,4,0,0,0,0,0,1,
        0,0,6,4,0,0,0,7,0,
        0,0,0,6,0,8,0,0,0,
        
        0,0,0,0,5,0,0,1,3,
        0,0,0,0,0,0,0,0,0,
        8,3,0,0,2,0,0,0,0,
        
        0,0,0,9,0,4,0,0,0,
        0,4,0,0,0,7,6,0,0,
        2,0,0,0,0,0,8,0,9
    };
    
    const int test05[] = {
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,5,0,4,
        1,2,9,0,6,0,0,0,0,
        
        0,6,0,0,2,0,0,9,0,
        5,0,0,0,0,0,0,0,0,
        0,1,0,3,0,0,0,0,0,
        
        3,0,0,5,0,4,0,0,0,
        0,0,0,7,0,0,0,0,0,
        0,0,0,0,0,0,0,2,0
    };
    
    const int test06[] = {
        8,0,0,0,0,0,0,0,0,
        0,0,3,6,0,0,0,0,0,
        0,7,0,0,9,0,2,0,0,
        
        0,5,0,0,0,7,0,0,0,
        0,0,0,0,4,5,7,0,0,
        0,0,0,1,0,0,0,3,0,
        
        0,0,1,0,0,0,0,6,8,
        0,0,8,5,0,0,0,1,0,
        0,9,0,0,0,0,4,0,0
    };
    
    const int test07[] = {
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,1,0,6,0,
        2,0,7,0,9,0,0,0,0,
        
        3,0,0,0,0,4,2,0,9,
        0,1,0,6,0,5,0,0,0,
        0,0,0,0,0,0,0,0,0,
        
        0,6,4,0,0,0,0,5,0,
        0,0,0,0,2,0,3,0,0,
        0,0,0,0,0,0,0,0,0
    };
    const int test08[] = {
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,1,0,8,0,
        6,4,0,0,0,0,7,0,0,
        
        0,0,0,0,0,3,0,0,0,
        0,0,1,8,0,5,0,0,0,
        9,0,0,0,0,0,4,0,2,
        
        0,0,0,0,0,9,3,5,0,
        0,0,0,0,6,0,0,0,0,
        0,0,0,0,2,0,0,0,0
    };
    const int test09[] = {
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,2,8,0,
        3,7,6,4,0,0,0,0,0,
        
        7,0,0,0,0,1,0,0,0,
        0,2,0,0,0,0,0,0,0,
        4,0,0,3,0,0,0,0,6,
        
        0,1,0,0,2,8,0,0,0,
        0,0,0,0,0,5,0,0,0,
        0,0,0,0,0,0,0,0,3
    };

    SudokuMap map;
    
    // Sudoku Resolver
    map.fillNumMap(test09);
    map.print();
    
    chrono::system_clock::time_point  start, end;
    start = std::chrono::system_clock::now();
    
    if (map.resolve_sodoku(0) == 0){
        printf("SUCCEED\n");
    }
    else{
        printf("Failed\n");
    }
        
    end = std::chrono::system_clock::now();
    double elapsed = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    cout << "Spent time is " << elapsed << " usec.\n";
    
    map.print();

    return 0;
}
