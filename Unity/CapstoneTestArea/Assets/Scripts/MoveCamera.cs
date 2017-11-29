using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveCamera : MonoBehaviour {
    [SerializeField] private float m_moveSpeed;
    [SerializeField] private float m_rotSpeed;
    private Vector3 m_rot;

    // Update is called once per frame
    void Update()
    {
        float h = (Input.GetKey(KeyCode.D) ? 1.0f : 0.0f) + (Input.GetKey(KeyCode.A) ? -1.0f : 0.0f);
        float v = (Input.GetKey(KeyCode.W) ? 1.0f : 0.0f) + (Input.GetKey(KeyCode.S) ? -1.0f : 0.0f);
        float u = (Input.GetKey(KeyCode.UpArrow) ? 1.0f : 0.0f) + (Input.GetKey(KeyCode.DownArrow) ? -1.0f : 0.0f);
        Vector3 move = ((h * transform.right) + (v * transform.forward) + (u * transform.up)).normalized * m_moveSpeed * Time.deltaTime;
        
        transform.position += move;

        if (Input.GetKey(KeyCode.Mouse1))
        {
            m_rot.y += Input.GetAxis("Mouse X") * Time.deltaTime * m_rotSpeed;
            m_rot.x -= Input.GetAxis("Mouse Y") * Time.deltaTime * m_rotSpeed;

            transform.rotation = Quaternion.Euler(m_rot);
        }
    }
}
