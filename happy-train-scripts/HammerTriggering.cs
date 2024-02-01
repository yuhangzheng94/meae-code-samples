using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HammerTriggering : MonoBehaviour
{
    public const int CLOSED = 0;
    public const int OPENING = 1;
    public const int OPENED = 2;
    public struct ControlRoomDoor {
        public GameObject gameObject;
        public int state = CLOSED;
        public float startTime = -1.0f;
    }

    public GameObject crackPrefab;
    public GameObject crackPrefab1;
    public GameObject[] displays;

    public struct TrainDoor {
        public GameObject leftDoor;
        public GameObject rightDoor;
        public int state = CLOSED;
        public float startTime = -1.0f;
    }

    public ControlRoomDoor controlRoomDoor = new ControlRoomDoor();
    public TrainDoor trainDoor = new TrainDoor();
    
    void Update()
    {
        if (controlRoomDoor.state == OPENING) {
            OpenControlRoomDoor();
        }

        if (trainDoor.startTime == OPENING)
        {
            OpenTrainDoor();
        }
    }
    
    private void OnCollisionEnter(Collision collision)
    {
        // case 0: the hammer collides with LED screens
        if (collision.gameObject.CompareTag("LEDScreen"))
        {
            // stop playing video
            foreach (GameObject display in displays)
            {
                display.SetActive(false);
            }
            
            // display crack image
            results = CalculateCrackImage(collision);
            Instantiate(crackPrefab, results[0], results[1]);
        
        // case 1: collides with control room door window
        } else if (collision.gameObject.CompareTag("ControlRoomGlass"))
        {
            if (controlRoomDoor.state == CLOSED)
            {
                // display crack image
                results = CalculateCrackImage(collision);
                GameObject crack = Instantiate(crackPrefab1, results[0], results[1]);
                crack.transform.SetParent(controlRoomDoor.gameObject.transform);
                controlRoomDoor.state = OPENING;
            }
        
        // case 2: collides with emergent exit release
        } else if (collision.gameObject.CompareTag("ReleaseGlass"))
        {
            if (trainDoor.state == CLOSED)
            {
                // display crack image
                results = CalculateCrackImage(collision);
                Instantiate(crackPrefab, results[0], results[1]);
                trainDoor.state = OPENING;
            }
        }
    }

    public (Vector3 position, Quaternion rotation) CalculateCrackImage(Collision collision) {
        ContactPoint contact = collision.contacts[0];
        Quaternion rotation = Quaternion.FromToRotation(Vector3.up, contact.normal);
        Vector3 position = contact.point;
        Transform t = GetComponent<Transform>();
        Vector3 positionHammer = t.position;
        position = Vector3.Lerp(position, positionHammer, 0.5f);

        return (position, rotation)
    }

    public void OpenTrainDoor()
    {
        if (trainDoor.startTime < 0)
        {
            trainDoor.startTime = Time.time;
        } else if (Time.time - trainDoor.startTime > 10.0f)
        {
            if (trainDoor.state == OPENING)
            {
                Transform t1 = trainDoor.leftDoor.GetComponent<Transform>();
                Transform t2 = trainDoor.rightDoor.GetComponent<Transform>();
                Vector3 destination1 = new Vector3(15.4200001f, 1.41299999f, 8.72000027f);
                Vector3 destination2 = new Vector3(17.3400002f, 1.42200005f, 5.48999977f);

                Vector3 pos1 = Vector3.Lerp(t1.position, destination1, 0.005f);
                t1.position = pos1;
                Vector3 pos2 = Vector3.Lerp(t2.position, destination2, 0.005f);
                t2.position = pos2;

                if (t1.position.Equals(destination1))
                {
                    trainDoor.state = OPENED;
                }
            }
        }
    }
    
    public void OpenControlRoomDoor()
    {
        if (controlRoomDoor.startTime < 0)
        {
            controlRoomDoor.startTime = Time.time;
        } else if (Time.time - controlRoomDoor.startTime > 10.0f)
        {
            if (controlRoomDoor.state != OPENED)
            {
                Transform t = controlRoomDoor.gameObject.GetComponent<Transform>();
                Vector3 destination = new Vector3(20.13f, 1.61f, 10.07f);
                Vector3 pos = Vector3.Lerp(t.position, destination, 0.005f);
                t.position = pos;

                if (t.position.Equals(destination))
                {
                    controlRoomDoor.state == OPENED;
                }
            }
        }
    }

