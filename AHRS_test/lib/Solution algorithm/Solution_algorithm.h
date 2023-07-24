#ifndef _SOLUTION_ALGORITHM_
#define _SOLUTION_ALGORITHM_

#define WINDOW_SIZE 5

extern void Caculate_RollPitch_by_Kalman();

extern float caculate_Yaw(float roll,float pitch,bool init_flag);

extern void offset_init();

extern void get_Euler_angle(float* roll, float* pitch, float* yaw);

#endif //_SOLUTION_ALGORITHM_