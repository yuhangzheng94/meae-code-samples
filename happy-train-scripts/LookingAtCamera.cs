using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LookingAtCamera : MonoBehaviour
{
    // assigned in Unity editor
    public GameObject target;   
    public GameObject thisObject;

    // assigned in Start()
    private Transform targetTransform;
    private Transform thisTransform;

    void Start()
    {
        targetTransform = target.GetComponent<Transform>();
        thisTransform = thisObject.GetComponent<Transform>();
    }
    
    void Update()
    {
        // Rotate the camera every frame so it keeps looking at the target (player)
        thisTransform.LookAt(targetTransform);
    }
}
