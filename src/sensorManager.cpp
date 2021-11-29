#include "sensorManager.h"

namespace Pulu {
    sensorManager::sensorManager(Sensors::config config) {
        #ifdef fakeLight
            lightSensors = {
                FakeLightSensor()
            };
        #else
            lightSensors = {
                new LTR329ALS(config.light[0].i2c, config.light[0].address)
            };
        #endif
        moistureSensors = {
            FakeMoistSensor(),
            FakeMoistSensor(),
            FakeMoistSensor(),
            FakeMoistSensor()
        };
        #ifdef fakeTemperature
            temperatureSensors = {
                FakeTemperatureSensor(),
                FakeTemperatureSensor()
            };
        #else
            for(uint8_t i = 0; i<temperatureSensors.size(); i++) {
                temperatureSensors[i] = new TCN75(
                        config.temperature[i].i2c,
                        config.temperature[i].address
                    );
            };
        #endif
        batterySensor = Battery();
    };

    bool sensorManager::init() {
        // TODO: init all sensors when not fake
        sensorManager_DEBUG("init: SUCCESS");
        return true;
    }

    sensorValues sensorManager::values() {
        wake_all();
        sensorValues values;
        sensorManager_DEBUG("start reading values");
        for(uint8_t i = 0; i<lightSensors.size(); i++) {
            #ifdef fakeLight
                values.light[i] = lightSensors[i].lightLevel();
            #else
                values.light[i] = lightSensors[i]->readLux();
            #endif
            sensorManager_DEBUG("light[%d]: %d", i, values.light[i]);
        }
        for(uint8_t i = 0; i<moistureSensors.size(); i++) {
            values.moisture[i] = moistureSensors[i].moisture();
            sensorManager_DEBUG("moisture[%d]: %d", i, values.moisture[i]);
        }
        for(uint8_t i = 0; i<temperatureSensors.size(); i++) {
            #ifdef fakeTemperature
                values.temperature[i] = temperatureSensors[i].temperature();
            #else
                values.temperature[i] = temperatureSensors[i]->temperature();
            #endif
            sensorManager_DEBUG("temperature[%d]: %d", i, values.temperature[i]);
        }
        values.battery = batterySensor.voltage();
        sensorManager_DEBUG("battery: %f", values.battery);
        sleep_all();
        return values;
    }

    void sensorManager::sleep_all() {
        #ifndef fakeLight
            for(uint8_t i = 0; i<lightSensors.size(); i++) {
                lightSensors[i]->sleep();
            }
        #endif
        #ifndef fakeTemperature
            for(uint8_t i = 0; i<temperatureSensors.size(); i++) {
                temperatureSensors[i]->sleep();
            }
        #endif
    }
    void sensorManager::wake_all() {
        #ifndef fakeLight
            for(uint8_t i = 0; i<lightSensors.size(); i++) {
                lightSensors[i]->wake();
            }
        #endif
        #ifndef fakeTemperature
            for(uint8_t i = 0; i<temperatureSensors.size(); i++) {
                temperatureSensors[i]->wake();
            }
        #endif
        // give time to sensors to start measuring after wake up
        ThisThread::sleep_for(200ms);
    }
};