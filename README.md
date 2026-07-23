# 가제: Katana

> **내일배움캠프 Unreal Track 9기 Chapter 3 12조 프로젝트**  
> Unreal Engine 5.8 기반 세키로라이크 보스 1:1 전투 프로젝트

---

## 프로젝트 세팅

| 항목 | 내용 |
|---|---|
| 프로젝트명 | Katana |
| 팀명 | 12조 |
| 저장소 | https://github.com/NBcampUnrealTrack/9th-Team12-CH3-Project.git |
| Unreal Engine | 5.8 |
| IDE | Rider |
| 언어 | C++ / Blueprint |
| 형상 관리 | Git / Git LFS |

---

## 혹시 에디터키고 푸시날려서 수정하지않은 파일이 수정사항으로 뜨시나요?

저런! 그렇다면 아래 지침을 확인하십시오.

```bash
git reset --hard HEAD

git fetch origin

git reset --hard origin/dev
```

## 커밋 규칙

커밋 메시지는 아래 형식을 사용합니다.

```text
<type>: <summary>
```

예시:

```bash
feat: add boss attack pattern
fix: resolve player parry timing bug
docs: update readme
chore: configure git lfs
refactor: split boss combat component
```

### Type 기준

| Type | 사용 시점 |
|---|---|
| `feat` | 신규 기능 구현 |
| `fix` | 버그 수정 |
| `docs` | 문서 추가/수정 |
| `chore` | 빌드 설정, 의존성, 기타 설정 변경 |
| `refactor` | 기능 변화 없는 구조 개선 |
| `ci` | GitHub Actions 등 CI 설정 수정 |
| `hotfix` | 긴급 수정 |

### 커밋 작성 기준

- 한 커밋에는 하나의 목적만 담습니다.
- 빌드가 깨진 상태로 커밋하지 않습니다.
- 의미 없는 커밋 메시지는 사용하지 않습니다.
  - 나쁜 예: `update`, `fix`, `asdf`, `수정`
  - 좋은 예: `fix: prevent boss phase transition crash`

---

## PR 생성 규칙

작업 완료 후 `dev` 브랜치로 Pull Request를 생성합니다.

PR 본문은 `.github/pull_request_template.md` 템플릿을 기준으로 작성합니다.

### PR 제목

```text
[type] 작업 요약
```

예시:

```text
[feat] 보스 1페이즈 공격 패턴 구현
[fix] 플레이어 패링 판정 오류 수정
[docs] README 프로젝트 세팅 수정
```

### PR 본문 작성 기준

PR 템플릿의 아래 항목을 작성합니다.

- 변경 요약
- 작업 유형
- 관련 이슈 / 작업 링크
- 변경 범위
- 구현 내용
- 테스트 결과
- 스크린샷 / 영상
- 에셋 / LFS 변경 사항
- 리뷰어 확인 포인트
- 리스크 / 미해결 사항
- 병합 전 체크리스트
- 추가 설명

### Unreal 프로젝트 PR 필수 확인 사항

- 테스트하지 않은 항목은 체크하지 않습니다.
- `.uasset`, `.umap` 변경 여부를 반드시 표시합니다.
- 신규 에셋, 삭제 에셋, Git LFS 추적 대상 변경 여부를 작성합니다.
- UI, VFX, Animation, Level 변경이 있으면 스크린샷 또는 영상을 첨부합니다.
- 리뷰어가 재현할 수 있도록 테스트 맵과 테스트 방법을 작성합니다.

### 병합 전 확인

- `dev` 최신 내용 반영
- Rider C++ 빌드 성공
- Unreal Editor 실행 확인
- Blueprint Compile 오류 없음
- 관련 기능 PIE 테스트 완료
- 불필요한 임시 파일 제거
- `.uasset`, `.umap` 등 LFS 파일 정상 추적 확인
- 충돌 가능성이 큰 Map / Blueprint / DataAsset 변경 사항 공유

---

## 브랜치 전략

### main

- 제출 또는 시연 가능한 안정 버전 브랜치
- 직접 작업 금지
- `dev`에서 검증된 내용만 병합

### dev

- 개발 기준 브랜치
- 모든 기능 브랜치는 `dev`에서 생성
- 작업 완료 후 PR을 통해 `dev`로 병합

### 작업 브랜치

```bash
git switch dev
git pull origin dev
git switch -c feat/my-task
```

브랜치명은 아래 형식을 사용합니다.

```text
<type>/<short-description>
```

예시:

```bash
feat/boss-phase-one
fix/player-parry
docs/update-readme
chore/setup-lfs
refactor/combat-system
```

---

## 프로젝트 시작

### 1. 저장소 클론

```bash
git clone https://github.com/NBcampUnrealTrack/9th-Team12-CH3-Project.git
cd 9th-Team12-CH3-Project
```

### 2. Git LFS 초기화

```bash
git lfs install
git lfs pull
```

### 3. Unreal 프로젝트 실행

1. `.uproject` 파일을 더블클릭합니다.
2. Unreal Editor에서 프로젝트 파일을 한 번 빌드합니다.
3. 빌드 완료 후 에디터를 종료합니다.
4. Rider로 `.uproject`를 엽니다.
5. Rider에서 전체 빌드를 실행합니다.

---

## Git LFS

Unreal 프로젝트는 `.uasset`, `.umap` 등 대용량 바이너리 파일을 사용하므로 Git LFS를 사용합니다.

### 최초 1회 설정

```bash
git lfs install
```

### 추적 권장 파일

```bash
git lfs track "*.uasset"
git lfs track "*.umap"
git lfs track "*.fbx"
git lfs track "*.png"
git lfs track "*.tga"
git lfs track "*.wav"
git lfs track "*.mp4"
```

`git lfs track` 실행 후 생성 또는 수정되는 `.gitattributes` 파일도 반드시 커밋합니다.

```bash
git add .gitattributes
git commit -m "chore: configure git lfs"
```

---

## Unreal 협업 주의사항

- 같은 `.umap`, `.uasset` 파일을 여러 명이 동시에 수정하지 않습니다.
- 공용 Blueprint, DataAsset, Material 수정 전 팀원에게 공유합니다.
- 에셋이 누락되면 먼저 `git lfs pull`을 실행합니다.
- `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`는 커밋하지 않습니다.
- 큰 구조 변경은 작업 전에 팀원에게 먼저 공유합니다.
