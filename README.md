# MachineHeart
<img width="330" height="200" alt="적상태 PNG" src="https://github.com/user-attachments/assets/85357508-dff5-4b2d-9761-a14529cf53c6" /> <img width="330" height="250" alt="저스트가드 PNG" src="https://github.com/user-attachments/assets/2f3b126f-df93-4adf-9bfb-c1b92a68f470" /><img width="330" height="200" alt="무기선택 PNG" src="https://github.com/user-attachments/assets/bf3adebd-f8ce-445e-b2be-d6cf35d9d219" />



숭실대학교 게임제작 동아리 겜마루 여름공모전 제작 게임입니다.

## 🎮 Game Overview
턴제 전투로 구성된 스테이지를 탐험하는 2.5D 로그라이트 던전RPG 게임입니다.

플레이어는 스테이지를 탐험하며 아이템과 장비를 획득하고, 적과 전투를 통해 최종 목표 지점에 도달해야 합니다.  
전투는 턴제로 진행되며, 정착 아이템에 따른 공격 선택과 버프/디버프 아이템 사용, 스킬 사용 등 전략적 요소를 활용해 승리를 노릴 수 있습니다.

## 👥 Team
기획 : 영어영문학과 박강민  
프로그래밍 : 컴퓨터학부 김형일  
아트 : 글로벌미디어학부 맹창영, 소프트웨어학부 김태인

## 🧱 Tech Stack
- **Engine**: Unreal Engine 5.4
- **Language**: 언리얼 C++ , 블루프린트
- **Plugins/Tools**: PaperZD, Paper2D

  
## ✨ Features
- 턴 기반 전투 시스템
- 플레이어 스킬 게이지 및 버프/디버프 장비,아이템 
- 인벤토리 & 장비 시스템
- 다양한 적 및 적 특수 행동
- 위젯과 레벨의 상호작용
- 여러개의 전투 스테이지
- 2.5D 하이브리드 픽셀아트
- 패링 및 보너스 데미지 시스템

## 📁 Code Structure

Source/  
└─ MachineHeart/  
&nbsp;&nbsp;&nbsp;&nbsp;├─ Behaviors/        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 적들의 특수 행동 로직  
&nbsp;&nbsp;&nbsp;&nbsp;├─ Character/        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 플레이어 및 적 클래스 베이스  
&nbsp;&nbsp;&nbsp;&nbsp;├─ CharacterStat/    &nbsp;&nbsp;&nbsp;→ 스탯 컴포넌트  
&nbsp;&nbsp;&nbsp;&nbsp;├─ GameInstance/     &nbsp;&nbsp;→ 스탯, 아이템, 클리어 레벨 저장 인스턴스  
&nbsp;&nbsp;&nbsp;&nbsp;├─ GameMode/         &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 전투/비전투 모드, 턴제 로직   
&nbsp;&nbsp;&nbsp;&nbsp;├─ Inventory/        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 인벤토리 시스템  
&nbsp;&nbsp;&nbsp;&nbsp;├─ Items/            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 아이템 데이터  
&nbsp;&nbsp;&nbsp;&nbsp;└─ UI/               &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ 각종 위젯 및 UI 바인딩  


## 🎥 Gameplay Video

썸네일 클릭시 영상 재생  

[![Watch the video](https://img.youtube.com/vi/2L8G2qruhKM/maxresdefault.jpg)](https://www.youtube.com/watch?v=2L8G2qruhKM)
