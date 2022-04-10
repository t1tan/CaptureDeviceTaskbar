#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DShowUtils.h"

#include "dshow.h"
#include "strmif.h"
#include "objidl.h"
#include "shlwapi.h"

#pragma comment(lib, "strmiids")

//inspiration for this from FFMpeg and the DirectShow SDK (Microsoft)

static void locShowDShowFilterProperties(IBaseFilter* pDeviceFilter)
{
    ISpecifyPropertyPages* pPropertyPages = NULL;
    IUnknown* pDeviceFilterIUnknown = NULL;
    HRESULT hr;
    FILTER_INFO filterInfo = { 0 }; /* a warning on this line is false positive GCC bug 53119 AFAICT */
    CAUUID ca_guid = { 0 };

    hr = pDeviceFilter->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPropertyPages);
    if (hr != S_OK) //error "requested filter does not have a property page to show"
        goto end;

    hr = pDeviceFilter->QueryFilterInfo(&filterInfo);
    if (hr != S_OK)
        goto fail;

    hr = pDeviceFilter->QueryInterface(IID_IUnknown, (void**)&pDeviceFilterIUnknown);
    if (hr != S_OK)
        goto fail;

    hr = pPropertyPages->GetPages(&ca_guid);
    if (hr != S_OK)
        goto fail;

    hr = OleCreatePropertyFrame(NULL, 0, 0, filterInfo.achName, 1, &pDeviceFilterIUnknown, ca_guid.cElems, ca_guid.pElems, 0, 0, NULL);
    if (hr != S_OK)
        goto fail;

    goto end;

fail:
    //error "Failure showing property pages for filter"
end:
    if (pPropertyPages)
        pPropertyPages->Release();
    if (pDeviceFilterIUnknown)
        pDeviceFilterIUnknown->Release();
    if (filterInfo.pGraph)
        filterInfo.pGraph->Release();
    if (ca_guid.pElems)
        CoTaskMemFree(ca_guid.pElems);
}

static HRESULT locEnumerateDevices(REFGUID category, IEnumMoniker** ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum* pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

static void locDisplayDeviceInformation(IEnumMoniker* pEnum, wxString sFriendlyName)
{
    IMoniker* pMoniker = NULL;

    bool foundIt = false;
    while (!foundIt && pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag* pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));

        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
            VariantClear(&var);
        }

        hr = pPropBag->Write(L"FriendlyName", &var);

        foundIt = sFriendlyName.CompareTo(var.bstrVal) == 0;
        if (foundIt)
        {
            IBaseFilter* pFilter;
            hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
            if (SUCCEEDED(hr))
            {
                locShowDShowFilterProperties(pFilter);
                pFilter->Release();
            }
        }

        pPropBag->Release();
        pMoniker->Release();
    }
}

static void locGetFriendlyDeviceNames(IEnumMoniker* pEnum, wxArrayString &outArrResult)
{
    IMoniker* pMoniker = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag* pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));

        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
            VariantClear(&var);
        }

        hr = pPropBag->Write(L"FriendlyName", &var);
        outArrResult.Add(var.bstrVal);

        pPropBag->Release();
        pMoniker->Release();
    }
}

void GetDevicesFriendlyName(wxArrayString& outArrResult)
{
    outArrResult.Clear();
    
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        IEnumMoniker* pEnum = NULL;
        hr = locEnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            locGetFriendlyDeviceNames(pEnum, outArrResult);
            pEnum->Release();
        }
        
        CoUninitialize();
    }
}

void ShowDevicePropertyDialog(wxString sFriendlyName)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        IEnumMoniker* pEnum;

        hr = locEnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            locDisplayDeviceInformation(pEnum, sFriendlyName);
            pEnum->Release();
        }

        CoUninitialize();
    }
}
