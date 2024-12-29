# Portfolio_Dx11_GR

김민혁의 포트폴리오용 Repository 입니다.

리소스가 포함되어 있지 않아 제대로 작동하지 않을 가능성이 높으며, 코드 공개를 위한 Repository 입니다. 

참고하여 만든 작품은 GhostRunner 라는 작품입니다.

![image](https://github.com/user-attachments/assets/7b07148c-733c-4ab6-8c73-db9b03443951)

---

## 링크  
[Ghost Runner 프로젝트 데모 영상](https://www.youtube.com/watch?v=e-euy_yL7h0)

---

# Extend Library Dependancy

> PhysX 5.3  
> FMOD  
> Assimp 5.3.1  
> Imgui  
> Imguizmo
> Effects11  
> DirectXTK  
> DirectMath  

<hr/>

# DX11 모작 포트폴리오: Ghost Runner

- **프로젝트명:** DX11 모작 포트폴리오 [Ghost Runner]  
- **기간:** 2024.06 ~ 2024.08  
- **팀 프로젝트 여부:** X  
- **기술 스택:**  
  C, C++, STL, FMOD, Win API, OpenCV, DLL, DirectX 11, vcpkg, HLSL, Effects11, Assimp, ImGui, ImGuizmo, DirectXTK, DirectMath, Nvidia PhysX 5.3 , DLL
- **IDE:** Visual Studio  

---

## 목적 및 설명
DirectX 11을 이용해 제작하였고 FPS 와이어 액션 게임 *Ghost Runner*의 기본 조작 및 액션, 스테이지 구성, 보스 제작 등을 개발 전반을 진행한 개인 프로젝트입니다.  
상용 엔진을 사용하지 않고 엔진 부분을 직접 DLL로 작업하여 클라이언트와 엔진을 분리하는 방식으로 개발했습니다.

---

## 주요 구현 기능

### 1. **멀티스레딩을 통한 로딩 최적화**  
   - 게임 로딩 속도를 단축하기 위해 멀티스레딩 로드를 구현했습니다.  
   - 리소스를 별도의 스레드에서 로드하고, 메인 스레드가 초기화 작업을 병렬로 처리하도록 구성하여 로딩 시간을 크게 줄였습니다.

### 2. **오브젝트 풀링을 이용한 투사체 최적화**  
   - 반복적으로 생성 및 소멸되는 오브젝트를 효율적으로 관리하기 위해 오브젝트 풀링(Object Pooling) 기법을 적용했습니다.  
   - 이를 통해 메모리 할당과 해제를 최소화하고 성능을 개선했습니다.

### 3. **멀티 렌더 타겟과 Deferred 쉐이더 처리 기법**  
   - 여러 렌더 타겟과 Deferred Shading 기술을 활용하여 조명, 그림자, Glow, Blur 등의 후처리 효과를 구현했습니다.  
   - Deferred Shading 시에 더블 버퍼링 기법을 적용해 렌더 타겟 개수를 줄이고 메모리 성능을 최적화했습니다.

![시퀀스 01_3](https://github.com/user-attachments/assets/e8135a88-427e-4cbb-9402-20f100eed3a4)

### 4. **PhysX와 직접 구현한 물리 엔진**  
   - NVIDIA PhysX를 활용하여 복잡한 메쉬 기반 충돌 처리를 구현했습니다.  
   - 직접 설계한 물리 엔진으로 캐릭터와 환경 간의 상호작용 요소를 추가했습니다.

### 5. **메쉬 데이터 직렬화를 통한 최적화**  
   - 메쉬 데이터를 이진 직렬화하여 파일 크기를 줄이고, 로드 시간을 단축했습니다.  
   - 대규모 스테이지 로딩 시 지연 시간을 효과적으로 감소시켰습니다.

### 6. **몬스터 상태 제어 및 관리**  
   - 상태 머신(State Machine)을 설계하여 몬스터의 이동, 공격, 피격, 사망 상태를 체계적으로 관리했습니다.  
   - 다양한 행동 전환을 자연스럽게 구현하며, 난이도 조절 기능을 추가했습니다.

![시퀀스 01_5](https://github.com/user-attachments/assets/4afc3583-748b-415c-a53e-b3cf472a3603)

### 7. **ImGui 기반 맵 에디터**  
   - ImGui를 활용한 맵 에디터를 개발하여 스테이지 오브젝트 배치와 조명 설정(정광원, 방향성 광원)을 직관적으로 조정할 수 있었습니다.

### 8. **사운드 랜덤 재생 시스템**  
   - 사운드 랜덤 재생 시스템을 구축하여 플레이어 몰입도를 강화했습니다.  
   - 유사한 사운드를 무작위로 재생해 반복 효과음을 방지했습니다.

### 9. **플레이어 제어**  
   - PhysX CCT에서 측면 충돌을 파악해 벽타기에 맞는 애니메이션과 카메라 회전을 구현하였습니다.

![시퀀스 01_3](https://github.com/user-attachments/assets/e8135a88-427e-4cbb-9402-20f100eed3a4)

   - 대쉬, 해킹, 원거리 검기 공격 등 스킬을 구현하였습니다.

![시퀀스 01_6](https://github.com/user-attachments/assets/01c21d0e-a7b0-4655-afdb-74d081b617fc)

   - 불릿 타임 기능을 추가하여 투사체를 피하거나 패링할 수 있게 설계했습니다.
   - 점프와 같은 물리는 점프 시작과 점프 루프, 착지와 같이 여러 분기처리를 하고 애니메이션을 연동하는 예외적인 부분도 전부 처리하였습니다.
   - 점프, 슬라이딩 등 다양한 물리 계산 및 Quake식 가속을 활용해 속도감을 높였습니다.  
   - 소드 트레일 효과를 통해 검 휘두를 때 궤적을 표현했습니다.

---

## 트러블슈팅

### 1. **맵 로딩 최적화**  
- 맵 자체에 오브젝트가 너무 많고 리소스의 퀄리티도 너무 높아, 로딩이 지나치게 오래 걸리는 문제가 있었습니다. (약 30분…)

한번 할 때마다 로딩을 30분씩 하는 광경을 도저히 볼 수가 없어서, 저는 일단 메쉬 데이터를 전부 이진 직렬화하는 과정을 거치고
시간을 재 보았을 때. 무려 15분이나 감소하는 성과를 얻었지만, 15분이 감소해도 15분이 남았기에 더욱 최적화를 해야 했습니다.

그 후 저는 멀티스레딩 기법을 이용해, 내부에서 여러 머테리얼들을 부르는 과정이나, 본을 노드화 시켜 생성하는 여러 과정들을 멀티스레딩 방식으로 처리하고, 성능이 조금 떨어지지만 뮤텍스를 이용해 메모리 중복 접근으로 인한 오류를 방지했습니다.

이 방식까지 사용 했을 때도 10분정도 감소되어 5분정도의 로딩 시간이 소요되었는데, 그 후 중복 텍스쳐의 로딩을 방지하기 위해, 텍스쳐의 이름들을 전부 고유하게 처리하여 중복 텍스쳐의 로딩을 방지하고, 기존에 있던 리소스 컨테이너에서 주소값만 참조하는 방식으로 변경하자, 기존 30분 이상 걸리던 로딩을 40초까지 줄이는 데에 성공하였습니다. 

그 이상의 내부 로직을 멀티스레딩을 이용해 최적화 작업을 시도하였는데, 컨텍스트 스위칭 코스트가 더 높게 잡히는 일이 일어나 역으로 속도가 더 내려가거나, 의도치 않은 결과가 나오는 일이 벌어져 마지막으로 30분이 걸리던 로딩 시간은 40초대 로딩으로 마무리하였습니다.

---

요약 
- **이진 직렬화**: 메쉬 데이터를 직렬화하여 로딩 시간을 15분으로 단축.  
- **멀티스레딩**: 본 생성, 머티리얼 로딩을 병렬로 처리하고 뮤텍스로 오류를 방지하며 로딩 시간을 5분으로 단축.  
- **중복 텍스처 방지**: 텍스처 이름을 고유화하고 주소 참조 방식을 변경해 최종 로딩 시간을 40초로 단축.

---

### 2. **와이어 액션 구현**  
와이어 액션에도 생각보다 생각할 거리가 여러 부분 있었습니다. 기본적으로 와이어 액션을 하는 경우 와이어의 방향으로 직선 운동을 하는 액션과, 
와이어는 대략적인 방향 벡터만 제시하고 타잔처럼 와이어의 원점을 스쳐 지나가는 액션이 있는데, 제가 필요한 것은 후자였습니다. 

전자의 경우는 구현하기는 굉장히 쉬웠지만, 후자의 경우에는 와이어 트레일 같은 트레일버퍼 쉐이딩은 쉽다고 생각될 정도의 물리였습니다. 

저는 다른 부분은 양보하더라도, 이런 무브먼트 부분에 관해서는 제가 할 수 있는 최대한의 정성을 써서 만드는 편입니다. 

와이어 액션은 와이어를 작동하는 순간 플레이어 내부의
Wire State를 작동시킵니다. Wire State는 3가지 상태로
출발 가속, 원점 도착, 감속 도착 이라는 상태로 나누었습니다.
Y축이 가속에 끼치는 영향은 최대한 줄이면서, 완전한 포물선 운동이 아니라 내가 중간에 세부적인 움직임이 가능해야 했습니다.
중간에 대쉬와 같은 상태를 끊어버리는 행위를 한다면 
Wire State는 강제 종료되고 가속은 초기화됩니다. 

![시퀀스 01_4](https://github.com/user-attachments/assets/e48d189b-a3e4-4b1b-9464-6c08e940be74)

가속 물리조차도 기본적인 뛰기나 점프, 슬라이딩과 와이어 액션중에 전부 다른 물리 공식을 사용했기에 정말 쉽지 않은 작업이었다고 생각합니다.

---

요약 

- 기본 직선 운동 대신 포물선 운동과 중간 대쉬를 결합한 복잡한 물리 동작을 구현.  
- Wire State를 출발 가속, 원점 도착, 감속 도착으로 나누어 세밀한 움직임 제어를 가능하게 했습니다.  
- 점프, 슬라이딩과의 물리 공식 차이를 극복하며 사실적인 와이어 액션을 구현했습니다.

---

## 후기
DirectX 11로 작업하면서 만든 첫 개인 포트폴리오입니다.

화려한 쉐이더와 배경, 네온사인과 와이어 불릿타임 액션이라는 재미있어 보이는 것만 들어가 있어서 작업하는 내내 즐거웠지만,

사실 개발 기간 중에 마지막 한달 전, 저는 제 실수로 외장 하드를 분실해 코드와 리소스가 한번 소실되어서 다시 작업하게 된 작품입니다…

코드는 그래도 구버전이 커밋을 해서 조금 남아있었지만, 리소스까지 Github에 올리기에는 너무 무거워 안 올리고 있었던 불찰 때문에 제가 원래 원했던 목표치까지 도달하지 못했다는 아쉬움이 좀 많이 남았습니다.

그래도 제가 제 게임을 플레이하면서도 꽤 재미있었다고 생각이 들 정도였기에, 물리 액션 자체는 정말 공을 많이 들인 결과가 나와 주어서 아쉬움이 남았지만 그래도 만족스러운 결과였다고 생각합니다.


