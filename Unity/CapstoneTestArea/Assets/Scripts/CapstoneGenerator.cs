using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

delegate void SetSRT(Vector3 s, Vector3 r, Vector3 t);

#region Variations

static class Variations
{
    static System.Random rand = new System.Random();

    private static float MyRand(float min, float max)
    {
        return (((float)rand.NextDouble()) * (max - min)) + min;
    }

    public static float ScalarUniform(float v1, float v2)
    {
        float min = Mathf.Min(v1, v2);
        float max = Mathf.Max(v1, v2);
        return MyRand(min, max);
    }

    public static Vector3 VectorComponentUniform(Vector3 v1, Vector3 v2)
	{
        return new Vector3(ScalarUniform(v1.x, v2.x),
                           ScalarUniform(v1.y, v2.y),
                           ScalarUniform(v1.z, v2.z));
    }

    public static Vector3 VectorUniform(Vector3 v1, Vector3 v2)
    {
        float t = ScalarUniform(0.0f, 1.0f);
        return (v1 * t) + ((1.0f - t) * v2);
    }

    public static void TripleVectorUniform(Vector3 l1, Vector3 h1, out Vector3 out1, Vector3 l2, Vector3 h2, out Vector3 out2, Vector3 l3, Vector3 h3, out Vector3 out3)
    {
        float t = ScalarUniform(0.0f, 1.0f);
        out1 = (l1 * t) + ((1.0f - t) * h1);
        out2 = (l2 * t) + ((1.0f - t) * h2);
        out3 = (l3 * t) + ((1.0f - t) * h3);
    }

    public static float ScalarBellApproximation(float v1, float v2, int sampleSize)
    {
        float low = Mathf.Min(v1, v2);
        float max = Mathf.Max(v1, v2);
        float avg = MyRand(low, max);
        for (int i = 1; i < sampleSize; ++i) { avg += MyRand(low, max); }
        return avg / sampleSize;
    }

    public static Vector3 VectorComponentBellApproximation(Vector3 v1, Vector3 v2, int sampleSize)
    {
        return new Vector3(ScalarBellApproximation(v1.x, v2.x, sampleSize),
                           ScalarBellApproximation(v1.y, v2.y, sampleSize),
                           ScalarBellApproximation(v1.z, v2.z, sampleSize));
    }

    public static Vector3 VectorBellApproximation(Vector3 v1, Vector3 v2, int sampleSize)
    {
        float t = ScalarBellApproximation(0.0f, 1.0f, sampleSize);
        return (v1 * t) + ((1.0f - t) * v2);
    }

    public static void TripleVectorBellApproximation(Vector3 l1, Vector3 h1, out Vector3 out1, Vector3 l2, Vector3 h2, out Vector3 out2, Vector3 l3, Vector3 h3, out Vector3 out3, int sampleSize)
    {
        float t = ScalarBellApproximation(0.0f, 1.0f, sampleSize);
        out1 = (l1 * t) + ((1.0f - t) * h1);
        out2 = (l2 * t) + ((1.0f - t) * h2);
        out3 = (l3 * t) + ((1.0f - t) * h3);
    }
}

#endregion 

#region VariationType

enum VariationType
{
    COMPONENT_UNIFORM = 0,
    VECTOR_UNIFORM = 1,
	SMOOTH_UNIFORM = 2,
	COMPONENT_BELL = 3,
	VECTOR_BELL = 4,
	SMOOTH_BELL = 5,
	USED_TYPES = 6,
	SCALAR_UNIFORM = 7,
	SCALAR_BELL = 8,
}

#endregion

#region VariationController

class VariationController
{
    public VariationController(Vector3 ls, Vector3 lr, Vector3 lt, Vector3 hs, Vector3 hr, Vector3 ht, VariationType vt, SetSRT callback)
    {
        m_lowScale = ls;
        m_lowRotation = lr;
        m_lowTranslation = lt;
        m_highScale = hs;
        m_highRotation = hr;
        m_highTranslation = ht;
        m_variationType = vt;
        m_setCallback = callback;
    }

