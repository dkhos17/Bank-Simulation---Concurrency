#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "branch.h"
#include "error.h"
#include "debug.h"

/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);  

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  sem_wait(&(account->lockAcc));
  sem_wait(&(bank->branches[AccountNum_GetBranchID(account->accountNumber)].lockBranch));

  if (account == NULL) {    
    sem_post(&(bank->branches[AccountNum_GetBranchID(account->accountNumber)].lockBranch));
    sem_post(&(account->lockAcc));
    return ERROR_ACCOUNT_NOT_FOUND;
  }
  
  Account_Adjust(bank,account, amount, 1);
  sem_post(&(account->lockAcc));
  sem_post(&(bank->branches[AccountNum_GetBranchID(account->accountNumber)].lockBranch));
  
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  sem_wait(&(account->lockAcc));
  sem_wait(&(bank->branches[AccountNum_GetBranchID(accountNum)].lockBranch));

  if (account == NULL) {
    sem_post(&(account->lockAcc));
    sem_post(&(bank->branches[AccountNum_GetBranchID(accountNum)].lockBranch));
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  if (amount > Account_Balance(account)) {
    sem_post(&(account->lockAcc));
    sem_post(&(bank->branches[AccountNum_GetBranchID(accountNum)].lockBranch));
    return ERROR_INSUFFICIENT_FUNDS;
  }

  Account_Adjust(bank,account, -amount, 1);

  sem_post(&(account->lockAcc));
  sem_post(&(bank->branches[AccountNum_GetBranchID(accountNum)].lockBranch));
  
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }


  if(srcAccountNum == dstAccountNum)return ERROR_SUCCESS;

  int srcID = AccountNum_GetBranchID(srcAccountNum);
  int dstID = AccountNum_GetBranchID(dstAccountNum);
  int same = !(srcID == dstID);
  int wait_case = srcID - dstID;

  if(wait_case > 0){
       waitSemas(bank, srcAccount, dstAccount, srcID, dstID); 
  } else if(wait_case < 0){
       waitSemas(bank, dstAccount, srcAccount, dstID, srcID);
  } else{
      if(srcAccountNum > dstAccountNum)waitAccSemas(bank, srcAccount, dstAccount);
      else waitAccSemas(bank, dstAccount, srcAccount);
  }
  
  if (amount > Account_Balance(srcAccount)) {
    postSemas(bank, srcAccount, dstAccount, srcID, dstID);
    return ERROR_INSUFFICIENT_FUNDS;
  }
  // sem_post(&(srcAccount->acc));
  // sem_post(&(dstAccount->acc));
  // sem_post(&(bank->branches[srcID].trans));
  // sem_post(&(bank->branches[dstID].trans));

  Account_Adjust(bank, srcAccount, -amount, same);
  Account_Adjust(bank, dstAccount, amount, same);

  // sem_wait(&(dstAccount->acc));
  // sem_wait(&(srcAccount->acc));
  // sem_wait(&(bank->branches[dstID].trans));
  // sem_wait(&(bank->branches[srcID].trans));

  postSemas(bank, srcAccount, dstAccount, srcID, dstID);

  return ERROR_SUCCESS;
}

//post waited semahores 
void postSemas(Bank *bank, Account *src, Account *dst, int srcID, int dstID){
    if(srcID - dstID != 0){
        sem_post(&(bank->branches[srcID].lockBranch));
        sem_post(&(bank->branches[dstID].lockBranch));
    }
    sem_post(&(src->lockAcc));
    sem_post(&(dst->lockAcc));
}

//wair accounts and branches semaphores
void waitSemas(Bank *bank, Account *srcAccount, Account *dstAccount, int srcID, int dstID){
    waitAccSemas(bank, srcAccount, dstAccount);
    sem_wait(&(bank->branches[dstID].lockBranch));
    sem_wait(&(bank->branches[srcID].lockBranch)); 
}

//wait only accounts semaphores
void waitAccSemas(Bank *bank, Account *src, Account* dst){
    sem_wait(&(dst->lockAcc));
    sem_wait(&(src->lockAcc));
}