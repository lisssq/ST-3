// Copyright 2026 GHA Test Team
#include "TimedDoor.h"
#include <unistd.h>
#include <stdexcept>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

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

void Timer::tregister(int timeout, TimerClient* cl) {
  client = cl;
  sleep(timeout);
  client->Timeout();
}

void Timer::sleep(int timeout) {
}