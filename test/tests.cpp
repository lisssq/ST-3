// Copyright 2026 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;
  void SetUp() override {
    door = new TimedDoor(5);
  }
  void TearDown() override {
    delete door;
  }
};

TEST_F(TimedDoorTest, InitialStateClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockWorks) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockWorks) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, CorrectTimeoutValue) {
  EXPECT_EQ(door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, ThrowExceptionIfOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoExceptionIfClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

TEST(AdapterTest, AdapterCallsThrowState) {
  TimedDoor tDoor(1);
  tDoor.unlock();
  DoorTimerAdapter adapter(tDoor);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, TimerTriggersClient) {
  MockTimerClient mockClient;
  Timer timer;
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(0, &mockClient);
}

TEST(MockTest, MockDoorUnlockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, unlock()).Times(1);
  mDoor.unlock();
}

TEST(IntegrationTest, FullSequenceException) {
  TimedDoor tDoor(3);
  tDoor.unlock();
  DoorTimerAdapter adapter(tDoor);
  Timer timer;
  EXPECT_THROW(timer.tregister(0, &adapter), std::runtime_error);
}

TEST(MockTest, MockDoorReturnsValue) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, isDoorOpened()).WillOnce(::testing::Return(true));
  EXPECT_TRUE(mDoor.isDoorOpened());
}

TEST(MockTest, MockDoorLockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, lock()).Times(1);
  mDoor.lock();
}

TEST_F(TimedDoorTest, StateToggleConsistency) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

TEST(AdapterTest, NoThrowIfClosedInTime) {
  TimedDoor tDoor(5);
  tDoor.unlock();
  tDoor.lock();
  DoorTimerAdapter adapter(tDoor);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(TimedDoorConstructor, ZeroTimeout) {
  TimedDoor tDoor(0);
  EXPECT_EQ(tDoor.getTimeOut(), 0);
}

TEST(IntegrationTest, FullSequenceNoException) {
  TimedDoor tDoor(1);
  tDoor.lock(); 
  DoorTimerAdapter adapter(tDoor);
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(0, &adapter));
}
