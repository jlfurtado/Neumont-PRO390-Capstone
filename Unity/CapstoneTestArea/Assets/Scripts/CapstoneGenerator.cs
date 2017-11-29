using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

delegate void SetSRT(Vector3 s, Vector3 r, Vector3 t);
delegate void VaryFunc();

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
        m_varyFuncs = new VaryFunc[] { VaryComponentUniform,
                                       VaryVectorUniform,
                                       VarySmoothUniform,
                                       VaryComponentBellApproximation,
                                       VaryVectorBellApproximation,
                                       VarySmoothBellApproximation };

    }

    private Vector3 m_lowScale;
    private Vector3 m_lowTranslation;
    private Vector3 m_lowRotation;
    private Vector3 m_highScale;
    private Vector3 m_highTranslation;
    private Vector3 m_highRotation;
    private Matrix4x4 m_mtw;
    private VariationType m_variationType;
    private SetSRT m_setCallback;
    private VaryFunc[] m_varyFuncs;

    public void SetCallback(SetSRT callback)
    {
        m_setCallback = callback;
    }

    public void Vary()
    {
        m_varyFuncs[(int)(m_variationType)]();
    }

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

    public VertexGroup()
    {
        m_scale = Vector3.one;
        m_rotation = Vector3.zero;
        m_translation = Vector3.zero;
        m_pivot = Vector3.zero;
    }

    public VertexGroup(Vector3 scale, Vector3 rotation, Vector3 translation, Vector3 pivot)
    {
        m_scale = scale;
        m_rotation = rotation;
        m_translation = translation;
        m_pivot = pivot;
    }

    public Vector3 GetPivot()
    {
        return m_pivot;
    }

    public void SetPivot(Vector3 pivot)
    {
        m_pivot = pivot;
    }

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

    public void SetVariation(VariationController variation)
    {
        m_variation = variation;
    }

    public Matrix4x4 CalcMTW()
    {
        return Matrix4x4.TRS(m_translation, Quaternion.Euler(m_rotation), m_scale);
    }

    public void Vary()
    {
        m_variation.Vary();
    }

    public int NumIndices()
    {
        return m_indices.Count;
    }

    public int IndexAt(int index)
    {
        return m_indices[index];
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
    public static bool LoadBaseMesh(string filePath, SetSRT objectVariationCallback, out float[] verts, out int floatsPerVertex, out List<VertexGroup> vertexGroups, out VariationController objectVariations)
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

        if (!ReadMeshInfo(objectVariationCallback, out verts, out floatsPerVertex, out vertexGroups, out objectVariations))
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
        string readDataStr = new string(readData);
        return string.Compare(readDataStr, FORMAT_CHECK_DATA) == 0;
    }

    private static bool CheckVersionData()
    {
        int readVersion = s_reader.ReadInt32();
        return (readVersion >= 0) && (readVersion == FORMAT_VERSION);
    }

    private static bool ReadMeshInfo(SetSRT objectVariationCallback, out float[] outVerts, out int floatsPerVertex, out List<VertexGroup> vertexGroups, out VariationController objectVariations)
    {
        outVerts = null;
        floatsPerVertex = 0;
        vertexGroups = null;
        objectVariations = null;

        int numVerts = 0;
        uint numGroups = 0;
        if (!ReadMeshHeader(out numVerts, out floatsPerVertex)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshHeader!\n"); return false; }
        if (!ReadMeshVerts(out outVerts, numVerts * floatsPerVertex)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadMeshVerts!\n"); ClearVertexData(out outVerts); return false; }
        if (!ReadObjectLevelVariations(out objectVariations, objectVariationCallback)) { Debug.Log("ERROR: Failed to ReadMeshInfo! Failed to ReadObjectLevelVariations!\n"); ClearVertexData(out outVerts); return false; }
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
            vertexGroups.Add(vg);
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

        lr *= Mathf.Rad2Deg;
        hr *= Mathf.Rad2Deg;

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

        vertexGroup = new VertexGroup(s, r, t, p);

        VariationController vc;
        if (!ReadVariationData(out vc, vertexGroup.SetScaleRotationTranslation)) { return false; }
        vertexGroup.SetVariation(vc);

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
            indices.Add(s_reader.ReadInt32());
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
    const string FORMAT_CHECK_DATA = "JUSTINS_FANCY_CUSTOM_CAPSTONE_FORMAT\0";
    const int FORMAT_VERSION = 2;
}

#endregion

#region HookerUpper

class HookerUpper
{
    private int[] m_indices;

    public void InitIndices(int vertexCount)
    {
        m_indices = new int[vertexCount];

        for (int i = 0; i < vertexCount; ++i)
        {
            m_indices[i] = i;
        }
    }

    public void SetMeshValues(Mesh mesh, Vector3[] positions, Vector3[] normals)
    {
        mesh.Clear();

        mesh.vertices = positions;
        mesh.normals = normals;
        mesh.triangles = m_indices;
    }

    public void VaryModel(Mesh mesh, int numVertices, float[] baseVerts, VariationController objectVariations, List<VertexGroup> vertexGroups)
    {
        const int fpv = 10;
        const int normalOffset = 7;

        Vector3[] positions = new Vector3[numVertices];
        Vector3[] normals = new Vector3[numVertices];

        VariationMath.OffsetTransformVertsIntoArrays(Vector3.zero, Matrix4x4.identity, out positions, out normals, numVertices, fpv, 7, baseVerts);

        objectVariations.Vary();
        for (int i = 0; i < vertexGroups.Count; ++i)
        {
            vertexGroups[i].Vary();

            Matrix4x4 mtw = vertexGroups[i].CalcMTW();
            Matrix4x4 inverseTranspose = mtw.transpose.inverse;
            Vector3 pivot = vertexGroups[i].GetPivot();

            int indicesInVerteXGroup = vertexGroups[i].NumIndices();
            for (int j = 0; j < indicesInVerteXGroup; ++j)
            {
                int vertIdx = vertexGroups[i].IndexAt(j);
                int posIdx = vertIdx * fpv;
                int normIdx = posIdx + normalOffset;
                positions[vertIdx] = mtw.MultiplyPoint3x4(new Vector3(baseVerts[posIdx + 0], baseVerts[posIdx + 1], baseVerts[posIdx + 2]) - pivot) + pivot;
                normals[vertIdx] = inverseTranspose.MultiplyVector(new Vector3(baseVerts[normIdx + 0], baseVerts[normIdx + 1], baseVerts[normIdx + 2]));
            }
        }

        SetMeshValues(mesh, positions, normals);
    }
}

#endregion

#region VariationMath

static class VariationMath
{
    public static void OffsetTransformVertsIntoArrays(Vector3 offset, Matrix4x4 transform, out Vector3[] positions, out Vector3[] normals, int vertexCount, int floatsPerVertex, int normalOffset, float[] vertices)
	{
        Matrix4x4 inverseTranspose = transform.transpose.inverse;

        positions = new Vector3[vertexCount];
        normals = new Vector3[vertexCount];

        for (int i = 0; i < vertexCount; ++i)
		{
			int posIdx = floatsPerVertex * i;
            int normIdx = posIdx + normalOffset;

            positions[i] = offset + transform.MultiplyPoint3x4(new Vector3(vertices[posIdx + 0], vertices[posIdx + 1], vertices[posIdx + 2]));
            normals[i] = inverseTranspose.MultiplyVector(new Vector3(vertices[normIdx + 0], vertices[normIdx + 1], vertices[normIdx + 2]));
        }
	}
}

#endregion

#region Script

public class CapstoneGenerator : MonoBehaviour {
    [SerializeField] private string m_fileName = "1.txt";
    [SerializeField] private Material m_meshMat;
    [SerializeField] [Range(0, 10000)] private int m_listPresize;

    private string m_filePath;
    private float[] m_baseVerts;
    private List<VertexGroup> m_vertexGroups;
    private VariationController m_objectVariations;
    private int m_numVertices = 0;
    private int m_floatsPerVertex = 0;
    private int m_normalOffset = 7;
    private bool m_regening = false;
    private HookerUpper m_hookerUpper;

    struct ComponentRef
    {
        public GameObjectSRT m_gameObjectSRT;
        public Renderer m_renderer;
        public Mesh m_mesh;
    }

    private List<ComponentRef> m_componentRefs;

    private void NullSetSRT(Vector3 s, Vector3 r, Vector3 t)
    {
        Debug.Log("SET SRT CALLBACK NOT SET!\n");
    }

    private void Awake()
    {
        m_hookerUpper = new HookerUpper();
        m_filePath = Application.dataPath + "/Capstone/" + m_fileName;
        CustomIO.LoadBaseMesh(m_filePath, NullSetSRT, out m_baseVerts, out m_floatsPerVertex, out m_vertexGroups, out m_objectVariations);
        m_numVertices = m_baseVerts.Length / m_floatsPerVertex;


        m_hookerUpper.InitIndices(m_numVertices);
        m_componentRefs = new List<ComponentRef>(m_listPresize);
    }

    public GameObject MakeInstance()
    {
        ComponentRef compRef = new ComponentRef();
        GameObject display = new GameObject();
        GameObjectSRT toVary = new GameObjectSRT();
        toVary.SetObject(display);

        compRef.m_mesh = display.AddComponent<MeshFilter>().mesh;
        compRef.m_renderer = display.AddComponent<MeshRenderer>();
        compRef.m_gameObjectSRT = toVary;
        
        MakeMesh(compRef);
        m_componentRefs.Add(compRef);
        return display;
    }

    private void MakeMesh(ComponentRef reference)
    {
        reference.m_renderer.material = m_meshMat;
        m_objectVariations.SetCallback(reference.m_gameObjectSRT.SetSRT);
        m_hookerUpper.VaryModel(reference.m_mesh, m_numVertices, m_baseVerts, m_objectVariations, m_vertexGroups);
    }

    private void MakeBaseMesh(ComponentRef reference)
    {
        reference.m_renderer.material = m_meshMat;
        m_objectVariations.SetCallback(reference.m_gameObjectSRT.SetSRT);
        Vector3[] positions, normals;
        VariationMath.OffsetTransformVertsIntoArrays(Vector3.zero, Matrix4x4.identity, out positions, out normals, m_numVertices, m_floatsPerVertex, m_normalOffset, m_baseVerts);
        m_hookerUpper.SetMeshValues(reference.m_mesh, positions, normals);
        m_objectVariations.ClearVariations();
    }

    public void Regen()
    {
        if (!m_regening) { StartCoroutine(ReMakeObjects()); }
    }

    public void BoringIfy()
    {
        if (!m_regening) { StartCoroutine(BoringifyObjects()); }
    }

    private IEnumerator BoringifyObjects()
    {
        m_regening = true;

        for (int i = 0; i < m_componentRefs.Count; ++i)
        {
            Transform storeTransform = m_componentRefs[i].m_gameObjectSRT.GetObject().transform;
            Vector3 loc = storeTransform.position;
            MakeBaseMesh(m_componentRefs[i]);
            storeTransform.position = loc;
            yield return null;
        }

        m_regening = false;
    }

    private IEnumerator ReMakeObjects()
    {
        m_regening = true;

        for (int i = 0; i < m_componentRefs.Count; ++i)
        {
            Transform storeTransform = m_componentRefs[i].m_gameObjectSRT.GetObject().transform;
            Vector3 loc = storeTransform.position;
            MakeMesh(m_componentRefs[i]);
            storeTransform.position = loc;
            yield return null;
        }

        m_regening = false;
    }
}

#endregion