using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using static UnityEngine.Mesh;

public class gamemanager : MonoBehaviour
{
    public UIController uicontroller;

    public UDP_Server sender = new UDP_Server();

    public GameObject Cube;
    public float smoothSpeed = 5f;

    private string datafromnode;
    private string[] data_split;

    void Start()
    {

    }

    void Update()
    {
        float angle_roll = 0, angle_pitch = 0, angle_yaw = 0;

        if (sender.newdatahereboys)
        {
            datafromnode = sender.getLatestUDPPacket();

            data_split = datafromnode.Split(",");

            if (data_split.Length >= 3)
            {
                uicontroller.roll_text.text = data_split[0].Replace(" ", "");
                uicontroller.pitch_text.text = data_split[1].Replace(" ", "");
                uicontroller.yaw_text.text = data_split[2].Replace(" ", "");
                angle_roll = float.Parse(data_split[0].Replace(" ", ""));
                angle_pitch = float.Parse(data_split[1].Replace(" ", ""));
                angle_yaw = float.Parse(data_split[2].Replace(" ", ""));
            }
        }

        if (uicontroller.checkBox_on)
        {
            Quaternion currentRotation = Cube.transform.localRotation;

            Quaternion targetRotation = Quaternion.Euler(-angle_pitch, -angle_yaw, angle_roll);

            float t = uicontroller.smoothSpeed.value * Time.deltaTime;
            Quaternion newRotation = Quaternion.Slerp(currentRotation, targetRotation, t);

            Cube.transform.localRotation = newRotation;
        }
        else
        {
            Cube.transform.localRotation = Quaternion.Euler(-angle_pitch, -angle_yaw, angle_roll);//指定角度，相对于父物体旋转
        }
    }

    public void UDP_start_init()
    {
        sender.init(uicontroller.RemoteIP.text, int.Parse(uicontroller.Remoteport.text), 25666);
        sender.sendString("Hello from Start. " + Time.realtimeSinceStartup);
    }

    public void OnDisable()
    {
        sender.ClosePorts();
    }

    public void OnApplicationQuit()
    {
        sender.ClosePorts();
    }

}