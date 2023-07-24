#include <Solution_algorithm.h>
#include <init.h>

/*计算偏移量*/
static float i;                                    //计算偏移量时的循环次数
static float ax_offset = 0, ay_offset = 0;         //x,y轴的加速度偏移量
static float gx_offset = 0, gy_offset = 0;         //x,y轴的角速度偏移量
static float yaw_offset = 0;
static float mag_offset_x = 0.0, mag_offset_y = 0.0, mag_offset_z = 0.0; //x,y,z轴的地磁偏移量

/*参数*/
static float rad2deg = 57.29578;                   //弧度到角度的换算系数
static float roll_v = 0, pitch_v = 0, yaw_v = 0;              //换算到x,y,z轴上的角速度
static float gravity = 9.8;
static float alpha = 0.5;  // 互补滤波系数
static float beta = 0.1;   // 倾斜补偿系数

/*定义微分时间*/
static float now = 0, lasttime = 0, dt = 0;        //定义微分时间

/*三个状态，先验状态，观测状态，最优估计状态*/
static float gyro_roll = 0, gyro_pitch = 0, gyro_yaw = 0;        //陀螺仪积分计算出的角度，先验状态
static float acc_roll = 0, acc_pitch = 0, acc_yaw = 0;          //加速度计观测出的角度，观测状态
static float k_roll = 0, k_pitch = 0, t_yaw = 0;              //卡尔曼滤波后估计出最优角度，最优估计状态
static float tilt_yaw = 0;  //倾斜补偿后的yaw角数据
static float yaw_buf[WINDOW_SIZE];

/*误差协方差矩阵P*/
static float e_P[2][2] ={{1,0},{0,1}};             //误差协方差矩阵，这里的e_P既是先验估计的P，也是最后更新的P

/*卡尔曼增益K*/
static float k_k[2][2] ={{0,0},{0,0}};             //这里的卡尔曼增益矩阵K是一个2X2的方阵

static float sliding_window_filter(float *buf, float new_data) 
{
    int i;
    float sum = 0.0;

    for(i = (WINDOW_SIZE - 1); i>0; i--)
    {
        buf[i - 1] = buf[i];
    }

    // 将新数据存入缓存区
    buf[WINDOW_SIZE - 1] = new_data;

    // 计算缓存区中所有数据的和
    for (i = 0; i < WINDOW_SIZE; i++)
    {
        sum += buf[i];
    }

    // 返回平均值
    return sum / WINDOW_SIZE;
}

void Caculate_RollPitch_by_Kalman()
{
    /*计算微分时间*/
    now = millis();                           //当前时间(ms)
    dt = (now - lasttime) / 1000.0;           //微分时间(s)
    lasttime = now;                           //上一次采样时间(ms)

    /*获取角速度和加速度 */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);//获取加速度、角速度、温度

    /*step1:计算先验状态*/
    /*计算x,y轴上的角速度*/
    roll_v = (g.gyro.x-gx_offset) + ((sin(k_pitch)*sin(k_roll))/cos(k_pitch))*(g.gyro.y-gy_offset) + ((sin(k_pitch)*cos(k_roll))/cos(k_pitch))*g.gyro.z;//roll轴的角速度
    pitch_v = cos(k_roll)*(g.gyro.y-gy_offset) - sin(k_roll)*g.gyro.z;//pitch轴的角速度
    gyro_roll = k_roll + dt*roll_v;//先验roll角度
    gyro_pitch = k_pitch + dt*pitch_v;//先验pitch角度

    /*step2:计算先验误差协方差矩阵P*/
    e_P[0][0] = e_P[0][0] + 0.0025;//这里的Q矩阵是一个对角阵且对角元均为0.0025
    e_P[0][1] = e_P[0][1] + 0;
    e_P[1][0] = e_P[1][0] + 0;
    e_P[1][1] = e_P[1][1] + 0.0025;

    /*step3:更新卡尔曼增益K*/
    k_k[0][0] = e_P[0][0]/(e_P[0][0]+0.3);
    k_k[0][1] = 0;
    k_k[1][0] = 0;
    k_k[1][1] = e_P[1][1]/(e_P[1][1]+0.3);

    /*step4:计算最优估计状态*/
    /*观测状态*/
    //roll角度
    acc_roll = atan((a.acceleration.y - ay_offset) / (a.acceleration.z)) * rad2deg;
    //pitch角度
    acc_pitch = -1*atan((a.acceleration.x - ax_offset) / sqrt(sq(a.acceleration.y - ay_offset) + sq(a.acceleration.z))) * rad2deg;

    /*最优估计状态*/
    k_roll = gyro_roll + k_k[0][0]*(acc_roll - gyro_roll);
    k_pitch = gyro_pitch + k_k[1][1]*(acc_pitch - gyro_pitch);

    /*step5:更新协方差矩阵P*/
    e_P[0][0] = (1 - k_k[0][0])*e_P[0][0];
    e_P[0][1] = 0;
    e_P[1][0] = 0;
    e_P[1][1] = (1 - k_k[1][1])*e_P[1][1];
}

