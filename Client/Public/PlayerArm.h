#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "PhysXCollider.h"

/*
0. katana flip1
1. katana flip2
2. katana flip3
3. katana flip4
4. block 1
5. block 2
6. hook pul 1
7. attack 1
8. attack 2
9. attack 3
10. attack 4
11. attack 5
12. attack 6
13. jump idle 
14. idle
15. jump start
16. jump loop
17. jump end
18. run
19. wall run L
20. wall run R
21. death 1
22. death 2
23. death 3

*/



BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayerArm final : public CGameObject
{
private:
	CPlayerArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerArm(const CPlayerArm& rhs);
	virtual ~CPlayerArm() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Soft_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	void Hard_Animation_Change(_uint _iAnimationNum, _bool _bLoop, _float _bSpeedMultiply = 1.f);
	bool AnimationFinished() { return m_pModelCom->Get_Finished(); }

	_uint Get_CurrentAnimIndex() { return m_pModelCom->Get_CurrentAnimIndex(); }
private:
	vector<CGameObject*> m_vecParts;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CPhysXCollider* m_pPhysXCollider = { nullptr };

	_float4x4 m_ViewMatrix;
	
	_float m_fRadian = 178.916672f;
	_float m_fFront = 0.179825f;
	_float m_fRight = -0.016626f;
	_float m_fUp = 1.616f;


	/*
	m_fRadian = 179.033371 178.916672
m_fFront = 0.183428 0.179825
right = 0.01406 -0.016626
up  1.616707 
	*/
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	CCollider* m_pColliderCom = { nullptr };
	class CWire* m_pWire = { nullptr };

	_bool m_bWireOn = false;
public:
	static CPlayerArm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END