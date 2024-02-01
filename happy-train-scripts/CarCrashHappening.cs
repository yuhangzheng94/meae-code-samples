using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.Video;

public class CarCrashHappening : MonoBehaviour
{
    public GameObject videoPlayerObj;
    private VideoPlayer videoPlayer;
    public GameObject crashSoundObj;
    private AudioSource crashAudioSource;
    public GameObject ambienceObj;
    private AudioSource ambienceAudioSource;
    public bool crashSoundPlayed;

    public GameObject[] lights;
    public bool carCrashIsDone;
    public GameObject controlRoomLightObj;
    private Light controlRoomLightCmpnt;
    public VideoClip glitchVideoClip;
    public AudioClip horrorBgmClip;
    public bool horrorBgmPlayed;
    
    void Start()
    {
        videoPlayer = videoPlayerObj.GetComponent<VideoPlayer>();
        crashAudioSource = crashSoundObj.GetComponent<AudioSource>();
        ambienceAudioSource = ambienceObj.GetComponent<AudioSource>();
        controlRoomLightCmpnt = controlRoomLightObj.GetComponent<Light>();
    }

    public void PlayHorrorBGM()
    {
        if (!horrorBgmPlayed)
        {
            ambienceAudioSource.clip = horrorBgmClip;
            ambienceAudioSource.Play();
            horrorBgmPlayed = true;
        }
    }
    
    void Update()
    {
        if ((!carCrashIsDone) && (videoPlayer.isPaused))
        {
            // pause white noise
            ambienceAudioSource.Stop();
            
            // play car crash sound
            if ((!crashSoundPlayed) && (!crashAudioSource.isPlaying))
            {
                crashAudioSource.Play();
                crashSoundPlayed = true;
                PlayHorrorBGM();
            }

            // turn off all the lights
            foreach (GameObject light in lights)
            {
                light.SetActive(false);
            }
            
            // change the control room light to red
            controlRoomLightCmpnt.color = Color.red;
            controlRoomLightCmpnt.intensity = 2.5f;
            
            // play glitch video
            videoPlayer.clip = glitchVideoClip;
            videoPlayer.Play();
            
            carCrashIsDone = true;
        }
    }
}
