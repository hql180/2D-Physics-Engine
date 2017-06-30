using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ragdoller : MonoBehaviour
{

	// Use this for initialization
	void Start ()
    {
        GameObject sphere = Resources.Load("Prefabs/PhysicsSphere") as GameObject;
        GameObject capsule = Resources.Load("Prefabs/PhysicsCapsule") as GameObject;

        Vector3 angle = new Vector3(0, 0, -90);
  
        GameObject pelvis = Instantiate(capsule, transform.position, Quaternion.Euler(angle));
        pelvis.name = "Pelvis";
        pelvis.transform.parent = transform;



        GameObject body = AddPart("Body", capsule, pelvis, new Vector3(0, 1, 0),
            new Vector3(0, 0, 1), -20, 20, new Vector3(-1, 0, 0), 10);
        GameObject head = AddPart("Head", sphere, body, new Vector3(0, 1, 0),
            new Vector3(1, 0, 0), -40, 25, new Vector3(0, 0, 1), 25);
        GameObject armUL = AddPart("ArmUL", capsule, body, new Vector3(-0.5f, 0, 0),
            new Vector3(0, -1, 0), -70, 10, new Vector3(1, 0, 0), 50);

        GameObject armLL = AddPart("ArmLL", capsule, armUL, new Vector3(0, -1, 0),
            new Vector3(1, 0, 0), -90, 0, new Vector3(0, -1, 0), 0);

        GameObject armUR = AddPart("ArmUR", capsule, body, new Vector3(0.5f, 0, 0),
            new Vector3(0, -1, 0), -70, 10, new Vector3(1, 0, 0), 50);

        GameObject armLR = AddPart("ArmLR", capsule, armUR, new Vector3(0, -1, 0),
            new Vector3(1, 0, 0), -90, 0, new Vector3(0, -1, 0), 0);

        GameObject legUL = AddPart("LegUL", capsule, pelvis, new Vector3(-0.5f, -1, 0),
            new Vector3(0, 0, 1), -20, 70, new Vector3(1, 0, 0), 30);

        GameObject legLL = AddPart("LegLL", capsule, legUL, new Vector3(0, -1, 0),
            new Vector3(0, 0, 1), -90, 0, new Vector3(1, 0, 0), 0);

        GameObject legUR = AddPart("LegUR", capsule, pelvis, new Vector3(0.5f, -1, 0),
            new Vector3(0, 0, 1), -20, 70, new Vector3(1, 0, 0), 30);

        GameObject legLR = AddPart("LegLR", capsule, legUR, new Vector3(0, -1, 0),
            new Vector3(0, 0, 1), -90, 0, new Vector3(1, 0, 0), 0);


    }

    // Update is called once per frame
    void Update ()
    {
		
	}

    GameObject AddPart(string name, GameObject template, GameObject parent, Vector3 pos, Vector3 axis, float lMin, float lMax, Vector3 swingAxis, float range)
    {
        GameObject part = Instantiate(template, parent.transform.position + pos, transform.rotation);
        part.name = name;
        part.transform.parent = parent.transform;

        CharacterJoint joint = part.AddComponent<CharacterJoint>();

        joint.autoConfigureConnectedAnchor = true;

        joint.connectedBody = parent.GetComponent<Rigidbody>();

        joint.axis = axis;
        joint.swingAxis = swingAxis;

        SoftJointLimit minLimit = joint.lowTwistLimit;
        minLimit.limit = lMin;
        joint.lowTwistLimit = minLimit;

        SoftJointLimit maxLimit = joint.highTwistLimit;
        maxLimit.limit = lMax;
        joint.highTwistLimit = maxLimit;

        SoftJointLimit jointRange = joint.swing1Limit;
        jointRange.limit = range;
        joint.swing1Limit = jointRange;

        return part;
    }


}

