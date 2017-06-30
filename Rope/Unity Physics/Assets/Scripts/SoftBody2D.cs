using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(MeshFilter))]
[RequireComponent(typeof(MeshRenderer))]
public class SoftBody2D : MonoBehaviour
{

    public enum JointType { HINGEJOINT, SPRINGJOINT, FIXEDJOINT }


    public int numRows = 10;
    public int numColumns = 10;
	public Vector3 columnSpacing = new Vector3(1, 0, 0);
	public Vector3 rowSpacing = new Vector3(0, 0, 1);
	public bool bendSprings = true;
	public bool startKinematic = true;
	public JointType jointType;

	[Header("Spring Settings")]
	public float springStrength = 5000000f;

	public float springDamp = 500000f;
	[Header("Ball Settings")]

	public Vector3 ballScale = new Vector3(0.5f, 0.5f, 0.5f);
	public float ballMass = 1f;
	public bool showBalls = false;


	GameObject[][] links;

	MeshFilter cloth;

// Use this for initialization
void Start ()
    {
        transform.localScale = Vector3.one;

        GameObject ball = Resources.Load<GameObject>("Prefabs/PhysicsSphere");

        Vector3 pos = transform.position;

        links = new GameObject[numColumns][];

        for (int col = 0; col < numColumns; ++col)
        {
            links[col] = new GameObject[numRows];
            for (int row = 0; row < numRows; ++row) 
            {
                Vector3 pos0 = pos + columnSpacing * col + rowSpacing * row;
                links[col][row] = Instantiate(ball, pos0, transform.rotation);
				links[col][row].AddComponent<RagdollTrigger>();

				links[col][row].transform.parent = gameObject.transform;
                links[col][row].name = string.Format( "Link {0} {1}", col + 1,  row + 1);

				links[col][row].tag = "RagdollTrigger";

				links[col][row].transform.localScale = ballScale;
				Rigidbody rb = links[col][row].GetComponent<Rigidbody>();


				rb.isKinematic = startKinematic;
				rb.mass = ballMass;

				//Bend springs
				if (col > 1 && bendSprings)
				{
					Joint joint = new Joint();
					switch (jointType)
					{
						case JointType.HINGEJOINT:
							joint = links[col][row].AddComponent<HingeJoint>();
							break;
						case JointType.FIXEDJOINT:
							joint = links[col][row].AddComponent<FixedJoint>();

							break;
						case JointType.SPRINGJOINT:
							joint = links[col][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
					}
					joint.connectedBody = links[col - 2][row].GetComponent<Rigidbody>();
					joint.anchor = Vector3.zero;
				}

				if (row > 1 && bendSprings) 
				{
					Joint joint = new Joint();
					switch (jointType)
					{
						case JointType.HINGEJOINT:
							joint = links[col][row].AddComponent<HingeJoint>();
							break;
						case JointType.FIXEDJOINT:
							joint = links[col][row].AddComponent<FixedJoint>();

							break;
						case JointType.SPRINGJOINT:
							joint = links[col][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
					}
					joint.connectedBody = links[col][row - 2].GetComponent<Rigidbody>();
					joint.anchor = Vector3.zero;
				}

				// shear srpings
				if (col != 0 && row != 0) 
				{
					Joint joint = new Joint();
					switch (jointType)
					{
						case JointType.HINGEJOINT:
							joint = links[col][row].AddComponent<HingeJoint>();
							break;
						case JointType.FIXEDJOINT:
							joint = links[col][row].AddComponent<FixedJoint>();

							break;
						case JointType.SPRINGJOINT:
							joint = links[col][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
					}
					joint.connectedBody = links[col - 1][row - 1].GetComponent<Rigidbody>();
					joint.anchor = Vector3.zero;
					

					Joint joint2 = new Joint();
					switch (jointType)
					{
						case JointType.HINGEJOINT:
							joint2 = links[col - 1][row].AddComponent<HingeJoint>();
							break;
						case JointType.FIXEDJOINT:
							joint2 = links[col - 1][row].AddComponent<FixedJoint>();

							break;
						case JointType.SPRINGJOINT:
							joint2 = links[col - 1][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
					}
					joint2.connectedBody = links[col][row - 1].GetComponent<Rigidbody>();
					joint2.anchor = Vector3.zero;								
				}

				//structural springs
                if (col != 0)
                {
                    Joint joint = new Joint();
                    switch (jointType)
                    {
                        case JointType.HINGEJOINT:
                            joint = links[col][row].AddComponent<HingeJoint>();
                            break;
                        case JointType.FIXEDJOINT:
                            joint = links[col][row].AddComponent<FixedJoint>();

							break;
                        case JointType.SPRINGJOINT:
                            joint = links[col][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
                    }
                    joint.connectedBody = links[col - 1][row].GetComponent<Rigidbody>();
                    joint.anchor = Vector3.zero;
				}

				//structural springs
				if (row != 0)
                {
                    Joint joint = new Joint();
                    switch (jointType)
                    {
                        case JointType.HINGEJOINT:
                            joint = links[col][row].AddComponent<HingeJoint>();
                            break;
                        case JointType.FIXEDJOINT:
                            joint = links[col][row].AddComponent<FixedJoint>();
							
							break;
                        case JointType.SPRINGJOINT:
                            joint = links[col][row].AddComponent<SpringJoint>();
							((SpringJoint)joint).spring = springStrength;
							((SpringJoint)joint).damper = springDamp;
							break;
                    }
                    joint.connectedBody = links[col][row - 1].GetComponent<Rigidbody>();
                    joint.anchor = Vector3.zero;				
				}
            }
        }

		cloth = GetComponent<MeshFilter>();
		if (cloth)
			SetUpCloth();

	}
	
	// Update is called once per frame
	void Update ()
    {
		if (cloth)
			UpdateCloth();
	}

	void SetUpCloth()
	{
		cloth.mesh = new Mesh();
		UpdateCloth();

		Vector2[] uvs = new Vector2[numRows * numColumns];
		for (int col = 0; col < numColumns; ++col)
		{
			for (int row = 0; row < numRows; ++row)
			{
				uvs[col + row * numColumns] = new Vector2((float)col / numColumns, (float)row / numRows);
			}
		}
		cloth.mesh.uv = uvs;

		int[] triangles = new int[(numRows - 1) * (numColumns - 1) * 12];

		int k = 0;

		for (int i = 0; i < numColumns - 1; ++i) 
		{
			for (int j = 0; j < numRows - 1; ++j)
			{
				triangles[k++] = i + j * numColumns;
				triangles[k++] = (i + 1) + j * numColumns;
				triangles[k++] = i + (j + 1) * numColumns;

				triangles[k++] = i + (j + 1) * numColumns;
				triangles[k++] = (i + 1) + j * numColumns;
				triangles[k++] = (i + 1) + (j + 1) * numColumns;

				// otherside
				triangles[k++] = i + j * numColumns;
				triangles[k++] = i + (j + 1) * numColumns;
				triangles[k++] = (i + 1) + j * numColumns;

				triangles[k++] = (i + 1) + j * numColumns;
				triangles[k++] = i + (j + 1) * numColumns;
				triangles[k++] = (i + 1) + (j + 1) * numColumns;
			}
		}

		cloth.mesh.triangles = triangles;
	}

	void UpdateCloth()
	{
		Vector3[] vertices = new Vector3[numRows * numColumns];
		Vector3[] normals = new Vector3[numRows * numColumns];
		for (int col = 0; col < numColumns; ++col)
		{
			for (int row = 0; row < numRows; ++row)
			{
				vertices[col + row * numColumns] = links[col][row].transform.position - transform.position;
				links[col][row].GetComponent<MeshRenderer>().enabled = showBalls;
			}			
		}
		for (int col = 0; col < numRows; ++col)
		{
			for (int row = 0; row < numColumns; ++row)
			{
				Vector3 left = col == 0 ? vertices[col + row * numColumns] : vertices[col - 1 + row * numColumns];
				Vector3 right = col == numColumns - 1 ? vertices[col + row * numColumns] : vertices[col + 1 + row * numColumns];

				Vector3 down = row == 0 ? vertices[col + row * numColumns] : vertices[col + (row - 1) * numColumns];
				Vector3 up = row == numRows - 1 ? vertices[col + row * numColumns] : vertices[col + (row + 1) * numColumns];

				normals[col + row * numColumns] = Vector3.Cross(right - left, up - down) * -1f;
				normals[col + row * numColumns].Normalize();
			}
		}

		cloth.mesh.vertices = vertices;
		cloth.mesh.normals = normals;
		cloth.mesh.RecalculateBounds();
	}
}
