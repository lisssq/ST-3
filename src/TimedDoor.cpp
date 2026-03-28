// Copyright 2026 GHA Test Team
#include "TimedDoor.h"
#include <unistd.h>
#include <stdexcept>

// DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  // Адаптер при срабатывании таймера просит дверь проверить состояние
  door.throwState();
}

// TimedDoor
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::unlock() {
  isOpened = true;
}

void TimedDoor::lock() {
  isOpened = false;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Timeout: Door is still open!");
  }
}

// Timer
void Timer::tregister(int timeout, TimerClient* cl) {
  client = cl;
  sleep(timeout);
  client->Timeout();
}

void Timer::sleep(int timeout) {
  // В учебных целях мы не будем реально засыпать на секунды в тестах,
  // чтобы они не шли вечно, но структура метода такая:
  // (Здесь можно было бы использовать usleep или std::this_thread::sleep_for)
}