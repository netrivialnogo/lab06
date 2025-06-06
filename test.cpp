#include <gtest/gtest.h>
#include <stdexcept>
#include "Account.h"
#include "Transaction.h"
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Ref;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MockTransaction() : Transaction() {}
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

TEST(Account, Mock) {
    MockAccount ac1(1, 1000);
    
    EXPECT_CALL(ac1, GetBalance())
        .Times(AtLeast(1))
        .WillOnce(Return(1000));
    EXPECT_CALL(ac1, Lock())
        .Times(AtLeast(1));
    EXPECT_CALL(ac1, ChangeBalance(1))
        .Times(AtLeast(1));
    EXPECT_CALL(ac1, Unlock())
        .Times(AtLeast(1));
    
    std::cout << ac1.GetBalance() << std::endl;
    ac1.Lock();
    ac1.ChangeBalance(1);
    ac1.Unlock();
}

TEST(Account, Methods) {
    Account ac1(1, 1000);
    EXPECT_EQ(1000, ac1.GetBalance());
    
    ac1.Lock();
    ac1.ChangeBalance(2000);
    ac1.Unlock();
    
    EXPECT_EQ(3000, ac1.GetBalance());
    
    try {
        ac1.ChangeBalance(1);
        FAIL() << "Expected std::runtime_error";
    }
    catch (const std::runtime_error& e) {
        EXPECT_STREQ("at first lock the account", e.what());
    }
    
    EXPECT_EQ(3000, ac1.GetBalance());
}

TEST(Transaction, Mock) {
    MockAccount ac1(1, 10000);
    MockAccount ac2(2, 10000);
    MockTransaction t1;
 
    EXPECT_CALL(ac1, Lock()).Times(1);
    EXPECT_CALL(ac2, Lock()).Times(1);
    EXPECT_CALL(ac1, Unlock()).Times(1);
    EXPECT_CALL(ac2, Unlock()).Times(1);
    
    EXPECT_CALL(t1, SaveToDataBase(Ref(ac1), Ref(ac2), 1999)).Times(1);
    t1.Make(ac1, ac2, 1999);
}

TEST(Transaction, Methods) {
    Account ac1(1, 10000);
    Account ac2(2, 10000);
    Transaction t1;
    Transaction t2; t2.set_fee(500);

    try {
        t1.Make(ac1, ac1, 100);
        FAIL() << "Expected std::logic_error";
    }
    catch (std::logic_error& el) {
        EXPECT_STREQ("invalid action", el.what());
    }

    try {
        t1.Make(ac1, ac2, -100);
        FAIL() << "Expected std::invalid_argument";
    }
    catch (std::invalid_argument& el) {
        EXPECT_STREQ("sum can't be negative", el.what());
    }


    try {
        t1.Make(ac1, ac2, 0);
        FAIL() << "Expected std::logic_error";
    }
    catch (std::logic_error& el) {
        EXPECT_STREQ("too small", el.what());
    }

    EXPECT_FALSE(t2.Make(ac1, ac2, 200));


    EXPECT_TRUE(t1.Make(ac1, ac2, 1999));
    
    EXPECT_EQ(10000, ac1.GetBalance());  
    EXPECT_EQ(9999, ac2.GetBalance());   
}
