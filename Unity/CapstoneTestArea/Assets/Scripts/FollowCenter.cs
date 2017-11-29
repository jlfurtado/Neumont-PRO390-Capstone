using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FollowCenter : MonoBehaviour {
    [SerializeField] private GameObject[] m_followed;

    [SerializeField] private float shakeStrength = 10.0f;
    [SerializeField] private float backBaseAmount;
    [SerializeField] private float upBaseAmount;
    [SerializeField] private float speed;
    [SerializeField] private float rotateSpeed;
    [SerializeField] private float slowdownRadius;
    [SerializeField] private float offsetMultiplier;
    private bool shaking = false;
    private float shakeTimer;
    private Vector3 vel;

    void FixedUpdate()
    {
        Vector3 center = CalcCenter();
        Vector3 avgForward = CalcForward().normalized;
        float avgDist = CalcAvgDist(center);

        // hack *= -1.0f
        Vector3 backVec = avgForward * backBaseAmount + Vector3.up * upBaseAmount;
        Vector3 toPos = center + (backVec * offsetMultiplier * avgDist);
        Vector3 move = (toPos - transform.position);
        float dist = move.magnitude;
        if (dist > 0.0f)
        {
            float rampSpeed = speed * (dist / slowdownRadius);
            float clipSpeed = Mathf.Min(rampSpeed, speed);
            vel = (clipSpeed / dist) * move;
            transform.position += vel * Time.deltaTime;

            if (shaking)
            {
                shakeTimer -= Time.deltaTime;
                if (shakeTimer <= 0.0f) { EndShake(); }
                transform.position += Random.onUnitSphere * Time.deltaTime * shakeStrength;
            }
        }

        transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation((center - transform.position).normalized), rotateSpeed * Time.deltaTime);
    }

    public void BeginShake(float shakeTime, float strength)
    {
        shakeStrength = strength;
        shakeTimer = shakeTime;
        shaking = true;
    }

    public void EndShake()
    {
        shakeTimer = 0.0f;
        shaking = false;
    }

    private Vector3 CalcCenter()
    { 
        Vector3 center = m_followed[0].transform.position; // must have at least one!
        for (int i = 1; i < m_followed.Length; ++i)
        {
            center += m_followed[i].transform.position;
        }

        return center / m_followed.Length;
    }

    private Vector3 CalcForward()
    {
        Vector3 forward = m_followed[0].transform.forward; // must have at least one!
        for (int i = 1; i < m_followed.Length; ++i)
        {
            forward += m_followed[i].transform.forward;
        }

        return forward / m_followed.Length;
    }

    private float CalcAvgDist(Vector3 center)
    {
        float avgDist = Vector3.Distance(m_followed[0].transform.position, center); // must have at least one!

        for (int i = 1; i < m_followed.Length; ++i)
        {
            avgDist += Vector3.Distance(m_followed[i].transform.position, center);
        }

        return avgDist / m_followed.Length;
    }
}
