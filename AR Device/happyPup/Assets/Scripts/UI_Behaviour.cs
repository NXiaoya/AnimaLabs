using UnityEngine;
using UnityEngine.UIElements; //Import the UI Toolkit
using UnityEngine.XR.ARFoundation; //ARFundation framework

public class UI_Behaviour : MonoBehaviour
{
    public UIDocument UIOnborading; //The UI Document Onboarding

    //The the Visual Elements from the UI Document
    private VisualElement findPlaneUI;
    private VisualElement tapToPlaceUI;

    // The two video players
    public GameObject findPlaneVideoP;
    public GameObject tapToPlaceVideoP;

    // Events for found plane and content created
    private ARPlaneManager m_ARPlaneManager;
    private TapToPlace m_TapToPlace;

    bool isContentVisible = false;
    
    private void Awake()
    {
        findPlaneUI = UIOnborading.rootVisualElement.Query<VisualElement>("FindPlane");
        tapToPlaceUI = UIOnborading.rootVisualElement.Query<VisualElement>("TapToPlace");

        m_ARPlaneManager = GetComponent<ARPlaneManager>();
        m_ARPlaneManager.planesChanged += planeFound; //Event Fired when a plane is detected
        m_TapToPlace=GetComponent<TapToPlace>();
        m_TapToPlace._contentVisibleEvent += contentVisible; //Event fired when the content is created (user Tap)
    }


    void planeFound(ARPlanesChangedEventArgs args)
    {
        //Plane found, turn off UI and Video
        findPlaneUI.style.display = DisplayStyle.None;
        findPlaneVideoP.SetActive(false);
        m_ARPlaneManager.planesChanged -= planeFound;

        if (isContentVisible)
        {   //Content created, turn off UI and Video
            tapToPlaceUI.style.display = DisplayStyle.None;
            tapToPlaceVideoP.SetActive(false);
        }
        else
        {
            //Content is not created yet, keep the UI on and the Video
            tapToPlaceUI.style.display = DisplayStyle.Flex;
            tapToPlaceVideoP.SetActive(true);
        }

    }

    void contentVisible(){
        isContentVisible = true; //if the content is there
        //Debug.Log("FIRED");
        m_TapToPlace._contentVisibleEvent -= contentVisible;
        //Content created, turn off UI and Video
        findPlaneUI.style.display = DisplayStyle.None;
        findPlaneVideoP.SetActive(false);
        tapToPlaceUI.style.display = DisplayStyle.None;
        tapToPlaceVideoP.SetActive(false);

    }
}