    private Vector3 m_lowScale;
    private Vector3 m_lowTranslation;
    private Vector3 m_lowRotation;
    private Vector3 m_highScale;
    private Vector3 m_highTranslation;
    private Vector3 m_highRotation;
    private VariationType m_variationType;
    private SetSRT m_setCallback;

    public void VaryVectorUniform()
    {
        m_setCallback(Variations.VectorUniform(m_lowScale, m_highScale),
                      Variations.VectorUniform(m_lowRotation, m_highRotation),
                      Variations.VectorUniform(m_lowTranslation, m_highTranslation));
    }

    public void VaryVectorBellApproximation()
    {
        m_setCallback(Variations.VectorBellApproximation(m_lowScale, m_highScale, 10),
                      Variations.VectorBellApproximation(m_lowRotation, m_highRotation, 10),
                      Variations.VectorBellApproximation(m_lowTranslation, m_highTranslation, 10));
    }

    public void VaryComponentUniform()
    {
        m_setCallback(Variations.VectorComponentUniform(m_lowScale, m_highScale),
                      Variations.VectorComponentUniform(m_lowRotation, m_highRotation),
                      Variations.VectorComponentUniform(m_lowTranslation, m_highTranslation));
    }

    public void VaryComponentBellApproximation()
    {
        m_setCallback(Variations.VectorComponentBellApproximation(m_lowScale, m_highScale, 10),
                      Variations.VectorComponentBellApproximation(m_lowRotation, m_highRotation, 10),
                      Variations.VectorComponentBellApproximation(m_lowTranslation, m_highTranslation, 10));
    }

    public void VarySmoothBellApproximation()
    {
        Vector3 s, r, t;
        Variations.TripleVectorBellApproximation(m_lowRotation, m_highRotation, out r,
                                                  m_lowScale, m_highScale, out s,
                                                  m_lowTranslation, m_highTranslation, out t,
                                                  10);
        m_setCallback(s, r, t);
    }

    public void VarySmoothUniform()
    {
        Vector3 s, r, t;
        Variations.TripleVectorUniform(m_lowRotation, m_highRotation, out r,
                                        m_lowScale, m_highScale, out s,
                                        m_lowTranslation, m_highTranslation, out t);
        m_setCallback(s, r, t);
    }

    public void ClearVariations()
    {
        m_setCallback(Vector3.one, Vector3.zero, Vector3.zero);
        //SaveLow();
        //SaveHigh();
        //CallChanged(ASSUME_SINGLE_INSTANCE);
    }
}

#endregion

#region VertexGroup

class VertexGroup
{
    private Vector3 m_scale;
    private Vector3 m_rotation;
    private Vector3 m_pivot;
    private Vector3 m_translation;
    private VariationController m_variation;
    private List<int> m_indices;

    public void SetScaleRotationTranslation(Vector3 scale, Vector3 rotation, Vector3 translation)
    {
        m_scale = scale;
        m_rotation = rotation;
        m_translation = translation;
    }

    public void Clear()
    {
        m_variation.ClearVariations();
        m_indices.Clear();
    }

    public void SetIndices(List<int> indices)
    {
        m_indices = indices;
    }

}

#endregion

#region GameObjectSRT

public class GameObjectSRT
{
    private GameObject m_object = null;

    public void SetObject(GameObject obj)
    {
        m_object = obj;
    }

    public GameObject GetObject()
    {
        return m_object;
    }

    public void SetSRT(Vector3 scale, Vector3 rotation, Vector3 translation)
    {
        m_object.transform.localScale = scale;
        m_object.transform.localRotation = Quaternion.Euler(rotation);
        m_object.transform.position = translation;
    }
}


#endregion

#region CustomIO

