#include <unity.h>
#include "SafetySystem.h"

SafetySystem* safetySystem;

void setUp(void) {
    safetySystem = new SafetySystem(2000); // 2 second watchdog timeout
    safetySystem->initialize();
}

void tearDown(void) {
    delete safetySystem;
    safetySystem = nullptr;
}

void test_safety_system_initialization(void) {
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    TEST_ASSERT_FALSE(safetySystem->isEmergencyStop());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
}

void test_controller_connection(void) {
    safetySystem->setControllerConnected(true);
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
}

void test_controller_disconnection(void) {
    // First connect
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    
    // Then disconnect
    safetySystem->setControllerConnected(false);
    
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
}

void test_arm_system(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, safetySystem->getSystemState());
    TEST_ASSERT_TRUE(safetySystem->isArmed());
    TEST_ASSERT_TRUE(safetySystem->canOperateMotors());
}

void test_disarm_system(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    safetySystem->disarmSystem();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
}

void test_cannot_arm_when_disconnected(void) {
    safetySystem->armSystem(); // Try to arm while disconnected
    
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
}

void test_emergency_stop(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    safetySystem->triggerEmergencyStop();
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, safetySystem->getSystemState());
    TEST_ASSERT_TRUE(safetySystem->isEmergencyStop());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
}

void test_clear_emergency_stop_when_connected(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->triggerEmergencyStop();
    safetySystem->clearEmergencyStop();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isEmergencyStop());
}

void test_clear_emergency_stop_when_disconnected(void) {
    safetySystem->triggerEmergencyStop();
    safetySystem->clearEmergencyStop();
    
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isEmergencyStop());
}

void test_cannot_arm_during_emergency_stop(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->triggerEmergencyStop();
    safetySystem->armSystem(); // Should not work
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
}

void test_watchdog_timeout(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    safetySystem->updateControllerActivity();
    
    // Set time to trigger watchdog (3 seconds > 2 second timeout)
    safetySystem->setCurrentTime(3000);
    safetySystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
}

void test_watchdog_reset_on_activity(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    
    // Start at time 0
    safetySystem->setCurrentTime(0);
    safetySystem->updateControllerActivity();
    
    // Jump to 1.5 seconds (within timeout)
    safetySystem->setCurrentTime(1500);
    safetySystem->updateControllerActivity(); // Reset watchdog
    
    // Jump to 3 seconds total, but only 1.5 since last activity
    safetySystem->setCurrentTime(3000);
    safetySystem->update();
    
    // Should still be connected because activity was updated
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, safetySystem->getSystemState());
}

void test_time_since_last_activity(void) {
    safetySystem->setCurrentTime(1000);
    safetySystem->updateControllerActivity();
    
    safetySystem->setCurrentTime(1500);
    TEST_ASSERT_EQUAL(500, safetySystem->getTimeSinceLastActivity());
}

void test_emergency_stop_overrides_watchdog_clear(void) {
    safetySystem->setControllerConnected(true);
    safetySystem->triggerEmergencyStop();
    
    // Even with controller activity, should remain in emergency stop
    safetySystem->updateControllerActivity();
    safetySystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, safetySystem->getSystemState());
}

void test_connection_updates_activity(void) {
    safetySystem->setCurrentTime(1000);
    safetySystem->setControllerConnected(true);
    
    // Connection should update activity time
    TEST_ASSERT_EQUAL(0, safetySystem->getTimeSinceLastActivity());
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_safety_system_initialization);
    RUN_TEST(test_controller_connection);
    RUN_TEST(test_controller_disconnection);
    RUN_TEST(test_arm_system);
    RUN_TEST(test_disarm_system);
    RUN_TEST(test_cannot_arm_when_disconnected);
    RUN_TEST(test_emergency_stop);
    RUN_TEST(test_clear_emergency_stop_when_connected);
    RUN_TEST(test_clear_emergency_stop_when_disconnected);
    RUN_TEST(test_cannot_arm_during_emergency_stop);
    RUN_TEST(test_watchdog_timeout);
    RUN_TEST(test_watchdog_reset_on_activity);
    RUN_TEST(test_time_since_last_activity);
    RUN_TEST(test_emergency_stop_overrides_watchdog_clear);
    RUN_TEST(test_connection_updates_activity);
    
    return UNITY_END();
}