float caculate_Yaw(float roll,float pitch,bool init_flag)
{
    float rollRad = 0, pitchRad = 0;
    float xh = 0,yh = 0;
    sensors_event_t event; 
    mag.getEvent(&event);

    if(init_flag)
    {
        /*获取角速度和加速度 */
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        rollRad = asin(-(a.acceleration.x/gravity));
        pitchRad = atan(a.acceleration.y/a.acceleration.z);
    }
    else
    {
        rollRad = roll/rad2deg;
        pitchRad = pitch/rad2deg;
    }

    xh = event.magnetic.x * cos(pitchRad) + event.magnetic.y * sin(pitchRad) * sin(rollRad) + event.magnetic.z * sin(pitchRad) * cos(rollRad);
    yh = event.magnetic.y * cos(rollRad) - event.magnetic.z * sin(rollRad);

    // Calculate tilt compensated heading
    float tiltCompensatedHeading = atan2(yh, xh);

    // Correct for when signs are reversed.
    if(tiltCompensatedHeading < 0)
        tiltCompensatedHeading += 2*PI;
        
    // Check for wrap due to addition of declination.
    if(tiltCompensatedHeading > 2*PI)
        tiltCompensatedHeading -= 2*PI;
    
    // Convert radians to degrees for readability.
    float headingDegrees = tiltCompensatedHeading * 180/M_PI;

    return headingDegrees;
}

void offset_init()
{
    for (i = 1; i <= 2000; i++) {
        sensors_event_t a, g, m, temp;
        sensors_event_t event; 
        mpu.getEvent(&a, &g, &temp);//获取加速度、角速度、温度
        mag.getEvent(&m);
        ax_offset = ax_offset + a.acceleration.x;
        ay_offset = ay_offset + a.acceleration.y;
        gx_offset = gx_offset + g.gyro.x;
        gy_offset = gy_offset + g.gyro.y;

        mag_offset_x += m.magnetic.x;
        mag_offset_y += m.magnetic.y;
        mag_offset_z += m.magnetic.z;
    }
    ax_offset = ax_offset / 2000;
    ay_offset = ay_offset / 2000;
    gx_offset = gx_offset / 2000;
    gy_offset = gy_offset / 2000;

    mag_offset_x /= 2000;
    mag_offset_y /= 2000;
    mag_offset_z /= 2000;

    for(i = 0; i < 100; i++)
    {
        yaw_offset += caculate_Yaw(k_roll, k_pitch, true);
    }

    yaw_offset /= 100;
}

void get_Euler_angle(float* roll, float* pitch, float* yaw)
{
    Caculate_RollPitch_by_Kalman();
    tilt_yaw = caculate_Yaw(k_roll, k_pitch, false);
    tilt_yaw = tilt_yaw >= yaw_offset ? (tilt_yaw - yaw_offset) : ((360.0 - yaw_offset) + tilt_yaw);

    if(tilt_yaw > 180.0)
    {
        tilt_yaw -= 360.0;
    }
    t_yaw = sliding_window_filter(yaw_buf, tilt_yaw);

    *roll = k_roll;
    *pitch = k_pitch;
    *yaw = t_yaw;
}