static class CustomIO
{
    public static bool LoadBaseMesh(string filePath, GameObjectSRT obj, out float[] verts, out int floatsPerVertex, out List<VertexGroup> vertexGroups, out VariationController objectVariations)
    {
        verts = null;
        floatsPerVertex = 0;
        vertexGroups = null;
        objectVariations = null;

        if (!OpenInputFile(filePath))
        {
            Debug.Log("ERROR: Failed to ReadMeshFromFile! Could not open input file [" + filePath + "]!");
            return false;
        }

        if (!CheckFormatData())
        {
            Debug.Log("ERROR: Failed to ReadMeshFromFile! Failed to CheckFormatData! Ensure that file is of the correct format!\n");
            CloseInputFile();
            return false;
        }

        if (!CheckVersionData())
        {
            Debug.Log("ERROR: Failed to ReadMeshFromFile! Failed to CheckVersionData! You are probably trying to load a file made using an older version of the CustomIO Exporter!\n");
            CloseInputFile();
            return false;
        }

        if (!ReadMeshInfo(obj, out verts, out floatsPerVertex, out vertexGroups, out objectVariations))
        {
            Debug.Log("ERROR: Failed to ReadMeshFromFile! Failed to ReadMeshInfo!\n");
            CloseInputFile();
            return false;
        }

        if (!CloseInputFile())
        {
            Debug.Log("ERROR: Failed to CloseInputFile! Continuing Execution!\n");
        }

        return true;
    }

    private static bool OpenInputFile(string filePath)
    {
        if (!File.Exists(filePath)) { return false; }
        s_reader = new BinaryReader(File.Open(filePath, FileMode.Open));
        return true;
    }

    private static bool CloseInputFile()
    {
        s_reader.Close();
        return true;
    }

    private static bool CheckFormatData()
    {
        char[] readData = s_reader.ReadChars(FORMAT_CHECK_DATA_SIZE);
        return readData != null && readData.ToString().Equals(FORMAT_CHECK_DATA);
    }

    private static bool CheckVersionData()
    {
        int readVersion = s_reader.ReadInt32();
        return (readVersion >= 0) && (readVersion == FORMAT_VERSION);
    }

