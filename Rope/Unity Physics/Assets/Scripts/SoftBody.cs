using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SoftBody : MonoBehaviour
{
    public enum JointType { HINGEJOINT, SPRINGJOINT, FIXEDJOINT }

    public int numLinks = 5;

    public Vector3 ballScale = new Vector3(0.5f, 0.5f, 0.5f);

    public JointType jointType;
    
    public float springStrength = 500f;

// Use this for initialization
void Start ()
    {
        transform.localScale = Vector3.one;

        GameObject ball = Resources.Load<GameObject>("Prefabs/PhysicsSphere");
        GameObject anchor = Instantiate(ball, transform.position, transform.rotation);
        anchor.transform.parent = transform;
        anchor.name = "Anchor";
        anchor.GetComponent<Rigidbody>().isKinematic = true;

        Vector3 offSet = new Vector3(0, ball.transform.localScale.y, 0);
        

        GameObject[] links = new GameObject[numLinks];
        for (int i = 0; i < numLinks; ++i)
        {
            links[i] = Instantiate(ball, transform.position, transform.rotation);
            links[i].transform.parent = transform;
            links[i].name = string.Format("Link {0}", i + 1);
            links[i].GetComponent<Rigidbody>().isKinematic = false;

            Joint joint = new Joint();
            switch (jointType)
            {
                case JointType.HINGEJOINT:
                    joint = links[i].AddComponent<HingeJoint>();
                    break;
                case JointType.FIXEDJOINT:
                    joint = links[i].AddComponent<FixedJoint>();
                    break;
                case JointType.SPRINGJOINT:
                    joint = links[i].AddComponent<SpringJoint>();
                    break;
            }

            if (jointType == JointType.SPRINGJOINT)
            {
                ((SpringJoint)joint).spring = springStrength;
            }  

            joint.connectedBody = (i == 0) ? anchor.GetComponent<Rigidbody>() : links[i - 1].GetComponent<Rigidbody>();           
            links[i].transform.position = joint.connectedBody.position - offSet;

            joint.anchor = Vector3.zero;
            joint.connectedAnchor = new Vector3(0, -1, 0);
        }
	}
	
	// Update is called once per frame
	void Update ()
    {
		
	}
}
