using System.Collections;
using System.Collections.Generic;
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

class VariationController
{

    private Vector3 m_lowScale;
    private Vector3 m_lowTranslation;
    private Vector3 m_lowRotation;
    private Vector3 m_highScale;
    private Vector3 m_highTranslation;
    private Vector3 m_highRotation;
    private SetSRT m_setCallback;

    //public void VaryVectorUniform()
    //{
    //    *m_pRotation = Variations::VectorUniform(m_lowRotation, m_highRotation);
    //    *m_pTranslation = Variations::VectorUniform(m_lowTranslation, m_highTranslation);
    //    *m_pScale = Variations::VectorUniform(m_lowScale, m_highScale);
    //}

    //public void VaryVectorBellApproximation()
    //{
    //    *m_pRotation = Variations::VectorBellApproximation(m_lowRotation, m_highRotation, 10);
    //    *m_pTranslation = Variations::VectorBellApproximation(m_lowTranslation, m_highTranslation, 10);
    //    *m_pScale = Variations::VectorBellApproximation(m_lowScale, m_highScale, 10);
    //}

    //public void VaryComponentUniform()
    //{
    //    *m_pRotation = Variations::VectorComponentUniform(m_lowRotation, m_highRotation);
    //    *m_pTranslation = Variations::VectorComponentUniform(m_lowTranslation, m_highTranslation);
    //    *m_pScale = Variations::VectorComponentUniform(m_lowScale, m_highScale);
    //}

    //public void VaryComponentBellApproximation()
    //{
    //    *m_pRotation = Variations::VectorComponentBellApproximation(m_lowRotation, m_highRotation, 10);
    //    *m_pTranslation = Variations::VectorComponentBellApproximation(m_lowTranslation, m_highTranslation, 10);
    //    *m_pScale = Variations::VectorComponentBellApproximation(m_lowScale, m_highScale, 10);
    //}

    //public void VarySmoothBellApproximation()
    //{
    //    Variations::TripleVectorBellApproximation(m_lowRotation, m_highRotation, m_pRotation,
    //                                              m_lowScale, m_highScale, m_pScale,
    //                                              m_lowTranslation, m_highTranslation, m_pTranslation,
    //                                              10);
    //}

    //public void VarySmoothUniform()
    //{
    //    Variations::TripleVectorUniform(m_lowRotation, m_highRotation, m_pRotation,
    //                                    m_lowScale, m_highScale, m_pScale,
    //                                    m_lowTranslation, m_highTranslation, m_pTranslation);
    //}
}

class VertexGroup
{
    private Vector3 m_scale;
    private Vector3 m_rotation;
    private Vector3 m_pivot;
    private Vector3 m_translation;
    private List<int> m_indices;

    private void SetScaleRotationTranslation(Vector3 scale, Vector3 rotation, Vector3 translation)
    {
        m_scale = scale;
        m_rotation = rotation;
        m_translation = translation;
    }
}

public class CapstoneGenerator : MonoBehaviour {
    
	// Use this for initialization
	void Start () {
		
	}
	
	//private void LoadBaseMesh(string filePath, out float[] outVerts, out int floatsPerVertex)
 //   {

 //   }
}