    private static bool ReadMeshInfo(GameObjectSRT obj, out float[] outVerts, out int floatsPerVertex, out List<VertexGroup> vertexGroups, out VariationController objectVariations)
    {
        outVerts = null;
        floatsPerVertex = 0;
        vertexGroups = null;
        objectVariations = null;

        int numVerts = 0;
        uint numGroups = 0;
        if (!ReadMeshHeader(out numVerts, out floatsPerVertex)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshHeader!\n"); return false; }
        if (!ReadMeshVerts(out outVerts, numVerts * floatsPerVertex)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshVerts!\n"); ClearVertexData(out outVerts); return false; }
        if (!ReadObjectLevelVariations(out objectVariations, obj.SetSRT)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadObjectLevelVariations!\n"); ClearVertexData(out outVerts); return false; }
        if (!ReadVertexGroupHeader(out numGroups)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadVertexGroupHeader!\n"); ClearVertexData(out outVerts); return false; }
        if (!ReadVertexGroupData(out vertexGroups, numGroups)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadVertexGroupData!\n"); ClearVertexData(out outVerts); ClearVertexGroups(vertexGroups); return false; }
        return true;
    }

    private static bool ReadMeshHeader(out int numVerts, out int floatsPerVertex)
    {
        numVerts = s_reader.ReadInt32();
        floatsPerVertex = s_reader.ReadInt32();
        return true;
    }

    private static bool ReadMeshVerts(out float[] outVerts, int numFloats)
    {
        outVerts = new float[numFloats];
        for (int i = 0; i < numFloats; ++i)
        {
            outVerts[i] = s_reader.ReadSingle();
        }

        return true;
    }

    private static bool ReadObjectLevelVariations(out VariationController objectVariations, SetSRT callback)
    {
        return ReadVariationData(out objectVariations, callback);
    }

    private static bool ReadVertexGroupHeader(out uint numGroups)
    {
        numGroups = s_reader.ReadUInt32();
        return true;
    }

    private static bool ReadVertexGroupData(out List<VertexGroup> vertexGroups, uint numGroups)
    {
        vertexGroups = new List<VertexGroup>((int)numGroups);

        for (int i = 0; i < numGroups; ++i)
        {
            VertexGroup vg;
            if (!ReadVertexGroup(out vg)) { Debug.Log("Failed to ReadVertexGroupData! Failed to ReadVertexGroup(" + i + ")!"); return false; }
            vertexGroups[i] = vg;
        }

        return true;
    }

    private static bool ReadVariationData(out VariationController variation, SetSRT callback)
    {
        float valuesImportantInEditorButNotHere = 0;
        Vector3 ls = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 lt = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 lr = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 hs = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 ht = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 hr = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        valuesImportantInEditorButNotHere = s_reader.ReadUInt32();
        valuesImportantInEditorButNotHere = s_reader.ReadUInt32();
        valuesImportantInEditorButNotHere = s_reader.ReadUInt32();
        VariationType vt = (VariationType)s_reader.ReadUInt32();
        valuesImportantInEditorButNotHere = s_reader.ReadUInt32();
        valuesImportantInEditorButNotHere = s_reader.ReadUInt32();

        variation = new VariationController(ls, lr, lt, hs, hr, ht, vt, callback);
        return true;
    }

    private static bool ReadVertexGroup(out VertexGroup vertexGroup)
    {
        float valuesImportantInEditorButNotHere = 0;
        Vector3 s = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 t = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 r = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();
        Vector3 p = new Vector3(s_reader.ReadSingle(), s_reader.ReadSingle(), s_reader.ReadSingle());
        valuesImportantInEditorButNotHere = s_reader.ReadSingle();

        vertexGroup = new VertexGroup();
        vertexGroup.SetScaleRotationTranslation(s, r, t);

        VariationController vc;
        if (!ReadVariationData(out vc, vertexGroup.SetScaleRotationTranslation)) { return false; }

        List<int> indices;
        if (!ReadVertexGroupIndices(out indices)) { return false; }
        vertexGroup.SetIndices(indices);
        return true;
    }

    private static bool ReadVertexGroupIndices(out List<int> indices)
    {
        uint indexCount = s_reader.ReadUInt32();
        indices = new List<int>((int)indexCount);

        for (int i = 0; i < indexCount; ++i)
        {
            indices[i] = s_reader.ReadInt32();
        }

        return true;
    }

    private static void ClearVertexData(out float[] outVerts)
    {
        outVerts = null;
    }

    private static void ClearVertexGroups(List<VertexGroup> vertexGroups)
    {
        for (int i = 0; i < vertexGroups.Count; ++i)
        {
            vertexGroups[i].Clear();
        }

        vertexGroups.Clear();
    }


    private static BinaryReader s_reader = null;
    const int FORMAT_CHECK_DATA_SIZE = 37;
    const string FORMAT_CHECK_DATA = "JUSTINS_FANCY_CUSTOM_CAPSTONE_FORMAT";
    const int FORMAT_VERSION = 2;
}

#endregion

#region HookerUpper

static class HookerUpper
{
    public static void HookUp(GameObjectSRT obj)
    {

    }
}

#endregion

#region Script

public class CapstoneGenerator : MonoBehaviour {

    GameObjectSRT m_objToVary;
    GameObject m_objToDisplay;

	// Use this for initialization
	void Start () {
        m_objToDisplay = new GameObject();
        m_objToDisplay.transform.parent = transform;
        m_objToVary = new GameObjectSRT();
        m_objToVary.SetObject(m_objToDisplay);

        m_objToDisplay.AddComponent<MeshFilter>();
        m_objToDisplay.AddComponent<MeshRenderer>();
        Mesh mesh = m_objToDisplay.GetComponent<MeshFilter>().mesh;

        mesh.Clear();

        // make changes to the Mesh by creating arrays which contain the new values
        mesh.vertices = new Vector3[] { new Vector3(0, 0, 0), new Vector3(0, 1, 0), new Vector3(1, 1, 0) };
        //mesh.uv = new Vector2[] { new Vector2(0, 0), new Vector2(0, 1), new Vector2(1, 1) };
        mesh.triangles = new int[] { 0, 1, 2 };
    }

}

#endregion