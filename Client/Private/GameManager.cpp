#pragma once
#include "GameManager.h"

IMPLEMENT_SINGLETON(CGame_Manager)

CGame_Manager::CGame_Manager()
{

}

void CGame_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::Get_Instance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
	
	//HRESULT hr = MFStartup(MF_VERSION);
	//if (FAILED(hr)) {
	//	// 초기화 실패 처리
	//	MSG_BOX(L"MFStartup Failed");
	//}

}

void CGame_Manager::PriorityTick(_float fTimeDelta)
{
}

void CGame_Manager::Tick(_float fTimeDelta)
{
}

void CGame_Manager::LateTick(_float fTimeDelta)
{
}

void CGame_Manager::Render()
{
}

void CGame_Manager::VideoSetting()
{
	//HRESULT hr = MFCreateSourceReaderFromURL(L"/Resources/Video/TitleScreen.mp4", NULL, &pReader);
	//if (FAILED(hr)) {
	//	// Source Reader 생성 실패 처리
	//	MSG_BOX(L"Source Reader 생성 실패");
	//}

	//hr = MFCreateMediaType(&pVideoType);
	//if (SUCCEEDED(hr)) {
	//	pVideoType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	//	pVideoType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	//	hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pVideoType);
	//	pVideoType->Release();
	//}

	//if (FAILED(hr)) {
	//	// 비디오 스트림 설정 실패 처리
	//	MSG_BOX(L"비디오 스트림 설정 실패");
	//}


}

void CGame_Manager::VideoRender()
{
    //DWORD dwFlags = 0;

    //    HRESULT hr = pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, &dwFlags, NULL, &pSample);

    //    IMFMediaBuffer* pBuffer = NULL;
    //    hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    //    if (SUCCEEDED(hr)) {
    //        BYTE* pData = NULL;
    //        DWORD cbBuffer = 0;
    //        hr = pBuffer->Lock(&pData, NULL, &cbBuffer);
    //        if (SUCCEEDED(hr)) {
    //            // DirectX 11 텍스처 업데이트
    //            D3D11_TEXTURE2D_DESC desc;
    //            ZeroMemory(&desc, sizeof(desc));
    //            desc.Width = 1280;
    //            desc.Height = 720;
    //            desc.MipLevels = 1;
    //            desc.ArraySize = 1;
    //            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    //            desc.SampleDesc.Count = 1;
    //            desc.Usage = D3D11_USAGE_DYNAMIC;
    //            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    //            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    //            D3D11_SUBRESOURCE_DATA initData;
    //            initData.pSysMem = pData;
    //            initData.SysMemPitch = 1280 * 4;
    //            ID3D11Texture2D* pTexture = NULL;
    //            hr = m_pDevice->CreateTexture2D(&desc, &initData, &pTexture);

    //            if (SUCCEEDED(hr)) {
    //                // 텍스처 렌더링 코드 추가
    //            }

    //            pBuffer->Unlock();
    //        }
    //        pBuffer->Release();
    //    }
    //    pSample->Release();
}

void CGame_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
