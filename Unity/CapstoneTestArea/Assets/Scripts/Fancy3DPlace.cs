using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Fancy3DPlace : MonoBehaviour {
    [SerializeField] private CapstoneGenerator m_generator;
    [SerializeField] private Vector3 m_minSpawn;
    [SerializeField] private Vector3 m_maxSpawn;
    [SerializeField] private int m_boxesPerX;
    [SerializeField] private int m_boxesPerY;
    [SerializeField] private int m_boxesPerZ;
    [SerializeField] [Range(0.0f, 1.0f)] private float m_shrinkBox = 1.0f;
    private static System.Random rand = new System.Random();

	// Use this for initialization
	void Start () {
        int spawnCount = m_boxesPerX * m_boxesPerY * m_boxesPerZ;
        Vector3 delta = (m_maxSpawn - m_minSpawn);
        delta = new Vector3(delta.x / m_boxesPerX, delta.y / m_boxesPerY, delta.z / m_boxesPerZ);
        float rmv = ((1.0f - m_shrinkBox) / 2.0f);
        Vector3 lowOffset = delta * rmv;
        Vector3 highOffset = delta - lowOffset;
        for (int i = 0; i < spawnCount; ++i)
        {
            int xLoc = i % m_boxesPerX;
            int yLoc = (i / m_boxesPerX) % m_boxesPerY;
            int zLoc = i / (m_boxesPerX * m_boxesPerY);
            Vector3 xyzDelta = new Vector3(xLoc * delta.x, yLoc * delta.y, zLoc * delta.z);

            MakeAt(RandVec(m_minSpawn + lowOffset + xyzDelta, m_minSpawn + highOffset + xyzDelta), gameObject, "RandomPlacement|" + i);
            //made.transform.GetChild(0).gameObject.AddComponent<MeshCollider>();
        }
    }

    private void MakeAt(Vector3 position, GameObject parent, string name)
    {
        GameObject created = m_generator.MakeInstance();
        created.transform.SetParent(parent.transform);
        created.transform.position = Vector3.zero;
        created.transform.localPosition = position;
        created.name = name;
    }

    public static Vector3 RandVec(Vector3 min, Vector3 max)
    {
        return new Vector3(((float)rand.NextDouble()) * (max.x - min.x) + min.x,
                           ((float)rand.NextDouble()) * (max.y - min.y) + min.y,
                           ((float)rand.NextDouble()) * (max.z - min.z) + min.z);
    }
}
