#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MockTransaction() : Transaction() {}
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));

    bool RealMake(Account& from, Account& to, int sum) {
        return Transaction::Make(from, to, sum);
    }
};

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockAcc = std::make_unique<MockAccount>(1, 1000);
        realAcc = std::make_unique<Account>(2, 2000);
    }
    
    std::unique_ptr<MockAccount> mockAcc;
    std::unique_ptr<Account> realAcc;
};

TEST_F(AccountTest, GetBalanceReturnsCorrectValue) {
    EXPECT_CALL(*mockAcc, GetBalance())
        .WillOnce(Return(1000));
    EXPECT_EQ(mockAcc->GetBalance(), 1000);
}

TEST_F(AccountTest, ChangeBalanceOnlyWhenLocked) {
    EXPECT_CALL(*mockAcc, Lock());
    EXPECT_CALL(*mockAcc, ChangeBalance(1000));
    EXPECT_CALL(*mockAcc, Unlock());
    
    mockAcc->Lock();
    mockAcc->ChangeBalance(1000);
    mockAcc->Unlock();

    EXPECT_CALL(*mockAcc, ChangeBalance(_))
        .WillOnce(Throw(std::runtime_error("Account is not locked")));
    EXPECT_THROW(mockAcc->ChangeBalance(1000), std::runtime_error);
}

TEST_F(AccountTest, RealAccountBehavior) {
    EXPECT_EQ(realAcc->GetBalance(), 2000);
    
    realAcc->Lock();
    realAcc->ChangeBalance(500);
    realAcc->Unlock();
    
    EXPECT_EQ(realAcc->GetBalance(), 2500);
    
    EXPECT_THROW(realAcc->ChangeBalance(100), std::runtime_error);
    EXPECT_EQ(realAcc->GetBalance(), 2500); 
}

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        acc1 = std::make_unique<Account>(1, 10000);
        acc2 = std::make_unique<Account>(2, 10000);
        mockTrans = std::make_unique<MockTransaction>();
    }
    
    std::unique_ptr<Account> acc1;
    std::unique_ptr<Account> acc2;
    std::unique_ptr<MockTransaction> mockTrans;
    Transaction realTrans;
};

TEST_F(TransactionTest, MakeValidTransaction) {
    EXPECT_CALL(*mockTrans, SaveToDataBase(Ref(*acc1), Ref(*acc2), 1999))
        .Times(1);
    mockTrans->Make(*acc1, *acc2, 1999);
}

TEST_F(TransactionTest, InvalidTransactionsThrow) {
    EXPECT_THROW(realTrans.Make(*acc1, *acc1, 100), std::logic_error);
    EXPECT_THROW(realTrans.Make(*acc1, *acc2, -100), std::invalid_argument);
    EXPECT_THROW(realTrans.Make(*acc1, *acc2, 0), std::logic_error);
}

TEST_F(TransactionTest, InsufficientFunds) {
    realTrans.set_fee(500);
    EXPECT_FALSE(realTrans.Make(*acc1, *acc2, 9600)); 
}

TEST_F(TransactionTest, SuccessfulTransactionUpdatesBalances) {
    ASSERT_TRUE(realTrans.Make(*acc1, *acc2, 2000));
    EXPECT_EQ(acc1->GetBalance(), 7999);
    EXPECT_EQ(acc2->GetBalance(), 12000); 
}

TEST_F(TransactionTest, CustomFeeCalculation) {
    Transaction customFeeTrans;
    customFeeTrans.set_fee(100);
    
    ASSERT_TRUE(customFeeTrans.Make(*acc1, *acc2, 1000));
    EXPECT_EQ(acc1->GetBalance(), 8900); 
    EXPECT_EQ(acc2->GetBalance(), 11000);
}
