#include <Madgwick.h>
#include <Fusion.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <init.h>

#define SAMPLE_RATE (260)
#define SAMPLE_PERIOD (0.0476f)
#define YAW_OFFSET_LIST_MAX (200)

// Define calibration (replace with actual calibration data if available)
const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

// Set AHRS algorithm settings
const FusionAhrsSettings settings = {
            .convention = FusionConventionNwu,
            .gain = 0.5f,
            .accelerationRejection = 0,
            .magneticRejection = 0,
            .rejectionTimeout = 0, /* 5 seconds */
};

// Initialise algorithms
FusionOffset offset;
FusionAhrs ahrs;

float yaw_offset_list[YAW_OFFSET_LIST_MAX];
float yaw_offset = 0;
int yaw_offset_list_count = 0;

void Madgwick_init()
{
    FusionOffsetInitialise(&offset, SAMPLE_RATE);
    FusionAhrsInitialise(&ahrs);

    // Set AHRS algorithm settings
    FusionAhrsSetSettings(&ahrs, &settings);
}

void get_Euler_angle_by_madgwick(float* roll, float* pitch, float* yaw)
{
    /*获取角速度和加速度 */
    sensors_event_t a, g, m, temp;
    mpu.getEvent(&a, &g, &temp);//获取加速度、角速度、温度
    mag.getEvent(&m);
    // Acquire latest sensor data
    const clock_t timestamp = millis(); 
    FusionVector gyroscope = {g.gyro.x, g.gyro.y, g.gyro.z}; 
    FusionVector accelerometer = {a.acceleration.x, a.acceleration.y, a.acceleration.z};
    FusionVector magnetometer = {m.magnetic.x, m.magnetic.y, m.magnetic.z}; 

    // Apply calibration
    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    static clock_t previousTimestamp;
    const float deltaTime = (float) (timestamp - previousTimestamp) / 100.0f;
    previousTimestamp = timestamp;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

    // Print algorithm outputs
    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
    const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

#if 1
    if(yaw_offset_list_count >= YAW_OFFSET_LIST_MAX)
    {
        *roll = euler.angle.roll;
        *pitch = euler.angle.pitch;
        if(yaw_offset > 0)
        {
            *yaw = euler.angle.yaw > 0 ? (euler.angle.yaw - yaw_offset) : ((360 - yaw_offset) + euler.angle.yaw);
        }
        else
        {
            *yaw = euler.angle.yaw < 0 ? (euler.angle.yaw - yaw_offset) : ((360 + yaw_offset) - euler.angle.yaw);
        }
    }
    else
    {
        int i = 0;
        yaw_offset_list[yaw_offset_list_count++] = euler.angle.yaw;
        if(yaw_offset_list_count == YAW_OFFSET_LIST_MAX)
        {
            for(i = yaw_offset_list_count/2; i < yaw_offset_list_count; i++)
            {
                yaw_offset += yaw_offset_list[i];
            }
            yaw_offset /= (yaw_offset_list_count/2); 
        }
    }
#else
    *roll = euler.angle.roll;
    *pitch = euler.angle.pitch;
    *yaw = euler.angle.yaw;
#endif

    // Serial.print("roll: ");
    // Serial.print(*roll);
    // Serial.print(",");
    // Serial.print("pitch: ");
    // Serial.print(*pitch);
    // Serial.print(",");
    // Serial.print("yaw: ");
    // Serial.println(*yaw);
}

void get_Euler_angle_by_madgwick_simple(float* roll, float* pitch, float* yaw) 
{
    /*获取角速度和加速度 */
    sensors_event_t a, g, m, temp;
    mpu.getEvent(&a, &g, &temp);//获取加速度、角速度、温度
    mag.getEvent(&m);
    // this loop should repeat each time new gyroscope data is available
    FusionVector gyroscope = {g.gyro.x, g.gyro.y, g.gyro.z}; 
    FusionVector accelerometer = {a.acceleration.x, a.acceleration.y, a.acceleration.z};

    FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);

    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

    *roll = euler.angle.roll;
    *pitch = euler.angle.pitch;
    *yaw = euler.angle.yaw;

    Serial.print("roll: ");
    Serial.print(*roll);
    Serial.print(",");
    Serial.print("pitch: ");
    Serial.print(*pitch);
    Serial.print(",");
    Serial.print("yaw: ");
    Serial.println(*yaw);
}