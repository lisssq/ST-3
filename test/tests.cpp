// Copyright 2026 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

// Мок для TimerClient
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Мок для Door
class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

// Фикстура для тестов TimedDoor
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

// --- ТЕСТЫ ---

// 1. Начальное состояние двери (закрыта)
TEST_F(TimedDoorTest, InitialStateClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

// 2. Дверь открывается
TEST_F(TimedDoorTest, UnlockWorks) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

// 3. Дверь закрывается
TEST_F(TimedDoorTest, LockWorks) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// 4. Проверка получения значения таймаута
TEST_F(TimedDoorTest, CorrectTimeoutValue) {
  EXPECT_EQ(door->getTimeOut(), 5);
}

// 5. Исключение при открытой двери
TEST_F(TimedDoorTest, ThrowExceptionIfOpen) {
  door->unlock();
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

// 6. Отсутствие исключения при закрытой двери
TEST_F(TimedDoorTest, NoExceptionIfClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

// 7. Тест адаптера: вызывает ли он throwState у двери
TEST(AdapterTest, AdapterCallsThrowState) {
  TimedDoor tDoor(1);
  tDoor.unlock();
  DoorTimerAdapter adapter(tDoor);
  // Если адаптер работает, этот вызов пробросит исключение от двери
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// 8. Тест таймера: вызывает ли он метод Timeout клиента (Mock)
TEST(TimerTest, TimerTriggersClient) {
  MockTimerClient mockClient;
  Timer timer;
  // Ожидаем, что метод Timeout будет вызван ровно 1 раз
  EXPECT_CALL(mockClient, Timeout()).Times(1);
  timer.tregister(0, &mockClient);
}

// 9. Тест MockDoor: проверка вызова unlock
TEST(MockTest, MockDoorUnlockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, unlock()).Times(1);
  mDoor.unlock();
}

// 10. Комбинированный тест: работа через Timer и Adapter
TEST(IntegrationTest, FullSequenceException) {
  TimedDoor tDoor(3);
  tDoor.unlock();
  DoorTimerAdapter adapter(tDoor);
  Timer timer;
  
  // Проверяем, что вся цепочка Timer -> Adapter -> Door приводит к throw
  EXPECT_THROW(timer.tregister(0, &adapter), std::runtime_error);
}

// 11. Проверка MockDoor: возвращаемое значение isDoorOpened
TEST(MockTest, MockDoorReturnsValue) {
  MockDoor mDoor;
  // Настраиваем мок так, чтобы он вернул true при вызове
  EXPECT_CALL(mDoor, isDoorOpened()).WillOnce(::testing::Return(true));
  EXPECT_TRUE(mDoor.isDoorOpened());
}

// 12. Проверка MockDoor: вызов метода lock
TEST(MockTest, MockDoorLockCall) {
  MockDoor mDoor;
  EXPECT_CALL(mDoor, lock()).Times(1);
  mDoor.lock();
}

// 13. Последовательное переключение состояний (Open -> Close -> Open)
TEST_F(TimedDoorTest, StateToggleConsistency) {
  door->unlock(); // open
  door->lock();   // closed
  EXPECT_FALSE(door->isDoorOpened());
  door->unlock(); // open again
  EXPECT_TRUE(door->isDoorOpened());
}

// 14. Адаптер НЕ должен кидать исключение, если дверь успели закрыть до таймаута
TEST(AdapterTest, NoThrowIfClosedInTime) {
  TimedDoor tDoor(5);
  tDoor.unlock();
  tDoor.lock(); // Закрыли дверь
  DoorTimerAdapter adapter(tDoor);
  
  // Адаптер срабатывает, но дверь уже закрыта — исключения быть не должно
  EXPECT_NO_THROW(adapter.Timeout());
}

// 15. Проверка конструктора с другим значением таймаута (граничное значение)
TEST(TimedDoorConstructor, ZeroTimeout) {
  TimedDoor tDoor(0);
  EXPECT_EQ(tDoor.getTimeOut(), 0);
}

// 16. Интеграционный тест: Таймер срабатывает для закрытой двери
TEST(IntegrationTest, FullSequenceNoException) {
  TimedDoor tDoor(1);
  tDoor.lock(); // Дверь закрыта
  DoorTimerAdapter adapter(tDoor);
  Timer timer;

  // Цепочка Timer -> Adapter -> Door (Closed) -> No Throw
  EXPECT_NO_THROW(timer.tregister(0, &adapter));
}