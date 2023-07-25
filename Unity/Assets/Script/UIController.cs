using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIController : MonoBehaviour
{
    public InputField RemoteIP, Remoteport;
    public Text roll_text, pitch_text, yaw_text;
    public Transform smoothSlider;
    public Toggle toggle;

    [HideInInspector]
    public Slider smoothSpeed;
    [HideInInspector]
    public bool checkBox_on;
    private Text smoothValue;

    private void Awake()
    {
        smoothSpeed = smoothSlider.GetComponent<Slider>();
        smoothValue = smoothSlider.Find("slider_value").GetComponent<Text>();

        RemoteIP.text = "192.168.31.191";
        Remoteport.text = "25666";
        smoothSpeed.value = 30.0f;
        smoothValue.text = smoothSpeed.value.ToString("F2");
    }
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        smoothValue.text = smoothSpeed.value.ToString("F2");
    }

    public void checkBox_on_changed()
    {
        checkBox_on = toggle.isOn;
        smoothSpeed.interactable = toggle.isOn;
    }

}
