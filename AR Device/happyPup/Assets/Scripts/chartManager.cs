using UnityEngine;
using XCharts.Runtime;

public class chartManager : MonoBehaviour
{
    public BarChart barChart;
    public mqttReceiverList _eventSender;
    public string tagOfTheMQTTReceiver = ""; //tag of the mqttReceiver, add from the Inspector panel
    int count = 0; //simple counter for the data
    public string topic;

    void Start()
    {
        if (GameObject.FindGameObjectsWithTag(tagOfTheMQTTReceiver).Length > 0)
        {
            _eventSender = GameObject.FindGameObjectsWithTag(tagOfTheMQTTReceiver)[0].gameObject.GetComponent<mqttReceiverList>();
        }
        else
        {
            Debug.LogError("At least one GameObject with mqttReceiver component and Tag == tagOfTheMQTTReceiver needs to be provided");
        }
        _eventSender.OnMessageArrived += OnMessageArrivedHandler;
    }

    private void OnMessageArrivedHandler(mqttObj mqttObject) //the mqttObj is defined in the mqttReceiverList.cs
    {
        //We need to check the topic of the message to know where to use it 
        if (mqttObject.topic == topic)
        {
            double num = float.Parse(mqttObject.msg); //parse the string as a float
            barChart.UpdateData(0, 0, num); // first value refer to the serieIndex, serieName can be used instead

        }

    }
}
