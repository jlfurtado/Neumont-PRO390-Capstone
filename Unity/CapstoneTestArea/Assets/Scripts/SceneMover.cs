using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneMover : MonoBehaviour {

    public void MoveToTitle()
    {
        SceneManager.LoadScene(Scenes.TITLE);
    }

    public void MoveToDargon()
    {
        SceneManager.LoadScene(Scenes.DARGON);
    }

    public void MoveToForest()
    {
        SceneManager.LoadScene(Scenes.FOREST);
    }

    public void MoveToLaptop()
    {
        SceneManager.LoadScene(Scenes.LAPTOP);
    }

    public void MoveToTeapot()
    {
        SceneManager.LoadScene(Scenes.TEAPOT);
    }

    public void Exit()
    {
        Application.Quit();
    }

    private void KeepTagged(string tag)
    {
        GameObject tagged = GameObject.FindGameObjectWithTag(tag);
        if (tagged != null) { DontDestroyOnLoad(tagged); }
    }

    private void DontKeepTagged(string tag)
    {
        GameObject tagged = GameObject.FindGameObjectWithTag(tag);
        if (tagged != null) { Destroy(tagged); }
    }
}
